#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

//variables
char *builtins[] = {"exit", "help", "pwd", "cd", "wait"};
char *specialFuncs[] = {">", "<", "|", "&"};
int numOfBuiltins = 5;
int numOfSpecials = 4;

//functions
char** parse(char *line, const char delim[]);//function to parse the input from command line
int executeProg(char **args);//function to execute specfic commands
int runProgram(char **args);//function to run programs using execvp
int helpFunc();//function for help command
int pwdFunc();//function for pwd command
int cdFunc(char **args);//function for cd command
int waitFunc();//function to wait for background processes
int redirectOutput(char **args, int index);//function to redirect stdout
int redirectInput(char **args , int index);//function to redirect stdin
int pipesFunc(char **args);//function for pipes
int background(char **args, int index);//function for background processes

int main(){
    char *BUFFER = NULL;//buffer to hold text from cmd line
    char **args;//arguements for function calls
    long unsigned int Buf_size = 0;//size of buffer

    while(1){//loop for shell
        printf("Command here: ");
        fflush(stdout);
        int t = getline(&BUFFER, &Buf_size, stdin);//get input from command line
        if(t == -1){//error check
            perror("getline)");
            break;
        }
        args = parse(BUFFER, " \r\n");//parse the buffer
        if(args == NULL || args[0] == NULL){//error check
            printf("Parse did not work");
            break;
        }
        int args_size = strlen(args[0]);//delete the ending newline of command
        if(args[0][args_size] == '\n'){
            args[0][args_size] = 0;
        }
        if(executeProg(args) == 0){//execute command
            break;
        }
        free(BUFFER);//free varaibles
        free(args);
        BUFFER = NULL;//reset buffer
        Buf_size = 0;
    }
    return 0;
}
char** parse(char *line, const char delim[]){
    char **array = (char**)malloc(sizeof(char*));//allocate memory for return array
    *array = NULL;
    int i = 0;
    char *buf = strtok(line,delim);//use strtok for parse the line
    if (buf == NULL){//check if strtok failed
        free(array);
        array=NULL;
        return array;
    }
    while (buf != NULL){//reallocate memory for each new char in array
        array = (char**)realloc(array,(i+2)*sizeof(char*));
        if(array==NULL){//check if realloc fails 
            free(array);
            array=NULL;
            return array;
        }
        array[i]=buf;//put buf into array
        i++;//increment arrray
        array[i]=NULL;//set next element to NULL
        buf = strtok(NULL,delim);//get next char
    }
    return array;//return parsed array
}
int executeProg(char **args){
    if(args[0] == NULL){//check if args is empty
        return 1;
    }
    for(int i = 0; i < numOfBuiltins; i++){//check for builtin functions
        if(strcmp(args[0], builtins[i]) == 0){
            switch(i){
                case 0:
                    return 0;
                case 1:
                    return(helpFunc());
                case 2:
                    return(pwdFunc());
                case 3:
                    return(cdFunc(args));
                case 4:
                    return(waitFunc());
                default:
                    printf("Something went wrong in executeProg\n");
                    return 0;
            }               
        }
    }
    for(int i = 0; i < numOfSpecials; i++){//check for special functions
        for(int j = 0; args[j] != NULL; j++){//increment this for each new special function
            if(strcmp(args[j], specialFuncs[i]) == 0){
                switch(i){
                    case 0://redirect output
                        redirectOutput(args, j);
                        return 1;
                    case 1://redirect Input
                        redirectInput(args, j);
                        return 1;
                    case 2://pipes
                        pipesFunc(args);
                        return 1;
                    case 3://background prosesses
                        background(args, j);
                        return 1;
                    default: 
                        printf("something went wrong in execute Prog\n");
                        return 0;
                }
            }
        }
    }
    return(runProgram(args));//if command is not builtin or special, run program
}
int runProgram(char **args){
    pid_t pid;
    pid_t wpid;
    int status;

    pid = fork();//fork
    if(pid < 0){//error check
        printf("fork did not work in runprogram\n");
        return 0;
    }
    if(pid == 0){//child
        if(execvp(args[0], args) == -1){//execute program
            printf("execvp did not work in runprogam\n");
            exit(1);
        }
    } else {//parent
        wpid = waitpid(pid, &status, 0);//wait for child
        if(wpid == -1){//error check
            printf("something went wrong in runprogam\n");
            return 0;
        }
        if(!WIFEXITED(status)){
            return 0;
        }
    }
    return 1;
}
int helpFunc(){//print various strings
    printf("\nList of commands available: \n\n");//keep this at the front always
    printf("exit - Exits Shell\n");
    printf("help - Lists available commands\n");
    printf("pwd - Prints out the current working directory\n");
    printf("cd - Changes current directory\n");
    printf("wait - Waits for all background processes to finish\n");
    printf("Run programs - type the name or directory of program you want run\n");
    printf("Redirect Output - put > to redirect output to a file\n");
    printf("Redirect Input - put < to redirect input from a file\n");
    printf("Piping - put | between commands to pipe output of a file to the input of another file\n");
    printf("Background Processes - add & at the end of your command to make it run in the background\n");
    printf("\n");//leave this at the end always
    return 1;
}
int pwdFunc(){
    char BUFFER[1024];//make BUFFER for directory
    if(getcwd(BUFFER, sizeof(BUFFER)) != NULL){
        printf("%s\n", BUFFER);//get directory and print
        return 1;
    } else{//print if cwd returned NULL
        printf("Directory could not be determined or Size was too small\n");
        return 0;
    }
}
int cdFunc(char **args){
    if(chdir(args[1]) != 0){//use chdir to change directory
        printf("Directory entered cannot be opened\n");
        return 0;
    } 
    return 1;
}
int waitFunc(){
    __pid_t p = wait(NULL);
    while(p != -1){//go through every background process and make sure it ends
        p = wait(NULL);
    }
    return 1;
}
int redirectOutput(char **args, int index){
    if(index == 0 || args[index+1] == NULL){//error check
        printf("index is zero in redirectOutput\n");
        return 0;
    }
    int temp = open(args[index+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);//create temp file
    if(temp < 0){
        printf("open failed in redirectOutput\n");
        return 0;
    }
    int saved_stdout = dup(STDOUT_FILENO);//save STDOUT
    if(saved_stdout < 0){
        printf("dup failed to save stdout in redirectOutput\n");
        return 0;
    }
    dup2(temp, STDOUT_FILENO);//reroute STDOUT to go to temp
    close(temp);
    char **arguments = (char**)malloc((index+1)*sizeof(char*));//make arguments
    if(arguments == NULL){
        printf("malloc did not work in redirectOutput\n");
        close(temp);
        return 0;
    }
    for(int i = 0; i < index; i++){//put arguments into variable
        arguments[i] = args[i];
    }
    arguments[index] = NULL;//args must be NULL terminated
    runProgram(arguments);//run the program
    dup2(saved_stdout, STDOUT_FILENO);//reinstate STDOUT
    close(saved_stdout);
    free(arguments);//free variables
    return 1;
}
int redirectInput(char **args , int index){
    if(index == 0 || args[index+1] == NULL){//error check
        printf("index is zero in redirectInput\n");
        return 0;
    }
    int temp = open(args[index+1], O_RDONLY);//open temp file
    if(temp < 0){
        printf("open failed in redirectInput\n");
        return 0;
    }
    int saved_stdin = dup(STDIN_FILENO);//save stdin
    if(saved_stdin < 0){
        printf("dup failed in redirectOutput\n");
        close(temp);
        return 0;
    }
    dup2(temp, STDIN_FILENO);//reroute STDIN to temp
    close(temp);
    char **arguments = (char**)malloc((index+1)*sizeof(char*));//create arguments variable
    if(arguments == NULL){
        printf("malloc failed in redirectInput\n");
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
        return 0; 
    }
    for(int i = 0; i < index; i++){//put arguments into variable
        arguments[i] = args[i];
    }
    arguments[index] = NULL;//must be NULL terminated
    runProgram(arguments);//run program
    dup2(saved_stdin, STDIN_FILENO);//reinstate STDIN
    close(saved_stdin);
    free(arguments);//free variables
    return 1;
}
int pipesFunc(char **args){
    int numOfCmds = 1;
    for(int i = 0; args[i] != NULL; i++){
        if(strcmp(args[i], "|") == 0){
            numOfCmds++;//count number of pipes
        }
    }
    //create pipes
    int pipes[2*(numOfCmds-1)];
    for(int i = 0; i < numOfCmds -1; i++){//create 2 FDs for each cmd
        if(pipe(pipes + i * 2) < 0){
            printf("pipe did not work in pipesFunc\n");
            return 0;
        }
    }

    int start = 0;//track beginning of current cmd
    for(int i = 0; i < numOfCmds; i++){
        int argc = 0;//get current cmds arguments from main args
        while(args[start + argc] != NULL && strcmp(args[start + argc], "|") != 0){
            argc++;
        }
        //make array for current cmd's call
        char **cmd = (char**)malloc((argc+1) * sizeof(char*));
        for(int j = 0; j < argc; j++){
            cmd[j] = args[start + j];//add arguements
        }
        cmd[argc] = NULL;//must be NULL terminated

        pid_t pid = fork();//fork
        if(pid < 0){
            printf("fork did not workin pipesFunc\n");
            return 0;
        }
        if(pid == 0){//child
            if(i > 0){//if not first, redirect STDIN to previous pipes read end
                dup2(pipes[(i-1)*2], STDIN_FILENO);
            }
            if(i < numOfCmds - 1){//if not last, redirect STDOUT to current pipes write end
                dup2(pipes[i*2+1], STDOUT_FILENO);
            }
            for(int k = 0; k < 2*(numOfCmds-1); k++){
                close(pipes[k]);//close all FDs 
            }
            execvp(cmd[0], cmd);//execute cmd
            exit(1);//exit if execvp fails
        }
        free(cmd);//free cmd
        start += argc+1;//skip to next cmd
    }
    //parent
    for(int i = 0; i < 2*(numOfCmds-1); i++){
        close(pipes[i]);//close all pipe FDs
    }
    for(int i = 0; i < numOfCmds; i++){
        wait(NULL);//wait for all children to complete
    }
    return 1;
}
int background(char **args, int index){
    pid_t pid;
    int status;
    args[index] = NULL;//get rid of & in args

    pid = fork();//fork 
    if(pid < 0){
        printf("fork did not work in background\n");
        return 0;
    }
    if(pid == 0){//child
        if(execvp(args[0], args) == -1){//execvp command
            printf("execvp did not work in background\n");
            exit(1);
        }
    } else {//parent
        pid_t result = waitpid(pid, &status, WNOHANG);//do not hang
        if(result == -1){//error check
            printf("waitpid did not work in background\n");
            return 0;
        }
        if(result == 0){
            printf("Process is now running in background\n");
            return 1;
        } 
    }
    return 1;
}