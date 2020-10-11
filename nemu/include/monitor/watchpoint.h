#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	char expr[32];
	uint32_t val;
	int b;

} WP;

WP* new_wp();
void free_wp(WP*);
bool check_wp();
void delete_wp(int );
void info_wp();


#endif
