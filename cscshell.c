/*	Csc 360
*	Assign #2
*	Shell Program
*	
*	Dustin Chang

*
*/
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

void chomp(char *line) {	/*Deals with end of char* characters for situations below*/
    int len;
    if(line==NULL) {
		return;
    }
    len = strlen(line);
    if(line[len-1] == ']') {
    	line[len-1] = '\0';
    }
    len = strlen(line);
    if(line[len-1] == '\n') {
		line[len-1] = '\0';
    }
    len = strlen(line);
    if(line[len-1] == ',') {
    	line[len-1] = '\0';
    }
    len = strlen(line);
    if(line[len-1] == '\r') {
		line[len-1] = '\0';
    }
}

void prompt(char* host, char* cwd, char* scan_prompt) {
	gethostname(host, 100);
	getcwd(cwd, 100);
	printf("%s%s$\n", host, cwd);		/*Name/CurrWorkingDir*/
	fgets(scan_prompt, 100, stdin);		/*Cmd stored in scan_prompt*/
	chomp(scan_prompt);
	/*printf("prompt:%s", scan_prompt);*/
}

void re_initialize(char* host, char* cwd, char* scan_prompt, char* token, char* cmd_in[8], char* cmd_in_cpy, char* cmd_in_cpy2, char* pth, char* cat, char* token_cat, int* path_count) {	/*Reinitialize variables needed for need commands*/
	int i;
	for(i=0; i<8; i++) {		/*Reintializing for new commands incoming*/
		cmd_in[i] = (char*) malloc(sizeof(char)* 100);
	}
	host = (char*) malloc(sizeof(char)* 100);
	cwd = (char*) malloc(sizeof(char)* 100);
	scan_prompt = (char*) malloc(sizeof(char)* 100);
	token = (char*) malloc(sizeof(char)* 100);
	cmd_in_cpy = (char*) malloc(sizeof(char)* 100);	/*Might not need*/
	cmd_in_cpy2 = (char*) malloc(sizeof(char)* 100);
	pth = (char*) malloc(sizeof(char)* 100);
	*path_count = 0; 	/*Need for reinitialization, and needs * for the dereferencing*/
	cat = (char*) malloc(sizeof(char)* 100);
	token_cat = (char*) malloc(sizeof(char)* 100);
}

/*Parsing through the cmd's*/
void parsing(char* cmd_in[8], char* cmd_in_cpy, char* cmd_in_cpy2, char* scan_prompt, char* spac, int tokD) {
	int leng;
	int brek;			/*To know if a continue happened*/
	brek = 0;
	cmd_in_cpy = strtok(scan_prompt, " ");
	strcpy(cmd_in[tokD], cmd_in_cpy);
	while(cmd_in_cpy != NULL) {			/*token through scan_prompt*/
		cmd_in_cpy = strtok(NULL, " ");
		if(cmd_in_cpy != NULL) {
			leng = strlen(cmd_in_cpy);
		} else {
			tokD++;
			cmd_in[tokD] = NULL;
			break;
		}
		if(cmd_in_cpy[0] == '[') {				/*If first arg has [ at the front*/
			cmd_in_cpy++;
			leng = strlen(cmd_in_cpy);
			if(cmd_in_cpy[leng-1] == ']') {		/*If the first arg has ] at the back*/
				chomp(cmd_in_cpy);
			} else {					/*If there is no Qoute at the back of the str*/
				strcpy(cmd_in_cpy2, cmd_in_cpy);	/*REWRTIE*/
				brek = 1;
				continue;
			}
		}
		if(brek == 1) {		/*If there was no qoute at back and then checks the next item/str */
			if(cmd_in_cpy[leng-1] == ']') {	/*If there is now a back qoute*/
				chomp(cmd_in_cpy);
				strcat(cmd_in_cpy2, spac);
				strcat(cmd_in_cpy2, cmd_in_cpy);
				tokD++;
				strcpy(cmd_in[tokD], cmd_in_cpy2);	/*Problem*/
				brek = 0;
				continue;
			} else {		/*To cover case when the string has 3 or more, so that there will be no \" at the end of the 2nd word/item. */
				strcat(cmd_in_cpy2, spac);
				strcat(cmd_in_cpy2, cmd_in_cpy);
				continue;
			}
		}
		tokD++;
		strcpy(cmd_in[tokD], cmd_in_cpy);
	}
}

