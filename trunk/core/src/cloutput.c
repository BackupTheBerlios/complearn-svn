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
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <complearn/ncdapp.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <libxml/encoding.h>

#define MY_ENCODING "ISO-8859-1"

#include <gsl/gsl_linalg.h>

#define PWD_RDY 1
#define UTS_RDY 1

#if PWD_RDY
#include <pwd.h>
#endif

#if UTS_RDY
#include <sys/utsname.h>
#endif

static void customPrintProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, const char *rowBegin, const char *rowEnd, const char *elemBegin, const char *elemEnd, struct GeneralConfig *cur);

double clXpremap(double inp, struct GeneralConfig *cur)
{
  if (cur->fDoExponentiate)
    return cur->multiplier * pow(2.0, -inp);
  else
    return inp;
}

gsl_matrix *clSvdProject(gsl_matrix *a)
{
  int retval;
  gsl_matrix *res;
  gsl_matrix *u, *v;
  gsl_vector *s;
  u = clGslmatrixClone(a);
  v = gsl_matrix_alloc(a->size2, a->size2);
  s = gsl_vector_alloc(a->size1);
  retval = gsl_linalg_SV_decomp_jacobi(u, v, s);
//  assert(retval == GSL_OK);
  res = gsl_matrix_alloc(a->size1, a->size2);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, a, u, 0.0, res);
  return res;
}

void clPrintProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur)
{
  if (cur->fHTML) {
    printf("<table>\n");
    customPrintProduct(a, b, "<tr>\n", "\n</tr>\n", "<td>", "</td>", cur);
    printf("</table>\n");
  }
  else
    customPrintProduct(a, b, "", "\n", "", "  ", cur);
}

struct DataBlock *clCreateCloneWithNLFree(struct DataBlock *db)
{
  struct DataBlock *result;
  result = clDatablockNewFromBlock(clDatablockData(db),clDatablockSize(db)+1);
  clDatablockData(result)[clDatablockSize(db)] = '\n';
  clDatablockFreePtr(db);
  return result;
}

gsl_matrix *clGetNCDMatrix(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, struct GeneralConfig *cur)
{
  gsl_matrix *gres;
//  const char *fmtString = "%03.3f ";
  int n1Counter = 0, n1Set = 0;
  int n2Counter = 0, n2Set = 0;
  struct DRA *da = clDraNew();
  struct DataBlockEnumerationIterator *ia, *ib;
  struct DataBlock *dba, *dbb;

  for ( ia = a->newenumiter(a); (dba = a->istar(a, ia)) ; a->istep(a, ia) ) {
    struct DataBlock *obj;
    n1Counter += 1;
    if (cur->fAddNLAtString)
      dba = clCreateCloneWithNLFree(dba);
    for ( ib = b->newenumiter(b); ( obj = b->istar(b,ib) ) ; b->istep(b,ib) ) {
      int madeNewOne = 0;
      double ncd;
      double outnum;
      dbb = obj;
      if (!n2Set) {
        n2Counter += 1;
      }
      if (cur->fAddNLAtString) {
        dbb = clCreateCloneWithNLFree(dbb);
        madeNewOne = 1;
      }
      ncd = clNcdFunc(dba, dbb, cur);
      outnum = clXpremap(ncd, cur);
//      printf(fmtString, outnum);
      clDraSetDValueAt(da, clDraSize(da), outnum);
      if (madeNewOne)
        clDatablockFreePtr(dbb);
    }
    n2Set = 1;
    b->ifree(ib);
//    printf("\n");
    clDatablockFreePtr(dba);
  }
  n1Set = 1;
  a->ifree(ia);
  gres = gsl_matrix_alloc(n1Counter, n2Counter);
  {
    int x1, x2, i = 0;
    for (x1 = 0; x1 < n1Counter; x1++)
      for (x2 = 0; x2 < n2Counter; x2++) {
        gsl_matrix_set(gres, x1, x2, clDraGetDValueAt(da, i++)); /**  ^-^ **/
      }
  }
  clDraFree(da);
  return gres;
}

struct StringStack *clDefaultLabels(int i);


