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

void exec_single_command(char* command, int pipe[]){
	if(isspace(command[0])){
		*command++;
	}
	printf("%s", command);
	
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
		int number_of_commands = 1;
		
		int i = 0;
		//determine whether or not to print a prompt
		if(interactive == 1)
			printf("%% ");
		//get input
		if(fgets(line, MAX_LENGTH, stdin) == NULL){
			fprintf(stderr, "fgets quiting");
			return 0;
		}
		
		//get rid of trailing \n
		line[strlen(line) - 1] = '\0';
	
		//make copy of the line
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
		
		//tokenize the line based on the
		//pipe symbol
		i = 0;
		char* commands[number_of_commands];
		token = strtok(line_copy, "|");
		while(token != NULL){
				commands[i] = token;
				token = strtok(NULL, "|");
				i++;
		}
		
		
		int tube[2];
		for(i = 0; i < number_of_commands; i++){
			exec_single_command(commands[i], tube);	
			printf("\n");
		}
		
		
	}
}
	
