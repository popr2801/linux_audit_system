#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include "system_info.h"
#include "daemon.h"
#include "processes.h"




int main(){

	if(daemonize() == -1){return -1;}
	FILE *log, *pid_file;
	if(load_files(&log,&pid_file) < 0){perror("load_files");return -1;}
	SystemInfo info;
	size_t max_processes = 256;
	Process processes[max_processes];
	
	// this happens every hour

	while(1){
		get_system_info(&info);
		print_system_info(&info,log);
		int count = load_processes(processes,max_processes);
		fflush(log);
		sleep(3600); // probably change the scan time to 10 seconds 
	}

	fclose(log);
	unlink("/tmp/daemon.pid");
	return 0;
}
