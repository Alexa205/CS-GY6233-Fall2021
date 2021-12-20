#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "oslabs.h"


int process_page_access_fifo(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp){
    if(page_table[page_number].is_valid == true){ //page table is already in memory
        page_table[page_number].reference_count += 1;
        page_table[page_number].last_access_timestamp = current_timestamp;
        return page_table[page_number].frame_number;
    }
    else if ((page_table[page_number].is_valid == false) && (*frame_cnt > 0)){ //there is available frame
        page_table[page_number].frame_number = frame_pool[*frame_cnt-1];
        *frame_cnt -= 1;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        page_table[page_number].is_valid = true;
        return page_table[page_number].frame_number;
    }
    else { //replaces page in memory
        bool flag_first = false;
        int temp_index = 0;
        int temp_frame = 0;
        int temp_arrival_stemp = 0;
        for (int index = 0; index < *table_cnt ; index++){
            if((page_table[index].is_valid == true) && (flag_first== false)){
                temp_index = index;
                temp_frame = page_table[index].frame_number;
                temp_arrival_stemp = page_table[index].arrival_timestamp;
                flag_first = true;
            }
            else if ((page_table[index].is_valid == true) && (temp_arrival_stemp > page_table[index].arrival_timestamp) && (flag_first== true)){
                temp_arrival_stemp = page_table[index].arrival_timestamp;
                temp_index = index;
                temp_frame = page_table[index].frame_number;
            }
        }
        if (flag_first == true){
            page_table[temp_index].arrival_timestamp = 0;
            page_table[temp_index].reference_count = 0;
            page_table[temp_index].last_access_timestamp = 0;
            page_table[temp_index].frame_number = -1;
            page_table[temp_index].is_valid = false;
            page_table[page_number].frame_number = temp_frame;
            page_table[page_number].arrival_timestamp = current_timestamp;
            page_table[page_number].is_valid = true;
            page_table[page_number].last_access_timestamp = current_timestamp;
            page_table[page_number].reference_count = 1;
        }
        return temp_frame;
    }
}
int count_page_faults_fifo(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt) {
    int timestamp = 1, page_fault = 0;
    for(int i = 0; i < reference_cnt; i++){
        if (page_table[refrence_string[i]].is_valid == true){
            page_table[refrence_string[i]].last_access_timestamp = timestamp;
            page_table[refrence_string[i]].reference_count += 1;
        }
        else if ((page_table[refrence_string[i]].is_valid == false) && (frame_cnt > 0)){
            page_table[refrence_string[i]].frame_number = frame_pool[frame_cnt-1];
            page_table[refrence_string[i]].last_access_timestamp = timestamp;
            page_table[refrence_string[i]].reference_count += 1;
            page_table[refrence_string[i]].is_valid = true;
            frame_cnt -= 1;
            page_fault += 1;
        }
        else if((page_table[refrence_string[i]].is_valid == false) && (frame_cnt == 0)){
            page_fault += 1;
            bool flag_first = false;
            int temp_index = 0;
            int temp_frame = 0;
            int temp_arrival_stemp = 0;
            for (int index = 0; index < table_cnt ; index++){
                if((page_table[index].is_valid == true) && (flag_first== false)){
                    temp_index = index;
                    temp_frame = page_table[index].frame_number;
                    temp_arrival_stemp = page_table[index].arrival_timestamp;
                    flag_first = true;
                }
                else if ((page_table[index].is_valid == true) && (temp_arrival_stemp > page_table[index].arrival_timestamp) && (flag_first== true)){
                    temp_arrival_stemp = page_table[index].arrival_timestamp;
                    temp_index = index;
                    temp_frame = page_table[index].frame_number;
                }
            }
            if (flag_first == true){
                page_table[temp_index].arrival_timestamp = 0;
                page_table[temp_index].reference_count = 0;
                page_table[temp_index].last_access_timestamp = 0;
                page_table[temp_index].frame_number = -1;
                page_table[temp_index].is_valid = false;
                page_table[refrence_string[i]].frame_number = temp_frame;
                page_table[refrence_string[i]].arrival_timestamp = temp_arrival_stemp;
                page_table[refrence_string[i]].is_valid = true;
                page_table[refrence_string[i]].last_access_timestamp = timestamp;
                page_table[refrence_string[i]].reference_count = 1;
            }
        }
        timestamp += 1;
    }
    return page_fault;
}