void execute(char* pth, char* pth_cpy, char* path_array[100], char* token, char* token_cat, char* cat, char* slash, char* cmd_in[8], int path_count, int tokD) {
	/*Find exec file corresponding to cmd and execute it*/
	int path_count_total;
	int path_track;				/*Keep track of tokening through paths*/
	pth = getenv("PATH");
	strcpy(pth_cpy, pth);		/*Copy path so not overwritten*/
	path_array[path_count] = strtok(pth_cpy, ":");
	while(path_array[path_count] != NULL) {
		path_count++;
		path_array[path_count] = strtok(NULL, ":");
	}
	path_count_total = path_count;
	path_count = 0;
	path_track = 0;
	tokD = 0;		/*tokD gets reintialized to 0 here and then doesn't increment before next loop so no need to reinitialize at the top*/
	while(path_count < path_count_total) {
		path_track = 0;			/*Reintialize path_track to 0*/
		token_cat = (char*) malloc(sizeof(char)* 100);	/*Reinitialize for next path*/
		token = strtok(path_array[path_count], "/");
		strcat(token_cat, slash);
		strcat(token_cat, token);
		while(token != NULL) {
			cat = (char*) malloc(sizeof(char)* 100);
			strcat(cat, token_cat);
			strcat(cat, slash);
			strcat(cat, cmd_in[tokD]);
			if(access(cat, F_OK) == 0) {	/*Code for if file exists*/
				pid_t pid = fork();
				if(pid == 0) {
					if(execv(cat, cmd_in) < 0) {
						perror("Error occured at execv");
					}
				} else {
					while(pid != wait(0));
					break;		/*Maybe continue*/
				}

			}

			strcat(token_cat, slash);		/*HERE is problem*/
			token = strtok(NULL, "/");
			if(token != NULL) {
				strcat(token_cat, token);		/*SEG FAULTING*/
				path_track++;
			}
		}
		path_count++;
	}/*Outer while loop for exe part*/
}

void change_dir(char* cmd_in[8], char* ch_dir, char* slash, int cmp_str) {
	getcwd(ch_dir, 100);
	strcat(ch_dir, slash);
	strcat(ch_dir, cmd_in[1]);
	cmp_str = chdir(ch_dir);
}

void pipe_parser(char* cmd_in[8], char* pipe_cmds[8], int pipe_size) {
	int j;
	for(j=0; j<8; j++) {		/*Intializing space*/
		pipe_cmds[j] = (char*) malloc(sizeof(char)* 100);
	}
	int leng;
	int pipe_count;
	pipe_count = 0;
	int first_time_through;
	first_time_through = 0;
	int space;		/*space to use for when a space between cmd's*/
	space = 0;
	int last;		/*Put a flag is the last value was a comma*/
	last = 0;
	char* cmd1_cpy = (char*) malloc(sizeof(char)* 100);		/*Copy of the pipe cmd's*/
	char* cmd1_cpy_token = (char*) malloc(sizeof(char)* 100);
	char* cmd2_cpy_token = (char*) malloc(sizeof(char)* 100);
	strcpy(cmd1_cpy, cmd_in[1]);
	cmd1_cpy_token = strtok(cmd1_cpy, " ");
	do {
		if(first_time_through != 0) {
			if(last != 1) {
				cmd1_cpy_token = strtok(NULL, " ");
			}
		}
		first_time_through++;
		if(cmd1_cpy_token != NULL) {
			leng = strlen(cmd1_cpy_token);
		} else {
			/*Else not needed cause it is caught at the end if next strtok=NULL*/
		}
		if(cmd1_cpy_token[leng-1] == ',') {
			chomp(cmd1_cpy_token);
			last = 1;
		} else {
			if(space == 0) {
				strcpy(cmd2_cpy_token, cmd1_cpy_token);
				space = 1;
			} else {
				strcat(cmd2_cpy_token, cmd1_cpy_token);
				space = 1;
			}
		}
		if(space == 1) {
			if((pipe_size-1) == pipe_count) {
				last = 1;
				cmd1_cpy_token = strtok(NULL, " ");
				if(cmd1_cpy_token == NULL) {
					strcpy(pipe_cmds[pipe_count], cmd2_cpy_token);
					pipe_count++;
					pipe_cmds[pipe_count] = NULL;
					break;
				} else {
					strcat(cmd2_cpy_token, " ");
					continue;
				}
			}
			if(last != 1) {
				strcat(cmd2_cpy_token, " ");
				continue;
			}
		}

		strcat(cmd2_cpy_token, cmd1_cpy_token);
		strcpy(pipe_cmds[pipe_count], cmd2_cpy_token);
		pipe_count++;
		last = 0;
		space = 0;
		strcpy(cmd2_cpy_token, "");
	} while(cmd1_cpy_token != NULL);
}

