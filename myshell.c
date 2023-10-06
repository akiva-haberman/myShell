#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

// standardizies input string
// deletes leading spaces
// turns all white space to single space
// between ';' and '>' put single space
// use the strtok_r for tokenization


void myPrint(char* msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}

void print_err() {
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
}

//runs the basic pwd command
//please note that thsi is not equipped to handler redirction lin:33

int pwd(char** arg_list) { 
    //char* path = getenv("PWD");
    char path[613];
    getcwd(path,sizeof(path));
    strcat(path, "\n");
    if (path == NULL || arg_list[1] != NULL) {
        return -1;
    }
    write(STDOUT_FILENO, path, strlen(path));
    return 1;
}

//please note that thsi is not equipped to handler redirction lin:44
int cd(char** arg_list) {
    if (arg_list[3] != NULL) {
        return -1;
    } else if (arg_list[2] != NULL) {
        return -1;
    }
    char* path;
    if (arg_list[1] == NULL) {
        path = getenv("HOME");
    }
    else {
        path = arg_list[1];
    }
    return chdir(path);
}

int check_for_redirect(char** arg_list) {
    while (arg_list[0]) {
        if (!strcmp(*arg_list, ">")) {
            return 1;
        }
        arg_list++;
    }
    return 0;
}



int redirect(char** arg_list) {
    //check if file exists
   // dup2(stdout,filename)
    return -1;
}


void run_cmd(char** arg_list) {
    // if (check_for_redirect(arg_list)){
    //     printf("under construction\n");
    // }
    if (arg_list[0] == NULL ){ //is the issue null or semi colon, find out
        return;
    }
    if (!strcmp(arg_list[0], "\n") || !strcmp(arg_list[0], "\t")) {
        //if (arg_list[0] == NULL ){
        print_err();
        return;
    }
    if (!(strcmp(arg_list[0], "exit"))) {
        if (arg_list[1] != NULL) {
            print_err();
            return;
        }
        exit(0);
    }
    else if (!strcmp(arg_list[0], "pwd")) {
        if (fork() == 0) {
            if (pwd(arg_list) == -1) {
                print_err();
                
            }
            exit(0);
        }
        else {
            wait(NULL);
        }
    }
    else if (!strcmp(arg_list[0], "cd")) {
        if (cd(arg_list) == -1) {
            print_err();
        }
    }
    else {
        if (fork() == 0) {
            if (execvp(arg_list[0], arg_list) == -1) {
                print_err();
                exit(0);
            }
        }
        else {
            wait(NULL);
        }
    }
}

char** parse_cmd(char* cmd) { // makes list of strings for individual commands
    char* status = NULL;
    char* tok = strtok_r(cmd, " \t\n", &status);
    char** tok_ls = (char**)malloc(sizeof(char*) * 512);
    int tok_num = 0;

    while (tok != NULL) {
        tok_ls[tok_num] = tok;
        tok_num++;
        tok = strtok_r(status, " \t\n", &status);
    }
    tok_ls[tok_num] = NULL;
    return tok_ls;
}

int is_Whitespace(char* str){
    while (*str){
        if (!isspace(*str)){
            return 0;
        }
        str++;
    }
    return 1;
}

void run_input(char* str, int flag) {
    // --- Parse through each command and call them sequentially ---
    char* status = NULL; // initialize strtok status
    char** cmd;
    char* tok = strtok_r(str, ";", &status); // tokenizes first command
    // if (flag){
    //     // if (is_Whitespace(str)){
    //     //     //printf("is white space");
    //     //     return;
    //     // }
    //        write(STDOUT_FILENO, str, strlen(str));
    //     }
    while (tok != NULL) {
        cmd = parse_cmd(tok);
        run_cmd(cmd);
        tok = strtok_r(status, ";", &status);
    }
    return;
}




// interactive
int main(int argc, char* argv[]) {
    //exit needs tice after pwd
    // ./myshell : argc = 1
    // ./myshell file.txt : argc = 2
    // FILE* fl = fopen(argv[1])
    int batch_flag = 0;
    if (argc - 1){
        batch_flag = 1;
    }
    char cmd_buff[513];
    char* pinput;
    if (batch_flag){
        if (access(argv[1], R_OK) != 0){
            print_err();
            printf("is this saving");
        }
        
    }
    FILE* fl = fopen(argv[1], "r");
    while (1) {  // still not handling long strings well
        if (!batch_flag){
            myPrint("myshell> ");
        }
       if (batch_flag) {
            pinput = fgets(cmd_buff, 514, fl);

        } else {
            pinput = fgets(cmd_buff, 514, stdin);
        }        
        //uses init_parse and calls command up to semi colon or null terminating
        //saves first cmd in var and rest in rest
        if (!pinput) {
            exit(0);
        }

        // --- Check if input is of valid size ---
        if (strlen(pinput) == 513 && cmd_buff[512] != '\n') {
            while (pinput) {
               myPrint(cmd_buff);
                if (strlen(pinput) != 513) {
                    break;
                }
                if (batch_flag) { 
                    pinput = fgets(cmd_buff, 514, fl);
                } else {
                    pinput = fgets(cmd_buff, 514, stdin);
                }
            }
            print_err();
            continue;
        }
        if (batch_flag){
            if(!is_Whitespace(cmd_buff) ){ //In batch mode, if the input file contains blank lines, do not print the blank lines. 
                myPrint(cmd_buff); //in batch mode need to print the line entered
                run_input(cmd_buff, batch_flag);
            }
        } else{
            run_input(cmd_buff, batch_flag);
        }
    }
   
}