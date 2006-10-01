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
#include <string.h>

#include <unistd.h>

#if HAVE_LIBCSOAP_SOAP_CLIENT_H

#include <libcsoap/soap-client.h>
#include <nanohttp/nanohttp-client.h>
#include <nanohttp/nanohttp-logging.h>
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

#if 0
static herror_t
_rsoap_client_build_result(hresponse_t * res, SoapEnv ** env)
{
  log_verbose2("Building result (%p)", res);

  if (res == NULL)
    return herror_new("_soap_client_build_result",
                      GENERAL_INVALID_PARAM, "hresponse_t is NULL");


  if (res->in == NULL)
    return herror_new("_soap_client_build_result",
                      GENERAL_INVALID_PARAM, "Empty response from server");

  if (res->errcode != 200)
    return herror_new("_soap_client_build_result",
                      GENERAL_INVALID_PARAM, "HTTP code is not OK (%i)", res->errcode);

  return soap_env_new_from_stream(res->in, env);
}

static herror_t
rsoap_client_invoke(SoapCtx * call, SoapCtx ** response, const char *url,
                   const char *soap_action)
{
  /* Status */
  herror_t status;

  /* Result document */
  SoapEnv *res_env;

  /* Buffer variables */
  xmlBufferPtr buffer;
  char *content;
  char tmp[15];

  /* Transport variables */
  httpc_conn_t *conn;
  hresponse_t *res;

  /* multipart/related start id */
  char start_id[150];
  static int counter = 1;
  part_t *part;

  /* for copy attachments */
  char href[MAX_HREF_SIZE];

  /* Create buffer */
  buffer = xmlBufferCreate();
  xmlNodeDump(buffer, call->env->root->doc, call->env->root, 1, 0);
  content = (char *) xmlBufferContent(buffer);

  /* Transport via HTTP */
  if (!(conn = httpc_new()))
  {
    return herror_new("rsoap_client_invoke", SOAP_ERROR_CLIENT_INIT,
                      "Unable to create SOAP client!");
  }

  /* Set soap action */
  if (soap_action != NULL)
    httpc_set_header(conn, "SoapAction", soap_action);

  httpc_set_header(conn, HEADER_CONNECTION, "Close");

  /* check for attachments */
  if (!call->attachments)
  {
    /* content-type is always 'text/xml' */
    httpc_set_header(conn, HEADER_CONTENT_TYPE, "text/xml");

    sprintf(tmp, "%d", (int) strlen(content));
    httpc_set_header(conn, HEADER_CONTENT_LENGTH, tmp);

    if ((status = httpc_post_begin(conn, url)) != H_OK)
    {
      httpc_close_free(conn);
      xmlBufferFree(buffer);
      return status;
    }

    if ((status = http_output_stream_write_string(conn->out, content)) != H_OK)
    {
      httpc_close_free(conn);
      xmlBufferFree(buffer);
      return status;
    }

    if ((status = httpc_post_end(conn, &res)) != H_OK)
    {
      httpc_close_free(conn);
      xmlBufferFree(buffer);
      return status;
    }
  }
  else
  {

    /* Use chunked transport */
    httpc_set_header(conn, HEADER_TRANSFER_ENCODING,
                     TRANSFER_ENCODING_CHUNKED);

    sprintf(start_id, "289247829121218%d", counter++);
    if ((status = httpc_mime_begin(conn, url, start_id, "", "text/xml")) != H_OK)
    {
      httpc_close_free(conn);
      xmlBufferFree(buffer);
      return status;
    }

    if ((status = httpc_mime_next(conn, start_id, "text/xml", "binary")) != H_OK)
    {
      httpc_close_free(conn);
      xmlBufferFree(buffer);
      return status;
    }

    if ((status = http_output_stream_write(conn->out, (unsigned char *) content, strlen(content))) != H_OK)
    {
      httpc_close_free(conn);
      xmlBufferFree(buffer);
      return status;
    }


    for (part = call->attachments->parts; part; part = part->next)
    {
      status = httpc_mime_send_file(conn, part->id,
                                    part->content_type,
                                    part->transfer_encoding, part->filename);
      if (status != H_OK)
      {
        log_error2("Send file failed. Status:%d", status);
        httpc_close_free(conn);
        xmlBufferFree(buffer);
        return status;
      }
    }

    if ((status = httpc_mime_end(conn, &res)) != H_OK)
    {
      httpc_close_free(conn);
      xmlBufferFree(buffer);
      return status;
    }
  }

  /* Free buffer */
  xmlBufferFree(buffer);

  /* Build result */
  if ((status = _rsoap_client_build_result(res, &res_env)) != H_OK)
  {
    hresponse_free(res);
    httpc_close_free(conn);
    return status;
  }

  /* Create Context */
  *response = soap_ctx_new(res_env);
/*	soap_ctx_add_files(*response, res->attachments);*/

  if (res->attachments != NULL)
  {
    part = res->attachments->parts;
    while (part)
    {
      soap_ctx_add_file(*response, part->filename, part->content_type, href);
      part->deleteOnExit = 0;
      part = part->next;
    }
    part = (*response)->attachments->parts;
    while (part)
    {
      part->deleteOnExit = 1;
      part = part->next;
    }
  }

  hresponse_free(res);
  httpc_close_free(conn);

  return H_OK;
}
#endif

static double rGetPageCount(struct StringStack *terms, const char *gkey)
{
  herror_t err;
  SoapCtx *ctx, *ctx2=NULL;
  char *method, *urn, *url;
  struct StringStack *mterms;
  char *estStr = NULL;
  int trynum = 0;
  int isDone = 0;
  double estDouble = -1;
  mterms = clStringstackClone(terms);
  xmlNodePtr function, node;
  method = "doGoogleSearch";
  urn = "urn:GoogleSearch";
  url = "http://api.google.com/search/beta2";
  while (!isDone) {
    trynum += 1;

    ctx = clSimplePrepareSOAPEnvForMethod(urn, method);
    soap_env_add_item(ctx->env, "xsd:string", "key", gkey);
    soap_env_add_item(ctx->env, "xsd:string", "q",clMakeQueryString(mterms));
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
      printf("error:%s():%s [%d]\n", herror_func(err), herror_message(err), herror_code(err));
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

  clStringstackFree(mterms);
  return estDouble;
}

double clGetPageCount(struct StringStack *terms, const char *gkey)
{
  char *word;

  if (clStringstackSize(terms) == 1) {
    word = clStringstackReadAt(terms, 0);
    if (word[1] == '\0' && (word[0] == 'm' || word[0] == 'M')) {
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
