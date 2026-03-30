#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "../include/common.h"

// Add property
void add_property(int id, char *title, int price, long start, long end) {
    int fd = open("data/properties.dat", O_WRONLY | O_CREAT | O_APPEND, 0666);

    Property p;
    p.property_id = id;
    strcpy(p.title, title);
    p.base_price = price;
    p.start_time = start;
    p.end_time = end;
    p.is_active = 1;

    p.is_sold = 0;
    p.winner_id = -1;
    p.final_price = 0;

    write(fd, &p, sizeof(Property));
    close(fd);

    printf("Property added successfully!\n");
}

// View properties won by user
void view_my_properties(int user_id) {

    int fd = open("data/properties.dat", O_RDONLY);

    if(fd < 0) return;

    Property p;
    int found = 0;

    printf("\n🏠 Properties Won:\n");

    while(read(fd, &p, sizeof(Property)) > 0) {

        if(p.is_sold && p.winner_id == user_id) {
            printf("Property ID: %d | %s | Price: %d\n",
                   p.property_id, p.title, p.final_price);
            found = 1;
        }
    }

    if(!found) {
        printf("No properties won yet.\n");
    }

    close(fd);
}

// 🔥 NEW: View all properties (for employee)
void view_all_properties() {

    int fd = open("data/properties.dat", O_RDONLY);

    if(fd < 0) {
        printf("No properties found.\n");
        return;
    }

    Property p;

    printf("\n📋 All Properties:\n");

    while(read(fd, &p, sizeof(Property)) > 0) {

        printf("ID:%d | %s | Base:%d | ",
               p.property_id, p.title, p.base_price);

        if(p.is_sold) {
            printf("STATUS: SOLD | Winner: %d | Price: %d\n",
                   p.winner_id, p.final_price);
        } else {
            printf("STATUS: UNSOLD\n");
        }
    }

    close(fd);
}
