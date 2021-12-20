#include <limits.h>
#include <stdlib.h>

#include "oslabs.h"

// Definition of NULLRCB
static struct RCB null_rcb;
struct RCB *null_rcb_ptr = &null_rcb;
#define NULLRCB (*null_rcb_ptr)

int compare_rcbs(struct RCB first, struct RCB second) {
	if (first.address == second.address
			&& first.arrival_timestamp == second.arrival_timestamp
			&& first.cylinder == second.cylinder
			&& first.process_id == second.process_id
			&& first.request_id == second.request_id) {
		return 1;
	}
	return 0;
}

void remove_from_queue(struct RCB request_queue[QUEUEMAX], int *queue_cnt,
		int index) {
	for (int i = index + 1; i < QUEUEMAX; i++) {
		request_queue[i - 1] = request_queue[i];
	}
	request_queue[QUEUEMAX - 1] = NULLRCB;
	(*queue_cnt)--;
}

struct RCB handle_request_arrival_fcfs(struct RCB request_queue[QUEUEMAX],
		int *queue_cnt, struct RCB current_request, struct RCB new_request,
		int timestamp) {
	struct RCB output_rcb = NULLRCB;

	if (compare_rcbs(current_request, NULLRCB)) {
		output_rcb = new_request;
	} else {
		request_queue[*queue_cnt] = new_request;
		(*queue_cnt)++;
		output_rcb = current_request;
	}

	return output_rcb;
}

struct RCB handle_request_completion_fcfs(struct RCB request_queue[QUEUEMAX],
		int *queue_cnt) {
	struct RCB output_rcb = NULLRCB;
	int output_rcb_index = -1;

	if ((*queue_cnt) > 0) {
		for (int i = 0; i < QUEUEMAX; i++) {
			if (compare_rcbs(request_queue[i], NULLRCB)) {
				continue;
			}

			if (compare_rcbs(output_rcb, NULLRCB)) {
				output_rcb = request_queue[i];
				output_rcb_index = i;
			} else if (output_rcb.arrival_timestamp
					> request_queue[i].arrival_timestamp) {
				output_rcb = request_queue[i];
				output_rcb_index = i;
			}
		}

		if (output_rcb_index != -1) {
			remove_from_queue(request_queue, queue_cnt, output_rcb_index);
		}
	}

	return output_rcb;
}

struct RCB handle_request_arrival_sstf(struct RCB request_queue[QUEUEMAX],
		int *queue_cnt, struct RCB current_request, struct RCB new_request,
		int timestamp) {
	struct RCB output_rcb = NULLRCB;

	if (compare_rcbs(current_request, NULLRCB)) {
		output_rcb = new_request;
	} else {
		request_queue[*queue_cnt] = new_request;
		(*queue_cnt)++;
		output_rcb = current_request;
	}

	return output_rcb;
}

struct RCB handle_request_completion_sstf(struct RCB request_queue[QUEUEMAX],
		int *queue_cnt, int current_cylinder) {
	struct RCB output_rcb = NULLRCB;
	int output_rcb_index = -1;
	int shortest_seek_time = INT_MAX;
	int current_rbc_seek_time = 0;

	if ((*queue_cnt) > 0) {
		for (int i = 0; i < QUEUEMAX; i++) {
			if (compare_rcbs(request_queue[i], NULLRCB)) {
				continue;
			}

			current_rbc_seek_time = abs(
					current_cylinder - request_queue[i].cylinder);

			if (current_rbc_seek_time <= shortest_seek_time) {
				if (compare_rcbs(output_rcb, NULLRCB)
						|| (output_rcb.arrival_timestamp
								> request_queue[i].arrival_timestamp)) {
					shortest_seek_time = current_rbc_seek_time;
					output_rcb = request_queue[i];
					output_rcb_index = i;
				}
			}
		}

		if (output_rcb_index != -1) {
			remove_from_queue(request_queue, queue_cnt, output_rcb_index);
		}
	}

	return output_rcb;
}

struct RCB handle_request_arrival_look(struct RCB request_queue[QUEUEMAX],
		int *queue_cnt, struct RCB current_request, struct RCB new_request,
		int timestamp) {
	struct RCB output_rcb = NULLRCB;

	if (compare_rcbs(current_request, NULLRCB)) {
		output_rcb = new_request;
	} else {
		request_queue[*queue_cnt] = new_request;
		(*queue_cnt)++;
		output_rcb = current_request;
	}

	return output_rcb;
}

struct RCB handle_request_completion_look(struct RCB request_queue[QUEUEMAX],
		int *queue_cnt, int current_cylinder, int scan_direction) {
	struct RCB output_rcb = NULLRCB;
	int output_rcb_index = -1;
	int shortest_seek_time_any = INT_MAX;
	int shortest_seek_time_any_index = -1;
	int shortest_seek_time_directional = INT_MAX;
	int current_rbc_seek_time = 0;

	if ((*queue_cnt) > 0) {
		for (int i = 0; i < QUEUEMAX; i++) {
			if (compare_rcbs(request_queue[i], NULLRCB)) {
				continue;
			}

			current_rbc_seek_time = abs(
					current_cylinder - request_queue[i].cylinder);

			if (current_rbc_seek_time <= shortest_seek_time_any) {
				shortest_seek_time_any = current_rbc_seek_time;
				shortest_seek_time_any_index = i;
			}

			if (current_rbc_seek_time <= shortest_seek_time_directional
					&& ((scan_direction == 0
							&& current_cylinder > request_queue[i].cylinder)
							|| (scan_direction == 1
									&& current_cylinder
											< request_queue[i].cylinder))) {
				if (compare_rcbs(output_rcb, NULLRCB)
						|| (output_rcb.arrival_timestamp
								> request_queue[i].arrival_timestamp)) {
					shortest_seek_time_directional = current_rbc_seek_time;
					output_rcb = request_queue[i];
					output_rcb_index = i;
				}
			}
		}

		if (output_rcb_index == -1 && shortest_seek_time_any_index != -1) {
			output_rcb = request_queue[shortest_seek_time_any_index];
			output_rcb_index = shortest_seek_time_any_index;
		}

		if (output_rcb_index != -1) {
			remove_from_queue(request_queue, queue_cnt, output_rcb_index);
		}
	}

	return output_rcb;
}
