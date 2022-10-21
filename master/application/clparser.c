#include <cargs.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../supplr-server/include/http.h"

static struct cag_option options[] = {
  {.identifier = 'c',
    .access_letters = "c",
    .access_name = "config-file",
    .value_name = "path/to/config.yaml",
    .description = "Path to configuration file"},
  {.identifier = 'h',
    .access_letters = "h",
    .access_name = "help",
    .description = "Shows the command help"}
};

struct configuration config = {NULL};

struct configuration *clparser(int argc, char** argv)
{
  if (argc == 1){
    printf("Usage: supplr-server [OPTION]...\n");
    cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
    exit(0);
  }

  char identifier;
  const char *value;
  cag_option_context context;

  cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&context)) {
    identifier = cag_option_get(&context);
    switch (identifier)
    {
      case 'c':
        value = cag_option_get_value(&context);
        config.path = value;
      break;
      case 'h':
        printf("Usage: supplr-server [OPTION]...\n");
        cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
        exit(0);
      default:
        printf("Usage: supplr-server [OPTION]...\n");
        cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
        exit(0);
      break;
    }
  }
  return &config;
}
