#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include "system_info.h"
#include "daemon.h"




int main(){

	if(daemonize() == -1){return -1;}
	FILE *log, *pid_file;
	if(load_files(&log,&pid_file) < 0){perror("load_files");return -1;}
	SystemInfo info;
	
	while(1){
		get_system_info(&info);
		print_system_info(&info,log);
		sleep(3600);
	}

	fclose(log);
	unlink("/tmp/daemon.pid");
	return 0;
}
