#include "system_info.h"
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

int get_system_info(SystemInfo *info){
	struct utsname name;
	if(uname(&name) == -1){perror("uname");return -1;}
	strcpy(info->os_name,name.sysname);
	strcpy(info->hostname,name.nodename);
	snprintf(info->kernel,sizeof(info->kernel),"%s Version: %s",name.release,name.version);
	strcpy(info->architecture,name.machine);
	return 0;
}
void print_system_info(SystemInfo *info, FILE *output){
	fprintf(output,"Hostname: %s\n",info->hostname);
	fprintf(output,"Kernel: %s\n",info->kernel);
	fprintf(output,"OS Name: %s\n",info->os_name);
	fprintf(output,"Architecture: %s\n",info->architecture);
	fprintf(output,"Current User: %s\n",info->current_user);

	fflush(output);
}