void parse_str_to_array(char* pipe_cmd_cpy, char* pipe_in[8]) {
	char* token = (char*) malloc(sizeof(char)* 100);
	int a;
	a = 0;
	token = strtok(pipe_cmd_cpy, " ");
	strcpy(pipe_in[a], token);
	a++;
	while(token != NULL) {
		token = strtok(NULL, " ");
		if(token != NULL) {
			strcpy(pipe_in[a], token);
		} else {
			pipe_in[a] = NULL;
		}
		a++;
	}
}

void forking(int in, int out, char* pipe_cmds[8], int arrayToParse) {	/*0=stdin, 1=stdout, 2=stderr*/
	/*Fork here*/
	/*Intiailizations for execute*/
	char* pth = (char*) malloc(sizeof(char)* 100);
	char* pth_cpy = (char*) malloc(sizeof(char)* 100);
	char* path_array[100];
	char* token  = (char*) malloc(sizeof(char)* 100);
	char* token_cat = (char*) malloc(sizeof(char)* 100);
	char* cat = (char*) malloc(sizeof(char)* 100);
	char* slash = "/";
	int path_count_tokD;
	path_count_tokD = 0;
	/**/
	char* pipe_cmd_cpy = (char*) malloc(sizeof(char)* 100);
	char* pipe_in[8];	/*Used as the array for execute instead of cmd_in*/
	int j;
	for(j=0; j<8; j++) {	/*Initialize space in array*/
		pipe_in[j] = (char*) malloc(sizeof(char)* 100);
	}
	strcpy(pipe_cmd_cpy, pipe_cmds[arrayToParse]);
	parse_str_to_array(pipe_cmd_cpy, pipe_in);
	pid_t pid;
	if((pid = fork()) == 0) {
		if(in != 0) {
			dup2(in, 0);
			close(in);
		}
		if(out != 1) {
			dup2(out, 1);
			close(out);
		}
		execute(pth, pth_cpy, path_array, token, token_cat, cat, slash, pipe_in, path_count_tokD, path_count_tokD);
	}
}

void pipe_fork(int pipe_size, char* pipe_cmds[8]) {	/*0=stdin, 1=stdout, 2=stderr*/
	/*Where forking occurs*/
	char* pth = (char*) malloc(sizeof(char)* 100);
	char* pth_cpy = (char*) malloc(sizeof(char)* 100);
	char* path_array[100];
	char* token  = (char*) malloc(sizeof(char)* 100);
	char* token_cat = (char*) malloc(sizeof(char)* 100);
	char* cat = (char*) malloc(sizeof(char)* 100);
	char* slash = "/";
	int path_count_tokD;
	path_count_tokD = 0;
	char* pipe_cmd_cpy = (char*) malloc(sizeof(char)* 100);
	char* pipe_in[8];	/*Used as the array for execute instead of cmd_in*/
	int j;
	for(j=0; j<8; j++) {	/*Initialize space in array*/
		pipe_in[j] = (char*) malloc(sizeof(char)* 100);
	}
	int k, in, fd[2];
	pid_t pid_A, pid_B;
	in = 0;
	FILE *fp;
	char line[130];

	for(k=0; k<(pipe_size-1); k++) {
		pipe(fd);

		if(k == (pipe_size-2)) {
			if( !(pid_A = fork()) ) {
				strcpy(pipe_cmd_cpy, pipe_cmds[k]);
				close(1);
				dup(fd[1]);
				close(fd[1]);
				close(fd[0]);
				parse_str_to_array(pipe_cmd_cpy, pipe_in);
				execute(pth, pth_cpy, path_array, token, token_cat, cat, slash, pipe_in, path_count_tokD, path_count_tokD);
			}
			/*Reinitialize*/
			pth = (char*) malloc(sizeof(char)* 100);
			pth_cpy = (char*) malloc(sizeof(char)* 100);
			token  = (char*) malloc(sizeof(char)* 100);
			token_cat = (char*) malloc(sizeof(char)* 100);
			cat = (char*) malloc(sizeof(char)* 100);
			for(j=0; j<8; j++) {	/*Initialize space in array*/
				pipe_in[j] = (char*) malloc(sizeof(char)* 100);
			}
			path_count_tokD = 0;

			if( !(pid_B = fork()) ) {
				close(0);
				dup(fd[0]);
				close(fd[1]);
				close(fd[0]);
				strcpy(pipe_cmd_cpy, pipe_cmds[k+1]);
				parse_str_to_array(pipe_cmd_cpy, pipe_in);
				execv("/usr/bin", pipe_in);

				pclose(fp);
			}
			close(fd[0]);
			close(fd[1]);
			waitpid(pid_B, NULL, 0);
			waitpid(pid_A, NULL, 0);
		}
	}
}

