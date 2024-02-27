#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
primes (int p[2])
{
  int p0[2], prime, num;

  close (p[1]);
  pipe (p0);
  if (read (p[0], &prime, 4) != 4)
    {
      fprintf (2, "Failed to read from pipe. \n");
      exit (1);
    }
  printf ("prime %d\n", prime);

  if (read (p[0], &num, 4))
    {

      if (fork () == 0)
        {
          primes (p0);
        }
      else
        {
          close (p0[0]);
          do
            {
              if (num % prime)
                write (p0[1], &num, 4);
            }
          while (read (p[0], &num, 4));
          close (p0[1]);
          close (p[0]);
          wait (0);
        }
    }
  else
    {
      close (p[0]);
    }
}

int
main ()
{
  int p[2];

  pipe (p);
  if (fork () == 0)
    {
      primes (p);
    }
  else
    {
      int i;
      close (p[0]);
      for (i = 2; i <= 35; i++)
        {
          if (write (p[1], &i, 4) != 4)
            {
              fprintf (2, "Failed to write to pipe. \n");
              exit (1);
            }
        }
      close (p[1]);
      wait (0);
    }

  exit (0);
}