struct DataBlock *clSprintfDM(struct CLDistMatrix *cl)
{
  struct StringStack *res;
  struct DataBlock *db;
  int i, acc=0, ptr=0;
  char buf[1024], *dat;
  res = clStringstackNew();
  sprintf(buf, "Username: %s\n", cl->username); clStringstackPush(res,buf);
  sprintf(buf, "Title   : %s\n", cl->title); clStringstackPush(res,buf);
  sprintf(buf, "CTime   : %s\n", cl->creationTime);
  clStringstackPush(res,buf);
  sprintf(buf, "FileVers: %s\n", cl->fileverstr); clStringstackPush(res,buf);
  sprintf(buf, "CLibVers: %s\n", cl->cllibver); clStringstackPush(res,buf);
  for (i = 0; i < clStringstackSize(cl->cmds); i += 1) {
    sprintf(buf, "%d: %s\n", i+1, clStringstackReadAt(cl->cmds, i));
    clStringstackPush(res, buf);
  }
  for (i = 0; i < clStringstackSize(res); i += 1)
    acc += strlen(clStringstackReadAt(res, i));
  dat = calloc(acc+1,1);
  for (i = 0; i < clStringstackSize(res); i += 1) {
    strcpy(dat+ptr, clStringstackReadAt(res, i));
    ptr +=   strlen(clStringstackReadAt(res,i));
  }
  db = clDatablockNewFromBlock(dat, acc+1);
  clFree(dat);
  return db;
}

static void handleStringList(xmlDocPtr doc, xmlNodePtr node, struct StringStack *ss, const char *tagname) {
  node = node->xmlChildrenNode;
  while (node != NULL) {
    if (strcmp(tagname, (char *) node->name) == 0)
      clStringstackPush(ss, (char *) xmlNodeListGetString(doc, node->xmlChildrenNode,1));
    node = node->next;
  }
}

static void handleDM(xmlDocPtr doc, xmlNodePtr node, struct StringStack *ss,
struct CLDistMatrix *result) {
  result->title = (char *) xmlGetProp(node, (unsigned char *) "title");
  result->creationTime = (char *) xmlGetProp(node, (unsigned char *) "creationtime");
  node = node->xmlChildrenNode;
  while (node != NULL) {
    do {
        if (strcmp("entries", (char *) node->name) == 0) {
          handleStringList(doc, node, ss, "number");
          continue;
        }
        if (strcmp((char *) node->name, "axis1") == 0) {
          handleStringList(doc, node, result->labels[0], "name");
          continue;
        }
        if (strcmp((char *) node->name, "axis2") == 0) {
          handleStringList(doc, node, result->labels[1], "name");
          continue;
        }
      } while(0);
    node = node->next;
  }
}

static void writeStringList(xmlTextWriterPtr tw, struct StringStack *ss, const char *topname, const char *itemname) {
  int rc;
  int i;
  rc = xmlTextWriterStartElement(tw, (unsigned char *) topname);
  for (i = 0; i < clStringstackSize(ss); i += 1)
    rc = xmlTextWriterWriteElement(tw, (unsigned char *) itemname, (unsigned char *) clStringstackReadAt(ss,i));
  rc = xmlTextWriterEndElement(tw);
}

static char *getTimestrNow(void)
{
  static char buf[32];
  sprintf(buf, "%lu", (unsigned long) clDatetimeStaticTimer());
  return buf;
}

static struct CLDistMatrix *fillInBlanks(struct CLDistMatrix *clb)
{
  int i;
  if (clb->mat == NULL) {
    clLogError("Cannot write NULL gsl_matrix, exitting.");
  }
  if (clb->mat->size1 < 1 || clb->mat->size2 < 1) {
    clLogError("Invalid gsl_matrix size, cannot write.  Exitting.");
  }
  if (clb->fileverstr == NULL)
    clb->fileverstr = clStrdup("1.0");
  if (clb->cllibver == NULL)
    clb->cllibver = clStrdup(PACKAGE_VERSION);
  if (clb->username == NULL)
    clb->username = clStrdup(clGetUsername());
  if (clb->hostname == NULL)
    clb->hostname = clStrdup(clGetHostname());
  if (clb->title == NULL)
    clb->title = clStrdup("untitled");
  if (clb->compressor == NULL)
    clb->compressor = clStrdup("unknown");
  if (clb->creationTime == NULL)
    clb->creationTime = clStrdup(getTimestrNow());
  if (clb->labels == NULL)
    clb->labels = (struct StringStack **) clCalloc(sizeof(struct StringStack *), 2);
  for (i = 0; i < 2; i += 1) {
    if (clb->labels[i] == NULL)
      clb->labels[i] = clStringstackNew();
    if (clStringstackSize(clb->labels[i]) == 0) {
      clStringstackFree(clb->labels[i]);
      clb->labels[i] =clDefaultLabels(i==0 ?clb->mat->size1:clb->mat->size2);
    }
  }
  return clb;
}

