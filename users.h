#ifndef USER_H
#define USER_H
#include "processes.h"
#include <stdio.h>
#include <sys/types.h>


typedef struct{
	uid_t uid;
	gid_t gid;

	char username[64];
	char home[128];
	char shell[128];	

}User;

typedef struct{
	uid_t uid;

	size_t process_count;
	size_t running_count;
	size_t sleeping_count;

}UserStats;


int load_users(User users[],size_t max_users);
void print_users(const User users[],size_t count,FILE *log);
const User *find_user_by_uid(const User users[],size_t users_count,uid_t uid);
void get_user_stats(const Process processes[],size_t process_count,uid_t uid,UserStats *stats);



#endif
