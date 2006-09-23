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

#if HAVE_LIBCSOAP_SOAP_CLIENT_H
#include <libcsoap/soap-client.h>
#endif

#include <stdlib.h>
#include <string.h>

struct SOAPCompInstance;
#if HAVE_LIBCSOAP_SOAP_CLIENT_H
SoapCtx *clSimplePrepareSOAPEnvForMethod(const char *urn, const char *method);

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
#else
void *clSimplePrepareSOAPEnvForMethod(const char *urn, const char *method)
{
  fprintf(stderr, "No SOAP support\n");
  exit(1);
}
#endif
