#include <complearn/complearn.h>

/** \brief SOAP-specific instancing information is kept in this structure.
 * \struct SOAPCompInstance
 *
 * In order to connect to a SOAP adaptor, a url and urn string must be
 * provided.  This information is stored here.
 */
struct SOAPCompInstance;

#if CSOAP_RDY
#include <libcsoap/soap-client.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>
#include "clalloc.h"


static double sca_compfunc(struct CompAdaptor *ca, struct DataBlock *src);
static void sca_freecompfunc(struct CompAdaptor *ca);
static char *sca_shortname(void);
static char *sca_longname(void);
static int sca_apiver(void);

struct SOAPCompInstance {
  char *url;
  char *urn;
  char *method;
  SoapCtx *ctx, *ctx2;
};

struct CompAdaptor *builtin_SC(const char *url, const char *urn, const char *method)
{
	struct CompAdaptor c =
  {
    cptr: NULL,
//    se:   sca_clsetenv,
    cf:   sca_compfunc,
    fcf:  sca_freecompfunc,
    sn:   sca_shortname,
    ln:   sca_longname,
    apiv: sca_apiver,
  };
  struct CompAdaptor *ca;
  struct SOAPCompInstance *sci;
  ca = clCalloc(sizeof(*ca), 1);
  *ca = c;

  ca->cptr = clCalloc(sizeof(struct SOAPCompInstance), 1);
  sci = (struct SOAPCompInstance *) ca->cptr;

  sci->url = clStrdup(url);
  sci->urn = clStrdup(urn);
  sci->method = clStrdup(method);

  compaInitParameters(ca);

  return ca;
}

static SoapCtx *invokeMethod(struct SOAPCompInstance *sci, SoapCtx *inp)
{
  herror_t err;
  const char *url;
  url = sci->url;
  err = soap_client_invoke(sci->ctx, &sci->ctx2, url, sci->method);
  if (err != H_OK) {
    log_error4("%s():%s [%d]", herror_func(err), herror_message(err), herror_code(err));
    herror_release(err);
    printf("Error invoking method %s in %s\n", sci->method, sci->url);
    printf("Is SOAP server running?\n");
    exit(1);
    return NULL;
  }
  return sci->ctx2;
}

static double getValue(struct SOAPCompInstance *sci)
{
  double compsize = -2.0;
  xmlNodePtr function, node;
  function = soap_env_get_method(sci->ctx2->env);
  node = soap_xml_get_children(function);

  while (node) {
    char *str;
	  str = (char*)xmlNodeListGetString(node->doc, node->xmlChildrenNode, 1);
		/* TODO find better way to get number */
		compsize = atof(str);
    node = soap_xml_get_next(node);
  }

//  soap_xml_doc_print(sci->ctx2->env->root->doc);
//  soap_ctx_free(sci->ctx2);
//  soap_ctx_free(sci->ctx);
  return compsize;
}

static SoapCtx *prepareSOAPEnvForMethod(struct SOAPCompInstance *sci)
{
  SoapCtx *ctx;
  const char *urn;
  herror_t err;
  //xmlNodePtr function, node;

  urn = sci->urn;
  err = soap_ctx_new_with_method(urn, sci->method, &ctx);
  if (err != H_OK) {
    log_error4("%s():%s [%d]", herror_func(err), herror_message(err), herror_code(err));
    herror_release(err);
    return NULL;
  }
  return ctx;
}

SoapCtx *simplePrepareSOAPEnvForMethod(const char *urn, const char *method)
{
  SoapCtx *ctx;
  herror_t err;
  //xmlNodePtr function, node;

  err = soap_ctx_new_with_method(urn, method, &ctx);
  if (err != H_OK) {
    log_error4("%s():%s [%d]", herror_func(err), herror_message(err), herror_code(err));
    herror_release(err);
    return NULL;
  }
  return ctx;
}

static double sca_compfunc(struct CompAdaptor *ca, struct DataBlock *src)
{
	struct SOAPCompInstance *sci = (struct SOAPCompInstance *) ca->cptr;
	//int s;
  double compsize;
  char *str = clCalloc(1,datablockSize(src)+1);
  str = (char *) datablockData(src);

  memcpy(str, datablockData(src), datablockSize(src));

  sci->ctx = prepareSOAPEnvForMethod(sci);
  soap_env_add_item(sci->ctx->env, "xsd:string", "str",str);

  invokeMethod(sci, sci->ctx);
  compsize = getValue(sci);
  clFreeandclear(str);
	return (double) compsize;
}

static void sca_freecompfunc(struct CompAdaptor *ca)
{
	struct SOAPCompInstance *ci = (struct SOAPCompInstance *) ca->cptr;
  clFreeandclear(ci->urn);
  clFreeandclear(ci->url);
  clFreeandclear(ci->method);
  clFreeandclear(ca->cptr);
	clFreeandclear(ca);
}

static char *sca_shortname(void)
{
	return "soap";
}

static char *sca_longname(void)
{
	return "General purpose compression library soap";
}

static int sca_apiver(void)
{
	return APIVER_V1;
}

#else
#include <stdio.h>
struct CompAdaptor *builtin_SC(void)
{
  return NULL;
}
#endif