void pipe_func(char* cmd_in[8]) {	/*0=stdin, 1=stdout, 2=stderr*/
	/*Do pipe stuff*/
	char* pipe_cmds[8];
	int pipe_size;
	pipe_size = 0;
	int i;
	char* pipecmd_cpy =(char*) malloc(sizeof(char)* 100);
	char* pipecmd_cpy2 =(char*) malloc(sizeof(char)* 100);
	strcpy(pipecmd_cpy, cmd_in[1]);
	pipecmd_cpy2 = strtok(pipecmd_cpy, ",");
	pipe_size++;
	for(i=1; i<8; i++) {		/*To check for pipe size*/
		pipecmd_cpy2 = strtok(NULL, ",");
		if(pipecmd_cpy2 == NULL) {
			break;
		}
		pipe_size++;
	}

	pipe_parser(cmd_in, pipe_cmds, pipe_size);

	/*Do pipe call to some function*/
	pipe_fork(pipe_size, pipe_cmds);


}

void cut_first_element_of_array(char* cmd_in[8], char* cm_in[8], int* elements) {
	int z;
	z = 0;
	while(cmd_in[z+1] != NULL) {
		strcpy(cm_in[z], cmd_in[z+1]);
		z++;
		*elements = z;
	}
	cm_in[z] = NULL;
}

void pipe_func2(char* cmd_in[8]) {
	FILE *fp;
	char line[150];
	int len;
	int x;
	x = 0;
	int flag;
	flag = 0;
	char* token = (char*) malloc(sizeof(char)* 100);
	char* token_cpy = (char*) malloc(sizeof(char)* 100);
	char* token2 = (char*) malloc(sizeof(char)* 100);
	strcpy(token_cpy, "");
	int cmd_count;
	cmd_count = 0;
	char* cmd_pipe_cpy[8];
	for(x=0; x<8; x++) {	/*This array is just used so i can cut the first pipe cmd off it*/
		cmd_pipe_cpy[x] = (char*) malloc(sizeof(char)* 100);
	}
	x = 0;
	cut_first_element_of_array(cmd_in, cmd_pipe_cpy, &x);

	x = 0;	/*Have to reintialize cause of for loop*/
	while(cmd_in[x] != NULL) {	/*Counts # of commands that are not NULL*/
		cmd_count++;
		x++;
	}

	char* pipe_array[cmd_count];
	for(x=0; x<cmd_count; x++) {	/*Initialize space for pipe_array*/
		pipe_array[x] = (char*) malloc(sizeof(char)* 100);
	}
	x = 0;
	token = strtok(cmd_pipe_cpy[0], " ");
	//for(x=0; x<cmd_count; x++) {
	while(token != NULL) {
		strcat(token_cpy, token);
		len = strlen(token_cpy);
		if(token_cpy[len-1] == ',') {
			chomp(token_cpy);
			strcat(token_cpy, " | ");
			x++;
			//token_cpy = (char*) malloc(sizeof(char)* 100);
		} else {
			if(flag == 1) {
				//strcpy(pipe_array[x], token_cpy);
				x++;
				//token_cpy = (char*) malloc(sizeof(char)* 100);
			}
			strcat(token_cpy, " ");
			flag = 1;
		}
		token = strtok(NULL, " ");
		chomp(token);
	}
	//printf("Pipe stuff:%s:%s\n", pipe_array[0], pipe_array[1]);
	
	fp = popen(token_cpy, "r");
	
	while(fgets(line, sizeof line, fp)) {
		printf("%s\n", line);
	}

	pclose(fp);
	 
}

