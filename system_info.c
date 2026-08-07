#include "system_info.h"
#include <stdio.h>

int get_system_info(SystemInfo *info){


	return 0;
}
void print_system_info(SystemInfo *info){
	printf("Hostname: %s\n",info->hostname);
	printf("Kernel: %s\n",info->kernel);
	printf("OS Name: %s\n",info->os_name);
	printf("Architecture: %s\n",info->architecture);
	printf("Current User: %s\n",info->current_user);

}
