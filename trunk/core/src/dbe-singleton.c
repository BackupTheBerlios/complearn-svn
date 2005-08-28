#include <assert.h>
#include <stdio.h>
#include <malloc.h>

#include <complearn/complearn.h>

struct DataBlockEnumeration *dbeLoadSingleton(struct DataBlock *db)
{
  assert(db);
  return dbeLoadArray(&db, 1);
}
