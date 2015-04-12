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

void exec_single_command(char* command, int index, int number_of_commands){
	//removing space at the beginning of commands caused by strtok()
	if(isspace(command[0])){
		*command++;
	}
	int i = 0;
	char* command_token;
	char* command_tokens[MAX_LENGTH];
	int number_of_command_tokens = 0;
	
	//tokenize the parts of the command
	command_token = strtok(command, " ");
	while(command_token != NULL){
		command_tokens[i] = command_token;
		command_token = strtok(NULL, " ");
		i++;
	}
	number_of_command_tokens = i;
	
	/* 
		   mask to describe the structure of the input
		   0 for command, 1 for argument, 2 for infile, 3 for outfile      
		   4 for <, 5 for >
	*/
	int mask[number_of_command_tokens];
	
	//initialize mask to known values
	for(i = 0; i < number_of_command_tokens; i++){
		mask[i] = -1;
	}
	
	//determine what each token represents
	for(i = 0; i < number_of_command_tokens; i++){
		if(i == 0){
			mask[i] = 0;
		}else if(*command_tokens[i] == '&'){
			mask[i] = 7;
		}else if(*command_tokens[i] == '<'){
			mask[i] = 4;
		}else if(*command_tokens[i] == '>'){
			mask[i] = 5;
		}else if(mask[i - 1] == 4){
			mask[i] = 2;
		}else if(mask[i - 1] == 5){
			mask[i] = 3;
		}else{
			mask[i] = 1;	
		}
	}
	
	//command attributes
	char* cmd;
	char* args[3] = {NULL, NULL, NULL};
	char* infile;
	char* outfile;
	int output_fd;
	int input_fd;
	int fork_rtn, child_status;
	int bg = 0;
	//parse each element and pull out commands
	//arguments, etc.
	for(i = 0; i < number_of_command_tokens; i++){
		if(mask[i] == 0){
			cmd = command_tokens[i];
			args[0] = cmd;
		}
		else if(mask[i] == 1){
			args[0] = command_tokens[i-1];
			args[1] = command_tokens[i];
			args[2] = NULL;
		}		
		else if(mask[i] == 7){
	
		}
	}
	
	if (fork_rtn = fork()) {	
		wait(&child_status);
	}else{
		
		if(index != 0){
			
		}
		if(index != number_of_commands - 1){
			
		}
		else{
			
		}
			
		for(i = 0; i < number_of_command_tokens; i++){
			if(mask[i] == 2){
				infile = command_tokens[i];
				if ((input_fd = open(infile, O_RDONLY)) >= 0) {
					close(0);
					dup2(input_fd, 0);
				} else {
					fprintf(stderr, "Unable to open file.\n");
					exit(1);
				}
			}
			else if(mask[i] == 3){
				outfile = command_tokens[i];
				if ((output_fd = creat(outfile, S_IRUSR | S_IWUSR)) >= 0) {
					close(1);
					dup2(output_fd, 1);
					fchmod(output_fd, S_IRUSR | S_IWUSR | S_IRGRP);
				} else {
					fprintf(stderr, "Unable to create file.\n");
					exit(2);
				}	
			}
		}
		
		if(execvp(args[0], args) == -1){
			fprintf(stderr, "exec error\n");
		}	
		
	}	
}

int main(int argc, char *argv[]) {
	int interactive;
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
		char line[MAX_LENGTH];
		char* tokens[MAX_LENGTH];
		char* token;
		int number_of_tokens;
		int number_of_commands = 0;
		int i = 0;
		
		//determine whether or not to print a prompt
		if(interactive == 1)
			printf("%% ");
			
		//get input
		if(fgets(line, MAX_LENGTH, stdin) == NULL){
			return 0;
		}
		
		//get rid of trailing \n
		line[strlen(line) - 1] = '\0';
		
		//make copy of the line, so that I have another to tokenize
		char line_copy[strlen(line)];
		strcpy(line_copy, line);
		
		//tokenize the line by spaces and store each
		//element in an array
		token = strtok(line, " ");
		while(token != NULL){
			tokens[i] = token;
			token = strtok(NULL, " ");
			i++;
		}
		number_of_tokens = i;
		
		//find out how many commands are on the line
		for(i = 0; i < number_of_tokens; i++){
			if(*tokens[i] == '|'){
				number_of_commands++;
			}
		}
		
		//only one command supplied or off by one
		if(number_of_commands == 0 && strlen(line) > 0){
			number_of_commands = 1;
		}else if(number_of_commands > 0){
			number_of_commands++;
		}
		
		//tokenize the line based on the
		//pipe symbol, breaking the line
		//into individual commands
		i = 0;
		char* commands[number_of_commands];
		token = strtok(line_copy, "|");
		while(token != NULL){
			commands[i] = token;
			token = strtok(NULL, "|");
			i++;
		}
	
		for(i = 0; i < number_of_commands; i++){
			exec_single_command(commands[i], i, number_of_commands);	
		}
	}
}
	