struct CLDistMatrix *clCloneCLDM(struct CLDistMatrix *clb)
{
  struct CLDistMatrix *r = clCalloc(sizeof(*r), 1);
  if (clb->mat == NULL) {
    clLogError("Cannot write NULL gsl_matrix, exitting.");
  }
#define DUPSTR(s) if (clb->s) r->s = clStrdup(clb->s)
  DUPSTR(cllibver);
  DUPSTR(username);
  DUPSTR(hostname);
  DUPSTR(title);
  DUPSTR(compressor);
  DUPSTR(creationTime);
  r->mat = gsl_matrix_alloc(clb->mat->size1, clb->mat->size2);
  gsl_matrix_memcpy(r->mat, clb->mat);
  if (clb->labels) {
    r->labels = clCalloc(sizeof(void *), 2);
    if (clb->labels[0]) r->labels[0] = clStringstackClone(clb->labels[0]);
    if (clb->labels[1]) r->labels[1] = clStringstackClone(clb->labels[1]);
  }
  if (clb->cmds) r->cmds = clStringstackClone(clb->cmds);
  if (clb->cmdtimes) r->cmdtimes = clStringstackClone(clb->cmdtimes);
  return r;
}

void clFreeCLDM(struct CLDistMatrix *clb)
{
  if (clb->cllibver) clFree(clb->cllibver);
  if (clb->username) clFree(clb->username);
  if (clb->hostname) clFree(clb->hostname);
  if (clb->title) clFree(clb->title);
  if (clb->compressor) clFree(clb->compressor);
  if (clb->creationTime) clFree(clb->creationTime);
  if (clb->mat) gsl_matrix_free(clb->mat);
  if (clb->cmds) clStringstackFree(clb->cmds);
  if (clb->cmdtimes) clStringstackFree(clb->cmdtimes);
  if (clb->labels) {
    if (clb->labels[0]) clStringstackFree(clb->labels[0]);
    if (clb->labels[1]) clStringstackFree(clb->labels[1]);
    clFree(clb->labels);
  }
  memset(clb, 0, sizeof(*clb));
  clFree(clb);
}

static char *clXMLQuoteStr(char *inp)
{
  int i, c, j;
  static char *outstr;
  if (outstr != NULL)
    clFree(outstr);
  outstr = clCalloc(strlen(inp) * 10, 1);
  j = 0;
  for (i = 0; inp[i]; i += 1) {
    c = inp[i];
    switch(c) {
      case '&':
        outstr[j++] = '&'; outstr[j++] = 'a'; outstr[j++] = 'm';
        outstr[j++] = 'p'; outstr[j++] = ';'; break;
      case '>':
        outstr[j++] = '&'; outstr[j++] = 'g'; outstr[j++] = 't';
        outstr[j++] = ';'; break;
      case '<':
        outstr[j++] = '&'; outstr[j++] = 'l'; outstr[j++] = 't';
        outstr[j++] = ';'; break;
      case '"':
        outstr[j++] = '&'; outstr[j++] = 'q'; outstr[j++] = 'o';
        outstr[j++] = 'o'; outstr[j++] = 't'; outstr[j++] = ';'; break;
      case '\'':
        outstr[j++] = '&'; outstr[j++] = 'a'; outstr[j++] = 'p';
        outstr[j++] = 'o'; outstr[j++] = 's'; outstr[j++] = ';'; break;
//      case '\\':
 //     case '#':
//        outstr[j++] = '\\'; outstr[j++] = c; break;
      default:
        outstr[j++] = c;
    }
  }
  outstr[j++] = 0;
  return outstr;
}

