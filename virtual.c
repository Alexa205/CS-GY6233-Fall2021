#include <limits.h>
#include "oslabs.h"

// Definition of null PTE
static struct PTE null_pte = { 0, 0, 0, 0, 0 };
struct PTE *null_pte_ptr = &null_pte;
#define NULL_PTE (*null_pte_ptr)

// Definitions of invalid PTE
static struct PTE invalid_pte = { 0, -1, -1, -1, -1 };
struct PTE *invalid_pte_ptr = &invalid_pte;
#define INVALID_PTE (*invalid_pte_ptr)

/**
 * @brief This function implements the logic to process a page access in a system that uses the First-In
 First-Out (FIFO) policy for page replacement.
 *
 * @param page_table           A page table that contains a list of entries.
 * @param table_cnt            Number of entries in the page table.
 * @param page_number          The number of the page being referenced.
 * @param frame_pool           A pool of frames that is allocated.
 * @param frame_cnt            Number of frames in the frame pool.
 * @param current_timestamp    The current time stamp.
 * @return The memory frame number for the logical page.
 */
int process_page_access_fifo(struct PTE page_table[TABLEMAX], int *table_cnt,
		int page_number, int frame_pool[POOLMAX], int *frame_cnt,
		int current_timestamp) {
	int frame_number = -1;
	int earliest_arrival_time_stamp = INT_MAX;
	int earliest_arrival_page_index = -1;

	if ((page_number >= 0 && page_number < TABLEMAX)
			&& page_table[page_number].is_valid) {
		// Page is in the memory. Update the entry and return its frame number.
		frame_number = page_table[page_number].frame_number;
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].reference_count++;
	} else if ((*frame_cnt) > 0) {
		// Page is not in the memory. Remove an entry from the frame_pool.
		frame_number = frame_pool[(*frame_cnt) - 1];
		(*frame_cnt)--;
		// Update the entry and return its frame number.
		page_table[page_number].frame_number = frame_number;
		page_table[page_number].is_valid = 1;
		page_table[page_number].arrival_timestamp = current_timestamp;
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].reference_count = 1;
	} else {
		// The page is not in the memory and the frame pool is empty.
		// Choose the entry with the earliest arrival time and replace it.
		for (int i = 0; i < (*table_cnt); i++) {
			if (page_table[i].is_valid && !(page_table[i].arrival_timestamp < 0)
					&& (page_table[i].arrival_timestamp
							< earliest_arrival_time_stamp)) {
				earliest_arrival_time_stamp = page_table[i].arrival_timestamp;
				earliest_arrival_page_index = i;
			}
		}

		if (earliest_arrival_page_index != -1) {
			frame_number = page_table[earliest_arrival_page_index].frame_number;
			page_table[earliest_arrival_page_index] = INVALID_PTE;
			page_table[page_number].is_valid = 1;
			page_table[page_number].frame_number = frame_number;
			page_table[page_number].arrival_timestamp = current_timestamp;
			page_table[page_number].last_access_timestamp = current_timestamp;
			page_table[page_number].reference_count = 1;
		}
	}

	return frame_number;
}

/**
 * @brief This function simulates the processing of a sequence of page references in a system that uses
 the First-In-First-Out (FIFO) policy for page replacement.
 *
 * @param page_table        A page table that contains a list of entries.
 * @param table_cnt         Number of entries in the page table.
 * @param refrence_string   A sequence of page references.
 * @param reference_cnt     Number of pages in the refrence_string.
 * @param frame_pool        A pool of frames that is allocated.
 * @param frame_cnt         Number of frames in the frame pool.
 * @return The total number of page faults encountered in the simulation.
 */
