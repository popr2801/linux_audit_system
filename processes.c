#include "processes.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>

#define GREEN  "\033[92m"
#define RED    "\033[91m"
#define RESET  "\033[0m"


int is_pid_directory(const char *name)
{
    if (*name == '\0') {
        return 0;
    }
    for (const char *c = name; *c != '\0'; c++) {
        if (!isdigit((unsigned char)*c)) {
            return 0;
        }
    }

    return 1;
}

const char *get_timestamp(){
	static char timestamp[64];
	time_t now = time(NULL);
	struct tm *tm_info = localtime(&now);

	strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S", tm_info);
	return timestamp;
}

enum State parse_state(char state){
    switch (state) {
        case 'R':
            return RUNNING;

        case 'S':
            return SLEEP;
        case 'I':
        	return IDLE;
        case 'D':
            return UNINTERRUPTIBLE_SLEEP;

        case 'Z':
            return ZOMBIE;

        case 'T':
        case 't':
            return STOPPED;

        default:
            return UNKNOWN;
    }
}

const char *convert_state_enum(enum State state){
	switch (state){
	case RUNNING:
		return "Running";
	case SLEEP:
        return "Sleeping";

    case IDLE:
        return "Idle";

    case UNINTERRUPTIBLE_SLEEP:
        return "Uninterruptible Sleep";

    case ZOMBIE:
        return "Zombie";

    case STOPPED:
        return "Stopped";

    case UNKNOWN:
    default:
        return "Unknown";

}
}



int load_process_by_pid(pid_t pid, Process *process){
	char path[64],line[256],state;
	FILE *file;
	unsigned int ppid;
	unsigned int uid;
	unsigned int gid;
	snprintf(path,sizeof(path),"/proc/%d/status",(int)pid);
	
	if((file = fopen(path,"r")) == NULL){return -1;}

	process->pid = pid;

	while(fgets(line,sizeof(line),file) != NULL){
		if(sscanf(line,"Name: %255s",process->proc_name) == 1){continue;}
		if(sscanf(line,"State: %c",&state) == 1){
			process->state = parse_state(state);
			continue;
		}
		if(sscanf(line,"PPid: %u",&ppid) == 1){
			process->ppid = (pid_t)ppid;
			continue;
		}
		if(sscanf(line,"Uid: %u",&uid) == 1){
			process->uid = (uid_t)uid;
			continue;
		}
		if(sscanf(line,"Gid: %u",&gid) == 1){
			process->gid = (gid_t)gid;
			continue;
		}
	}
	fclose(file);
	return 0;
}

int load_processes(Process process[],size_t max_processes){
	size_t index = 0;
	DIR *dir = opendir("/proc");
	if(dir == NULL){return -1;}

	struct dirent *entry;

	while((entry = readdir(dir)) != NULL){
		if(!is_pid_directory(entry->d_name)){continue;}
		if(index >= max_processes){ fprintf(stderr, "Maximum process limit reached\n");
    break;}

		if(load_process_by_pid(atoi(entry->d_name),&process[index])==0){
			index++;
		}


	}
	closedir(dir);
	return (int)index;
}

int print_process(Process *process, FILE *log){
	if(process == NULL || log == NULL){return -1;}

	struct passwd *pw = getpwuid(process->uid);

	fprintf(log,"PID: %d\n",process->pid);
	fprintf(log,"Name: %s\n",process->proc_name);
	fprintf(log,"State: %s\n",convert_state_enum(process->state));
	fprintf(log,"PPID: %d\n",process->ppid);
	fprintf(log,"UID: %u (%s)\n",(unsigned)process->uid,pw ? pw->pw_name : "unknown");
	fprintf(log,"GID: %d\n",process->gid);
	fprintf(log,"-----------------------------\n");

	return 0;
}

void new_process_detected(const Process *process, FILE *log){
	struct passwd *pw = getpwuid(process->uid);

	fprintf(log,GREEN "[%s] Process %d(%s) started by %s (UID %u)\n" RESET,
		get_timestamp(),
		process->pid,
		process->proc_name,
		pw ? pw->pw_name : "unknown",
		(unsigned)process->uid);
}

void terminated_process_detected(const Process *process,FILE *log){

	fprintf(log, RED "[%s] Process %d(%s) terminated\n" RESET,
		get_timestamp(),
		process->pid,
		process->proc_name);
}

int detect_new_processes(const Process previous[],size_t previous_count,const Process current[],size_t current_count,FILE *log){
	if(previous == NULL || current == NULL){return -1;}
	
	for(int i = 0; i < current_count; i++){
		int found = 0;
		for(int j = 0; j < previous_count; j++){
			if(current[i].pid == previous[j].pid){found = 1;break;}
		}
		if(!found){new_process_detected(&current[i],log);}
	}
	for(int i = 0; i < previous_count; i++){
		int found = 0;
		for(int j = 0; j < current_count; j++){
			if(previous[i].pid == current[j].pid){found = 1;break;}
		}
		if(!found){terminated_process_detected(&previous[i],log);}
	}

	return 0;

}