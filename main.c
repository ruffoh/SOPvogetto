#include "parafilter.h"

int main(int argc, char *argv[]) {
  char *keyword;
  char *logfile = NULL;
  int primo_file;
  // Sez-1 : Parsing argomenti
  if (argc < 3) {
    fprintf(stderr, "Uso: %s [-e error.log] <keyword> <file1> ... <fileN>\n",
            argv[0]);
    return 1;
  }
  // controlla se è presente l'opzione
  if (strcmp(argv[1], "-e") == 0) {
    if (argc < 5) {
      fprintf(stderr, "Uso: %s [-e error.log] <keyword> <file1> ... <fileN>\n",
              argv[0]);
      return 1;
    }
    logfile = argv[2];
    keyword = argv[3];
    primo_file = 4;
  } else {

    keyword = argv[1];
    primo_file = 2;
  }

  /* Sez-2 :Configurazione pipe e  I/O*/

  int n_file = argc - primo_file;
  if (n_file <= 0) {
    fprintf(stderr, "Errore: nessun file specificato.\n");
    return 1;
  }
  int pipes[n_file][2]; // Array di pipe per ogni file

  if (logfile !=
      NULL) // Se è specificato un logfile, reindirizza lo stderr su quel file
  {
    int fd_err = open(logfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_err == -1) {
      perror("Errore apertura logfile");
      exit(EXIT_FAILURE);
    }
    dup2(fd_err, STDERR_FILENO); // Dirotta lo stderr sul file
    close(fd_err);
  }

  // PIPE:Creazione delle pipe per ogni figlio
  for (int i = 0; i < n_file; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("Errore creazione pipe");
      exit(EXIT_FAILURE);
    }
  }
  /* --- SEZIONE 3: Creazione dei processi paralleli --- */
  for (int i = primo_file; i < argc; i++) {
    int f_indice = i - primo_file;

    pid_t pid = fork();

    if (pid < 0) {
      fprintf(stderr, "Errore fork\n");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      // figlio: eseguiamo la ricerca separatamente per ogni file
      for (int j = 0; j < n_file; j++) {
        // chiudo la lettura ai figli 
        close(pipes[j][0]);
      }
      for (int j = 0; j < n_file; j++) {
        // chiudo la scrittura ai figli tranne quella del figlio corrente
        if (f_indice != j)
          close(pipes[j][1]);
      }
      cerca(keyword, argv[i], pipes[f_indice][1]);
      exit(EXIT_SUCCESS);
    }
  }
  /* --- SEZIONE 4: Raccolta dati e sincronizzazione nel Padre --- */

  // --PADRE--
  // chiudo la scrittura delle pipe nel padre(necessario per evitare deadlock)
  for (int i = 0; i < n_file; i++) {
    close(pipes[i][1]);
  }
  // legge i risultati inviati dai figli tramite le pipe
  char buffer[8192]; // 8KB di buffer per la lettura
  for (int i = 0; i < n_file; i++) {
    ssize_t n;
    while ((n = read(pipes[i][0], buffer, sizeof(buffer) - 1)) > 0) {
      buffer[n] = '\0';                // aggiunge il terminatore di stringa
      write(STDOUT_FILENO, buffer, n); // scrive sullo standard output
    }
    close(pipes[i]
               [0]); // chiude la lettura della pipe dopo aver finito di leggere
  }

  // padre aspetta la terminazione dei figli per pulire la tabella dei processi
  for (int i = primo_file; i < argc; i++) {
    wait(NULL);
  }

  return 0;
}