static struct DataBlock *clRealWriteCLBDistMatrix(struct CLDistMatrix *clb)
{
  int rc;
  struct DataBlock *result, *cres;
  int dim1, dim2, i, j;
  gsl_matrix *m = clb->mat;
  xmlBufferPtr b;
  xmlTextWriterPtr tw;
  clb = fillInBlanks(clCloneCLDM(clb));
  b = xmlBufferCreate();
  tw =xmlNewTextWriterMemory(b, 0);
  rc = xmlTextWriterStartDocument(tw, NULL, MY_ENCODING, NULL);
  rc = xmlTextWriterStartElement(tw, (unsigned char *) "clb");
  rc = xmlTextWriterWriteAttribute(tw, (unsigned char *) "version", (unsigned char *) clb->fileverstr);

    rc = xmlTextWriterWriteElement(tw, (unsigned char *) "cllibver", (unsigned char *) clb->cllibver);
    rc = xmlTextWriterWriteElement(tw, (unsigned char *) "username", (unsigned char *) clb->username);
    rc = xmlTextWriterWriteElement(tw, (unsigned char *) "hostname", (unsigned char *) clb->hostname);
    rc = xmlTextWriterWriteElement(tw, (unsigned char *) "compressor", (unsigned char *) clb->compressor);
    rc = xmlTextWriterStartElement(tw,(unsigned char *)  "distmatrix");
    rc = xmlTextWriterWriteAttribute(tw,(unsigned char *)  "creationtime", (unsigned char *) clb->creationTime);
    rc = xmlTextWriterWriteAttribute(tw, (unsigned char *) "title",(unsigned char *)  clb->title);
    writeStringList(tw, clb->labels[0],  "axis1",  "name");
    writeStringList(tw, clb->labels[1],  "axis2",  "name");
    dim1 = clStringstackSize(clb->labels[0]);
    dim2 = clStringstackSize(clb->labels[1]);
      rc = xmlTextWriterStartElement(tw,(unsigned char *)  "entries");
      for (i = 0; i < dim1; i += 1) {
        for (j = 0; j < dim2; j += 1) {
          double g = gsl_matrix_get(m, i, j);
            xmlTextWriterWriteFormatElement(tw, (unsigned char *) "number", (char *) "%f", g);
        }
      }
      rc = xmlTextWriterEndElement(tw);
    rc = xmlTextWriterEndElement(tw);
    if (clb->cmds) {
      char *str;
      rc = xmlTextWriterStartElement(tw,(unsigned char *)  "commands");
      for (i = 0; i < clStringstackSize(clb->cmds); i += 1) {
        char *t = clb->cmdtimes ? clStringstackReadAt(clb->cmdtimes, i) : NULL;
        rc = xmlTextWriterStartElement(tw,(unsigned char *)  "cmdstring");
        if (t)
          rc = xmlTextWriterWriteAttribute(tw,(unsigned char *)  "creationtime", (unsigned char *) t);
        str = clStringstackReadAt(clb->cmds, i);
        xmlTextWriterWriteRaw(tw, (unsigned char *)  clXMLQuoteStr(str));
        rc = xmlTextWriterEndElement(tw);
      }
      rc = xmlTextWriterEndElement(tw);
    }
  rc = xmlTextWriterEndElement(tw);
  rc = xmlTextWriterEndDocument(tw);
  xmlFreeTextWriter(tw);
  result = clStringToDataBlockPtr((char *) b->content);
  xmlBufferFree(b);
  /* TODO: remove more mem leaks from above func */
  clFreeCLDM(clb);
  cres = clBZ2CompressDB(result);
  clDatablockFreePtr(result);
  return cres;
}

