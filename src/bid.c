#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/select.h>

#include "../include/common.h"
#include "../include/lock.h"
#include "../include/bid.h"

// 🔥 Get base price of property
int get_base_price(int property_id) {
    int fd = open("data/properties.dat", O_RDONLY);

    if(fd < 0) return 0;

    Property p;

    while(read(fd, &p, sizeof(Property)) > 0) {
        if(p.property_id == property_id) {
            close(fd);
            return p.base_price;
        }
    }

    close(fd);
    return 0;
}

// Get highest bid
int get_max_bid(int property_id, int *winner) {
    int fd = open("data/bids.dat", O_RDONLY | O_CREAT, 0666);

    Bid b;
    int max = 0;
    int win = -1;

    while(read(fd, &b, sizeof(Bid)) > 0) {
        if(!b.is_deleted && b.property_id == property_id) {
            if(b.amount > max) {
                max = b.amount;
                win = b.user_id;
            }
        }
    }

    close(fd);

    if(winner) *winner = win;
    return max;
}

// Mark property sold
void mark_property_sold(int property_id, int winner, int price) {

    int fd = open("data/properties.dat", O_RDWR);

    Property p;
    off_t pos;

    while(read(fd, &p, sizeof(Property)) > 0) {

        if(p.property_id == property_id) {

            pos = lseek(fd, 0, SEEK_CUR) - sizeof(Property);

            p.is_sold = 1;
            p.winner_id = winner;
            p.final_price = price;
            p.is_active = 0;

            lseek(fd, pos, SEEK_SET);
            write(fd, &p, sizeof(Property));
            break;
        }
    }

    close(fd);
}

// 🔥 Live auction with base price validation
void auction_session(int property_id, int user_id) {

    int fd = open("data/bids.dat", O_RDWR | O_CREAT | O_APPEND, 0666);

    printf("\n🔥 Live Auction Started\n");

    time_t last_bid_time = time(NULL);
    int last_seen = -1;

    int base_price = get_base_price(property_id);

    printf("💰 Base Price: %d\n", base_price);

    while(1) {

        int winner;
        int current_max = get_max_bid(property_id, &winner);

        if(current_max != last_seen) {
            printf("\n📢 Highest Bid: %d (User %d)\n", current_max, winner);
            last_seen = current_max;
        }

        time_t now = time(NULL);

        if(difftime(now, last_bid_time) >= 60) {
            printf("\n⏱️ Auction ended!\n");
            printf("🏆 Winner: User %d with bid %d\n", winner, current_max);

            mark_property_sold(property_id, winner, current_max);
            break;
        }

        fd_set set;
        struct timeval timeout;

        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);

        if(rv > 0) {
            int amount;
            printf("Enter bid: ");

            if(scanf("%d", &amount) != 1) {
                while(getchar() != '\n');
                continue;
            }

            lock_file(fd);

            // 🔥 NEW LOGIC
            if(current_max == 0) {
                // First bid
                if(amount < base_price) {
                    printf("❌ Bid must be at least base price (%d)\n", base_price);
                    unlock_file(fd);
                    continue;
                }
            } else {
                // Normal bidding
                if(amount <= current_max) {
                    printf("❌ Must be higher than %d\n", current_max);
                    unlock_file(fd);
                    continue;
                }
            }

            Bid b;
            b.bid_id = now % 100000;
            b.property_id = property_id;
            b.user_id = user_id;
            b.amount = amount;
            b.timestamp = now;
            b.is_deleted = 0;

            write(fd, &b, sizeof(Bid));

            printf("✅ Bid placed: %d\n", amount);

            last_bid_time = time(NULL);

            unlock_file(fd);
        }
    }

    close(fd);
}
