#include <stdio.h>
#include <sys/stat.h>
#include "daemon.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int daemonize(){
	pid_t pid;
	if((pid = fork()) < 0){perror("fork");return 1;}
	else if(pid > 0){exit(EXIT_SUCCESS);}


	setsid();
	umask(077);
	
	if(chdir("/") != 0){perror("chdir");return 1;}

	for(int i=0;i<1024;i++){
		close(i);
	}
	int fd1 = open("/dev/null", O_RDWR);
	int fd2 = dup(0);
	int fd3 = dup(0);

	return 0;

}
