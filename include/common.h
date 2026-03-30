#ifndef COMMON_H
#define COMMON_H

#include <time.h>

typedef struct {
    int user_id;
    char name[50];
    char password[50];
    int role;      // 0=Admin,1=Manager,2=Employee,3=User
    int approved;
} User;

typedef struct {
    int property_id;
    char title[50];
    int base_price;
    long start_time;
    long end_time;
    int is_active;

    int is_sold;
    int winner_id;
    int final_price;

} Property;

typedef struct {
    int bid_id;
    int property_id;
    int user_id;
    int amount;
    long timestamp;
    int is_deleted;
} Bid;

#endif
