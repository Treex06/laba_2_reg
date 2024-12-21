#include <stdio.h>
#include <stdlib.h>
#include "../include/sed_operations.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <command>\n", argv[0]);
    return 1;
  }

  const char *filename = argv[1];
  const char *command_str = argv[2];

    SedCommand command = parse_command(command_str);

   if(command.type == -1){
        fprintf(stderr, "Invalid command format: %s\n", command_str);
        return 1;
    }
   int result = process_file(filename, command);

    if (command.regex){
       free(command.regex);
    }
     if(command.replacement){
       free(command.replacement);
    }


  return result;
}
