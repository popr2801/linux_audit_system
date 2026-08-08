#ifndef USER_H
#define USER_H

typedef struct{
	char username[64];
	int uid;

}UserInfo;

int load_users(UserInfo users[],int max_users);
void print_users(const UserInfo users[],int count);

#endif
