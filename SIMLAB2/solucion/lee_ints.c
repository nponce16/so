#include <unistd.h>
#include <stdio.h>
void
main ()
{
  int leido, ret, spr;
  char buff[64];
  while ((ret = read (0, &leido, sizeof (int))) > 0)
    {
      spr = sprintf (buff, "%d\n", leido);
      write (1, buff, spr);
    }
}