void arrayToString(char* newCMD_cpy, char* newCMD[8]) {	/*Send and array in and puts int into a string in newCMD_cpy*/
	int h;
	h = 0;
		while(newCMD[h] != NULL)  {
			if(newCMD[h+1] == NULL) {
				strcat(newCMD_cpy, newCMD[h]);
			} else {
				strcat(newCMD_cpy, newCMD[h]);
				strcat(newCMD_cpy, " ");
			}
			h++;
		}
}

void update_history(char* newCMD[8], char* history[5]) {
	/*update history here*/
	char* newCMD_cpy = (char*) malloc(sizeof(char)* 100);
	char* history_cpy[5];
	int h;
	for(h=0; h<5; h++) {
		history_cpy[h] = (char*) malloc(sizeof(char)* 100);
	}
	for(h=0; h<5; h++) {		/*Copy history to history_cpy*/
		strcpy(history_cpy[h], history[h]);
	}
	arrayToString(newCMD_cpy, newCMD);
	if(strcmp("<empty>", history[0]) == 0) {
		strcpy(history[0], newCMD_cpy);
	} else {
		strcpy(history[0], newCMD_cpy);
		for(h=0; h<4; h++) {		/*Only 4 because the first one is now the newCMD*/
			strcpy(history[h+1], history_cpy[h]);
		}
	}
}

void histlist(char* history[5]) {
	int h;
	for(h=0; h<5; h++) {
		printf("Histlist%d:%s\n", h, history[h]);
	}
}

void handle_sigchld(int sig) {
	while(waitpid((pid_t)(-1), 0, WNOHANG) > 0) {
		printf("Child terminated\n");
	}		/*Unless printing something*/
}

void update_listback(char* bglist[5], char* cmd_num_str) {
	int u;
	for(u=0; u<5; u++){
		if(strcmp(bglist[u], cmd_num_str) == 0) {
			strcpy(bglist[u], "<empty>");
		}
	}	

}

void cmdNumber(char* bglist[5], char* cmd_num_str, int* cmd_num) {
	int c;
	for(c=0; c<5; c++){
		if(strcmp(bglist[c], cmd_num_str) == 0) {
			*cmd_num = c;
		}
	}
}

void goback(char* bglist[5], char* cmd_in[8], char* pth, char* pth_cpy, char* path_array[100], char* token, char* token_cat, char* cat, char* slash, int path_count) {
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	char* cmd_num_str = (char*) malloc(sizeof(char)* 100);
	int cmd_num;
	cmd_num = 0;
	int elements;
	elements = 0;
	char* cm_in[8];
	int y;
	for(y=0; y<8; y++) {
		cm_in[y] = (char*) malloc(sizeof(char)* 100);
	}
	cut_first_element_of_array(cmd_in, cm_in, &elements);
	arrayToString(cmd_num_str, cmd_in);

	pid_t pid;
	struct sigaction sa;
	sa.sa_handler = &handle_sigchld;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if(sigaction(SIGCHLD, &sa, 0) == -1) {
		printf("IN ERROR\n");
		perror(0);
		exit(1);
	}
	if((pid = fork()) > 0) {	/*Parent*/
		/*Only forked so that child could execute*/
	} else {					/*Child*/
		sleep(8);
		printf("The command was: %s\n", cm_in[0]);
		printf("The arg's were:");
		for(y=0; y<(elements-1); y++) {
			printf(" %s", cm_in[y+1]);
		}
		timeinfo = localtime(&rawtime);
		cmdNumber(bglist, cmd_num_str, &cmd_num);
		printf("\nThe time bg process completed is:%s\n", asctime(timeinfo));
		execute(pth, pth_cpy, path_array, token, token_cat, cat, slash, cm_in, path_count, path_count);

		exit(0);
	}

}

void histrun(char* bglist[5], char* history[5], char* cmd_in[8], char* pth, char* pth_cpy, char* path_array[100], char* token, char* token_cat, char* cat, char* slash, int path_count) {
	char* history_cpy = (char*) malloc(sizeof(char)* 100);
	char* hist_place = (char*) malloc(sizeof(char)* 100);
	char* history_cmpr = (char*) malloc(sizeof(char)* 100);
	char* c_in[8];
	int x;
	for(x=0; x<8; x++) {
		c_in[x] = (char*) malloc(sizeof(char)* 100);
	}
	strcpy(hist_place, cmd_in[1]);		/*cmd_in = histrun %d*/
	x = atoi(hist_place);
	strcpy(history_cpy, history[x]);
	parse_str_to_array(history_cpy, c_in);		/*Gets command from history and stores into history_cpy*/
	if(strcmp(history[x], "<empty>") == 0 || strcmp("histrun", c_in[0]) == 0) {
		printf("Error, history position unavailable\n");
	} else {
		if(strcmp(c_in[0], "goback") == 0) {
			goback(bglist, c_in, pth, pth_cpy, path_array, token, token_cat, cat, slash, path_count);
		} else {
			execute(pth, pth_cpy, path_array, token, token_cat, cat, slash, c_in, path_count, path_count);
		}
	}
}

