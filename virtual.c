#include "oslab.h"

#define EMPTY -1
#define TRUE 1
#define FALSE 0

int free_frame_from ( int *frame_pool, int *frame_cnt ) {
    for ( int i = 0; i < *frame_cnt; i ++ ) {
        if ( frame_pool[i] > EMPTY ) {
            int temp = frame_pool[i];
            frame_pool[i] = EMPTY;
            return temp;
        }
    }
    return EMPTY;
}

int find_min ( struct PTE *page_table, int *table_cnt ) {
    int min_index = 0;
    for ( int i = 1; i < *table_cnt; i ++ ) {
        if ( page_table[i].is_valid ) {
            if ( page_table[i].arrival_timestamp < page_table[min_index].arrival_timestamp ) min_index = i;
        }
    }
    return min_index;
}

int find_min_last_access_timestamp ( struct PTE *page_table, int *table_cnt ) {
    int min_index = 0;
    for ( int i = 1; i < *table_cnt; i ++ ) {
        if ( page_table[i].is_valid ) {
            if ( page_table[i].last_access_timestamp < page_table[min_index].last_access_timestamp ) min_index = i;
        }
    }
    return min_index;
}



int find_min_reference_count ( struct PTE *page_table, int *table_cnt ) {
    int min_index = 0;
    for ( int i = 1; i < *table_cnt; i ++ ) {
        if ( page_table[i].is_valid ) {
            if ( page_table[i].reference_count < page_table[min_index].reference_count ) min_index = i;
        }
    }
    return min_index;
}

int process_page_access_fifo (
    struct PTE page_table[TABLEMAX],
    int *table_cnt, 
    int page_number, 
    int frame_pool[POOLMAX],
    int *frame_cnt, 
    int current_timestamp
) {
    if ( page_table[ page_number ].is_valid ) {
        page_table[ page_number ].last_access_timestamp = current_timestamp;
        page_table[ page_number ].reference_count++;
        return page_table[ page_number ].frame_number;
    }
    int frame_num;
    if ( (frame_num = free_frame_from( frame_pool, frame_cnt )) > EMPTY ) {
        page_table[ page_number ].is_valid = TRUE;
        page_table[ page_number ].frame_number = frame_num;
        page_table[ page_number ].arrival_timestamp = current_timestamp;
        page_table[ page_number ].last_access_timestamp = current_timestamp;
        page_table[ page_number ].reference_count++;
        return frame_num;
    }

    int page_with_min_arrival_timestamp = find_min(page_table, table_cnt);
    page_table[ page_with_min_arrival_timestamp ].is_valid = FALSE;
    page_table[ page_with_min_arrival_timestamp ].arrival_timestamp = 0;
    page_table[ page_with_min_arrival_timestamp ].last_access_timestamp = 0;
    page_table[ page_with_min_arrival_timestamp ].reference_count = 0;
    
    page_table[ page_number ].frame_number = page_table[ page_with_min_arrival_timestamp ].frame_number;
    page_table[ page_number ].arrival_timestamp = current_timestamp;
    page_table[ page_number ].last_access_timestamp = current_timestamp;
    page_table[ page_number ].reference_count++; 
    return page_table[ page_number ].frame_number;
}

int count_page_faults_fifo(
    struct PTE page_table[TABLEMAX],
    int table_cnt, 
    int refrence_string[REFERENCEMAX],
    int reference_cnt,
    int frame_pool[POOLMAX],
    int frame_cnt
) {
    int timestamp = 1, faults = 0;
    for ( int i = 0; i < reference_cnt; i ++ ) {
        if ( page_table [ refrence_string [ i ] ].is_valid ) {
            timestamp++;
            page_table[ refrence_string [ i ] ].last_access_timestamp = timestamp;
            page_table[ refrence_string [ i ] ].reference_count++;
        }
        else {
            int frame_num;
            if ( (frame_num = free_frame_from( frame_pool, &frame_cnt )) > EMPTY ) {
                page_table[ refrence_string [ i ] ].is_valid = TRUE;
                page_table[ refrence_string [ i ] ].frame_number = frame_num;
                page_table[ refrence_string [ i ] ].arrival_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].last_access_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].reference_count++;
            }
            else {
                int page_with_min_arrival_timestamp = find_min(page_table, &table_cnt);
                page_table[ page_with_min_arrival_timestamp ].is_valid = FALSE;
                page_table[ page_with_min_arrival_timestamp ].arrival_timestamp = 0;
                page_table[ page_with_min_arrival_timestamp ].last_access_timestamp = 0;
                page_table[ page_with_min_arrival_timestamp ].reference_count = 0;
                
                page_table[ refrence_string [ i ] ].frame_number = page_table[ page_with_min_arrival_timestamp ].frame_number;
                page_table[ refrence_string [ i ] ].arrival_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].last_access_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].reference_count++; 
            }
            faults++;
        }
    }
    return faults;
}

