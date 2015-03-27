/*
 *  
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#define MAX_LENGTH 256
int main(int argc, char *argv[]) {
	int interactive;
	char line[MAX_LENGTH];
	char* tokens[MAX_LENGTH];
	char* token;
	int number_of_tokens;
	int number_of_commands;
	if(argc == 2){
		if(strcmp("-i", argv[1]) == 0){
			interactive = 1;
		}else{
			fprintf(stderr, "Incorrect command line arguments\n");
			exit(-1);
		}
	}else{
		interactive = 0;
	}
	while(1){
		int i = 0;
		//determine whether or not to print a prompt
		if(interactive == 1)
			printf("%% ");
		//get input
		if(fgets(line, MAX_LENGTH, stdin) == NULL)
			return 0;
		//get rid of trailing \n
		line[strlen(line) - 1] = '\0';
		//tokenize input and store it in an array
		token = strtok(line, " ");
		while(token != NULL){
				tokens[i] = token;
				token = strtok(NULL, " ");
				i++;
		}
		number_of_tokens = i;
		/* 
		   mask to describe the structure of the input
		   0 for command, 1 for argument, 2 for infile, 3 for outfile
		   4 for <, 5 for >, 6 for |, 7 for &
		*/
		int mask[number_of_tokens];
		//initialize mask to known values
		for(i = 0; i < number_of_tokens; i++){
			mask[i] = -1;
		}
		//determine what each token represents
		for(i = 0; i < number_of_tokens; i++){
			if(i == 0){
				mask[i] = 0;
			}else if(*tokens[i] == '|'){
				mask[i] = 6;
			}else if(*tokens[i] == '&'){
				mask[i] = 7;
			}else if(mask[i - 1] == 6){
				mask[i] = 0;
			}else if(*tokens[i] == '<'){
				mask[i] = 4;
			}else if(*tokens[i] == '>'){
				mask[i] = 5;
			}else if(mask[i - 1] == 4){
				mask[i] = 2;
			}else if(mask[i - 1] == 5){
				mask[i] = 3;
			}else{
				mask[i] = 1;	
			}
		}
		for(i = 0; i < number_of_tokens; i++){
			if(mask[i] == 6){
				number_of_commands++;
			}
		}
		
		char* cmd;
		char* args[3] = {NULL, NULL, NULL};
		char* infile;
		char* outfile;
		for(i = 0; i < number_of_tokens; i++){
			if(mask[i] == 0){
				cmd = tokens[i];
				args[0] = cmd;
			}
			else if(mask[i] == 1){
				args[0] = tokens[i-1];
				args[1] = tokens[i];
				args[2] = NULL;
			}
			else if(mask[i] == 2){
				infile = tokens[i];
				int input_fd;
				if ((input_fd = open(infile, O_RDONLY)) >= 0) {
					close(0);
					dup2(input_fd, 0);
				} else {
					fprintf(stderr,
					"%s: Unable to open file 'this'.\n", argv[0]);
					exit(1);
				}
			}
			else if(mask[i] == 3){
				outfile = tokens[i];
				int output_fd;
				if ((output_fd = creat(outfile, S_IRUSR | S_IWUSR)) >= 0) {
					close(1);
					dup2(output_fd, 1);
					fchmod(output_fd, S_IRUSR | S_IWUSR | S_IRGRP);
				} else {
					fprintf(stderr, "%s: Unable to create file 'that'.\n", argv[0]);
					exit(2);
				}	
			}
			else if(mask[i] == 4){
					
			}
			else if(mask[i] == 5){
					
			}
			else if(mask[i] == 6){
					
			}
			else if(mask[i] == 7){
					
			}		
		}

		int fork_rtn, child_status;
			if (fork_rtn = fork()) {
				wait(&child_status);
			}else{	
				if(execvp(args[0], args) == -1){
					fprintf(stderr, "exec error\n");
				}
			}
	}
}
	
