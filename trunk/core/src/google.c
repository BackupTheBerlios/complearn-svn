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
#include <complearn/google.h>
#include <assert.h>
#include <stdio.h>

#include <unistd.h>

#if HAVE_LIBCSOAP_SOAP_CLIENT_H

#include <libcsoap/soap-client.h>
#include <stdlib.h>

SoapCtx *clSimplePrepareSOAPEnvForMethod(const char *urn, const char *method);

void clNormalizeSearchTerms(struct StringStack *terms)
{
  clStringstackSort(terms);
}

const char *clMakeQueryString(struct StringStack *terms)
{
  static char buf[2536];
  int sb = 0;
  int i;
  for (i = 0; i < clStringstackSize(terms); ++i) {
    assert(sb + 100 < sizeof(buf));
    sb += sprintf(buf+sb, "%s+\"%s\"",i==0?"":" ",clStringstackReadAt(terms, i));
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

    ctx = clSimplePrepareSOAPEnvForMethod(urn, method);
    soap_env_add_item(ctx->env, "xsd:string", "key", gkey);
    soap_env_add_item(ctx->env, "xsd:string", "q",clMakeQueryString(terms));
    soap_env_add_item(ctx->env, "xsd:int", "start","0");
    soap_env_add_item(ctx->env, "xsd:int", "maxResults","1");
    soap_env_add_item(ctx->env, "xsd:boolean", "filter","true");
    soap_env_add_item(ctx->env, "xsd:string", "restrict","");
    soap_env_add_item(ctx->env, "xsd:boolean", "safeSearch","false");
    soap_env_add_item(ctx->env, "xsd:string", "lr","");
    soap_env_add_item(ctx->env, "xsd:string", "inputEncoding","UTF-8");
    soap_env_add_item(ctx->env, "xsd:string", "outputEncoding","UTF-8");
//    printf("Invoking...\n");
    err = soap_client_invoke(ctx, &ctx2, url, method);
//    printf("Done: %d,%d\n", err,trynum);
    if (err != H_OK) {
      //log_error4("%s():%s [%d]", herror_clFunc(err), herror_message(err), herror_code(err));
      herror_release(err);
      sleep(trynum*trynum+5);
      continue;
    }
    function = soap_env_get_method(ctx2->env);
    node = soap_xml_get_children(function);
    node = soap_xml_get_children(node);

    while (node) {
  //    printf("Node is %p, name is %p\n", node, node ? node->name : 0);
  //    printf("cur->name: %s\n", node->name);
      if (!xmlStrcmp(node->name, (const xmlChar *) "estimatedTotalResultsCount")) {
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

double clGetPageCount(struct StringStack *terms, const char *gkey)
{
  char *word;

  if (clStringstackSize(terms) == 1) {
    word = clStringstackReadAt(terms, 0);
    if (word[1] == '\0' && (word[0] == 'm' || word[0] == 'M')) {
      const char *daystr;
      double res;
      res = clCalculateMbase(NULL, gkey);
      return 1.0/res;
    }
  }
  return rGetPageCount(terms, gkey);
}

#else

double clGetPageCount(struct StringStack *terms, const char *gkey)
{
  assert(0 && "No SOAP support installed");
  return -1;
}

void clNormalizeSearchTerms(struct StringStack *terms)
{
  assert(0 && "No SOAP support installed");
}

const char *clMakeQueryString(struct StringStack *terms)
{
  assert(0 && "No SOAP support installed");
  return NULL;
}

#endif
