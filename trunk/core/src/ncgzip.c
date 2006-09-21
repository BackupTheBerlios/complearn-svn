#include "newcomp.h"
struct GZipCompressionInstance {
  void *baseClass;
  int compressionLevel;
};

static int specificInitCB(struct CompressionBase *cb)
{
  struct GZipCompressionInstance *gzci = (struct GZipCompressionInstance *) cb;
  printf("gzip specific init func!\n");
  gzci->compressionLevel = 9;
}

static struct CompressionBaseAdaptor cba = {
  NULL,
  //specificInitCB,
};

static void initGZ(void)
{
  clInitCB("gzip", "GZip Lempel-Ziv 77",
                   sizeof(struct GZipCompressionInstance), &cba);
}

int main(int argc, char **argv) {
  initGZ();
  return 0;
}
