#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

void
error_y_exit (char *str)
{
  perror (str);
  exit (EXIT_FAILURE);
}

void
Usage ()
{
  char buffer[256];
  sprintf (buffer, "USAGE: userprocs username\n");
  write (1, buffer, strlen (buffer));
  exit (EXIT_FAILURE);
}

int
main (int argc, char *argv[])
{

  int ps2grep[2], grep2wc[2];
  int read_wc2parent, write_wc2parent;
  int ret;
  char wcbuffer[256];
  char buffer[500];

  if (argc != 2)
    Usage ();

  pipe (ps2grep);
  pipe (grep2wc);

  ret = mknod ("wc2parent.fifo", S_IFIFO | 0666, 0);	// Creo la NAMED-PIPE, control de EEXIST
  if ((ret == -1) && (errno != EEXIST))
    error_y_exit ("Creando la named pipe");

  switch (fork ())		//fork para mutar a ps
    {
    case -1:
      error_y_exit ("Fork para ps");
      break;
    case 0:
      dup2 (ps2grep[1], 1);
      close (ps2grep[1]);	//redirecció Salida Estandar a pipe. Cierre canal duplicado.

      close (ps2grep[0]);	//cierro canales de pipe que no uso
      close (grep2wc[0]);	//evitando bloqueos
      close (grep2wc[1]);

//kill(getpid(),SIGSTOP);

      execlp ("ps", "ps", "-eaf", (char *) NULL);
      error_y_exit ("Mutación a ps");
      break;

    }

  switch (fork ())		//fork para mutar a grep
    {
    case -1:
      error_y_exit ("Fork para grep");
      break;
    case 0:
      dup2 (ps2grep[0], 0);
      close (ps2grep[0]);	//redirecció entrada Estandar a pipe. Cierre canal duplicado.

      dup2 (grep2wc[1], 1);
      close (grep2wc[1]);	//redirecció Salida Estandar a pipe. Cierre canal duplicado.

      close (ps2grep[1]);	//cierro canales de pipe que no uso
      close (grep2wc[0]);	//evitando bloqueos

//kill(getpid(),SIGSTOP);

      execlp ("grep", "grep", argv[1], (char *) NULL);
      error_y_exit ("Mutación a grep");
      break;

    }

  switch (fork ())		//fork para mutar a wc
    {
    case -1:
      error_y_exit ("Fork para wc");
      break;
    case 0:
      dup2 (grep2wc[0], 0);
      close (grep2wc[0]);	//redirecció entrada Estandar a pipe. Cierre canal duplicado.

      write_wc2parent = open ("wc2parent.fifo", O_WRONLY, 0);
      dup2 (write_wc2parent, 1);
      close (write_wc2parent);	//redirecció Salida Estandar a named pipe. Cierre canal duplicado. Bloqueo de open

      close (ps2grep[0]);
      close (ps2grep[1]);	//cierro canales de pipe que no uso
      close (grep2wc[1]);	//evitando bloqueos

//kill(getpid(),SIGSTOP);

      execlp ("wc", "wc", "-l", (char *) NULL);
      error_y_exit ("Mutación a wc");
      break;

    }

///PADRE
//Cierro los canales de pipe que no uso
  close (grep2wc[0]);
  close (grep2wc[1]);
  close (ps2grep[0]);
  close (ps2grep[1]);

  read_wc2parent = open ("wc2parent.fifo", O_RDONLY, 0);	//abre la named-pipe. Hay bloqueo

//kill(getpid(),SIGSTOP);
  int valor;
  while ((ret = read (read_wc2parent, wcbuffer, 256)) > 0)	//la lectura se bloquea hasta que wc acaba
    {
      valor = atoi (wcbuffer);	//nos guardamos el valor para escribirlo en el fichero
      wcbuffer[ret - 1] = '\0';
      sprintf (buffer,
	       "El usuario %s está ejecutando %s procesos\n", argv[1],
	       wcbuffer);
      write (1, buffer, strlen (buffer));
    }

  close (read_wc2parent);
  while (waitpid (-1, NULL, 0) > 0);

//apertura/creación del fichero
  char nfich[256], pregunta[1000], respuesta[80];

  sprintf (nfich, "%s.dat", argv[1]);	//composicion del nombre del fichero
  int fichero = open (nfich, O_WRONLY | O_CREAT | O_EXCL, 0644);  // pregunta sobre existencia
  if (fichero == -1)
    {
      if (errno != EEXIST)
	         error_y_exit ("crear fichero");   // eror open, diferente de existencia
      else
	    {   // el fichero ya existe
	        ret = sprintf (pregunta,
		     "\n!!!! El fichero ya existe: Pulsa S + return para SOBREESCRIBIRLO, \ncualquier otra cosa + return para AÑADIR los datos al final:  ");
	        write (1, pregunta, ret);
	        ret = read (0, respuesta, 80);

	  if ((strncmp (respuesta, "S\n",ret)) == 0)

	    fichero = open (nfich, O_WRONLY | O_CREAT | O_TRUNC, 0644);   // sobreescritura del fichero existente

	  else
	    fichero = open (nfich, O_WRONLY);  // apertura normal del fichero, si ya existe - añadir datos al final

	}
    }
  lseek (fichero, 0, SEEK_END);   // tanto si es fichero nuevo, sobreescrito o para añadir al final, siempre se escribe al final
  write (fichero, &valor, sizeof (int));
  close (fichero);
}
