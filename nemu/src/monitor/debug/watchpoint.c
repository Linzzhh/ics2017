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

/* TODO: Implement the functionality of watchpoint */

WP* new_wp()
{
  if(free_==NULL) assert(0);
  WP* now=free_;
  free_=free_->next;
  now->next=NULL;
  if(head==NULL) head=now;
  else {
    WP *wp=head;
    for(;wp->next!=NULL;wp=wp->next){
      if(wp->NO<now->NO&&now->NO<wp->next->NO){
        now->next = wp->next;
	wp->next = now;
      }
    }
    if(wp->NO<now->NO) { wp->next=now;}
  }
  return now;
}
void free_wp(WP *wp)
{
  if(head->NO==wp->NO) head=head->next; 
  else{
  for(WP *h=head;h->next!=NULL;h=h->next){
    if(h->next->NO==wp->NO){
      h->next = h->next->next;
    }
  }
  }
  
  if(free_->NO>wp->NO) { wp->next=free_; free_=wp;}
  else{
    WP *f=free_;
    for(;f->next!=NULL;f=f->next){
      if(f->NO<wp->NO&&wp->NO<f->next->NO){
        wp->next=f->next;
        f->next=wp;
      }
    }
    if(f->NO<wp->NO) {f->next=wp;wp->next=NULL;}
  }
}