int process_page_access_lru(
    struct PTE page_table[TABLEMAX],
    int *table_cnt, 
    int page_number, 
    int frame_pool[POOLMAX],
    int *frame_cnt, 
    int current_timestamp
) {
    if ( page_table[ page_number ].is_valid ) {
        page_table[ page_number ].last_access_timestamp = current_timestamp;
        page_table[ page_number ].reference_count++;
        return page_table[ page_number ].frame_number;
    }
    int frame_num;
    if ( (frame_num = free_frame_from( frame_pool, frame_cnt )) > EMPTY ) {
        page_table[ page_number ].is_valid = TRUE;
        page_table[ page_number ].frame_number = frame_num;
        page_table[ page_number ].arrival_timestamp = current_timestamp;
        page_table[ page_number ].last_access_timestamp = current_timestamp;
        page_table[ page_number ].reference_count++;
        return frame_num;
    }

    int page_with_min_last_access_timestamp = find_min_last_access_timestamp(page_table, table_cnt);
    page_table[ page_with_min_last_access_timestamp ].is_valid = FALSE;
    page_table[ page_with_min_last_access_timestamp ].arrival_timestamp = 0;
    page_table[ page_with_min_last_access_timestamp ].last_access_timestamp = 0;
    page_table[ page_with_min_last_access_timestamp ].reference_count = 0;
    
    page_table[ page_number ].frame_number = page_table[ page_with_min_last_access_timestamp ].frame_number;
    page_table[ page_number ].arrival_timestamp = current_timestamp;
    page_table[ page_number ].last_access_timestamp = current_timestamp;
    page_table[ page_number ].reference_count++; 
    return page_table[ page_number ].frame_number;
}

int count_page_faults_lru(
    struct PTE page_table[TABLEMAX],
    int table_cnt, 
    int refrence_string[REFERENCEMAX],
    int reference_cnt,
    int frame_pool[POOLMAX],
    int frame_cnt
) {
    int timestamp = 1, faults = 0;
    for ( int i = 0; i < reference_cnt; i ++ ) {
        if ( page_table [ refrence_string [ i ] ].is_valid ) {
            timestamp++;
            page_table[ refrence_string [ i ] ].last_access_timestamp = timestamp;
            page_table[ refrence_string [ i ] ].reference_count++;
        }
        else {
            int frame_num;
            if ( (frame_num = free_frame_from( frame_pool, &frame_cnt )) > EMPTY ) {
                page_table[ refrence_string [ i ] ].is_valid = TRUE;
                page_table[ refrence_string [ i ] ].frame_number = frame_num;
                page_table[ refrence_string [ i ] ].arrival_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].last_access_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].reference_count++;
            }
            else {
                int page_with_min_last_access_timestamp = find_min_last_access_timestamp(page_table, &table_cnt);
                page_table[ page_with_min_last_access_timestamp ].is_valid = FALSE;
                page_table[ page_with_min_last_access_timestamp ].arrival_timestamp = 0;
                page_table[ page_with_min_last_access_timestamp ].last_access_timestamp = 0;
                page_table[ page_with_min_last_access_timestamp ].reference_count = 0;
                
                page_table[ refrence_string [ i ] ].frame_number = page_table[ page_with_min_last_access_timestamp ].frame_number;
                page_table[ refrence_string [ i ] ].arrival_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].last_access_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].reference_count++; 
            }
            faults++;
        }
    }
    return faults;
}

