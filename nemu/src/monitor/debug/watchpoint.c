#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp(){
	if(free_){
		WP* result = free_;
		free_ = free_->next;
		result->next = head;
		head = result;
		return result;
	}
	else{
		assert(0);
	}
}

void free_wp(WP* wp){
	WP *p = head;
	if(p->NO == wp->NO)
		head = head->next;
	else{
		while(p->next->NO != wp->NO){
			p = p->next;
		}
		p->next = p->next->next;
	}
	wp->next = free_;
	free_ = wp;

}
/* TODO: Implement the functionality of watchpoint */


