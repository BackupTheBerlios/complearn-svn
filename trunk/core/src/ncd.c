#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <glib.h>
#include <gmodule.h>
#include <libintl.h>
#include "complearn-external-compressor.h"
#include "complearn-environment.h"
#include "complearn/complearn.h"
#include "complearn/clconfig.h"

#define _(O) gettext(O)

int main(int argc, char **argv)
{
  int i;
  char t[1] = { 0 };
  setlocale(LC_ALL, t);
  bindtextdomain (PACKAGE, "/usr/share/locale");
  textdomain (PACKAGE);
  g_type_init();
  char **names;
  CompLearnEnvironment *cle;
  complearn_environment_handle_log_domain(G_LOG_DOMAIN);
  cle = complearn_environment_top();
  CompLearnNcd *cncd = COMPLEARN_NCD(complearn_ncd_top());
  complearn_ncd_process_options((char **) argv);
  exit(0);
}
