/* All of rio function implementations are obtained from csapp.c */
/* Please note the usage of wrapper function */

/* $begin cpfile */
#include "csapp.c"
#define DEBUG 1
int main(int argc, char **argv) 
{
    int n;
    rio_t rio;
    char buf[MAXLINE];
    
    if (DEBUG){
    	printf("%s\n",argv[0]);
    	printf("%s\n",argv[1]);
    }
	if (argc>1) {
		FILE *file_pt1;
		file_pt1=fopen(argv[1],"r");
		if (file_pt1==NULL){
			puts("e: File read error!");
			exit(-10);
		}
		ssize_t file_pt1_no= fileno(file_pt1);
		
		Rio_readinitb(&rio, file_pt1_no);
		while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
			Rio_writen(STDOUT_FILENO, buf, n);
		exit(0);
	}

    Rio_readinitb(&rio, STDIN_FILENO); //no edit
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) //no-edit
	Rio_writen(STDOUT_FILENO, buf, n); //no edit
    exit(0);
}
/* $end cpfile */