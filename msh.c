/* Name:Alisha Kunwar
   ID: 1001668106
   Programming Assignment 1
   Description: Create a Mav Shell
*/

#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#define WHITESPACE " \t\n" 

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5 // Mav shell only supports five arguments

int pid_count = 0;
int pid_history[15]; //array to store the pid value of newly created processes

int main()
{
    char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);
    int record_count = 0;
    int i;
    int x = 1;

    //memory allocation to store the history of all the commands
    //allocating 15 memory space which stores the most 15 latest command
    char *record[15];
    for (i = 0; i < 15; i++)
    {
        record[i] = (char *)malloc(sizeof(char) * 255);
    }
    while (1)
    {
        // Print out the msh prompt
        printf("msh> ");

        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
            ;

        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];
        int token_count = 0;

        // Pointer to point to the token
        // parsed by strsep
        char *argument_ptr;

        //array for storing the history of the command for 15 or less than 15
        //post incrementing takes the record pointer to point to the next address
        if (record_count <= 14)

        {
            strncpy(record[record_count++], cmd_str, MAX_COMMAND_SIZE);
        }
        // We have 15 address space to store the history of the commands
        // once it is filled the old one i is replaced by i+1 consecutively
        // the new command get stored in the last space i =14 in the array

        else
        {
            for (i = 0; i < 14; i++)
            {
                strncpy(record[i], record[i + 1], MAX_COMMAND_SIZE);
            }

            strncpy(record[14], cmd_str, MAX_COMMAND_SIZE);
        }

        //if user just click enter it strcmp compares the enter command with new line
        // takes to new line of input
        if (strcmp(cmd_str, "\n") == 0)
        {
            continue;
        }

        // an array of pointers which points to the address
        //where strdup() function returns a pointer to it which is a duplicate of the cmd_str
        char *arr[x];
        if (cmd_str[0] == '!')
        {
            arr[0] = strdup(cmd_str);

            // atoi converts the string argument of arr[0][1] and stores in n
            //for getting user input i.e the number of previous command to rerun the command
            int n = atoi(&arr[0][1]);
            if (!(n <= record_count) || (n < 1 && n > 15))
            {
                printf("Command not in history\n");
            }

            else
            {
                if (record_count < 15)
                {
                    record_count = record_count - 1;
                    strcpy(cmd_str, record[n - 1]);
                    strcpy(record[record_count++], cmd_str);
                }
                else
                {
                    record_count = record_count - 1;
                    strcpy(cmd_str, record[n - 2]);
                    strcpy(record[record_count++], cmd_str);
                }
            }
        }

        char *working_str = strdup(cmd_str);
        // we are going to move the working_str pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *working_root = working_str;

        // Tokenize the input strings with whitespace used as the delimiter
        while (((argument_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
               (token_count < MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
            if (strlen(token[token_count]) == 0)
            {
                token[token_count] = NULL;
            }
            token_count++;
        }

        //if the command is cd then strcmp compares it with cd and
        //if it is true return 0
        //chdir changes the command directory to the one user inputs
        //make sure any command will be handled
        //show error if command is not valid
        if (strcmp(token[0], "cd") == 0)
        {
            int x = chdir(token[1]);

            if (x == -1)
            {
                perror("Command not found\n");
            }

            continue;
        }

        //the strcmp compares user-input command with history string
        //if it is true
        //list the history of the commands that user listed
        //it would be for 15 recent commands or less than 15

        else if (strcmp(token[0], "history") == 0)
        {

            for (i = 0; i < record_count; i++)
            {
                printf("%d: %s\n", i + 1, record[i]);
            }
        }

        //the strcmp compares user-input command with showpids string
        //if it is true
        //display the pids of the newly created process
        //it would be for 15 or less than 15

        else if (strcmp(token[0], "showpids") == 0)
        {
            for (i = 0; i < pid_count; i++)
            {
                printf("%d: %d\n", i + 1, pid_history[i]);
            }
        }

        //the strcmp compares user-input command with exit or quit string and
        //if it is true terminates the program i.e exit from the shell

        else if (strcmp(token[0], "quit") == 0 || strcmp(token[0], "exit") == 0)
        {
            break;
        }

        // fork creates a new process called child process
        // for every fork(), it gets updated with the latest pid.
        else
        {

            pid_t pid = fork();
            //returns 0 for child
            if (pid == 0)
            {
                // Notice you can add as many NULLs on the end as you want
                //make sure any command will be handled
                //show error if command is not valid
                int ret = execvp(token[0], &token[0]);
                if (ret == -1)
                {
                    printf("%s: Invalid command\n", token[0]);
                }
                exit(1);
            }

            //return child pid to the parent
            else
            {
                //used to track up to 15 pids or
                //for less than 15 newly created processes's pid
                //pid of child process is stored in an array

                if (pid_count <= 14)
                {
                    pid_history[pid_count++] = pid;
                }

                //for more than 15 processes, the pid of each new process is added in an array
                // the older process's pid is replaced consecutively
                else
                {
                    for (i = 0; i < 14; i++)
                    {
                        pid_history[i] = pid_history[i + 1];
                    }
                    //the new process's is added and it's pid is stored to the end of an array
                    pid_history[14] = pid;
                }

                int status;
                //wait for child to finish first
                wait(&status);
            }
        }
        //deallocate the memory
        //free up the space of working_root
        free(working_root);
    }

    return 0;
}