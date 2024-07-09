#include <unistd.h>
#include <stdio.h>

#define MAX_SIZE 8

unsigned int
char2int (char c)
{
  return c - '0';
}

int mi_atoi (char *s)
{
  int value = 0;
  int sign = 1;

  while (*s == '\n' || *s == ' ' || *s == '\t' || *s != '\r' || *s == '\v'
	 || *s == '\f')
    {
      ++s;
    }
  if (*s == '+' || *s == '-')
    {
      if (*s == '-')
	sign = 0;
      s++;
    }
  while ((*s) >= '0' && (*s) <= '9')
    {
      value *= 10;
      value = (sign ? value + (char2int (*s)) : value - (char2int (*s)));
      s++;
    }
  return (value);
}

int
esNumero (char *str)
{
  if (str)
    {
      int i = 0;
      if (str[i] == '-') ++str;
      for (; str[i] != '\0'; ++i)
	{
	  if (str[i] < '0' || str[i] > '9')
	    return 0;
	}
      return (i > 0) && (i <= MAX_SIZE);
    }
  return 0;
}

int
main (int argc, char *argv[])
{
  int t_suma = 0;
  int suma = 0;
  for (int i = 1; i < argc; ++i) {
    if (esNumero (argv[i])) {
	    if (t_suma == 0) suma += mi_atoi(argv[i]);
	  }
    else if (t_suma == 0) t_suma = i;
  }
  if (t_suma == 0) printf("La suma es %i\n", suma);
  else printf ("Error: el parámetro \"%s\" no es un número\n", argv[t_suma]);
}