int count_page_faults_fifo(struct PTE page_table[TABLEMAX], int table_cnt,
		int refrence_string[REFERENCEMAX], int reference_cnt,
		int frame_pool[POOLMAX], int frame_cnt) {
	int fault_count = 0;
	int time_stamp = 1;
	int frame_number = -1;
	int current_page_number = -1;
	int current_frame_count = frame_cnt;
	int earliest_arrival_time_stamp = INT_MAX;
	int earliest_arrival_page_index = -1;

	for (int i = 0; i < reference_cnt; i++) {
		current_page_number = refrence_string[i];

		if (page_table[current_page_number].is_valid) {
			// Page is in the memory. Just update the entry.
			page_table[current_page_number].last_access_timestamp = time_stamp;
			page_table[current_page_number].reference_count++;
		} else if (!page_table[current_page_number].is_valid
				&& current_frame_count > 0) {
			// Page is not in the memory. Remove an entry from the frame_pool.
			current_frame_count--;
			page_table[current_page_number].frame_number =
					frame_pool[current_frame_count];
			// Update the entry and count it as  page fault.
			page_table[current_page_number].is_valid = 1;
			page_table[current_page_number].arrival_timestamp = time_stamp;
			page_table[current_page_number].last_access_timestamp = time_stamp;
			page_table[current_page_number].reference_count = 1;
			fault_count++;
		} else {
			// The page is not in the memory and the frame pool is empty.
			// Choose the entry with the earliest arrival time and replace it.
			for (int i = 0; i < table_cnt; i++) {
				if (page_table[i].is_valid
						&& !(page_table[i].arrival_timestamp < 0)
						&& (page_table[i].arrival_timestamp
								< earliest_arrival_time_stamp)) {
					earliest_arrival_time_stamp =
							page_table[i].arrival_timestamp;
					earliest_arrival_page_index = i;
				}
			}

			if (earliest_arrival_page_index != -1) {
				frame_number =
						page_table[earliest_arrival_page_index].frame_number;
				page_table[earliest_arrival_page_index] = NULL_PTE;
				page_table[current_page_number].is_valid = 1;
				page_table[current_page_number].frame_number = frame_number;
				page_table[current_page_number].arrival_timestamp = time_stamp;
				page_table[current_page_number].last_access_timestamp =
						time_stamp;
				page_table[current_page_number].reference_count = 1;
				earliest_arrival_page_index = -1;
				earliest_arrival_time_stamp = INT_MAX;
			}

			// Count this as a page fault.
			fault_count++;
		}
		// Increment the time stamp.
		time_stamp++;
	}

	return fault_count;
}

/**
 * @brief This function implements the logic to process a page access in a system that uses the
 Least-Recently-Used (LRU) policy for page replacement.
 *
 * @param page_table           A page table that contains a list of entries.
 * @param table_cnt            Number of entries in the page table.
 * @param page_number          The number of the page being referenced.
 * @param frame_pool           A pool of frames that is allocated.
 * @param frame_cnt            Number of frames in the frame pool.
 * @param current_timestamp    The current time stamp.
 * @return The memory frame number for the logical page.
 */
int process_page_access_lru(struct PTE page_table[TABLEMAX], int *table_cnt,
		int page_number, int frame_pool[POOLMAX], int *frame_cnt,
		int current_timestamp) {
	int frame_number = -1;
	int smallest_last_access = INT_MAX;
	int smallest_last_access_index = -1;

	if ((page_number >= 0 && page_number < TABLEMAX)
			&& page_table[page_number].is_valid) {
		// Page is in the memory. Update the entry and return its frame number.
		frame_number = page_table[page_number].frame_number;
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].reference_count++;
	} else if ((*frame_cnt) > 0) {
		// Page is not in the memory. Remove an entry from the frame_pool.
		frame_number = frame_pool[(*frame_cnt) - 1];
		(*frame_cnt)--;
		// Update the entry and return its frame number.
		page_table[page_number].frame_number = frame_number;
		page_table[page_number].is_valid = 1;
		page_table[page_number].arrival_timestamp = current_timestamp;
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].reference_count = 1;
	} else {
		// The page is not in the memory and the frame pool is empty.
		// Choose the entry with the smallest last access time and replace it.
		for (int i = 0; i < (*table_cnt); i++) {
			if (page_table[i].is_valid
					&& !(page_table[i].last_access_timestamp < 0)
					&& (page_table[i].last_access_timestamp
							< smallest_last_access)) {
				smallest_last_access = page_table[i].arrival_timestamp;
				smallest_last_access_index = i;
			}
		}

		if (smallest_last_access_index != -1) {
			frame_number = page_table[smallest_last_access_index].frame_number;
			page_table[smallest_last_access_index] = INVALID_PTE;
			page_table[page_number].is_valid = 1;
			page_table[page_number].frame_number = frame_number;
			page_table[page_number].arrival_timestamp = current_timestamp;
			page_table[page_number].last_access_timestamp = current_timestamp;
			page_table[page_number].reference_count = 1;
		}
	}

	return frame_number;
}

/**
 * @brief This function simulates the processing of a sequence of page references in a system that uses
 the Least-Recently-Used (LRU) policy for page replacement.
 *
 * @param page_table        A page table that contains a list of entries.
 * @param table_cnt         Number of entries in the page table.
 * @param refrence_string   A sequence of page references.
 * @param reference_cnt     Number of pages in the refrence_string.
 * @param frame_pool        A pool of frames that is allocated.
 * @param frame_cnt         Number of frames in the frame pool.
 * @return The total number of page faults encountered in the simulation.
 */