int process_page_access_lru(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp){
    if(page_table[page_number].is_valid == true){ //page table is already in memory
        page_table[page_number].reference_count += 1;
        page_table[page_number].last_access_timestamp = current_timestamp;
        return page_table[page_number].frame_number;
    }
    else if ((page_table[page_number].is_valid == false) && (*frame_cnt > 0)){ //there is available frame
        page_table[page_number].frame_number = frame_pool[*frame_cnt-1];
        *frame_cnt -= 1;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        page_table[page_number].is_valid = true;
        return page_table[page_number].frame_number;
    }
    else { //replaces page in memory
        bool flag_first = false;
        int temp_index = 0;
        int temp_frame = 0;
        int temp_last_access_timestamp = 0;
        for (int index = 0; index < *table_cnt ; index++){
            if((page_table[index].is_valid == true) && (flag_first== false)){
                temp_index = index;
                temp_frame = page_table[index].frame_number;
                temp_last_access_timestamp = page_table[index].last_access_timestamp;
                flag_first = true;
            }
            else if ((page_table[index].is_valid == true) && (temp_last_access_timestamp > page_table[index].last_access_timestamp) && (flag_first== true)){
                temp_last_access_timestamp = page_table[index].last_access_timestamp;
                temp_index = index;
                temp_frame = page_table[index].frame_number;
            }
        }
        if (flag_first == true){
            page_table[temp_index].arrival_timestamp = 0;
            page_table[temp_index].reference_count = 0;
            page_table[temp_index].last_access_timestamp = 0;
            page_table[temp_index].frame_number = -1;
            page_table[temp_index].is_valid = false;
            page_table[page_number].frame_number = temp_frame;
            page_table[page_number].arrival_timestamp = current_timestamp;
            page_table[page_number].is_valid = true;
            page_table[page_number].last_access_timestamp = current_timestamp;
            page_table[page_number].reference_count = 1;
        }
        return temp_frame;
    }
}
int count_page_faults_lru(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt){
    int timestamp = 1, page_fault = 0;
    for(int i = 0; i < reference_cnt; i++){
        if (page_table[refrence_string[i]].is_valid == true){
            page_table[refrence_string[i]].last_access_timestamp = timestamp;
            page_table[refrence_string[i]].reference_count += 1;
        }
        else if ((page_table[refrence_string[i]].is_valid == false) && (frame_cnt > 0)){
            page_table[refrence_string[i]].frame_number = frame_pool[frame_cnt-1];
            page_table[refrence_string[i]].last_access_timestamp = timestamp;
            page_table[refrence_string[i]].reference_count += 1;
            page_table[refrence_string[i]].is_valid = true;
            frame_cnt -= 1;
            page_fault += 1;
        }
        else if((page_table[refrence_string[i]].is_valid == false) && (frame_cnt == 0)){
            page_fault += 1;
            bool flag_first = false;
            int temp_index = 0;
            int temp_frame = 0;
            int temp_last_arrival_stemp = 0;
            for (int index = 0; index < table_cnt ; index++){
                if((page_table[index].is_valid == true) && (flag_first== false)){
                    temp_index = index;
                    temp_frame = page_table[index].frame_number;
                    temp_last_arrival_stemp = page_table[index].arrival_timestamp;
                    flag_first = true;
                }
                else if ((page_table[index].is_valid == true) && (temp_last_arrival_stemp > page_table[index].last_access_timestamp) && (flag_first== true)){
                    temp_last_arrival_stemp = page_table[index].last_access_timestamp;
                    temp_index = index;
                    temp_frame = page_table[index].frame_number;
                }
            }
            if (flag_first == true){
                page_table[temp_index].arrival_timestamp = 0;
                page_table[temp_index].reference_count = 0;
                page_table[temp_index].last_access_timestamp = 0;
                page_table[temp_index].frame_number = -1;
                page_table[temp_index].is_valid = false;
                page_table[refrence_string[i]].frame_number = temp_frame;
                page_table[refrence_string[i]].arrival_timestamp = timestamp;
                page_table[refrence_string[i]].is_valid = true;
                page_table[refrence_string[i]].last_access_timestamp = timestamp;
                page_table[refrence_string[i]].reference_count = 1;
            }
        }
        timestamp += 1;
    }
    return page_fault;
}

int process_page_access_lfu(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp){
    if(page_table[page_number].is_valid == true){ //page table is already in memory
        page_table[page_number].reference_count += 1;
        page_table[page_number].last_access_timestamp = current_timestamp;
        return page_table[page_number].frame_number;
    }
    else if ((page_table[page_number].is_valid == false) && (*frame_cnt > 0)){ //there is available frame
        page_table[page_number].frame_number = frame_pool[*frame_cnt-1];
        *frame_cnt -= 1;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        page_table[page_number].is_valid = true;
        return page_table[page_number].frame_number;
    }
    else { //replaces page in memory
        bool flag_first = false;
        int temp_index = 0;
        int temp_frame = 0;
        int temp_f_used = 0;
        int temp_arrivel_time = 0;
        for (int index = 0; index < *table_cnt ; index++){
            if((page_table[index].is_valid == true) && (flag_first== false)){
                temp_index = index;
                temp_frame = page_table[index].frame_number;
                temp_f_used = page_table[index].reference_count;
                temp_arrivel_time = page_table[index].arrival_timestamp;
                flag_first = true;
            }
            else if ((page_table[index].is_valid == true) && (temp_f_used >= page_table[index].reference_count) && (flag_first== true)){
                if(((temp_f_used == page_table[index].reference_count) && (temp_arrivel_time > page_table[index].arrival_timestamp)) || (temp_f_used > page_table[index].reference_count)){
                    temp_f_used = page_table[index].reference_count;
                    temp_index = index;
                    temp_frame = page_table[index].frame_number;
                    temp_arrivel_time = page_table[index].arrival_timestamp;
                }
            }
        }
        if (flag_first == true){
            page_table[temp_index].arrival_timestamp = 0;
            page_table[temp_index].reference_count = 0;
            page_table[temp_index].last_access_timestamp = 0;
            page_table[temp_index].frame_number = -1;
            page_table[temp_index].is_valid = false;
            page_table[page_number].frame_number = temp_frame;
            page_table[page_number].arrival_timestamp = current_timestamp;
            page_table[page_number].is_valid = true;
            page_table[page_number].last_access_timestamp = current_timestamp;
            page_table[page_number].reference_count = 1;
        }
        return temp_frame;
    }
}

