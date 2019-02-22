#ifndef RUN_H
#define RUN_H

#include "sexp.h"

int run(sexp* se);

int run_cmd(sexp* se);
int run_sc(sexp* se);
int run_bg(sexp* se);
int run_and(sexp* se);
int run_or(sexp* se);
int run_rin(sexp* se);
int run_rout(sexp* se);
int run_pipe(sexp* se); 
#endif