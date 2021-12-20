#include "oslabs.h"

int is_NULLblock(struct MEMORY_BLOCK inblock){
    if(inblock.process_id == 0 && inblock.start_address == 0 && inblock.end_address == 0 &&
       inblock.segment_size == 0)
        return 1;
    else
        return 0;
}

void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt){
    int this_is_freed_block;
    for (int i = 0; i < *map_cnt; i++){
        if(freed_block.process_id == memory_map[i].process_id){
            this_is_freed_block = i;
            break;
        }
    }
    if (memory_map[this_is_freed_block-1].process_id == 0 && memory_map[this_is_freed_block + 1]. process_id == 0){
        memory_map[this_is_freed_block-1].end_address = memory_map[this_is_freed_block + 1].end_address;
        memory_map[this_is_freed_block-1].segment_size = (memory_map[this_is_freed_block-1].end_address - memory_map[this_is_freed_block-1].start_address) + 1;
        *map_cnt = *map_cnt - 2;
        for(int i = this_is_freed_block; i < *map_cnt; i++)
            memory_map[i] = memory_map[i+2];
    }
    else if (memory_map[this_is_freed_block-1].process_id ==0){
        memory_map[this_is_freed_block-1].end_address = memory_map[this_is_freed_block].end_address;
        memory_map[this_is_freed_block-1].segment_size = (memory_map[this_is_freed_block-1].end_address - memory_map[this_is_freed_block-1].start_address) + 1;
        *map_cnt = *map_cnt - 1;
        for(int i = this_is_freed_block; i < *map_cnt; i++)
            memory_map[i] = memory_map[i+1];
    }
    else if (memory_map[this_is_freed_block+1].process_id ==0){
        memory_map[this_is_freed_block].end_address = memory_map[this_is_freed_block+1].end_address;
        memory_map[this_is_freed_block].segment_size = (memory_map[this_is_freed_block].end_address - memory_map[this_is_freed_block].start_address) + 1;
        memory_map[this_is_freed_block].process_id = 0;
        *map_cnt = *map_cnt - 1;
        for(int i = this_is_freed_block; i < *map_cnt; i++)
            memory_map[i] = memory_map[i+1];
    }
    else{
        memory_map[this_is_freed_block].process_id = 0;
    }
}

struct MEMORY_BLOCK setup_NULLBLOCK(struct MEMORY_BLOCK *inblock){
    (*inblock).process_id =0;
    (*inblock).start_address = 0;
    (*inblock).end_address = 0;
    (*inblock).segment_size = 0;
    return *inblock;
}

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    int size_difference;
    int min_size_difference = 10000;
    int candidate_index = 0;
    for (int i = 0; i < *map_cnt; i++){
        if(memory_map[i].segment_size - request_size <= min_size_difference && memory_map[i].process_id == 0){
            min_size_difference = memory_map[i].segment_size - request_size;
            candidate_index = i;
        }
    }
    if(min_size_difference == 10000){
        struct MEMORY_BLOCK emptyblock;
        return setup_NULLBLOCK(&emptyblock);
    }
    else if(min_size_difference == 0){
        memory_map[candidate_index].process_id = process_id;
        return memory_map[candidate_index];
    }
    else{
        struct MEMORY_BLOCK piece2;
        piece2.process_id = 0;
        piece2.start_address = memory_map[candidate_index].start_address + request_size;
        piece2.end_address = memory_map[candidate_index].end_address;
        piece2.segment_size = piece2.end_address - piece2.start_address + 1;
        memory_map[*map_cnt] = piece2;
        (*map_cnt)++;

        memory_map[candidate_index].end_address = memory_map[candidate_index].start_address + request_size - 1;
        memory_map[candidate_index].process_id = process_id;
        memory_map[candidate_index].segment_size = request_size;

        return memory_map[candidate_index];
    }
}

struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    int size_difference;
    int min_size_difference = 10000;
    int candidate_index = 0;
    for (int i = 0; i < *map_cnt; i++){
        if(memory_map[i].segment_size - request_size <= min_size_difference && memory_map[i].process_id == 0){
            min_size_difference = memory_map[i].segment_size - request_size;
            candidate_index = i;
        }
    }
    if(min_size_difference == 10000){
        struct MEMORY_BLOCK emptyblock;
        return setup_NULLBLOCK(&emptyblock);
    }
    else if(min_size_difference == 0){
        memory_map[candidate_index].process_id = process_id;
        return memory_map[candidate_index];
    }
    else{
        struct MEMORY_BLOCK piece2;
        piece2.process_id = 0;
        piece2.start_address = memory_map[candidate_index].start_address + request_size;
        piece2.end_address = memory_map[candidate_index].end_address;
        piece2.segment_size = piece2.end_address - piece2.start_address + 1;
        memory_map[*map_cnt] = piece2;
        (*map_cnt)++;

        memory_map[candidate_index].end_address = memory_map[candidate_index].start_address + request_size - 1;
        memory_map[candidate_index].process_id = process_id;
        memory_map[candidate_index].segment_size = request_size;

        return memory_map[candidate_index];
    }
}

struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    int size_difference;
    int min_size_difference = 10000;
    int candidate_index = 0;
    for (int i = 0; i < *map_cnt; i++){
        if(memory_map[i].segment_size - request_size <= min_size_difference && memory_map[i].process_id == 0){
            min_size_difference = memory_map[i].segment_size - request_size;
            candidate_index = i;
        }
    }
    if(min_size_difference == 10000){
        struct MEMORY_BLOCK emptyblock;
        return setup_NULLBLOCK(&emptyblock);
    }
    else if(min_size_difference == 0){
        memory_map[candidate_index].process_id = process_id;
        return memory_map[candidate_index];
    }
    else{
        struct MEMORY_BLOCK piece2;
        piece2.process_id = 0;
        piece2.start_address = memory_map[candidate_index].start_address + request_size;
        piece2.end_address = memory_map[candidate_index].end_address;
        piece2.segment_size = piece2.end_address - piece2.start_address + 1;
        memory_map[*map_cnt] = piece2;
        (*map_cnt)++;

        memory_map[candidate_index].end_address = memory_map[candidate_index].start_address + request_size - 1;
        memory_map[candidate_index].process_id = process_id;
        memory_map[candidate_index].segment_size = request_size;

        return memory_map[candidate_index];
    }
}

struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id, int last_address){
    int size_difference;
    int min_size_difference = 10000;
    int candidate_index = 0;
    for (int i = 0; i < *map_cnt; i++){
        if(memory_map[i].segment_size - request_size <= min_size_difference && memory_map[i].process_id == 0){
            min_size_difference = memory_map[i].segment_size - request_size;
            candidate_index = i;
        }
    }
    if(min_size_difference == 10000){
        struct MEMORY_BLOCK emptyblock;
        return setup_NULLBLOCK(&emptyblock);
    }
    else if(min_size_difference == 0){
        memory_map[candidate_index].process_id = process_id;
        return memory_map[candidate_index];
    }
    else{
        struct MEMORY_BLOCK piece2;
        piece2.process_id = 0;
        piece2.start_address = memory_map[candidate_index].start_address + request_size;
        piece2.end_address = memory_map[candidate_index].end_address;
        piece2.segment_size = piece2.end_address - piece2.start_address + 1;
        memory_map[*map_cnt] = piece2;
        (*map_cnt)++;

        memory_map[candidate_index].end_address = memory_map[candidate_index].start_address + request_size - 1;
        memory_map[candidate_index].process_id = process_id;
        memory_map[candidate_index].segment_size = request_size;

        return memory_map[candidate_index];
    }
}

