#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main ()
{
  int pid, p[2];

  pipe (p);

  if (fork () == 0)
    {
      char buf[2];
      pid = getpid ();

      if (read (p[0], buf, 1) != 1)
        {
          fprintf (2, "child fails to read\n");
          exit (1);
        }
      fprintf (1, "%d: received ping\n", pid);
      close (p[0]);

      if (write (p[1], buf, 1) != 1)
        {
          fprintf (2, "child fails to write\n");
          exit (1);
        }
      close (p[1]);
    }
  else
    {
      char buf[2];
      pid = getpid ();

      if (write (p[1], buf, 1) != 1)
        {
          fprintf (2, "parent fails to write\n");
          exit (1);
        }
      close (p[1]);

      if (read (p[0], buf, 1) != 1)
        {
          fprintf (2, "parent fails to read\n");
          exit (1);
        }
      fprintf (1, "%d: received pong\n", pid);
      close (p[0]);
    }

  exit (0);
}