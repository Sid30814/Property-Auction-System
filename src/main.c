#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#include "../include/user.h"
#include "../include/property.h"
#include "../include/bid.h"
#include "../include/common.h"

// Check if user exists
int user_exists(char *name) {
    int fd = open("data/users.dat", O_RDONLY | O_CREAT, 0666);

    if(fd < 0) return 0;

    User u;

    while(read(fd, &u, sizeof(User)) > 0) {
        if(strcmp(u.name, name) == 0) {
            close(fd);
            return 1;
        }
    }

    close(fd);
    return 0;
}

// Clear input buffer
void clear_input() {
    while(getchar() != '\n');
}

int main() {

    // Initialize admin + manager (only once)
    if(!user_exists("admin")) {
        add_user(1, "admin", "admin123", 0, 1);
    }

    if(!user_exists("manager")) {
        add_user(2, "manager", "manager123", 1, 1);
    }

    int main_choice;

    while(1) {
        printf("\n====== Property Auction System ======\n");
        printf("1. Login\n");
        printf("2. Register User\n");
        printf("3. Register Employee\n");
        printf("4. Exit\n");

        printf("Enter choice: ");

        if(scanf("%d", &main_choice) != 1) {
            printf("Invalid input! Enter a number.\n");
            clear_input();
            continue;
        }

        clear_input();

        // ================= LOGIN =================
        if(main_choice == 1) {
            User current_user;
            char name[50], password[50];

            printf("Enter username: ");
            scanf("%s", name);
            clear_input();

            printf("Enter password: ");
            scanf("%s", password);
            clear_input();

            if(!login(name, password, &current_user)) {
                printf("Login failed!\n");
                continue;
            }

            printf("Login successful! Role: %d\n", current_user.role);

            int choice;

            // ================= ADMIN =================
            if(current_user.role == 0) {
                while(1) {
                    printf("\n====== Admin Menu ======\n");
                    printf("1. View Users\n");
                    printf("2. Exit\n");

                    printf("Enter choice: ");

                    if(scanf("%d", &choice) != 1) {
                        clear_input();
                        continue;
                    }

                    clear_input();

                    if(choice == 1) {
                        view_users();
                    }
                    else if(choice == 2) {
                        break;
                    }
                    else {
                        printf("Invalid choice!\n");
                    }
                }
            }

            // ================= MANAGER =================
            else if(current_user.role == 1) {
                while(1) {
                    printf("\n====== Manager Menu ======\n");
                    printf("1. Approve Employee\n");
                    printf("2. View Users\n");
                    printf("3. Exit\n");

                    printf("Enter choice: ");

                    if(scanf("%d", &choice) != 1) {
                        clear_input();
                        continue;
                    }

                    clear_input();

                    if(choice == 1) {
                        int id;
                        printf("Enter employee_id to approve: ");

                        if(scanf("%d", &id) != 1) {
                            clear_input();
                            continue;
                        }

                        clear_input();
                        approve_employee(id);
                    }
                    else if(choice == 2) {
                        view_users();
                    }
                    else if(choice == 3) {
                        break;
                    }
                    else {
                        printf("Invalid choice!\n");
                    }
                }
            }

            // ================= EMPLOYEE =================
            else if(current_user.role == 2) {
                while(1) {
                    printf("\n====== Employee Menu ======\n");
                    printf("1. Add Property\n");
                    printf("2. View Properties\n");   // ✅ NEW
                    printf("3. Exit\n");

                    printf("Enter choice: ");

                    if(scanf("%d", &choice) != 1) {
                        clear_input();
                        continue;
                    }

                    clear_input();

                    if(choice == 1) {
                        int id, price;
                        char title[50];

                        long start = time(NULL);
                        long end = start + 3600;

                        printf("Enter property_id title base_price: ");

                        if(scanf("%d %s %d", &id, title, &price) != 3) {
                            clear_input();
                            continue;
                        }

                        clear_input();

                        add_property(id, title, price, start, end);
                        printf("Property added successfully!\n");
                    }
                    else if(choice == 2) {
                        view_all_properties();   // ✅ NEW
                    }
                    else if(choice == 3) {
                        break;
                    }
                    else {
                        printf("Invalid choice!\n");
                    }
                }
            }

            // ================= USER =================
            else if(current_user.role == 3) {
                while(1) {
                    printf("\n====== User Menu ======\n");
                    printf("1. Join Auction\n");
                    printf("2. View My Properties\n");
                    printf("3. Exit\n");

                    printf("Enter choice: ");

                    if(scanf("%d", &choice) != 1) {
                        clear_input();
                        continue;
                    }

                    clear_input();

                    if(choice == 1) {
                        int property_id;

                        printf("Enter property_id: ");
                        if(scanf("%d", &property_id) != 1) {
                            clear_input();
                            continue;
                        }

                        clear_input();

                        auction_session(property_id, current_user.user_id);
                    }
                    else if(choice == 2) {
                        view_my_properties(current_user.user_id);
                    }
                    else if(choice == 3) {
                        break;
                    }
                    else {
                        printf("Invalid choice!\n");
                    }
                }
            }
        }

        // ================= REGISTER USER =================
        else if(main_choice == 2) {
            int id;
            char name[50], pass[50];

            printf("Enter user_id name password: ");

            if(scanf("%d %s %s", &id, name, pass) != 3) {
                clear_input();
                continue;
            }

            clear_input();

            add_user(id, name, pass, 3, 1);
        }

        // ================= REGISTER EMPLOYEE =================
        else if(main_choice == 3) {
            int id;
            char name[50], pass[50];

            printf("Enter employee_id name password: ");

            if(scanf("%d %s %s", &id, name, pass) != 3) {
                clear_input();
                continue;
            }

            clear_input();

            add_user(id, name, pass, 2, 0);
            printf("Employee registered. Waiting for manager approval.\n");
        }

        // ================= EXIT =================
        else if(main_choice == 4) {
            printf("Exiting system...\n");
            break;
        }

        else {
            printf("Invalid choice!\n");
        }
    }

    return 0;
}