int count_page_faults_lfu(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt){
    int timestamp = 1, page_fault = 0;
    for(int i = 0; i < reference_cnt; i++){
        if (page_table[refrence_string[i]].is_valid == true){
            page_table[refrence_string[i]].last_access_timestamp = timestamp;
            page_table[refrence_string[i]].reference_count += 1;
        }
        else if ((page_table[refrence_string[i]].is_valid == false) && (frame_cnt > 0)){
            page_table[refrence_string[i]].frame_number = frame_pool[frame_cnt-1];
            page_table[refrence_string[i]].last_access_timestamp = timestamp;
            page_table[refrence_string[i]].arrival_timestamp = timestamp;
            page_table[refrence_string[i]].reference_count += 1;
            page_table[refrence_string[i]].is_valid = true;
            frame_cnt -= 1;
            page_fault += 1;
        }
        else if((page_table[refrence_string[i]].is_valid == false) && (frame_cnt == 0)){
            page_fault += 1;
            bool flag_first = false, flag_equal = false;
            int temp_index = 0;
            int temp_frame = 0;
            int temp_f_used = 0;
            int temp_arrivel_time = 0;
            for (int index = 0; index < table_cnt ; index++){
                if((page_table[index].is_valid == true) && (flag_first== false)){
                    temp_index = index;
                    temp_frame = page_table[index].frame_number;
                    temp_f_used = page_table[index].reference_count;
                    temp_arrivel_time = page_table[index].arrival_timestamp;
                    flag_first = true;
                }
                else if ((page_table[index].is_valid == true) && (temp_f_used >= page_table[index].reference_count) && (flag_first== true)){
                    if(((temp_f_used == page_table[index].reference_count) && (temp_arrivel_time > page_table[index].arrival_timestamp))){
                        temp_f_used = page_table[index].reference_count;
                        temp_index = index;
                        temp_frame = page_table[index].frame_number;
                        temp_arrivel_time = page_table[index].arrival_timestamp;
                        flag_equal = true;
                    }
                    else if((temp_f_used > page_table[index].reference_count) && (flag_equal == false)){
                        temp_f_used = page_table[index].reference_count;
                        temp_index = index;
                        temp_frame = page_table[index].frame_number;
                        temp_arrivel_time = page_table[index].arrival_timestamp;
                    }
                }
            }
            if (flag_first == true){
                page_table[temp_index].arrival_timestamp = 0;
                page_table[temp_index].reference_count = 0;
                page_table[temp_index].last_access_timestamp = 0;
                page_table[temp_index].frame_number = -1;
                page_table[temp_index].is_valid = false;
                page_table[refrence_string[i]].frame_number = temp_frame;
                page_table[refrence_string[i]].arrival_timestamp = timestamp;
                page_table[refrence_string[i]].is_valid = true;
                page_table[refrence_string[i]].last_access_timestamp = timestamp;
                page_table[refrence_string[i]].reference_count = 1;
            }
        }
        timestamp += 1;
    }
    return page_fault;
}

//int main(){
//    struct PTE page_table[TABLEMAX] = {
//            {false, -1, -1, -1, -1},
//            {false, -1, -1, -1, -1},
//            {true, 10, 3, 3, 1},
//            {false, -1, -1, -1, -1},
//            {false, -1, -1, -1, -1},
//            {true, 20 , 2, 4, 2},
//            {false, -1, -1, -1, -1},
//            {true, 30, 1, 1, 1}
//    };
//    int table_cnt = 8;
//    int page_number = 0;
//    int frame_pool[POOLMAX] = {};
//    int frame_cnt = 0;
//    int current_timestamp = 12;
//    int output = process_page_access_fifo(page_table,
//                                          &table_cnt,
//                                          page_number,
//                                          frame_pool,
//                                          &frame_cnt,
//                                          current_timestamp);
//    printf("OUTPUT: %d", output);
//    return 0;
//}