static struct DataBlock *clWriteCLBDistMatrix(gsl_matrix *mat, struct StringStack *labels, struct StringStack *cmds, const char *compressor_name)
{
  struct CLDistMatrix inp;
  struct StringStack *lab[2] = { NULL, NULL };
  struct DataBlock *result;
  memset(&inp, 0, sizeof(inp));
  inp.mat = gsl_matrix_alloc(mat->size1, mat->size2);
  gsl_matrix_memcpy(inp.mat, mat);
  if (labels) {
    lab[0] = clStringstackClone(labels);
    lab[1] = clStringstackClone(labels);
    inp.labels = lab;
  }
  if (cmds)
    inp.cmds = clStringstackClone(cmds);
  if (compressor_name)
    inp.compressor = clStrdup(compressor_name);
  result =  clRealWriteCLBDistMatrix(&inp);
  if (labels) {
    clStringstackFree(lab[0]);
    clStringstackFree(lab[1]);
  }
  if (cmds)
    clStringstackFree(inp.cmds);
  if (compressor_name)
    clFree(inp.compressor);
  gsl_matrix_free(inp.mat);
  memset(&inp, 0, sizeof(inp));
  return result;
}

gsl_matrix *clbDBDistMatrix(struct DataBlock *db)
{
  return clReadCLBDistMatrix(db)->mat;
}

struct CLDistMatrix *clReadCLBDistMatrix(struct DataBlock *udb)
{
  struct CLDistMatrix *result = calloc(sizeof(struct CLDistMatrix), 1);
  struct StringStack *ents = clStringstackNew();
  struct DataBlock *db;
  int dim1, dim2, i, j, k;
  xmlDocPtr doc;
  xmlNodePtr node;
  struct TransformAdaptor *b;
  b = clBuiltin_UNBZIP();
  if (b && b->pf(udb))
    db = b->tf(udb);
  else
    db = clDatablockClonePtr(udb);

  result->fileverstr = "";
  result->username = "";
  result->title = "";
  result->creationTime = "";
  result->mat = NULL;
  result->cmds = clStringstackNew();
  result->cmdtimes = clStringstackNew();
  result->labels = clCalloc(sizeof(void *), 2);
  result->labels[0] = clStringstackNew();
  result->labels[1] = clStringstackNew();
  doc = xmlReadMemory((char *)clDatablockData(db), clDatablockSize(db), "noname.xml",
                      NULL, 0);
  if (doc == NULL) {
    fprintf(stderr, "Failed to parse document\n");
    clDatablockFreePtr(db);
    free(result);
    return NULL;
  }
  node = doc->children;
  if (strcmp((char *) node->name, "clb") != 0) {
    free(result);
    clDatablockFreePtr(db);
    return NULL;
  }
  result->fileverstr = (char *) xmlGetProp(node, (unsigned char *) "version");
  node = node->xmlChildrenNode;
 while (node != NULL) {
    do {
      if (strcmp((char *) node->name, "commands") == 0) {
        handleStringList(doc, node, result->cmds, "cmdstring");
        continue;
      }
      if (strcmp((char *) node->name, "distmatrix") == 0) {
        handleDM(doc, node, ents, result);
        continue;
      }
      if (strcmp((char *) node->name, "cllibver") == 0) {
        result->cllibver = (char *) clStrdup((char *) xmlNodeListGetString(doc, node->xmlChildrenNode, 1));
        continue;
      }
      if (strcmp((char *) node->name, "compressor") == 0) {
        result->compressor = (char *) clStrdup((char *) xmlNodeListGetString(doc, node->xmlChildrenNode, 1));
        continue;
      }
      if (strcmp((char *) node->name, "username") == 0) {
        result->username = (char *) clStrdup((char *) xmlNodeListGetString(doc, node->xmlChildrenNode, 1));
        continue;
      }
    } while (0);
    node = node->next;
    }

// xmlFreeDoc(doc);
  dim1 = clStringstackSize(result->labels[0]);
  dim2 = clStringstackSize(result->labels[1]);
  if (clStringstackSize(ents) != dim1*dim2) {
    clLogError("Found dim1 %d and dim2 %d but %d entries instead of %d\n", dim1, dim2, clStringstackSize(ents), dim1*dim2);
  }
  result->mat = gsl_matrix_alloc(dim1, dim2);
  k = 0;
  for (i = 0; i < dim1; i += 1)
    for (j = 0; j < dim2; j += 1)
      gsl_matrix_set(result->mat, i, j, atof(clStringstackReadAt(ents,k++)));
  clStringstackFree(ents);
  clDatablockFreePtr(db);
  return result;
}

