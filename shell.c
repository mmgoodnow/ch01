#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "tokens.h"
#include "vec.h"
#include "sexp.h"
#include "run.h"

static char temp_line[128];

char* read_line(int fd) {
	int ii;
	for (ii = 0; ii < 100; ++ii) {
		int rv = read(fd, temp_line + ii, 1);
		if (rv == 0) {
			return 0;
		}
		if (temp_line[ii] == '\n') {
			temp_line[ii] = 0;
			break;
		}
	}
	return temp_line;
}

int run_file(char* filename) {
	int input = open(filename, O_RDONLY);
	if (input == -1) {
		perror("open failed");
		return 1;
	}

	char* line;
	int rv = 0;
	
	while ((line = read_line(input))) {
		vec* tokens = tokenize(line);
		sexp* se = sexp_parse_sexpr(tokens);
		vec_free(tokens);
		rv = run(se);
		
		sexp_free(se);
	}

	close(input);
	return rv;
}

int run_interactive() {
	char line[256];
	int rv;

	printf("mgosh$ ");
	fflush(stdout);
	while(fgets(line, 256, stdin)) {

		vec* tokens = tokenize(line);
		vec_print(tokens);
		sexp* se = sexp_parse(tokens);
		sexp_print_sexpr(se);
		vec_free(tokens);

		rv = run(se);
		sexp_free(se);

		printf("mgosh$ ");
		fflush(stdout);
	}
	return rv;
}

int main(int argc, char* argv[])
{
	if (argc == 2) {
		return run_file(argv[1]);
	} else {
		return run_interactive();
	}
}

