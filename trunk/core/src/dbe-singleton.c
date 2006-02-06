#include <assert.h>
#include <stdio.h>


#include <complearn/complearn.h>

struct DataBlockEnumeration *clDbeLoadSingleton(struct DataBlock *db)
{
  assert(db);
  return clDbeLoadArray(&db, 1);
}
