#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#include <complearn/complearn.h>

struct DataBlockEnumeration *loadSingletonDBE(struct DataBlock *db)
{
  assert(db);
  return loadArrayDBE(db, 1);
}
