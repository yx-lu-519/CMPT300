#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <math.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_LENGTH 10
#define MAXSIZE 1024

char hist[HISTORY_LENGTH][COMMAND_LENGTH];
int commandIndex=1;
bool flag=0;//false
bool signalInt=0;
/**
 * Command Input and Processing
 */

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */
void searchHis(char*buff);
void recHistory(char* buf);
int tokenize_command(char *buff, char *tokens[]);
void read_command(char *buff, char *tokens[], _Bool *in_background);
void printInt(int arg);
void printHistory();


/* Signal handler function */

void recHistory(char* buf){
	if(commandIndex>10){
		for (int i=9;i>0;i--)
		{
			strcpy(hist[9-i], hist[9-i+1]); 
		}
		strcpy(hist[9],buf);
	}
	else{
		strcpy(hist[commandIndex-1],buf);
	}
	commandIndex++;
}
int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	
	if(signalInt==1){	
		return 0;//return if receive signal ^c
	}

	if(buff!=NULL){
		if(buff[0]=='!'){
			searchHis(buff);
			if(flag)
				recHistory(buff);
		}
		else		
			recHistory(buff);
	}

	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	return token_count;
}

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL tergetcwd()minated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
	*in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

	if (length < 0&&errno != EINTR) {
		perror("Unable to read command from keyboard. Terminating.\n");
		exit(-1);
	}

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
}
void printInt(int arg){
	char temp[1024];
	sprintf(temp,"%d",arg);
	write(STDOUT_FILENO, temp, strlen(temp));
}

void printHistory(){
	if(commandIndex>=10){
		for(int i=0;i<=9;i++){
			printInt(commandIndex-8+i);
			write(STDOUT_FILENO, "\t", strlen("\t"));
			write(STDOUT_FILENO, hist[i], strlen(hist[i]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
	}
	else{
		for(int i=0;i<commandIndex-1;i++){
			printInt(i+1);
			write(STDOUT_FILENO, "\t", strlen("\t"));
			write(STDOUT_FILENO, hist[i], strlen(hist[i]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}			
	}
}
//5.1 !! & !n
void searchHis(char*buff){
	char cmd[1024];
	flag=0;//use flag to check input is valid or not
	if(commandIndex<=1){
		write(STDOUT_FILENO, "Error: No previous command\n", strlen("Error: No previous command\n"));
	}
	else if(buff[1]=='\0'){
		write(STDOUT_FILENO, "Error: No argument\n", strlen("Error: No argument\n"));
	}
	else if(buff[1]=='!'){
		if(buff[2]=='\0'){
			if(commandIndex<10){
				strcpy(buff, hist[commandIndex-2]);
				flag=1;
			}
			else{
				strcpy(buff, hist[9]);
				flag=1;  
			}		
		}
		else
			write(STDOUT_FILENO, "Error: Wrong command\n", strlen("Error: Wrong command\n"));
	}
	else{
		int i=1;
		while(true){
			if(buff[i]=='\0'){
				cmd[i-1]='\0';
				break;
			}
			else{
				cmd[i-1]=buff[i];
				i++;
			}	
		}
		int cmdIndex=atoi(cmd);
		//printInt(cmdIndex); //cheack the Index
		if(cmdIndex<1||cmdIndex>=commandIndex||cmdIndex<commandIndex-10){
			write(STDOUT_FILENO, "Error: Out of bounds\n", strlen("Error: Out of bounds\n"));
		}
		else if(commandIndex<10){
			strcpy(buff,hist[cmdIndex-1]);
			flag=1;
		}
		else{
			strcpy(buff,hist[9-((commandIndex-1)-cmdIndex)]);
			flag=1;		
		}
	}
}
void handle_SIGINT(){
	signalInt=1;
}

/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[]){


	/* set up the signal handler */
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	handler.sa_flags =0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT, &handler, NULL);

	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];

	while (true) {
		int status;
		pid_t fpid;
		size_t size=sizeof(char)*MAXSIZE;
		char* buf=(char*)malloc(size);
		char* cwd;

		//4 Internal Commands
		cwd=getcwd(buf,size);
		if(cwd==NULL){
			perror("Error 01");
		}
		write(STDOUT_FILENO,cwd,strlen(cwd));
		free(buf);
		// Get command
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().
		write(STDOUT_FILENO, "> ", strlen("> "));
		_Bool in_background = false;

		read_command(input_buffer, tokens, &in_background);

		//write(STDOUT_FILENO, hist[histIndex][0], strlen(hist[histIndex][0]));	
	
		if(signalInt==1){
			write(STDOUT_FILENO, "\n", strlen("\n"));
			printHistory();
			signalInt=0;
			continue;
		}		

		// DEBUG: Dump out arguments:
		for (int i = 0; tokens[i] != NULL; i++) {
			write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
			write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
		if (in_background) {
			write(STDOUT_FILENO, "Run in background.\n", strlen("Run in background.\n"));
		}

		if(tokens[0]!=NULL){
			if(tokens[0][0]=='!'){
				continue;// !! & !n have been solved in tokenize_command
			}
			else{
				//4 Internal Commands
				//exit
				if(strcmp(tokens[0],"exit")==0)
					exit(0);
				//pwd
				else if(strcmp(tokens[0],"pwd")==0){
					char buf[1024];
					cwd=getcwd(buf,sizeof(buf));
					if(cwd==NULL){
						perror("Error 01");
					}
					write(STDOUT_FILENO,cwd,strlen(cwd));
					write(STDOUT_FILENO,"\n",strlen("\n"));
					continue;
				}
				//cd
				else if(strcmp(tokens[0],"cd")==0){
					if (chdir(tokens[1])<0)
						perror("Error 01");
					continue;
				}	
				//history
				else if(strcmp(tokens[0],"history")==0){
					printHistory();
					continue;
				}
			}
		}

		/**
		 * Steps For Basic Shell:
		 * 1. Fork a child process
		 * 2. Child process invokes execvp() using results in token array.
		 * 3. If in_background is false, parent waits for
		 *    child to finish. Otherwise, parent loops back to
		 *    read_command() again immediately.
		 */
		fpid=fork();
		if(fpid<0){
			write(STDOUT_FILENO," fork fails ",strlen(" fork fails "));
			exit(1);
		}
		if(fpid==0){
			if(execvp(tokens[0], tokens)==-1){
				write(STDOUT_FILENO,"command not found or errors\n",strlen("command not found or errors\n"));
				exit(1);
			}
			exit(0);
		}
		else{
			if (in_background){
				while (waitpid(-1, &status, WNOHANG) > 0);
			}
			else{
				waitpid(fpid, &status, 0);
			}
		}
	}
	return 0;
}
