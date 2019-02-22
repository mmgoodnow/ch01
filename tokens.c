#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "vec.h"
#include "tokens.h"

// This file was taken from hints folder and modified slightly.

typedef int (*cpred)(int); 

int streq(const char* aa, const char* bb) {
    return strcmp(aa, bb) == 0;
}

int isop(char* str) {
	return streq(str, ";") 
		|| streq(str, "&")
		|| streq(str, "&&")
		|| streq(str, "||") 
		|| streq(str, "|")
		|| streq(str, "<")
		|| streq(str, ">");
}

int isoparen(char* str) {
	return streq(str, "(");
}

int iscparen(char* str) {
	return streq(str, ")");
}

int isparen(int cc) {
	return cc == '(' || cc == ')';
}

int isquote(int cc) {
	return cc == '"';
}

int istext(int cc) {
	return !isparen(cc) && !ispunct(cc) && !isspace(cc);
}

void copy_next_token(char* text, char* tokn, cpred pred) {
	int ii = 0;
	while (text[ii] && pred(text[ii])) {
		tokn[ii] = text[ii];
		ii++;
	}

	tokn[ii] = 0;
}

void copy_quote(char* text, char* buf) {
	int ii;
	for (ii = 0; text[ii] && !isquote(text[ii]); ++ii) {
		buf[ii] = text[ii];
	}
	buf[ii] = 0;
}

vec* tokenize(char* text) {
	char tok[80];
	vec* tokens = vec_make();

	cpred preds[] = { ispunct, istext };

	int ii = 0;
	while (ii < strlen(text)) {
		if (isspace(text[ii])) {
			ii++;
			continue;
		}

		if (isparen(text[ii])) {
			tok[0] = text[ii];
			tok[1] = 0;
			ii += 1;
			vec_add(tokens, tok);
			continue;
		}

		if (isquote(text[ii])) {
			copy_quote(text + ii + 1, tok);
			vec_add(tokens, tok);
			ii += strlen(tok);
			ii += 2;
			continue;
		}

		for (int jj = 0; jj < 2; ++jj) {
			if (preds[jj](text[ii])) {
				copy_next_token(text + ii, tok, preds[jj]);
				vec_add(tokens, tok);
				break;
			}
		}

		ii += strlen(tok);
	}
	
	return tokens;
}

