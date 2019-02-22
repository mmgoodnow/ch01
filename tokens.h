#ifndef TOKENS_H
#define TOKENS_H 

#include "vec.h"

typedef int (*cpred)(int); 

int isop(char* str);

int streq(const char* aa, const char* bb);

int isoparen(char* str);
int iscparen(char* str);



int isparen(int cc);
int isquote(int cc);
int istext(int cc);



void copy_next_token(char* text, char* tokn, cpred pred);
void copy_quote(char* text, char* buf);

vec* tokenize(char* text);

#endif