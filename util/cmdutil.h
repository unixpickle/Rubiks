#include <string.h>
#include "pltools.h"

RubiksMap * parse_premoves_argument(const char * argument);

int parse_moveset_argument(PlMoveset * moveset, const char * argument);
int parse_operations_argument(const char * argument, RubiksMap *** maps, char *** descriptions);
