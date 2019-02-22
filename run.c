#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "vec.h"
#include "run.h"
#include "tokens.h"

// included for reference
// #define KIND_TEXT 0
// #define KIND_CMD 1
// #define KIND_UNOP 2
// #define KIND_BINOP 3
// 
// typedef struct sexp {
// 	int kind;
//
// 	char* text;
//
// 	char* cmd;
// 	struct vec* args;
//
// 	char* op;
// 	struct sexp* cmd1;
// 	struct sexp* cmd2;
//
//
// } sexp;

int run(sexp* se) {
	switch (se->kind) {
		case KIND_CMD: return run_cmd(se);
		
		case KIND_UNOP:
		case KIND_BINOP:
		
		if (streq(se->op, ";")) {
			return run_sc(se);
		} else if (streq(se->op, "&")) {
			return run_bg(se);
		} else if (streq(se->op, "&&")) {
			return run_and(se);
		} else if (streq(se->op, "||")) {
			return run_or(se);
		} else if (streq(se->op, "<")) {
			return run_rin(se);
		} else if (streq(se->op, ">")) {
			return run_rout(se);
		} else if (streq(se->op, "|")) {
			return run_pipe(se);
		}
		
		break;
		default:
		assert(0);
	}
	
	
}

int run_cmd(sexp* se) {
	if (streq(se->cmd, "exit")) {
		sexp_free(se);
		exit(0);
	}
	
	if (streq(se->cmd, "cd")) {
		return chdir(vec_get(se->args, 1));
	}
	
	int cpid;
	if ((cpid = fork())) {
		// parent
		int status;
		waitpid(cpid, &status, 0);
		return WEXITSTATUS(status);
	} else {
		// child
		
		char** args = malloc(sizeof(char*) * (vec_size(se->args) + 1));
		
		for (int i = 0; i < vec_size(se->args); i++) {
			args[i] = vec_get(se->args, i);
		}
		
		args[vec_size(se->args)] = 0;
		
		execvp(se->cmd, args);
		assert(0);
	}
}

int run_sc(sexp* se) {
	int tmp = run(se->cmd1);
	if (se->cmd2) {
		return run(se->cmd2);
	} else {
		return tmp;
	}
	
}

int run_bg(sexp* se) {
	
	int cpid;
	if ((cpid = fork())) {
		
		// parent

	} else { 
		
		// child
		
		run(se->cmd1);
		exit(0);
	}
	
	if (se->cmd2) {
		return run(se->cmd2);
	}
	return 0;
}

int run_and(sexp* se) {
	int exitcode = run(se->cmd1);
	
	// if the first command was not successful, don't bother with the second
	if (exitcode != 0) {
		return exitcode;
	}
	return run(se->cmd2);
}

int run_or(sexp* se) {
	int exitcode = run(se->cmd1);
	
	// if the first command was successful, don't bother with the second
	if (exitcode == 0) {
		return exitcode;
	}
	return run(se->cmd2);
}

int run_rin(sexp* se) {
	
	int cpid;
	if ((cpid = fork())) {
		// parent
		
		int status;
		waitpid(cpid, &status, 0);
		return WEXITSTATUS(status);
	} else { 
		// child
		
		int fd = open(se->cmd2->text, O_RDONLY);
		// 0 is fd for stdin
		close(0);
		int rv = dup(fd);
		assert(rv != -1);
		close(fd);
		run(se->cmd1);
		close(0);
		exit(0);
	}
}

int run_rout(sexp* se) {
	int cpid;
	if ((cpid = fork())) {
		// parent
		
		int status;
		waitpid(cpid, &status, 0);
		return WEXITSTATUS(status);
	} else { 
		// child
		
		int fd = open(se->cmd2->text, O_CREAT|O_TRUNC|O_WRONLY, 0644);
		// 1 is fd for stdout
		close(1);
		int rv = dup(fd);
		assert(rv != -1);
		close(fd);
		
		run(se->cmd1);
		close(1);
		exit(0);
	}
}

int run_pipe(sexp* se) {
	int cpid;
	if ((cpid = fork())) {
		// parent
		
		int status;
		waitpid(cpid, &status, 0);
		return WEXITSTATUS(status);
	} else {
		// child
		int pipes[2];
		// pipes[0] is the right/read side of the pipe
		// pipes[1] is the left/write side of the pipe
		
		int pipe_retval = pipe(pipes);
		assert(pipe_retval != -1);
		
		int ccpid;
		if ((ccpid = fork())) {
			// right/read side of pipe
			// need to redirect stdin
			
			close(pipes[1]);
			close(0);
			int fd = dup(pipes[0]);
			assert(fd != -1);
			int ret = run(se->cmd2);
			int status;
			waitpid(ccpid, &status, 0);
			close(fd);
			exit(ret);
			
		} else {
			// left/write side of pipe
			// need to redirect stdout
			close(pipes[0]);
			close(1);
			int fd = dup(pipes[1]);
			assert(fd != -1);
			int ret = run(se->cmd1);
			close(fd);
			exit(ret);
		}
		
	}
}
