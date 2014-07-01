/*	Csc 360
*	Assign #2
*	Shell Program
*	
*
*
*/
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void chomp(char *line) {	/*Added to get rid of newline at end of str's*/
    int len;
    if(line==NULL) {
		return;
    }
    len = strlen(line);
    if(line[len-1] == '\n') {
		line[len-1]= '\0';
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
	int len;							/*These lines for testing, except chomp*/
	len = strlen(scan_prompt);
	if(scan_prompt[len-1] == '\n') {
		printf("%s\n", "YES THERE IS A NEWLINE");
	}
	chomp(scan_prompt);
	/*printf("prompt:%s", scan_prompt);*/
}

int main() {
	/*Initializations*/
	char* host = (char*) malloc(sizeof(char)* 100);
	char* cwd = (char*) malloc(sizeof(char)* 100);
	char* scan_prompt = (char*) malloc(sizeof(char)* 100);
	char* token = (char*) malloc(sizeof(char)* 100);
	char* cmd_in[8];
	int tokD;	/*Used for testing|Possbily use to keep track of how many cmd's*/
	tokD = 0;
	char* pth = (char*) malloc(sizeof(char)* 100);
	char* pth_cpy = (char*) malloc(sizeof(char)* 100);
	char* path_array[100];	/*Might not need*/
	int path_count;			/*Used to keep track of number of paths*/
	path_count = 0;
	int path_count_total;
	int path_track;			/*Keep track of tokening through paths*/
	char* cat = (char*) malloc(sizeof(char)* 100);		/*Need for strcat*/
	char* slash = "/";
	char* token_cat = (char*) malloc(sizeof(char)* 100);	/*Need for strcat*/
	int acc;	/*For TESTING*/
	acc = 0;
	int brek;	/*To know if a break happened*/
	brek = 0;

	/*LOOP*/
	while(1) {/*For while*/
	host = (char*) malloc(sizeof(char)* 100);
	cwd = (char*) malloc(sizeof(char)* 100);
	scan_prompt = (char*) malloc(sizeof(char)* 100);
	token = (char*) malloc(sizeof(char)* 100);
	tokD = 0;
	pth = (char*) malloc(sizeof(char)* 100);
	path_count = 0;
	cat = (char*) malloc(sizeof(char)* 100);
	token_cat = (char*) malloc(sizeof(char)* 100);
	acc = 0;
	brek = 0;	/*Still need to implement but fix loop first!!!!!!!!!!!!!!!*/



	printf("%s\n", "cscShell bootup");

	/*Prompt up to here*/
	prompt(host, cwd, scan_prompt);

	/*Parsing*/
	cmd_in[tokD] = strtok(scan_prompt, " ");
	while(cmd_in[tokD] != NULL) {
		printf("cmd%d:%s\n",tokD, cmd_in[tokD]);
		tokD++;
		cmd_in[tokD] = strtok(NULL, " ");
	}
	printf("cmd:%d Last:%s\n",tokD, cmd_in[tokD]);	/*TESTING for NULL*/

	/*Find exec file corresponding to cmd*/
	pth = getenv("PATH");
	strcpy(pth_cpy, pth);	/*Copy path so not overwritten*/
	printf("This Path#%s\nCopy Path#%s\n", pth, pth_cpy);
	path_array[path_count] = strtok(pth_cpy, ":");
	/*printf("FIRST=%s\n", path_array[path_count]);*/
	while(path_array[path_count] != NULL) {
		printf("P%d=%s\n", path_count, path_array[path_count]);
		path_count++;
		path_array[path_count] = strtok(NULL, ":");
	}
	/*printf("Last%d=%s\n",path_count, path_array[path_count]);*/	/*TESTING:Should be NULL*/
	path_count_total = path_count;
	/*printf("%s%d\n", "Path count tot=", path_count_total);*/	/*TESTING*/
	path_count = 0;
	path_track = 0;
	tokD = 0;
	while(path_count < path_count_total) {
		path_track = 0;			/*Reintialize path_track to 0*/
		printf("C%d=%s\t", path_count, path_array[path_count]);
		token_cat = (char*) malloc(sizeof(char)* 100);	/*Reinitialize for next path*/
		token = strtok(path_array[path_count], "/");
		strcat(token_cat, slash);
		strcat(token_cat, token);
		/*printf("CurrToken=%s\n", path_array[path_count]);*/
		printf("\n");
		while(token != NULL) {
			acc = 0;		/*TESTING*/
			printf("TOK#%d=%s\t", path_track, token_cat);
			cat = (char*) malloc(sizeof(char)* 100);
			strcat(cat, token_cat);
			strcat(cat, slash);
			strcat(cat, cmd_in[tokD]);
			printf("The pathFile=%s\n", cat);		/* cat=path/file */
			/*DO CHECKING FOR FILE HERE OF CAT AS THE STRING*/
			/*acc = access(cat, F_OK);		TESTING
			printf("acc=%d\tstrlen=%d", acc, (int)strlen(cat));*/
			if(access(cat, F_OK) == 0) {
				/*Do code for if it matches*/
				printf("The file exists in path:%s\n", cat);
				/*NOW MAKE A CHILD PROCESS TO EXECUTE FILE/OPTIONS/ARGS*/
				pid_t pid = fork();
				printf("I'm a fork pid:%d\n", pid);
				if(pid == 0) {
					printf("I'm a child:%d\n", pid);
					if(execv(cat, cmd_in) < 0) {
						perror("Error occured at execv");
					}
				} else {
					printf("I'm pear:%d\n", pid);
					while(pid != wait(0));
					printf("After while=%d\n", pid);
					break;		/*Maybe continue*/
					/*return 0;*/
				}


			}

			printf("Token before strtok:%s\t", token);
			strcat(token_cat, slash);		/*HERE is problem*/
			token = strtok(NULL, "/");
			printf("TokCat_Tok:%s+%s\n", token_cat, token);			
			if(token != NULL) {
				strcat(token_cat, token);		/*SEG FAULTING*/
				printf("TOKEN:%s\tTOKENCAT:%s\n", token, token_cat);
				path_track++;
			}
		}
		/*path_track++;*/
		printf("OUTTTT pCount:%d\tpTotal:%d\n", path_count, path_count_total);
		path_count++;
	}/*Outer while loop for exe part*/


}/*For while*/
	printf("%s\n", "DONE");
	return 0;
}