int process_page_access_lfu(
    struct PTE page_table[TABLEMAX],
    int *table_cnt, 
    int page_number, 
    int frame_pool[POOLMAX],
    int *frame_cnt, 
    int current_timestamp
) {
    if ( page_table[ page_number ].is_valid ) {
        page_table[ page_number ].last_access_timestamp = current_timestamp;
        page_table[ page_number ].reference_count++;
        return page_table[ page_number ].frame_number;
    }
    int frame_num;
    if ( (frame_num = free_frame_from( frame_pool, frame_cnt )) > EMPTY ) {
        page_table[ page_number ].is_valid = TRUE;
        page_table[ page_number ].frame_number = frame_num;
        page_table[ page_number ].arrival_timestamp = current_timestamp;
        page_table[ page_number ].last_access_timestamp = current_timestamp;
        page_table[ page_number ].reference_count++;
        return frame_num;
    }

    int page_with_min_reference_count = find_min_reference_count(page_table, table_cnt);
    page_table[ page_with_min_reference_count ].is_valid = FALSE;
    page_table[ page_with_min_reference_count ].arrival_timestamp = 0;
    page_table[ page_with_min_reference_count ].last_access_timestamp = 0;
    page_table[ page_with_min_reference_count ].reference_count = 0;
    
    page_table[ page_number ].frame_number = page_table[ page_with_min_reference_count ].frame_number;
    page_table[ page_number ].arrival_timestamp = current_timestamp;
    page_table[ page_number ].last_access_timestamp = current_timestamp;
    page_table[ page_number ].reference_count++; 
    return page_table[ page_number ].frame_number;
}

int count_page_faults_lfu(
    struct PTE page_table[TABLEMAX],
    int table_cnt, 
    int refrence_string[REFERENCEMAX],
    int reference_cnt,
    int frame_pool[POOLMAX],
    int frame_cnt
) {
    int timestamp = 1, faults = 0;
    for ( int i = 0; i < reference_cnt; i ++ ) {
        if ( page_table [ refrence_string [ i ] ].is_valid ) {
            timestamp++;
            page_table[ refrence_string [ i ] ].last_access_timestamp = timestamp;
            page_table[ refrence_string [ i ] ].reference_count++;
        }
        else {
            int frame_num;
            if ( (frame_num = free_frame_from( frame_pool, &frame_cnt )) > EMPTY ) {
                page_table[ refrence_string [ i ] ].is_valid = TRUE;
                page_table[ refrence_string [ i ] ].frame_number = frame_num;
                page_table[ refrence_string [ i ] ].arrival_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].last_access_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].reference_count++;
            }
            else {
                int page_with_min_reference_count = find_min_reference_count(page_table, &table_cnt);
                page_table[ page_with_min_reference_count ].is_valid = FALSE;
                page_table[ page_with_min_reference_count ].arrival_timestamp = 0;
                page_table[ page_with_min_reference_count ].last_access_timestamp = 0;
                page_table[ page_with_min_reference_count ].reference_count = 0;
                
                page_table[ refrence_string [ i ] ].frame_number = page_table[ page_with_min_reference_count ].frame_number;
                page_table[ refrence_string [ i ] ].arrival_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].last_access_timestamp = timestamp;
                page_table[ refrence_string [ i ] ].reference_count++; 
            }
            faults++;
        }
    }
    return faults;
}

/*************************************************************************************************************/

#include <stdio.h>

struct PTE page_table [ TABLEMAX ];
int page_count, frame_count;
int frame_pool[POOLMAX];
int refrence_string[REFERENCEMAX];
int reference_count;

int exit_prog = 1;

void load_page_count () {
    printf("ENTER PAGE TABLE COUNT: ");
    scanf("%d", &page_count);
}

void load_frame_count () {
    printf("ENTER FRAME COUNT: ");
    scanf("%d", &frame_count);
}

void load_reference_count () {
    printf("ENTER REFERENCE COUNT: ");
    scanf("%d", &reference_count);
}

void load_page_table () {
    for ( int i = 0; i < page_count; i ++ ) {
        printf("Page %d\n", i + 1 );
        printf("ENTER IV (0 or 1): ");
        scanf("%d", &page_table [ i ].is_valid );
        printf("ENTER FN: ");
        scanf("%d", &page_table [ i ].frame_number );
        printf("ENTER ATS: ");
        scanf("%d", &page_table [ i ].arrival_timestamp );
        printf("ENTER LATS: ");
        scanf("%d", &page_table [ i ].last_access_timestamp );
        printf("ENTER RC: ");
        scanf("%d", &page_table [ i ].reference_count );
        if ( page_table [ i ].is_valid > 1 ) page_table [ i ].is_valid = 1;
        else if ( page_table [ i ].is_valid < 0 ) page_table [ i ].is_valid = 0;
    }
}
void print_page_table () {
    for ( int i = 0; i < page_count; i ++ ) {
        printf("Page: %d IV: %d FN: %d ATS: %d LATS: %d RC: %d", i + 1, page_table [ i ].is_valid, page_table [ i ].frame_number, page_table [ i ].arrival_timestamp, page_table [ i ].last_access_timestamp, page_table [ i ].reference_count );
    }
}

