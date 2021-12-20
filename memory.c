#include "oslabs.h"


static struct MEMORY_BLOCK null_block;
struct MEMORY_BLOCK *null_block_ptr = &null_block;
#define NULLBLOCK (*null_block_ptr)

int compare_blocks(struct MEMORY_BLOCK *first_block,
		struct MEMORY_BLOCK *second_block) {
	if (first_block->process_id == second_block->process_id
			&& first_block->start_address == second_block->start_address
			&& first_block->end_address == second_block->end_address
			&& first_block->segment_size == second_block->segment_size) {
		return 1;
	}
	return 0;
}

int is_better_fit(struct MEMORY_BLOCK *first_block,
		struct MEMORY_BLOCK *second_block, int request_size) {
	if (first_block->segment_size - request_size
			> second_block->segment_size - request_size) {
		return 1;
	}
	return 0;
}

int is_worse_fit(struct MEMORY_BLOCK *first_block,
		struct MEMORY_BLOCK *second_block, int request_size) {
	if (first_block->segment_size - request_size
			< second_block->segment_size - request_size) {
		return 1;
	}
	return 0;
}

void merge_consecutive_free_blocks(struct MEMORY_BLOCK memory_map[MAPMAX],
		int *map_cnt) {
}

struct MEMORY_BLOCK* segment_and_allocate(
		struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int block_index,
		int request_size) {
	// Shift all the blocks after the block to be segmented by 1
	// since 2 blocks are needed for segmentation.
	for (int i = MAPMAX - 2; i >= block_index; i--) {
		memory_map[i + 1] = memory_map[i];
	}

	// Segment the block and increment the map count.
	memory_map[block_index].segment_size = request_size;
	memory_map[block_index].end_address = memory_map[block_index].start_address
			+ request_size - 1;
	memory_map[block_index + 1].segment_size -= request_size;
	memory_map[block_index + 1].start_address =
			memory_map[block_index].end_address + 1;
	(*map_cnt)++;

	return &(memory_map[block_index]);
}