/*
int main(int argc, char **argv)
{
  struct DataBlock *db;
  struct CLDistMatrix *clb;
  db = clFileToDataBlockPtr("dm.xml");
  clb = clReadCLBDistMatrix(db);
  db = clSprintfDM(clb);
  printf("%s\n", clDatablockData(db));
  db = clWriteCLBDistMatrix(clb->mat, NULL);
  clDatablockWriteToFile(db, "newdm.xml");
  return 0;
}
*/
struct DataBlock *clMakeCLBDistMatrix(gsl_matrix *gres, struct StringStack *labels, struct StringStack *cmds, struct EnvMap *em)
{
  return clWriteCLBDistMatrix(gres, labels, cmds, clEnvmapValueForKey(em, "compressor"));
}

/* TODO: NCD only function; move to a better location */
static void customPrintProduct(struct DataBlockEnumeration *a, struct DataBlockEnumeration *b, const char *rowBegin, const char *rowEnd, const char *elemBegin, const char *elemEnd, struct GeneralConfig *cur)
{
  int n1c, n2c;
  struct DataBlockEnumerationIterator *dei = a->newenumiter(a);
  struct StringStack *labels = clStringstackNew();
  struct DataBlock *curdb;
  struct NCDConfig *ncdcfg = (struct NCDConfig *) cur->ptr;
  gsl_matrix *gres;
  while ( ( curdb = a->istar(a, dei) ) ) {
    clStringstackPush(labels, a->ilabel(a, dei));
    a->istep(a, dei);
    clDatablockFreePtr(curdb);
  }
  a->ifree(dei);
  gres = clGetNCDMatrix(a, b, cur);

  if (cur->fSVD) {
    gres = clSvdProject(gres);
  }
  if (cur->fBinary) {
    struct DataBlock *db;
    db = clMakeCLBDistMatrix(gres, labels, cur->cmdKeeper, clGetParametersCB(cur->ca));
    clDatablockWriteToFile(db, ncdcfg->output_distmat_fname);
    clDatablockFreePtr(db);
  }
  for (n1c = 0; n1c < gres->size1; n1c++) {
    printf(rowBegin);
    if (cur->fShowLabels) {
      printf(elemBegin);
      printf("%s", clStringstackReadAt(labels, n1c));
      printf(elemEnd);
    }
    for (n2c = 0; n2c < gres->size2; n2c++) {
      printf(elemBegin);
      printf("%09.9f  ", gsl_matrix_get(gres, n1c, n2c));
      printf(elemEnd);
    }
    printf(rowEnd);
  }
  gsl_matrix_free(gres);
	clStringstackFree(labels);
}

#define PARAMLINESIZE 1024
static struct StringStack *convertParamsToStringStack(struct EnvMap *em, char
    *startparamstr, char *endparamstr)
{
  int i;
  struct StringStack *ss = clStringstackNew();
  char param[PARAMLINESIZE];

  for (i = 0; i < clEnvmapSize(em); i += 1) {
    union PCTypes p;
    p = clEnvmapKeyValAt(em, i);
    if (clEnvmapIsMarkedAt(em, i) && !clEnvmapIsPrivateAt(em, i) ) {
      if (strcmp(p.sp.key,"GoogleKey")==0)
        continue;
      printf("Pushing onto stringstack key <%s> with value <%s>\n", p.sp.key, p.sp.val);
      sprintf(param, "%s%s: %s%s",startparamstr,p.sp.key,p.sp.val,endparamstr);
      clStringstackPush(ss, param);
    }
  }
  sprintf(param,"%sUsername: %s%s", startparamstr, clGetUsername(), endparamstr);
  clStringstackPush(ss,param);
  return ss;
}

struct DataBlock *clConvertTreeToDot(struct TreeAdaptor *ta, double score, struct StringStack *labels, struct CLNodeSet *flips, struct GeneralConfig *cur, struct TreeMaster *tm, gsl_matrix *dm)
{
  int i, j;
  struct LabelPerm *labelperm = clTreeaLabelPerm(ta);
  struct AdjAdaptor *ad = clTreeaAdjAdaptor(ta);
  struct DRA *nodes;
  static char labbuf[128];
  static char lab[1024];
  struct DataBlock *result;
  struct StringStack *dotacc;
  struct StringStack *params = NULL;
  char con1[1024];
  int dasize, tmpsize;
  char *startparamstr, *endparamstr;
  unsigned char *dbuff;

