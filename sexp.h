#ifndef SEXP_H
#define SEXP_H

#include "vec.h"

#define KIND_TEXT 0
#define KIND_CMD 1
#define KIND_UNOP 2
#define KIND_BINOP 3

typedef struct sexp {
	int kind;
	
	char* text;

	char* cmd;
	struct vec* args;
	
	char* op;
	struct sexp* cmd1;
	struct sexp* cmd2;
	
	
} sexp;

// Meta functions
sexp* sexp_make(int kind);

sexp* sexp_make_text(char* file);

sexp* sexp_make_cmd(char* cmd, vec* args);

sexp* sexp_make_unop(char* op, sexp* cmd1);

sexp* sexp_make_binop(char* op, sexp* cmd1, sexp* cmd2);

void sexp_free(sexp* se);

sexp* sexp_parse(vec* tokens);

sexp* sexp_parse_sexpr(vec* tokens);

void sexp_print(sexp* se);

void sexp_println(sexp* se);

#endif

