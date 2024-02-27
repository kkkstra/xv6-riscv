#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *
get_filename (char *path)
{
  static char buf[DIRSIZ + 1];
  char *p;

  // Find first character after last slash.
  for (p = path + strlen (path); p >= path && *p != '/'; p--)
    ;
  p++;

  if (strlen (p) >= DIRSIZ)
    return p;
  memmove (buf, p, strlen (p));
  buf[strlen (p)] = '\0';
  return buf;
}

void
find (char *path, const char *file)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open (path, O_RDONLY)) < 0)
    {
      fprintf (2, "find: cannot open %s\n", path);
      exit (1);
    }

  if (fstat (fd, &st) < 0)
    {
      fprintf (2, "find: cannot stat %s\n", path);
      close (fd);
      exit (1);
    }

  switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
      char *name = get_filename (path);
      if (strcmp (name, file) == 0)
        {
          printf ("%s\n", path);
        }
      break;

    case T_DIR:
      if (strlen (path) + 1 + DIRSIZ + 1 > sizeof (buf))
        {
          printf ("find: path too long\n");
          break;
        }
      strcpy (buf, path);
      p = buf + strlen (buf);
      *p++ = '/';

      while (read (fd, &de, sizeof (de)) == sizeof (de))
        {
          // Don't recurse into "." and ".."!
          if (de.inum == 0 || strcmp (de.name, ".") == 0
              || strcmp (de.name, "..") == 0)
            continue;
          memmove (p, de.name, DIRSIZ);
          p[DIRSIZ] = 0;
          if (stat (buf, &st) < 0)
            {
              printf ("find: cannot stat %s\n", buf);
              continue;
            }
          find (buf, file);
        }
      break;
    }
  close (fd);
}

int
main (int argc, char *argv[])
{
  if (argc != 3)
    {
      fprintf (2, "Usage: find <dir> <file>\n");
      exit (1);
    }

  find (argv[1], argv[2]);
  exit (0);
}