  assert(ad != NULL);
  nodes = clSimpleWalkTree(ta, flips);
  dasize = clAdjaSize(ad);
  assert(dasize > 0);
  assert(clDraSize(nodes != NULL) == dasize);
  dotacc = clStringstackNew();
  assert(dotacc != NULL);
  assert(labelperm != NULL);

  clStringstackPush(dotacc, "graph \"tree\" {");
  if (cur && cur->fSuppressVisibleDetails) {
    startparamstr = "/* "; endparamstr = " */";
  } else {
    clStringstackPush(dotacc, "label=\"\\");
    startparamstr = ""; endparamstr = "\\n\\";
  }

  sprintf(con1, "%s%s version %s%s", startparamstr, PACKAGE_NAME,
      PACKAGE_VERSION, endparamstr);
  clStringstackPush(dotacc, con1);
  if (score != -1.0) {
    sprintf(con1, "%stree score S(T) = %f%s", startparamstr, score,
        endparamstr);
    clStringstackPush(dotacc, con1);
  }
  if (cur && cur->ca) {
    params = convertParamsToStringStack(clGetParametersCB(cur->ca), startparamstr, endparamstr);
    for (i = 0; i < clStringstackSize(params); i += 1)
      clStringstackPush(dotacc, clStringstackReadAt(params,i));
  }

  if (!(cur && cur->fSuppressVisibleDetails))
    clStringstackPush(dotacc, "\";");

  if (cur) {
    for (i = 0; i < clStringstackSize(cur->cmdKeeper); i += 1) {
      char *cmd = clStringstackReadAt(cur->cmdKeeper, i);
      sprintf(con1, "/* Step %d: %s */", i+1, cmd);
      clStringstackPush(dotacc, con1);
    }
  }
  if (tm) {
    int t0, tf, dt;
    if (clTreemasterStartTime(tm)) {
      sprintf(con1, "/* start time %s */", clDatetimeToHumString(clTreemasterStartTime(tm)));
      clStringstackPush(dotacc, con1);
    }
    if (clTreemasterEndTime(tm)) {
      sprintf(con1, "/* end   time %s */", clDatetimeToHumString(clTreemasterEndTime(tm)));
      clStringstackPush(dotacc, con1);
    }
    sprintf(con1, "/* total trees: %d */", clTreemasterTreeCount(tm));
    clStringstackPush(dotacc, con1);
    if (clTreemasterEndTime(tm) && clTreemasterStartTime(tm)) {
      t0 = clDatetimeToInt(clTreemasterStartTime(tm));
      tf = clDatetimeToInt(clTreemasterEndTime(tm));
      dt = tf - t0;
      if (dt > 0) {
        sprintf(con1, "/* trees / sec : %f */", clTreemasterTreeCount(tm) /
            (double) dt);
        clStringstackPush(dotacc, con1);
      }
    }
  }
  sprintf(con1,"/* Hostname: %s */", clGetHostname());
  clStringstackPush(dotacc,con1);
  sprintf(con1,"/* Machine desc: %s */", clGetUTSName());
  clStringstackPush(dotacc,con1);
  sprintf(con1,"/* PID: %d */", clGetPID());
  clStringstackPush(dotacc,con1);

