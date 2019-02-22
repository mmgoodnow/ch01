#ifndef VEC_H
#define VEC_H

typedef struct vec {
	int size;
	int cap;
	char** data;
} vec;


// Meta functions
vec* vec_make();

void vec_free(vec* v);

// head ops
void vec_push(vec* v, char* str);
char* vec_peek(vec* v);
char* vec_pop(vec* v);

// tail ops
void vec_add(vec* v, char* str);
char* vec_peek_tail(vec* v);
char* vec_pop_tail(vec* v);

// random access
char* vec_get(vec* v, int i);
void vec_set(vec* v, int i, char* str);

// utils

int vec_size(vec* v);
int vec_index_of(vec* v, char* str);

// copies from range [start, end)
vec* vec_subvec(vec* old, int start, int end);
vec* vec_tail(vec* old, int start);
vec* vec_copy(vec* old);
void vec_reverse(vec* v);
void vec_print(vec* v);


#endif
