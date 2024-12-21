#include "../include/sed_operations.h"


SedCommand parse_command(const char *command_str) {
    SedCommand command;
    command.type = -1; // Invalid value
    command.regex = NULL;
    command.replacement = NULL;

    if (strncmp(command_str, "s/", 2) == 0) {
        command.type = OP_SUBSTITUTE;
        const char* start = command_str + 2;
        const char* delim1 = strchr(start, '/');
        if (!delim1) return command;

        const char* delim2 = strchr(delim1 + 1, '/');
        if (!delim2) return command;

        size_t regexLen = delim1 - start;
        size_t replacementLen = delim2 - delim1 - 1;

        command.regex = (char*) malloc(regexLen + 1);
        strncpy(command.regex, start, regexLen);
        command.regex[regexLen] = '\0';

        command.replacement = (char*) malloc(replacementLen + 1);
        strncpy(command.replacement, delim1 + 1, replacementLen);
        command.replacement[replacementLen] = '\0';


    } else if(strncmp(command_str, "/", 1) == 0 && strchr(command_str, '/') != command_str + strlen(command_str)-1){
         command.type = OP_DELETE;
         const char* start = command_str + 1;
        const char* end = strchr(start, '/');
        if (!end) return command;

         size_t regexLen = end - start;
        command.regex = (char*) malloc(regexLen + 1);
        strncpy(command.regex, start, regexLen);
         command.regex[regexLen] = '\0';
    } else if (strncmp(command_str, "s/^/", 4) == 0) {
          command.type = OP_PREFIX;

          const char* start = command_str + 4;
          if (*start != '\0'){
              command.replacement = strdup(start);
          }
    } else if (strncmp(command_str, "s/$/", 4) == 0){
          command.type = OP_SUFFIX;
          const char* start = command_str + 4;
           if (*start != '\0'){
              command.replacement = strdup(start);
          }
    }
    return command;
}

int process_file(const char *filename, const SedCommand command) {
  FILE *file = fopen(filename, "r+");
  if (!file) {
    perror("Error opening file");
    return 1;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  long filePos = 0;

  FILE *tempFile = tmpfile();
  if (!tempFile) {
    fclose(file);
     perror("Error creating temporary file");
    return 1;
  }

    regex_t regex;
    int regcomp_result = 0;
    if(command.regex){
        regcomp_result = regcomp(&regex, command.regex, REG_EXTENDED);
    }

  while ((read = getline(&line, &len, file)) != -1) {
    bool skipLine = false;
    if (command.type == OP_SUBSTITUTE){
        if (command.regex && command.replacement){
             regmatch_t match;
             int regexec_result = regexec(&regex, line, 1, &match, 0);
              if (regexec_result == 0) {
                size_t newLineSize = strlen(line) - (match.rm_eo - match.rm_so) + strlen(command.replacement) + 1;
                char* newLine = (char*)malloc(newLineSize);
                if (!newLine)
                {
                    free(line);
                    fclose(tempFile);
                    fclose(file);
                    regfree(&regex);
                    return 1;
                }
                strncpy(newLine, line, match.rm_so);
                newLine[match.rm_so] = '\0';
                strcat(newLine, command.replacement);
                strcat(newLine, line + match.rm_eo);
               
                 fprintf(tempFile, "%s", newLine);
                free(newLine);
              } else {
                  fprintf(tempFile, "%s", line);
              }
        } else {
            fprintf(tempFile, "%s", line);
        }
    } else if (command.type == OP_DELETE){
         if(command.regex){
             int regexec_result = regexec(&regex, line, 0, NULL, 0);
             if(regexec_result == 0){
                skipLine = true;
             }
         }
        if (!skipLine){
           fprintf(tempFile, "%s", line);
       }
    } else if (command.type == OP_PREFIX){
        if (command.replacement){
            fprintf(tempFile, "%s%s", command.replacement, line);
        } else {
            fprintf(tempFile, "%s", line);
        }

    } else if (command.type == OP_SUFFIX){
         size_t lineLen = strlen(line);
        if (command.replacement && lineLen > 1) {
             line[lineLen - 1] = '\0';
            fprintf(tempFile, "%s%s\n", line, command.replacement);
        }
          else {
            fprintf(tempFile, "%s", line);
        }

    }

  }

    if(command.regex){
          regfree(&regex);
    }
    
  free(line);

    fseek(tempFile, 0, SEEK_SET);
    fseek(file, 0, SEEK_SET);
    ftruncate(fileno(file), 0);
    char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), tempFile)) > 0) {
        fwrite(buffer, 1, bytes_read, file);
    }



  fclose(tempFile);
  fclose(file);
  return 0;
}
