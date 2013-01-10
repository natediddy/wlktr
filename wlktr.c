/*
 * wlktr.c
 * Nathan Forbes
 */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "wlktr.h"

#define ERRBUFMAX        256

/* use pascal style strings for speed */
#define SUCCESS_MSG      "\07Success"
#define ERR_OPENDIR_MSG  "\032Error calling opendir(3): "
#define ERR_READDIR_MSG  "\032Error calling readdir(3): "
#define ERR_LSTAT_MSG    "\030Error calling lstat(2): "
#define ERR_CLOSEDIR_MSG "\033Error calling closedir(3): "
#define ERR_NA_MSG       "\03n/a"

int errno_save = 0;
int status = WLKTR_SUCCESS;

char *
wlktr_error (void)
{
  static char buf[ERRBUFMAX];
  char *s;
  size_t n;

  if (status == WLKTR_SUCCESS) {
    strncpy (buf, SUCCESS_MSG + 1, *SUCCESS_MSG);
    buf[*SUCCESS_MSG] = '\0';
  } else if (status == WLKTR_ERR_OPENDIR) {
    s = strerror (errno_save);
    n = strlen (s);
    strncpy (buf, ERR_OPENDIR_MSG + 1, *ERR_OPENDIR_MSG);
    strncpy (buf + *ERR_OPENDIR_MSG, s, n);
    buf[*ERR_OPENDIR_MSG + n] = '\0';
  } else if (status == WLKTR_ERR_READDIR) {
    s = strerror (errno_save);
    n = strlen (s);
    strncpy (buf, ERR_READDIR_MSG + 1, *ERR_READDIR_MSG);
    strncpy (buf + *ERR_READDIR_MSG, s, n);
    buf[*ERR_READDIR_MSG + n] = '\0';
  } else if (status == WLKTR_ERR_LSTAT) {
    s = strerror (errno_save);
    n = strlen (s);
    strncpy (buf, ERR_LSTAT_MSG + 1, *ERR_LSTAT_MSG);
    strncpy (buf + *ERR_LSTAT_MSG, s, n);
    buf[*ERR_LSTAT_MSG + n] = '\0';
  } else if (status == WLKTR_ERR_CLOSEDIR) {
    s = strerror (errno_save);
    n = strlen (s);
    strncpy (buf, ERR_CLOSEDIR_MSG + 1, *ERR_CLOSEDIR_MSG);
    strncpy (buf + *ERR_CLOSEDIR_MSG, s, n);
    buf[*ERR_CLOSEDIR_MSG + n] = '\0';
  } else {
    strncpy (buf, ERR_NA_MSG + 1, *ERR_NA_MSG);
    buf[*ERR_NA_MSG] = '\0';
  }
  return buf;
}

int
wlktr (const char *root, wlktr_action_t *act)
{
  int ret;
  int last;
  DIR *d;
  struct dirent *e;
  struct stat st;
  size_t i;
  size_t n;
  size_t nroot;
  size_t ndn;

  ret = 0;
  d = opendir (root);

  if (!d) {
    errno_save = errno;
    status = WLKTR_ERR_OPENDIR;
    return WLKTR_ERR_OPENDIR;
  }

  for (;;) {
    e = readdir (d);
    if (!e) {
      if (errno != 0) {
        errno_save = errno;
        closedir (d);
        status = WLKTR_ERR_READDIR;
        return WLKTR_ERR_READDIR;
      }
      break;
    }
    if (!strcmp (e->d_name, ".") || !strcmp (e->d_name, ".."))
      continue;
    nroot = strlen (root);
    ndn = strlen (e->d_name);
    n = nroot + ndn + 1;
    char buf[n + 1];
    strncpy (buf, root, nroot);
    buf[nroot] = '/';
    strncpy (buf + (nroot + 1), e->d_name, ndn);
    buf[n] = '\0';
    memset (&st, 0, sizeof (struct stat));
    if (lstat (buf, &st) != 0) {
      errno_save = errno;
      closedir (d);
      status = WLKTR_ERR_LSTAT;
      return WLKTR_ERR_LSTAT;
    }
    i = 0;
    do {
      last = 0;
      switch (act[i].wt_otype) {
      case WLKTR_OTYPE_NIL:
        if (!act[i].wt_fun)
          last = 1;
        else
          act[i].wt_fun (buf);
        break;
      case WLKTR_OTYPE_REG:
        if (S_ISREG (st.st_mode))
          act[i].wt_fun (buf);
        break;
      case WLKTR_OTYPE_DIR:
        if (S_ISDIR (st.st_mode)) {
          act[i].wt_fun (buf);
          ret = wlktr (buf, act);
        }
        break;
      case WLKTR_OTYPE_CHR:
        if (S_ISCHR (st.st_mode))
          act[i].wt_fun (buf);
        break;
      case WLKTR_OTYPE_BLK:
        if (S_ISBLK (st.st_mode))
          act[i].wt_fun (buf);
        break;
      case WLKTR_OTYPE_FIFO:
        if (S_ISFIFO (st.st_mode))
          act[i].wt_fun (buf);
        break;
      case WLKTR_OTYPE_LNK:
        if (S_ISLNK (st.st_mode))
          act[i].wt_fun (buf);
        break;
      case WLKTR_OTYPE_SOCK:
        if (S_ISSOCK (st.st_mode))
          act[i].wt_fun (buf);
        break;
      default:
        /* not reached */;
      }
      if (last)
        break;
      ++i;
    } while (1);
  }

  if (closedir (d) != 0) {
    errno_save = errno;
    status = WLKTR_ERR_CLOSEDIR;
    return WLKTR_ERR_CLOSEDIR;
  }
  return WLKTR_SUCCESS;
}

