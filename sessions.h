#ifndef SESSIONS_H
#define SESSIONS_H
#include <time.h>
#include <sys/types.h>
#include <stdlib.h>
#include "users.h"

#define MAX_USERS 128

typedef struct{
	uid_t uid;
	char username[64];
	char tty[64];
	char host[256];
	time_t time;
}Session;

size_t load_sessions(Session sessions[],size_t max_count);
bool convert_name_to_uid(const User users[],size_t user_count,char *username,uid_t *uid);
size_t get_sessions_by_uid(
	const Session sessions[],
	size_t session_count,
	uid_t uid,
	Session result[]);
void print_sessions(const Session sessions[],size_t session_count,FILE *log);

#endif