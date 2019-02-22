#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// user headers
#include "vec.h"
#include "tokens.h"

// this file's header
#include "sexp.h"

// #define KIND_TEXT 0
// #define KIND_CMD 1
// #define KIND_UNOP 2
// #define KIND_BINOP 3
//
// typedef struct sexp {
// 	const int kind;
//
// 	char* text;
//
// 	char* cmd;
// 	vec* args;
//
// 	char* op;
// 	sexp* cmd1;
// 	sexp* cmd2;
//
// } sexp;

// Meta functions
sexp* sexp_make(int kind) {
	sexp* se = malloc(sizeof(sexp));
	se->kind = kind;
	
	se->cmd = 0;
	se->args = 0;
	se->op = 0;
	se->cmd1 = 0;
	se->cmd2 = 0;
	se->text = 0;
	return se;
}

sexp* sexp_make_text(char* file) {
	sexp* se = sexp_make(KIND_TEXT);
	se->text = strdup(file);
	return se;
}

sexp* sexp_make_cmd(char* cmd, vec* args) {
	sexp* se = sexp_make(KIND_CMD);
	se->cmd = strdup(cmd);
	se->args = vec_copy(args);
	return se;
}

sexp* sexp_make_unop(char* op, sexp* cmd1) {
	sexp* se = sexp_make(KIND_UNOP);
	se->op = strdup(op);
	se->cmd1 = cmd1;
	return se;
}

sexp* sexp_make_binop(char* op, sexp* cmd1, sexp* cmd2) {
	if (!cmd2) {
		return sexp_make_unop(op, cmd1);
	}
	sexp* se = sexp_make(KIND_BINOP);
	se->op = strdup(op);
	se->cmd1 = cmd1;
	se->cmd2 = cmd2;
	return se;
}

void sexp_free(sexp* se) {
	if (se->cmd) free(se->cmd);
	if (se->args) vec_free(se->args);
	if (se->op) free(se->op);
	if (se->cmd1) sexp_free(se->cmd1);
	if (se->cmd2) sexp_free(se->cmd2);
	if (se->text) free(se->text);
	free(se);
}

int max(int a, int b) {
	return a > b ? a : b;
}

sexp* sexp_parse(vec* tokens) {
	
	int loc_semicolon = vec_index_of(tokens, ";");
	if (loc_semicolon >= 0) {
		vec* before = vec_subvec(tokens, 0, loc_semicolon);
		vec* after = vec_tail(tokens, loc_semicolon + 1);
		sexp* se = sexp_make_binop(";", sexp_parse(before), sexp_parse(after));
		vec_free(before);
		vec_free(after);
		return se;
	}
	
	int loc_bg = vec_index_of(tokens, "&");
	if (loc_bg >= 0) {
		vec* before = vec_subvec(tokens, 0, loc_bg);
		sexp* se = sexp_make_unop("&", sexp_parse(before));
		vec_free(before);
		return se;
	}
	
	int loc_or = vec_index_of(tokens, "||");
	if (loc_or >= 0) {
		vec* before = vec_subvec(tokens, 0, loc_or);
		vec* after = vec_tail(tokens, loc_or + 1);
		sexp* se = sexp_make_binop("||", sexp_parse(before), sexp_parse(after));
		vec_free(before);
		vec_free(after);
		return se;
	}
	
	int loc_and = vec_index_of(tokens, "&&");
	if (loc_and >= 0) {
		vec* before = vec_subvec(tokens, 0, loc_and);
		vec* after = vec_tail(tokens, loc_and + 1);
		sexp* se = sexp_make_binop("&&", sexp_parse(before), sexp_parse(after));
		vec_free(before);
		vec_free(after);
		return se;
	}
	
	int loc_pipe = vec_index_of(tokens, "|");
	if (loc_pipe >= 0) {
		vec* before = vec_subvec(tokens, 0, loc_pipe);
		vec* after = vec_tail(tokens, loc_pipe + 1);
		sexp* se = sexp_make_binop("|", sexp_parse(before), sexp_parse(after));
		vec_free(before);
		vec_free(after);
		return se;
	}
	
	int loc_out = vec_index_of(tokens, ">");
	if (loc_out >= 0) {
		vec* before = vec_subvec(tokens, 0, loc_out);
		sexp* se = sexp_make_binop(">", sexp_parse(before), 
			sexp_make_text(vec_get(tokens, loc_out+1)));
		vec_free(before);
		return se;
	}
	
	int loc_in = vec_index_of(tokens, "<");
	if (loc_in >= 0) {
		vec* before = vec_subvec(tokens, 0, loc_in);
		sexp* se = sexp_make_binop("<", sexp_parse(before), 
			sexp_make_text(vec_get(tokens, loc_in+1)));
		vec_free(before);
		return se;
	}
		
	vec* args = vec_make();
	char* cmd = vec_get(tokens, 0);
	for (int i = 0; i < vec_size(tokens); i++) {
		vec_add(args, vec_get(tokens, i));
	}
	
	sexp* se = sexp_make_cmd(cmd, args);
	vec_free(args);
	return se;

}

int find_close_paren(vec* tokens, int start) {
	int nest = 1;
	int i;
	for (i = start + 1; nest > 0; i++) {
		char* cur = vec_get(tokens, i);
		if (isoparen(cur)) nest++;
		else if (iscparen(cur)) nest--;
	}
	return i-1;
}

sexp* sexp_parse_sexpr(vec* tokens) {
	if (!vec_size(tokens)) {
		return 0;
	}
	char* cur = vec_peek(tokens);

	if (isoparen(cur)) {
		// vec[0] = "("
		// vec[1] = op OR cmd
		// vec[2] = "(" OR arg1
		int loc_outer_close_paren = find_close_paren(tokens, 0);
		cur = vec_get(tokens, 1);
		
		if (isop(cur)) {
			int loc_inner_close_paren = find_close_paren(tokens, 2);
			vec* arg1 = vec_subvec(tokens, 2, loc_inner_close_paren + 1);
			vec* arg2 = vec_subvec(tokens, loc_inner_close_paren + 1, loc_outer_close_paren);
			sexp* se = sexp_make_binop(cur, sexp_parse_sexpr(arg1), sexp_parse_sexpr(arg2));
			vec_free(arg1);
			vec_free(arg2);
			return se;
		} else {
			vec* args = vec_subvec(tokens, 1, loc_outer_close_paren);
			sexp* se = sexp_make_cmd(cur, args);
			vec_free(args);
			return se;
		}
	} else {
		return sexp_make_text(cur);
	}
}

void sexp_print(sexp* se) {
	switch (se->kind) {
		
		case KIND_TEXT: 
		printf("\"%s\"", se->text);		
		break;
		
		case KIND_CMD:
		printf("(\"%s\"", se->cmd);
		vec* args = se->args;
		for (int i = 1; i < vec_size(args); i++) {
			printf(" \"%s\"", vec_get(args, i));
		}
		printf(")");
		break;
		
		case KIND_UNOP:
		printf("(\"%s\" ", se->op);
		sexp_print(se->cmd1);
		printf(")");
		break;
		
		case KIND_BINOP:
		printf("(\"%s\" ", se->op);
		sexp_print(se->cmd1);
		printf(" ");
		sexp_print(se->cmd2);
		printf(")");
		break;
	}
}

void sexp_println(sexp* se) {
	sexp_print(se);
	printf("\n");
}
