#ifndef PROCESSES_H
#define PROCESSES_H
#include <sys/types.h>
#include <limits.h>
#include <stdio.h>


enum State{
	RUNNING,
	SLEEP,
	UNINTERRUPTIBLE_SLEEP,
	IDLE,
	ZOMBIE,
	STOPPED,
	UNKNOWN
};


typedef struct{
	pid_t pid;
	gid_t gid;
	pid_t ppid;
	uid_t uid;
	char proc_name[256];
	char exe_path[PATH_MAX];
	char cmdline[4096];
	enum State state;
}Process;

const char *convert_state_enum(enum State state);

int load_processes(Process processes[],size_t max_processes);

int load_process_by_pid(pid_t pid, Process *process);
enum State parse_state(char state);

int print_process(Process *process,FILE *log);

int detect_new_processes(
	const Process process[],
	int process_count,
	Process new_processes[],
	int max_new_processes);

#endif