struct MEMORY_BLOCK best_fit_allocate(int request_size,
		struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {
	struct MEMORY_BLOCK *memory_block = &NULLBLOCK;
	int best_fit_block_index = -1;
	int is_segmentation_needed = 0;

	merge_consecutive_free_blocks(memory_map, map_cnt);

	for (int i = 0; i < MAPMAX; i++) {
		if (memory_map[i].segment_size >= request_size
				&& memory_map[i].process_id == 0) {

			if (memory_map[i].segment_size == request_size) {
				best_fit_block_index = i;
				is_segmentation_needed = 0;
				break;
			} else if (best_fit_block_index == -1
					&& (((*map_cnt) + 1) != MAPMAX)) {
				best_fit_block_index = i;
				is_segmentation_needed = 1;
			} else if ((is_better_fit(&(memory_map[best_fit_block_index]),
					&memory_map[i], request_size))
					&& (((*map_cnt) + 1) != MAPMAX)) {
				best_fit_block_index = i;
				is_segmentation_needed = 1;
			}
		}
	}


	if (best_fit_block_index != -1) {
		if (is_segmentation_needed) {
			memory_block = segment_and_allocate(memory_map, map_cnt,
					best_fit_block_index, request_size);
		} else {
			memory_block = &(memory_map[best_fit_block_index]);
		}

		memory_block->process_id = process_id;
	}

	return (*memory_block);
}

struct MEMORY_BLOCK first_fit_allocate(int request_size,
		struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {
	struct MEMORY_BLOCK *memory_block = &NULLBLOCK;
	int first_fit_block_index = -1;
	int is_segmentation_needed = 0;

	merge_consecutive_free_blocks(memory_map, map_cnt);


	for (int i = 0; i < MAPMAX; i++) {
		if (memory_map[i].segment_size >= request_size
				&& memory_map[i].process_id == 0) {
			// Found one. Update first_fit_block_index and break the loop.
			if (memory_map[i].segment_size != request_size) {
				is_segmentation_needed = 1;
			}
			first_fit_block_index = i;
			break;
		}
	}

	if (first_fit_block_index != -1) {
		if (is_segmentation_needed) {
			memory_block = segment_and_allocate(memory_map, map_cnt,
					first_fit_block_index, request_size);
		} else {
			memory_block = &memory_map[first_fit_block_index];
		}

		memory_block->process_id = process_id;
	}

	return (*memory_block);
}


struct MEMORY_BLOCK worst_fit_allocate(int request_size,
		struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {
	struct MEMORY_BLOCK *memory_block = &NULLBLOCK;
	int worst_fit_block_index = -1;
	int is_segmentation_needed = 0;

	merge_consecutive_free_blocks(memory_map, map_cnt);


	for (int i = 0; i < (*map_cnt); i++) {
		if (memory_map[i].segment_size >= request_size
				&& memory_map[i].process_id == 0) {
			if (worst_fit_block_index == -1) {

				if (memory_map[i].segment_size != request_size) {
					is_segmentation_needed = 1;
				}
				worst_fit_block_index = i;
			} else if (is_worse_fit(&(memory_map[worst_fit_block_index]),
					&(memory_map[i]), request_size)) {
				// memory_map[i] is larger than current worst_fit. Update worst_fit.
				if (memory_map[i].segment_size != request_size) {
					is_segmentation_needed = 1;
				}
				worst_fit_block_index = i;
			}
		}
	}


	if (worst_fit_block_index != -1) {
		if (is_segmentation_needed) {
			memory_block = segment_and_allocate(memory_map, map_cnt,
					worst_fit_block_index, request_size);
		} else {
			memory_block = &(memory_map[worst_fit_block_index]);
		}

		memory_block->process_id = process_id;
	}

	return (*memory_block);
}


struct MEMORY_BLOCK next_fit_allocate(int request_size,
		struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id,
		int last_address) {
	struct MEMORY_BLOCK *memory_block = &NULLBLOCK;
	int first_fit_block_index = -1;
	int is_segmentation_needed = 0;

	merge_consecutive_free_blocks(memory_map, map_cnt);

	for (int i = 0; i < MAPMAX; i++) {
		if (memory_map[i].segment_size >= request_size
				&& memory_map[i].process_id == 0
				&& memory_map[i].start_address >= last_address) {
			if (memory_map[i].segment_size != request_size) {
				is_segmentation_needed = 1;
			}
			first_fit_block_index = i;
			break;
		}
	}

	if (first_fit_block_index != -1) {
		if (is_segmentation_needed) {
			memory_block = segment_and_allocate(memory_map, map_cnt,
					first_fit_block_index, request_size);
		} else {
			memory_block = &memory_map[first_fit_block_index];
		}

		memory_block->process_id = process_id;
	}

	return (*memory_block);
}

void release_memory(struct MEMORY_BLOCK freed_block,
		struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt) {
	int block_index = 0;

	for (block_index = 0; block_index < MAPMAX; block_index++) {
		if (compare_blocks(&freed_block, &(memory_map[block_index]))) {
			memory_map[block_index].process_id = 0;
			break;
		}
	}

	if (block_index > 0 && memory_map[block_index - 1].process_id == 0) {
		memory_map[block_index - 1].end_address +=
				memory_map[block_index].segment_size;
		memory_map[block_index - 1].segment_size +=
				memory_map[block_index].segment_size;

		for (int i = block_index; i < (MAPMAX - 1); i++) {
			memory_map[i] = memory_map[i + 1];
		}
		(*map_cnt)--;

		block_index--;
	}

	if (block_index < (MAPMAX - 1)
			&& memory_map[block_index + 1].process_id == 0) {
		memory_map[block_index].end_address +=
				memory_map[block_index + 1].segment_size;
		memory_map[block_index].segment_size +=
				memory_map[block_index + 1].segment_size;

		for (int i = block_index + 1; i < (MAPMAX - 1); i++) {
			memory_map[i] = memory_map[i + 1];
		}
		(*map_cnt)--;
	}
}
