#ifndef SED_OPERATIONS_H
#define SED_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

typedef enum {
  OP_SUBSTITUTE,
  OP_DELETE,
  OP_PREFIX,
  OP_SUFFIX
} OperationType;

typedef struct {
    OperationType type;
    char *regex;
    char *replacement;
} SedCommand;

SedCommand parse_command(const char *command_str);
int process_file(const char *filename, const SedCommand command);

#endif