int count_page_faults_lru(struct PTE page_table[TABLEMAX], int table_cnt,
		int refrence_string[REFERENCEMAX], int reference_cnt,
		int frame_pool[POOLMAX], int frame_cnt) {
	int fault_count = 0;
	int time_stamp = 1;
	int frame_number = -1;
	int current_page_number = -1;
	int current_frame_count = frame_cnt;
	int smallest_last_access = INT_MAX;
	int smallest_last_access_index = -1;

	for (int i = 0; i < reference_cnt; i++) {
		current_page_number = refrence_string[i];

		if (page_table[current_page_number].is_valid) {
			// Page is in the memory. Just update the entry.
			page_table[current_page_number].last_access_timestamp = time_stamp;
			page_table[current_page_number].reference_count++;
		} else if (!page_table[current_page_number].is_valid
				&& current_frame_count > 0) {
			// Page is not in the memory. Remove an entry from the frame_pool.
			current_frame_count--;
			page_table[current_page_number].frame_number =
					frame_pool[current_frame_count];
			// Update the entry and count it as  page fault.
			page_table[current_page_number].is_valid = 1;
			page_table[current_page_number].arrival_timestamp = time_stamp;
			page_table[current_page_number].last_access_timestamp = time_stamp;
			page_table[current_page_number].reference_count = 1;
			fault_count++;
		} else {
			// The page is not in the memory and the frame pool is empty.
			// Choose the entry with the smallest last access time and replace it.
			for (int i = 0; i < table_cnt; i++) {
				if (page_table[i].is_valid
						&& !(page_table[i].last_access_timestamp < 0)
						&& (page_table[i].last_access_timestamp
								< smallest_last_access)) {
					smallest_last_access = page_table[i].last_access_timestamp;
					smallest_last_access_index = i;
				}
			}

			if (smallest_last_access_index != -1) {
				frame_number =
						page_table[smallest_last_access_index].frame_number;
				page_table[smallest_last_access_index] = NULL_PTE;
				page_table[current_page_number].is_valid = 1;
				page_table[current_page_number].frame_number = frame_number;
				page_table[current_page_number].arrival_timestamp = time_stamp;
				page_table[current_page_number].last_access_timestamp =
						time_stamp;
				page_table[current_page_number].reference_count = 1;
				smallest_last_access_index = -1;
				smallest_last_access = INT_MAX;
			}

			// Count this as a page fault.
			fault_count++;
		}
		// Increment the time stamp.
		time_stamp++;
	}

	return fault_count;
}

/**
 * @brief This function implements the logic to process a page access in a system that uses the
 Least-Frequently-Used (LFU, also known as Not-Frequently-Used) policy for page replacement.
 *
 * @param page_table           A page table that contains a list of entries.
 * @param table_cnt            Number of entries in the page table.
 * @param page_number          The number of the page being referenced.
 * @param frame_pool           A pool of frames that is allocated.
 * @param frame_cnt            Number of frames in the frame pool.
 * @param current_timestamp    The current time stamp.
 * @return The memory frame number for the logical page.
 */
int process_page_access_lfu(struct PTE page_table[TABLEMAX], int *table_cnt,
		int page_number, int frame_pool[POOLMAX], int *frame_cnt,
		int current_timestamp) {
	int frame_number = -1;
	int smallest_reference_count = INT_MAX;
	int smallest_reference_count_index = -1;

	if ((page_number >= 0 && page_number < TABLEMAX)
			&& page_table[page_number].is_valid) {
		// Page is in the memory. Update the entry and return its frame number.
		frame_number = page_table[page_number].frame_number;
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].reference_count++;
	} else if ((*frame_cnt) > 0) {
		// Page is not in the memory. Remove an entry from the frame_pool.
		frame_number = frame_pool[(*frame_cnt) - 1];
		(*frame_cnt)--;
		// Update the entry and return its frame number.
		page_table[page_number].frame_number = frame_number;
		page_table[page_number].is_valid = 1;
		page_table[page_number].arrival_timestamp = current_timestamp;
		page_table[page_number].last_access_timestamp = current_timestamp;
		page_table[page_number].reference_count = 1;
	} else {
		// The page is not in the memory and the frame pool is empty.
		// Choose the entry with the smallest reference count and replace it.
		for (int i = 0; i < (*table_cnt); i++) {
			if (page_table[i].is_valid
					&& !(page_table[i].reference_count < 0)) {
				if ((page_table[i].reference_count < smallest_reference_count)
				// If there are multiple entries with the smallest reference count then,
				// choose the one with the smallest arrival time stamp.
						|| ((smallest_reference_count
								== page_table[i].reference_count)
								|| (page_table[i].arrival_timestamp
										< page_table[smallest_reference_count_index].arrival_timestamp))) {
					smallest_reference_count = page_table[i].reference_count;
					smallest_reference_count_index = i;
				}
			}
		}

		if (smallest_reference_count_index != -1) {
			frame_number =
					page_table[smallest_reference_count_index].frame_number;
			page_table[smallest_reference_count_index] = INVALID_PTE;
			page_table[page_number].is_valid = 1;
			page_table[page_number].frame_number = frame_number;
			page_table[page_number].arrival_timestamp = current_timestamp;
			page_table[page_number].last_access_timestamp = current_timestamp;
			page_table[page_number].reference_count = 1;
		}
	}

	return frame_number;
}