  int rootnode = -1;
  for (i = 0; i < dasize; i += 1) {
    int nodenum = clDraGetValueAt(nodes, i).i;
    assert(nodenum >= 0 && nodenum <= 3 * dasize);
    char *str;
    char *extrastr = "";
    if (clTreeaIsRoot(ta, nodenum)) {
      extrastr=",style=bold";
      rootnode = nodenum;
    }
    if (clTreeaIsQuartettable(ta, nodenum) && labels)
      str = clStringstackReadAt(labels, clLabelpermColIndexForNodeID(labelperm,nodenum));
    else {
      sprintf(labbuf, "%c%d", clTreeaIsQuartettable(ta, nodenum) ? 'L' : 'k', nodenum);
      str = labbuf;
    }
    sprintf(lab,"%d [label=\"%s\"%s];", nodenum, str, extrastr);
    clStringstackPush(dotacc, lab);
  }
  for (i = 0; i < dasize; i += 1) {
    int n1 = clDraGetValueAt(nodes, i).i;
    assert(n1 >= 0 && n1 <= 3 * dasize);
    sprintf(con1, "%d", n1);
    for (j = 0; j < dasize; j += 1) {
      int n2 = clDraGetValueAt(nodes, j).i;
//      printf("For %d, got %d on labelperm %p\n", j, n2, labelperm);
      assert(n2 >= 0 && n2 <= 3 * dasize);
      char con2[1024];
      sprintf(con2, "%d", n2);
      if (n1 > n2 && clAdjaGetConState(ad, n1, n2)) {
        char buf[4096];
        sprintf(buf, "%s -- %s [weight=\"2\"];", con1, con2);
        clStringstackPush(dotacc, buf);
      }
    }
  }
  if (rootnode != -1) {
    sprintf(lab,"%d [label=\"%s\",color=white,fontcolor=black];", 9999, "root");
    clStringstackPush(dotacc, lab);
    sprintf(lab,"%d -- %d;", 9999, rootnode);
    clStringstackPush(dotacc, lab);
  }
  /* Perimeter walker */
  if (flips)
  {
    int i;
    struct DRA *dapairs;
    dapairs = clTreeaPerimPairs(ta, flips);
    for (i = 0; i < clDraSize(dapairs); i += 1) {
      int dmx = clLabelpermColIndexForNodeID(labelperm,clDraGetValueAt(dapairs,i).ip.x);
      int dmy = clLabelpermColIndexForNodeID(labelperm,clDraGetValueAt(dapairs,i).ip.y);
      double disthere = gsl_matrix_get(dm, dmx, dmy);
      sprintf(lab, "i%d [label=\"%03.3f\",color=\"white\"];", i, disthere);
      clStringstackPush(dotacc, lab);
      sprintf(lab, "i%d -- %d [style=\"dotted\"];",  i, clDraGetValueAt(dapairs, i).ip.x);
      clStringstackPush(dotacc, lab);
      sprintf(lab, "i%d -- %d [style=\"dotted\"];",  i, clDraGetValueAt(dapairs, i).ip.y);
      clStringstackPush(dotacc, lab);
    }
    clDraFree(dapairs);
  }
  clStringstackPush(dotacc, "}");
  tmpsize = 0;
  for (i = 0; i < clStringstackSize(dotacc); i += 1)
    tmpsize += strlen(clStringstackReadAt(dotacc, i)) + 1; /* for the \n */
  dbuff = clCalloc(tmpsize+1, 1); /* extra byte for temporary \0 */
  j = 0;
  for (i = 0; i < clStringstackSize(dotacc); i += 1)
    j += sprintf((char *) (dbuff + j), "%s\n", clStringstackReadAt(dotacc, i));
  result = clDatablockNewFromBlock(dbuff,tmpsize);
  clFree(dbuff);
  clStringstackFree(dotacc);
  if (cur && params)
    clStringstackFree(params);
  clDraFree(nodes);
  clLabelpermFree(labelperm);
  labelperm = NULL;
  return result;
}

const char *clGetUsername(void)
{
  static char username[128];
#if PWD_RDY
  struct passwd *p;
  uid_t whoami;
  whoami = getuid();
  p = getpwuid(whoami);
  strcpy(username, p->pw_name);
#else
  strcpy(username, "unknown");
#endif
  return username;
}

int clGetPID(void)
{
  return getpid();
}

const char *clGetHostname(void)
{
  static char hostname[1024];
  gethostname(hostname, 1024);
  return hostname;
}

const char *clGetUTSName(void)
{
  static char utsname[1024];
#if UTS_RDY
  struct utsname uts;
  uname(&uts);
  sprintf(utsname, "[%s (%s), %s]", uts.sysname, uts.release, uts.machine);
#else
  strcpy(utsname, "[unknown]");
#endif
  return utsname;
}