void listback(char* bglist[5]) {
	int lb;
	for(lb=0; lb<5; lb++) {
		if(strcmp(bglist[lb], "<empty>") != 0) {
			printf("ListBack%d:%s\n", lb, bglist[lb]);
		}
	}
}


int main() {
	/*Initializations*/
	char* bglist[5];
	char* history[5];
	int h;
	for(h=0; h<5; h++) {
		history[h] = (char*) malloc(sizeof(char)* 100);
		bglist[h] = (char*) malloc(sizeof(char)* 100);
	}
	for(h=0; h<5; h++) {
		strcpy(history[h], "<empty>");
		strcpy(bglist[h], "<empty>");
	}
	char* host = (char*) malloc(sizeof(char)* 100);
	char* cwd = (char*) malloc(sizeof(char)* 100);
	char* scan_prompt = (char*) malloc(sizeof(char)* 100);
	char* token = (char*) malloc(sizeof(char)* 100);
	char* token_cat = (char*) malloc(sizeof(char)* 100);	/*Need for strcat*/
	char* cmd_in[8];
	char* cmd_in_cpy = (char*) malloc(sizeof(char)* 100);
	char* cmd_in_cpy2 = (char*) malloc(sizeof(char)* 100);
	int tokD;	/*Used for testing|Possbily use to keep track of how many cmd's*/
	tokD = 0;
	char* pth = (char*) malloc(sizeof(char)* 100);
	char* pth_cpy = (char*) malloc(sizeof(char)* 100);
	char* path_array[100];
	int path_count;			/*Used to keep track of number of paths*/
	path_count = 0;
	char* cat = (char*) malloc(sizeof(char)* 100);		/*Need for strcat*/
	char* slash = "/";
	char* spac = " ";
	char* ch_dir = (char*) malloc(sizeof(char)* 100);	/*Used to store the path to chdir*/
	int cmp_str;	/*Used to store
 value when comparing str's | the chdir value*/

	/*LOOP*/
	while(1) {/*While loop for my terminal*/
		re_initialize(host, cwd, scan_prompt, token, cmd_in, cmd_in_cpy, cmd_in_cpy2, pth, cat, token_cat, &path_count);
		printf("%s\n", "csc360Shell bootup");

		/*Prompt here*/
		prompt(host, cwd, scan_prompt);

		parsing(cmd_in, cmd_in_cpy, cmd_in_cpy2, scan_prompt, spac, tokD);
		if(strcmp(scan_prompt, "histrun") == 0) {
			histrun(bglist, history, cmd_in, pth, pth_cpy, path_array, token, token_cat, cat, slash, path_count);
			update_history(cmd_in, history);
			continue;

		}
		if(strcmp(scan_prompt, "histlist") != 0) {
			update_history(cmd_in, history);
		}

		cmp_str = strcmp(cmd_in[0], "cd");
		if(cmp_str == 0) {
			change_dir(cmd_in, ch_dir, slash, cmp_str);
		} else {
			cmp_str = strcmp(cmd_in[0], "pipe");
			if(cmp_str == 0) {
				pipe_func2(cmd_in);
			} else {
				cmp_str = strcmp(cmd_in[0], "histlist");
				if(cmp_str == 0) {
					histlist(history);
				} else {
					if(strcmp(cmd_in[0], "goback") == 0) {
						update_history(cmd_in, bglist);
						goback(bglist, cmd_in, pth, pth_cpy, path_array, token, token_cat, cat, slash, path_count);
					} else {
						if(strcmp(cmd_in[0], "listback") == 0) {
							listback(bglist);
						} else {

							/*Executing the cmd's*/
							execute(pth, pth_cpy, path_array, token, token_cat, cat, slash, cmd_in, path_count, tokD);	/*Might not need path_count in here, just init in execute()*/
						}
					}
				}
			}/*End of else of pipe*/
		}/*End of else for cd*/


	}/*For while loop of my terminal*/
	return 0;
}