#include <complearn/complearn.h>
#include <complearn/google.h>
#include <assert.h>
#include <stdio.h>

#if CSOAP_RDY

#include <libcsoap/soap-client.h>
#include <stdlib.h>

SoapCtx *simplePrepareSOAPEnvForMethod(const char *urn, const char *method);

void normalizeSearchTerms(struct StringStack *terms)
{
  sortSS(terms);
}

const char *makeQueryString(struct StringStack *terms)
{
  static char buf[2536];
  int sb = 0;
  int i;
  for (i = 0; i < sizeSS(terms); ++i) {
    assert(sb + 100 < sizeof(buf));
    sb += sprintf(buf+sb, "%s+\"%s\"",i==0?"":" ",readAtSS(terms, i));
  }
  return buf;
}

static double rGetPageCount(struct StringStack *terms, const char *gkey)
{
  herror_t err;
  SoapCtx *ctx, *ctx2;
  char *method, *urn, *url;
  char *estStr = NULL;
  int trynum = 0;
  int isDone = 0;
  double estDouble = -1;
  xmlNodePtr function, node;
  method = "doGoogleSearch";
  urn = "urn:GoogleSearch";
  url = "http://api.google.com/search/beta2";
  while (!isDone) {
    trynum += 1;

    ctx = simplePrepareSOAPEnvForMethod(urn, method);
    soap_env_add_item(ctx->env, "xsd:string", "key", gkey);
    soap_env_add_item(ctx->env, "xsd:string", "q",makeQueryString(terms));
    soap_env_add_item(ctx->env, "xsd:int", "start","0");
    soap_env_add_item(ctx->env, "xsd:int", "maxResults","1");
    soap_env_add_item(ctx->env, "xsd:boolean", "filter","true");
    soap_env_add_item(ctx->env, "xsd:string", "restrict","");
    soap_env_add_item(ctx->env, "xsd:boolean", "safeSearch","false");
    soap_env_add_item(ctx->env, "xsd:string", "lr","");
    soap_env_add_item(ctx->env, "xsd:string", "ie","latin1");
    soap_env_add_item(ctx->env, "xsd:string", "oe","latin1");
    err = soap_client_invoke(ctx, &ctx2, url, method);
    if (err != H_OK) {
      log_error4("%s():%s [%d]", herror_func(err), herror_message(err), herror_code(err));
      herror_release(err);
      exit(1);
      return -2;
    }
    function = soap_env_get_method(ctx2->env);
    node = soap_xml_get_children(function);
    node = soap_xml_get_children(node);

    while (node) {
  //    printf("Node is %p, name is %p\n", node, node ? node->name : 0);
  //    printf("cur->name: %s\n", node->name);
      if (!xmlStrcmp(node->name, "estimatedTotalResultsCount")) {
        estStr = (char*)xmlNodeListGetString(node->doc, node->xmlChildrenNode, 1);
        break;
      }
   //   compsize = atof(str);
  //    node = soap_xml_get_next(node);
      node = node->next;
  //    node = soap_xml_get_next(node);
   }
    if (estStr) {
      estDouble = atof(estStr);
      isDone = 1;
    }
  else {
    printf("error in Google soap call, return value was:  <<EOF\n");
    soap_xml_doc_print(node->doc);
    printf("EOF\n");
    printf("Retrying...\n");
    }
    soap_ctx_free(ctx2);
    soap_ctx_free(ctx);
  }


/*
 * while (node) {
    char *str;
	  str = (char*)xmlNodeListGetString(node->doc, node->xmlChildrenNode, 1);
    printf("Got result: <%s>\n", str);
    node = soap_xml_get_next(node);
  }
  */

//  soap_xml_doc_print(sci->ctx2->env->root->doc);
//  soap_ctx_free(sci->ctx2);
//  soap_ctx_free(sci->ctx);

  return estDouble;
}

double getPageCount(struct StringStack *terms, const char *gkey)
{
  char *word;

  if (sizeSS(terms) == 1) {
    word = readAtSS(terms, 0);
    if (word[1] == '\0' && (word[0] == 'm' || word[0] == 'M')) {
      const char *daystr;
      struct CLDateTime *cldt;
      double res;
      cldt = cldtNow();
      daystr = cldt_daystring(cldt);
      res = calculateMbase(daystr, gkey);
      cldtfree(cldt);
      return 1.0/res;
    }
  }
  return rGetPageCount(terms, gkey);
}

#else

double getPageCount(struct StringStack *terms, const char *gkey)
{
  assert(0 && "No SOAP support installed");
  return -1;
}

void normalizeSearchTerms(struct StringStack *terms)
{
  assert(0 && "No SOAP support installed");
}

const char *makeQueryString(struct StringStack *terms)
{
  assert(0 && "No SOAP support installed");
  return NULL;
}

#endif
