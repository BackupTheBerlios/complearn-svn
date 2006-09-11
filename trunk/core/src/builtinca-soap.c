/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <complearn/complearn.h>

/** \brief SOAP-specific instancing information is kept in this structure.
 *
 * In order to connect to a SOAP adaptor, a url and urn string must be
 * provided.  This information is stored here.
 *
 * \struct SOAPCompInstance
 */
struct SOAPCompInstance;

#if HAVE_LIBCSOAP_SOAP_CLIENT_H
#include <libcsoap/soap-client.h>
#include <stdlib.h>
#include <string.h>

static double sca_compclFunc(struct CompAdaptor *ca, struct DataBlock *src);
static void sca_freecompclFunc(struct CompAdaptor *ca);
static char *sca_shortname(void);
static char *sca_longname(void);
static int sca_apiver(void);

struct SOAPCompInstance {
  char *url;
  char *urn;
  char *method;
  SoapCtx *ctx, *ctx2;
};

struct CompAdaptor *clBuiltin_SC(const char *url, const char *urn, const char *method)
{
	struct CompAdaptor c =
  {
    cptr: NULL,
//    se:   sca_clsetenv,
    cf:   sca_compclFunc,
    fcf:  sca_freecompclFunc,
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

  clCompaInitParameters(ca);

  return ca;
}

static SoapCtx *invokeMethod(struct SOAPCompInstance *sci, SoapCtx *inp)
{
  herror_t err;
  const char *url;
  url = sci->url;
  err = soap_client_invoke(sci->ctx, &sci->ctx2, url, sci->method);
  if (err != H_OK) {
/*    log_error4("%s():%s [%d]", herror_func(err), herror_message(err), herror_code(err)); */
    printf("Error invoking method %s in %s, leading to %s:%s\n", sci->method, sci->url, herror_func(err), herror_message(err));
    herror_release(err);
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
    /*log_error4("%s():%s [%d]", herror_func(err), herror_message(err), herror_code(err)); */
    printf("ERROR A\n");
    herror_release(err);
    return NULL;
  }
  return ctx;
}

SoapCtx *clSimplePrepareSOAPEnvForMethod(const char *urn, const char *method)
{
  SoapCtx *ctx;
  herror_t err;
  //xmlNodePtr function, node;

  err = soap_ctx_new_with_method(urn, method, &ctx);
  if (err != H_OK) {
    printf("ERROR B\n");
    /*log_error4("%s():%s [%d]", herror_func(err), herror_message(err), herror_code(err)); */
    herror_release(err);
    return NULL;
  }
  return ctx;
}

static double sca_compclFunc(struct CompAdaptor *ca, struct DataBlock *src)
{
	struct SOAPCompInstance *sci = (struct SOAPCompInstance *) ca->cptr;
	//int s;
  double compsize;
  char *str = clCalloc(1,clDatablockSize(src)+1);
  str = (char *) clDatablockData(src);

  memcpy(str, clDatablockData(src), clDatablockSize(src));

  sci->ctx = prepareSOAPEnvForMethod(sci);
  soap_env_add_item(sci->ctx->env, "xsd:string", "str",str);

  invokeMethod(sci, sci->ctx);
  compsize = getValue(sci);
  clFreeandclear(str);
	return (double) compsize;
}

static void sca_freecompclFunc(struct CompAdaptor *ca)
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
struct CompAdaptor *clBuiltin_SC(void)
{
  return NULL;
}
#endif