/**
 * @brief This function simulates the processing of a sequence of page references in a system that uses
 the Least-Frequently-Used (LFU, also known as Not-Frequently-Used) policy for page replacement.
 *
 * @param page_table        A page table that contains a list of entries.
 * @param table_cnt         Number of entries in the page table.
 * @param refrence_string   A sequence of page references.
 * @param reference_cnt     Number of pages in the refrence_string.
 * @param frame_pool        A pool of frames that is allocated.
 * @param frame_cnt         Number of frames in the frame pool.
 * @return The total number of page faults encountered in the simulation.
 */
int count_page_faults_lfu(struct PTE page_table[TABLEMAX], int table_cnt,
		int refrence_string[REFERENCEMAX], int reference_cnt,
		int frame_pool[POOLMAX], int frame_cnt) {
	int fault_count = 0;
	int time_stamp = 1;
	int frame_number = -1;
	int current_page_number = -1;
	int current_frame_count = frame_cnt;
	int smallest_reference_count = INT_MAX;
	int smallest_reference_count_index = -1;

	for (int i = 0; i < reference_cnt; i++) {
		current_page_number = refrence_string[i];

		if (page_table[current_page_number].is_valid) {
			// Page is in the memory. Just update the entry.
			page_table[current_page_number].last_access_timestamp = time_stamp;
			page_table[current_page_number].reference_count++;
		} else if (!page_table[current_page_number].is_valid
				&& current_frame_count > 0) {
			// Page is not in the memory. Remove an entry from the frame_pool.
			current_frame_count--;
			page_table[current_page_number].frame_number =
					frame_pool[current_frame_count];
			// Update the entry and count it as  page fault.
			page_table[current_page_number].is_valid = 1;
			page_table[current_page_number].arrival_timestamp = time_stamp;
			page_table[current_page_number].last_access_timestamp = time_stamp;
			page_table[current_page_number].reference_count = 1;
			fault_count++;
		} else {
			// The page is not in the memory and the frame pool is empty.
			// Choose the entry with the smallest reference count and replace it.
			for (int i = 0; i < table_cnt; i++) {
				if (page_table[i].is_valid
						&& !(page_table[i].reference_count < 0)) {
					if ((page_table[i].reference_count
							< smallest_reference_count)
							// If there are multiple entries with the smallest reference count then,
							// choose the one with the smallest arrival time stamp.
							|| ((smallest_reference_count
									== page_table[i].reference_count)
									|| (page_table[i].arrival_timestamp
											< page_table[smallest_reference_count_index].arrival_timestamp))) {
						smallest_reference_count =
								page_table[i].reference_count;
						smallest_reference_count_index = i;
					}
				}
			}

			if (smallest_reference_count_index != -1) {
				frame_number =
						page_table[smallest_reference_count_index].frame_number;
				page_table[smallest_reference_count_index] = NULL_PTE;
				page_table[current_page_number].is_valid = 1;
				page_table[current_page_number].frame_number = frame_number;
				page_table[current_page_number].arrival_timestamp = time_stamp;
				page_table[current_page_number].last_access_timestamp =
						time_stamp;
				page_table[current_page_number].reference_count = 1;
				smallest_reference_count_index = -1;
				smallest_reference_count = INT_MAX;
			}

			// Count this as a page fault.
			fault_count++;
		}
		// Increment the time stamp.
		time_stamp++;
	}

	return fault_count;
}
