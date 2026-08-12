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

int is_pid_directory(const char *name);

int load_processes(Process processes[],size_t max_processes);

int load_process_by_pid(pid_t pid, Process *process);
enum State parse_state(char state);

int print_process(Process *process,FILE *log);

int detect_new_processes(
	const Process previous[], // processes for previous scan
	size_t previous_count,
	const Process current[] // processes from this scan
	size_t current_count,
	Process new_processes[], // processes that are in current[] but not in previous[]
	size_t max_new_processes);
// for the function above track each change and log into log_file, for example:
// PROCESS EVENTS: [12:00:03] Process created : PID 1821, nginx, UID 0
// [12:04:17] Process terminated : PID 1821, nginx, UID 0


#endif