/*
 * Nathan Gibson
 * 
 * Simple Shell
 * 
 * This program is a simple shell that supports input/output 
 * redirection, piping, and backgrounding of processes. It works by taking
 * a line of input, parsing it into single commands, parsing those commands
 * into arguments and input/output files. Then these arguments are passed to
 * the exec_single_command function that sets up the necessary file 
 * descriptors and then finally executes the commands via execvp. 
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

void exec_single_command(char* command, int index,int number_of_commands);

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
		
		//execute each individual command
		for(i = 0; i < number_of_commands; i++){
			exec_single_command(commands[i], i, number_of_commands);	
		}
	}
}

/*
 * 
 * takes a single command including its arguments, I/O redirection,
 * and background indicator, the number of commands that have already
 * been executed, and the total number of commands that will be
 * executed - each command is parsed and redirects are setup before
 * the command is executed  
 * 
 */
	
void exec_single_command(char* command, int index,int number_of_commands){    
	
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
	
	
    //mask to describe the structure of the input
    //0 for command, 1 for argument, 2 for infile, 3 for outfile      
    //4 for <, 5 for >
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
	int bg;
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
			bg = 1;
		}
	}
	
	int filedes[2]; 
	int filedes2[2];
	
	//odd numbered commands share another set of pipes than even numbered
	if (index % 2 != 0){
		pipe(filedes); 
	}else{
		pipe(filedes2); 
	}
	
	if (fork_rtn = fork()) {
		//dont wait on process to finish	
		if(bg != 1){
			wait(&child_status);
		}
	}else{
		//if there is only one command dont alter its stdout, stdin
		if(number_of_commands > 1){
			//first command only needs to alter its stdout
			if(index == 0){
				dup2(filedes2[1], 1);
			}
			//if it is the last command, only its stdout must be altered
			else if(index == number_of_commands - 1){
				if (number_of_commands % 2 != 0){ 
					dup2(filedes[0],0);
				}else{ 
					dup2(filedes2[0],0);
				} 
			}else{ 
				//middle commands must alter both their stdin and stdout
				if (i % 2 != 0){
					dup2(filedes2[0],0); 
					dup2(filedes[1],1);
				}else{ 
					dup2(filedes[0],0); 
					dup2(filedes2[1],1);					
				} 
			}
		}
		//check to see if the command uses output/input redirection
		//if so create/open the file for use
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
		//execute the command
		if(execvp(args[0], args) == -1){
			fprintf(stderr, "exec error\n");
		}	
	}	
	//close unneeded pipes
	if (index == 0){
		close(filedes2[1]);
	}
	else if (index == number_of_commands - 1){
		if (number_of_commands % 2 != 0){					
			close(filedes[0]);
		}else{					
			close(filedes2[0]);
		}
	}else{
		if (index % 2 != 0){					
			close(filedes2[0]);
			close(filedes[1]);
		}else{					
			close(filedes[0]);
			close(filedes2[1]);
		}
	}
}
