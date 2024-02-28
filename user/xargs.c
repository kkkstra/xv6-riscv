#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int
getline (char *argv[])
{
  int argc, offset;
  char buf[512];

  argc = 0, offset = 0;
  while (read (0, buf + offset, 1))
    {
      if (offset == 511)
        {
          fprintf (2, "xargs: argument too long\n");
          exit (1);
        }
      char c = buf[offset];
      if (c == ' ' || c == '\n')
        {
          if (offset > 0 && buf[offset - 1] != ' ')
            {
              buf[offset] = '\0';
              argv[argc] = (char *)malloc (strlen (buf));
              strcpy (argv[argc], buf);
              argc++;
              offset = -1;
            }
        }
      if (c == '\n')
        {
          return argc;
        }
      offset++;
    }
  return argc;
}

int
main (int argc, char *argv[])
{
  int i, new_argc;
  char *new_argv[MAXARG];

  for (i = 1; i < argc; i++)
    {
      new_argv[i - 1] = (char *)malloc (strlen (argv[i]) + 1);
      strcpy (new_argv[i - 1], argv[i]);
    }

  while ((new_argc = getline (new_argv + argc - 1)) > 0)
    {
      if (fork () == 0)
        {
          char path[512];
          strcpy (path, new_argv[0]);
          exec (path, new_argv);
        }
      else
        {
          wait (0);
          for (i = argc - 1; i < argc + new_argc - 1; i++)
            {
              free (new_argv[i]);
              new_argv[i] = 0;
            }
        }
    }
  exit (0);
}