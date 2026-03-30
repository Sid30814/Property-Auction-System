#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "../include/common.h"
#include "../include/user.h"

// Add user
void add_user(int id, char *name, char *password, int role, int approved) {
    int fd = open("data/users.dat", O_WRONLY | O_CREAT | O_APPEND, 0666);

    User u;
    u.user_id = id;
    strcpy(u.name, name);
    strcpy(u.password, password);
    u.role = role;
    u.approved = approved;

    write(fd, &u, sizeof(User));
    close(fd);

    printf("User registered successfully!\n");
}

// Login
int login(char *name, char *password, User *logged_user) {
    int fd = open("data/users.dat", O_RDONLY | O_CREAT, 0666);

    User u;

    while(read(fd, &u, sizeof(User)) > 0) {
        if(strcmp(u.name, name) == 0 && strcmp(u.password, password) == 0) {

            if(u.approved == 0) {
                printf("Account not approved yet!\n");
                close(fd);
                return 0;
            }

            *logged_user = u;
            close(fd);
            return 1;
        }
    }

    close(fd);
    return 0;
}

// View users
void view_users() {
    int fd = open("data/users.dat", O_RDONLY);

    User u;

    printf("\n--- Users ---\n");

    while(read(fd, &u, sizeof(User)) > 0) {
        if(u.approved != -1) { // skip deleted users
            printf("ID:%d Name:%s Role:%d Approved:%d\n",
                   u.user_id, u.name, u.role, u.approved);
        }
    }

    close(fd);
}

// ✅ FIXED approve employee
void approve_employee(int target_id) {
    int fd = open("data/users.dat", O_RDWR);

    if(fd < 0) {
        perror("File error");
        return;
    }

    User u;
    off_t pos;

    while(read(fd, &u, sizeof(User)) > 0) {

        if(u.user_id == target_id && u.role == 2) {

            pos = lseek(fd, 0, SEEK_CUR) - sizeof(User);

            u.approved = 1;

            lseek(fd, pos, SEEK_SET);
            write(fd, &u, sizeof(User));

            printf("Employee approved!\n");

            close(fd);
            return;
        }
    }

    printf("Employee not found!\n");
    close(fd);
}

// Delete user (safe)
void delete_user(int target_id) {
    int fd = open("data/users.dat", O_RDWR);

    User u;
    off_t pos;

    while(read(fd, &u, sizeof(User)) > 0) {

        if(u.user_id == target_id) {

            if(u.role == 0 || u.role == 1) {
                printf("❌ Cannot delete Admin or Manager!\n");
                close(fd);
                return;
            }

            pos = lseek(fd, 0, SEEK_CUR) - sizeof(User);

            u.approved = -1;

            lseek(fd, pos, SEEK_SET);
            write(fd, &u, sizeof(User));

            printf("User deleted successfully\n");
            close(fd);
            return;
        }
    }

    close(fd);
}
