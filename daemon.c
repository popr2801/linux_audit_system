#include <stdio.h>
#include <sys/stat.h>
#include "daemon.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int daemonize(){
	pid_t pid;
	if((pid = fork()) < 0){perror("fork");return -1;}
	else if(pid > 0){exit(EXIT_SUCCESS);}


	if((setsid() < 0)){perror("setsid");return -1;}
	

	pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(077);
	
	if(chdir("/") != 0){perror("chdir");return -1;}

	for(int i=0;i<1024;i++){
		close(i);
	}
	int fd1 = open("/dev/null", O_RDWR);
	int fd2 = dup(0);
	int fd3 = dup(0);

	return 0;
}

int load_files(FILE **log,FILE **pid_file){
	*log = fopen("/tmp/auditor.log","a");
	if(log == NULL){perror("fopen");return -1;}

	*pid_file = fopen("/tmp/daemon.pid","w");
	if(*pid_file == NULL){perror("fopen");fclose(*log);*log = NULL;return -1;}
	fprintf(*pid_file,"%d",getpid());
	fclose(*pid_file);

	return 0;
}
