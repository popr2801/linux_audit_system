#include "system_info.h"
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>




double get_memory_usage(unsigned long *total_ram,unsigned long *available_ram){
	FILE *mem_file = fopen("/proc/meminfo","r");
	if (mem_file == NULL) {
    	perror("fopen");
    	return -1;
	}
	*total_ram = 0;
	*available_ram = 0;
	char line[256];
	while(fgets(line,sizeof(line),mem_file) != NULL){
		if(sscanf(line,"MemTotal: %lu kB",total_ram) == 1){continue;}
		if(sscanf(line,"MemAvailable: %lu kB",available_ram) == 1){continue;}
		if (*total_ram > 0 && *available_ram > 0){break;}
	}
	fclose(mem_file);
	if (*total_ram == 0 || *available_ram == 0){return -1.0;}
	double memory_usage = 100.0*(*total_ram - *available_ram)/ *total_ram;

	return memory_usage;
}


int get_system_info(SystemInfo *info){
	struct utsname name;
	if(uname(&name) == -1){perror("uname");return -1;}

	strcpy(info->os_name,name.sysname);
	strcpy(info->hostname,name.nodename);
	snprintf(info->kernel,sizeof(info->kernel),"%s Version: %s",name.release,name.version);
	strcpy(info->architecture,name.machine);
	
	info->memory_usage = get_memory_usage(&info->total_ram,&info->available_ram);
	
	return 0;
}
void print_system_info(SystemInfo *info, FILE *log){

	time_t now = time(NULL);
	struct tm *tm_info = localtime(&now);
	fprintf(log,"Last log ~ [%04d-%02d-%02d %02d:%02d:%02d]\n",
		tm_info->tm_year + 1900,
        tm_info->tm_mon + 1,
        tm_info->tm_mday,
        tm_info->tm_hour,
        tm_info->tm_min,
        tm_info->tm_sec);

	fprintf(log,"Hostname: %s\n",info->hostname);
	fprintf(log,"Kernel: %s\n",info->kernel);
	fprintf(log,"OS Name: %s\n",info->os_name);
	fprintf(log,"Architecture: %s\n",info->architecture);
	fprintf(log,"Current User: %s\n",info->current_user);
	fprintf(log,"Memory Usage: %.2f%%\n",info->memory_usage);

	fflush(log);
}
