/*
 * example.c
 *
 * An example of how to use wlktr, that simply walks a directory
 * and prints the name, type, and size (if a regular file), of files it finds.
 *
 * Compile with:
 *     gcc example.c wlktr.c -o example
 * Usage:
 *     ./example ~/some/path
 *
 * Nathan Forbes
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "wlktr.h"

static void
reg_action (const char *p)
{
  struct stat s;

  printf ("file: %s", p);
  memset (&s, 0, sizeof (struct stat));
  if (!stat (p, &s))
    printf (" (%lu bytes)", (unsigned long) s.st_size);
  fputc ('\n', stdout);
}

static void
dir_action (const char *p)
{
  printf ("directory: %s\n", p);
}

static void
chr_action (const char *p)
{
  printf ("character device: %s\n", p);
}

static void
blk_action (const char *p)
{
  printf ("block device: %s\n", p);
}

static void
fifo_action (const char *p)
{
  printf ("fifo: %s\n", p);
}

static void
lnk_action (const char *p)
{
  printf ("symlink: %s\n", p);
}

static void
sock_action (const char *p)
{
  printf ("socket: %s\n", p);
}

static void
any_action (const char *p)
{
  printf ("object: %s\n", p);
}

int
main (int argc, char **argv)
{
  if (argc == 1) {
    fprintf (stderr, "usage: %s PATH...\n", argv[0]);
    return 1;
  }

  wlktr_action_t action[] = {
    {WLKTR_OTYPE_REG, &reg_action},
    {WLKTR_OTYPE_DIR, &dir_action},
    {WLKTR_OTYPE_CHR, &chr_action},
    {WLKTR_OTYPE_BLK, &blk_action},
    {WLKTR_OTYPE_FIFO, &fifo_action},
    {WLKTR_OTYPE_LNK, &lnk_action},
    {WLKTR_OTYPE_SOCK, &sock_action},
    {0, NULL}
  };
  int i;

  for (i = 1; argv[i]; ++i)
    if (wlktr (argv[i], action) != 0)
      fprintf (stderr, "%s: %s\n", argv[0], wlktr_error ());
  return 0;
}

