This project attempts to replicate a shell that can handle basic commands and functions

Here is a list of accepted commands
exit - terminates the shell
help - lists possible commands and usage instructions
cd - changes current directory
pwd - prints current working directory
wait - waits for all background processes
redirectOutput (>) - redirects standard output to a specific file
redirectInput (<) - redirects standard input form a specific file
pipes (|) - pipes output of one program to the input of another program, this can be done multiple times
background (&) - runs commands in the background, allowing the shell to still be used

It can execute commands such as ls as well as compiled files
  *Compiled files must be put in the formal ./program
  *To execute programs from other directories, you must specify a path
