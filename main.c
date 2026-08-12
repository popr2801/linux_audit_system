#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include "system_info.h"
#include "daemon.h"
#include "processes.h"
#include <string.h>

#define MAX_PROCESSES 4096




int main(){

	if(daemonize() == -1){return -1;}
	FILE *log, *pid_file;
	if(load_files(&log,&pid_file) < 0){perror("load_files");return -1;}
	SystemInfo info;
	static Process previous[MAX_PROCESSES];
	static Process current[MAX_PROCESSES];
	
	get_system_info(&info);
	print_system_info(&info,log);

	size_t previous_count = 0;

	while(1){
		int count = load_processes(current,MAX_PROCESSES);

		if(count < 0){fprintf(log,"Failed to load processes\n");}
		else{
			size_t current_count = (size_t)count;
			if(previous_count > 0){
				detect_new_processes(previous,previous_count,current,current_count,log);
			}

			memcpy(previous,current,current_count * sizeof(Process));
			previous_count = current_count;

		}

		fflush(log);
		sleep(10);
	}

	fclose(log);
	unlink("/tmp/daemon.pid");
	return 0;
}
