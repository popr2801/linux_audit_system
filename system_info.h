#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
#include <stdio.h>


typedef struct{
	char hostname[256];
	char kernel[256];
	char os_name[128];
	char architecture[64];
	char current_user[64];

	char cpu_model[256];
	unsigned int cpu_cores;
	
	unsigned total_ram;
	unsigned long free_ram;
	
	unsigned long uptime;
}SystemInfo;

int get_system_info(SystemInfo *info);
void print_system_info(SystemInfo *info,FILE *output);

#endif
