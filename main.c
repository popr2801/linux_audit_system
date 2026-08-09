#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include "system_info.h"
#include "daemon.h"

int main(){

	FILE *log = fopen("/tmp/auditor.log","a");
	if(log == NULL){perror("fopen");return 1;}
	if(daemonize() == -1){fclose(log);return 1;}
	fprintf(log, "Passed daemonize()\n");
	while(1){
		sleep(5);
		SystemInfo info;
		get_system_info(&info);
		print_system_info(&info,log);
	}

	fclose(log);

	return 0;
}
