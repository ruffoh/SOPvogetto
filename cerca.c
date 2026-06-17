#include "parafilter.h"

void cerca(char *keyword, char *nomefile, int pipe_write) {
  FILE *fp = fopen(nomefile, "r");
  if (fp == NULL) {
    fprintf(stderr, "[%s]: %s\n", nomefile, strerror(errno));
    return;
  }
  char *linea = NULL;
  size_t lung = 0;

  while (getline(&linea, &lung, fp) != -1) {
    // Controlla se la keyword è presente nella linea
    if (strstr(linea, keyword) != NULL) {
      dprintf(pipe_write, "[%s]: %s", basename(nomefile), linea);
    }
  }
  // Libera la memoria allocata per la linea e chiude il file
  free(linea);
  fclose(fp);
}
