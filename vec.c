#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vec.h"

// typedef struct vec {
// 	int size;
// 	int cap;
// 	char** data;
// } vec;


vec* vec_make() {
	vec* v = malloc(sizeof(vec));
	v->size = 0;
	v->cap = 16;
	v->data = malloc(v->cap * sizeof(char*));
	return v;
}

vec* vec_copy(vec* old) {
	vec* v = vec_make();
	for (int i = 0; i < vec_size(old); i++) {
		vec_add(v, vec_get(old, i));
	}
	return v;
}

vec* vec_subvec(vec* old, int start, int end) {
	vec* v = vec_make();
	for (int i = start; i < vec_size(old) && i < end; i++) {
		vec_add(v, vec_get(old, i));
	}
	return v;
}

vec* vec_tail(vec* old, int start) {
	return vec_subvec(old, start, vec_size(old));
}

void vec_free(vec* v) {
	for (int i = 0; i < v->size; i++) {
		if (v->data[i]) {
			free(v->data[i]);
		}
	}
	free(v->data);
	free(v);
}

void vec_push(vec* v, char* str) {
	vec_reverse(v);
	vec_add(v, str);
	vec_reverse(v);
}
char* vec_peek(vec* v) {
    if (vec_size(v) <= 0) {
        exit(1);
    }
	return vec_get(v, 0);
}
char* vec_pop(vec* v) {
    if (vec_size(v) <= 0) {
        exit(1);
    }
	vec_reverse(v);
	char* ret = vec_pop_tail(v);
	vec_reverse(v);
	return ret;
}

void vec_add(vec* v, char* str) {
	if (v->size >= v->cap) {
		v->cap *= 2;
		v->data = realloc(v->data, v->cap * sizeof(char*));
	}
		v->data[v->size] = strdup(str);
		v->size++;
}

char* vec_peek_tail(vec* v) {
    if (vec_size(v) <= 0) {
        exit(1);
    }
    return vec_get(v, vec_size(v) - 1);
}

char* vec_pop_tail(vec* v) {
    char* ret = vec_peek_tail(v);
    if (vec_size(v) - 1 <= v->cap / 4) {
        v->cap /= 2;
        v->data = realloc(v->data, v->cap * sizeof(char*)); 
	}
    v->size--;
    return ret;
}

char* vec_get(vec* v, int i) {
	if (i < 0 || i >= v->size) {
		exit(1);
	} 
	return v->data[i];
}

void vec_set(vec* v, int i, char* str) {
	if (i < 0 || i >= v->size) {
		exit(1);
	}
	v->data[i] = strdup(str);
}

int vec_size(vec* v) {
	return v->size;
}

int vec_index_of(vec* v, char* str) {
	for (int i = 0; i < vec_size(v); i++) {
		if (strcmp(vec_get(v, i), str) == 0) {
			return i;
		}
	}
	return -1;
}

void vec_reverse(vec* v) {
	char* temp;
	int start = 0;
	int end = vec_size(v) - 1;
	char** data = v->data;
	while (start < end) {
		temp = data[end];
		data[end] = data[start];
		data[start] = temp;
		start++;
		end--;
	}
}

void vec_print(vec* v) {
	for (int i = 0; i < vec_size(v); i++) {
		printf("\"%s\" ", vec_get(v, i));
	}
	printf("\n");
}

