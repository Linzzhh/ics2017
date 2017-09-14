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

void print_list()
{
  printf("current free list:\n");
  for(WP *w1=free_;w1!=NULL; w1=w1->next){
    printf("%d    ",w1->NO);
  }
  printf("\n");

  printf("current head list:\n");
  for(WP *w1=head;w1!=NULL; w1=w1->next){
    printf("%d    ",w1->NO);
  }
  printf("\n");

}
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
	break;
      }
    }
    if(wp->NO<now->NO) { wp->next=now;}
  }
  print_list();
  return now;
}
void free_wp(WP *wp)
{
	printf("!!!!!!!!!!!\n");
  if(head->NO==wp->NO) head=head->next; 
  else{
    WP *h=head;
    for(;h->next!=NULL;h=h->next){
      if(h->next->NO==wp->NO){
        h->next = h->next->next;
	break;
      }
    } 
    }
  if(free_->NO>wp->NO) { wp->next=free_; free_=wp;}
  else{
    WP *f=free_;
    for(;f->next!=NULL;f=f->next){
      if(f->NO<wp->NO&&wp->NO<f->next->NO){
	printf("%d    %d     %d\n",f->NO,wp->NO,f->next->NO);
        wp->next=f->next;
        f->next=wp;
	break;
      }
    }
    if(f->NO<wp->NO) {f->next=wp;wp->next=NULL;}
  }
  print_list();
}
void watch(char *expr)
{
  new_wp();
  new_wp();
  new_wp();
  new_wp();
  WP *wp1=&wp_pool[1];
  WP *wp3=&wp_pool[3];
  WP *wp2=&wp_pool[2];
  free_wp(wp1);
  free_wp(wp3);
  free_wp(wp2);
}
