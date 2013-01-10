/*
 * wlktr.h
 * Nathan Forbes
 */

#ifndef WLKTR_H_INCLUDED
#define WLKTR_H_INCLUDED

#define WLKTR_SUCCESS      0
#define WLKTR_ERR_OPENDIR  1
#define WLKTR_ERR_READDIR  2
#define WLKTR_ERR_LSTAT    3
#define WLKTR_ERR_CLOSEDIR 4


typedef enum {
  WLKTR_OTYPE_NIL = 0,
  WLKTR_OTYPE_REG,
  WLKTR_OTYPE_DIR,
  WLKTR_OTYPE_CHR,
  WLKTR_OTYPE_BLK,
  WLKTR_OTYPE_FIFO,
  WLKTR_OTYPE_LNK,
  WLKTR_OTYPE_SOCK
} wlktr_otype_t;

typedef void (*wlktr_cllbck_t) (const char *);

typedef struct {
  wlktr_otype_t wt_otype;
  wlktr_cllbck_t wt_fun;
} wlktr_action_t;

char *wlktr_error (void);
int wlktr (const char *root, wlktr_action_t *act);

#endif /* WLKTR_H_INCLUDED */