void load_frame_pool () {
    for ( int i = 0; i < frame_count; i ++ ) {
        printf( "ENTER FRAME VALUE: " );
        scanf("%d", &frame_pool[i] );
    }
}

void load_reference () {
    for ( int i = 0; i < reference_count; i ++ ) {
        printf( "ENTER REFERENCE STRING %d: ", i );
        scanf("%d", &refrence_string[i] );
    }
}

void set_all_to_invalid () {
    for ( int i = 0; i < page_count; i ++ ) {
        page_table [ i ].is_valid = 0;
    }
}

void fifo_menu () {
    printf( "1. Process Page Access\n" );
    printf( "2. Count Page Faults\n" );
    printf( "3. Back\n " );

    int input = 0;
    scanf( "%d", &input );
    
    int page_number, current_timestamp;

    switch (input) {
        case 1:
            load_page_count();
            load_page_table();

            printf("ENTER PAGE NUMBER: ");
            scanf("%d", &page_number);

            load_frame_count();
            load_frame_pool();


            printf("ENTER CURRENT TIMESTAMP: ");
            scanf("%d", &current_timestamp);

            printf ( "FIFO Return Value is: %d\n", process_page_access_fifo(page_table, &page_count, page_number, frame_pool, &frame_count, current_timestamp) );
            break;
        case 2:
            load_page_count();
            load_page_table();

            load_frame_count();
            load_frame_pool();

            load_reference_count();
            load_reference();

            printf("FAULT found are: %d\n", count_page_faults_fifo(page_table, page_count, refrence_string, reference_count, frame_pool, frame_count));
            break;
        case 3:
            return;;
            break;
        default:
            printf("error\n");
            break;
    }
}

void lru_menu () {
    printf( "1. Process Page Access\n" );
    printf( "2. Count Page Faults\n" );
    printf( "3. Back\n " );

    int input = 0;
    scanf( "%d", &input );
    
    int page_number, current_timestamp;

    switch (input) {
        case 1:
            load_page_count();
            load_page_table();

            printf("ENTER PAGE NUMBER: ");
            scanf("%d", &page_number);

            load_frame_count();
            load_frame_pool();


            printf("ENTER CURRENT TIMESTAMP: ");
            scanf("%d", &current_timestamp);

            printf ( "LRU Return Value is: %d\n", process_page_access_lru(page_table, &page_count, page_number, frame_pool, &frame_count, current_timestamp) );
            break;
        case 2:
            load_page_count();
            load_page_table();

            load_frame_count();
            load_frame_pool();

            load_reference_count();
            load_reference();

            printf("FAULT found are: %d\n", count_page_faults_lru(page_table, page_count, refrence_string, reference_count, frame_pool, frame_count));
            break;
        case 3:
            return;;
            break;
        default:
            break;
    }
}

void lfu_menu () {
    printf( "1. Process Page Access\n" );
    printf( "2. Count Page Faults\n" );
    printf( "3. Back\n " );

    int input = 0;
    scanf( "%d", &input );
    
    int page_number, current_timestamp;

    switch (input) {
        case 1:
            load_page_count();
            load_page_table();

            printf("ENTER PAGE NUMBER: ");
            scanf("%d", &page_number);

            load_frame_count();
            load_frame_pool();


            printf("ENTER CURRENT TIMESTAMP: ");
            scanf("%d", &current_timestamp);

            printf ( "LFU Return Value is: %d\n", process_page_access_lfu(page_table, &page_count, page_number, frame_pool, &frame_count, current_timestamp) );
            break;
        case 2:
            load_page_count();
            load_page_table();

            load_frame_count();
            load_frame_pool();

            load_reference_count();
            load_reference();

            printf("FAULT found are: %d\n", count_page_faults_lfu(page_table, page_count, refrence_string, reference_count, frame_pool, frame_count));
            break;
        case 3:
            return;;
            break;
        default:
            printf("error\n");
            break;
    }
}

void main_menu () {
    int input = 0;

    while (exit_prog == 1) {
        printf("Test Program :- \n");
        printf("1. FIFO\n");
        printf("2. LRU\n");
        printf("3. LFU\n");
        printf("4. EXIT\n");

        printf("> ");
        scanf("%d", &input);
        switch (input) {
            case 1:
                fifo_menu();
                break;
            case 2:
                lru_menu();
                break;
            case 3:
                lfu_menu();
                break;
            case 4:
                exit_prog = 0;
                break;
            default:
                break;
        }
    }
}

int main () {
    main_menu();
    return 0;
}