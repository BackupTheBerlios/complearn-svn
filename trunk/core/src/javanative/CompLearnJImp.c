#include <jni.h>
#include <complearn/complearn.h>
#include "CompLearnJ.h"
#include <stdio.h>

struct GeneralConfig *gc;
JNIEXPORT jdouble JNICALL
Java_CompLearnJ_ncd(JNIEnv *env, jobject obj, jstring a, jstring b)
{
  double result;
  const char *stra = (*env)->GetStringUTFChars(env, a, 0);
  const char *strb = (*env)->GetStringUTFChars(env, b, 0);
  struct DataBlock dba, dbb;
  if (!gc) {
    gc = loadDefaultEnvironment();
    loadCompressor(gc);
    assert(gc->ca);
  }
  dba = convertStringToDataBlock(stra);
  dbb = convertStringToDataBlock(strb);
  result = ncdPair(gc->ca, dba, dbb);
  freeDataBlock(dba);
  freeDataBlock(dbb);
  (*env)->ReleaseStringUTFChars(env, a, stra);
  (*env)->ReleaseStringUTFChars(env, b, strb);
  return result;
}
