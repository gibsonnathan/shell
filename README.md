# shell

This program is a simple shell that supports input/output 
redirection, piping, and backgrounding of processes. It works by taking
a line of input, parsing it into single commands, parsing those commands
into arguments and input/output files. Then these arguments are passed to
the exec_single_command function that sets up the necessary file 
descriptors and then finally executes the commands via execvp. 
 
