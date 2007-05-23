#include <stdio.h>
#include <complearn/complearn.h>

int main(int argc, char **argv)
{
  CompLearnEnvironment *cle = NULL;
  g_type_init();
  cle = complearn_environment_top();
  printf("%p\n", cle);
  return 0;
}

