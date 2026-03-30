#ifndef USER_H
#define USER_H

#include "common.h"

// UPDATED FUNCTION SIGNATURE
void add_user(int id, char *name, char *password, int role, int approved);

int login(char *name, char *password, User *logged_user);

void view_users();

void approve_employee(int id);

void delete_user(int id);

#endif
