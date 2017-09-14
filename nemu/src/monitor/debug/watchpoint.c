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
void print_watch()
{
  for(WP *w1=head;w1!=NULL; w1=w1->next){

      printf("the watchpoint num is %d\n",w1->NO);
      printf("the expr is %s\n",w1->exp);
      printf("the value is: %d\n",w1->value);
    }

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
        wp->next=f->next;
        f->next=wp;
	printf("%d    %d    %d\n",f->NO,wp->NO,f->next->NO);
      }
    }
    if(f->NO<wp->NO) {f->next=wp;wp->next=NULL;}
  }
  print_list();
}
int record_watch(char *e)
{
  WP *wp = new_wp();
  bool flag=true;
  bool *s=&flag;
  uint32_t val =expr(e,s);
  wp->value=val;
 // wp->exp=e;
  strcpy(wp->exp,e);
  print_watch();
  if(head==NULL) return 0;
  return 1;
}
int exam_watch()
{
  int flag=0;
  for(WP *w1=head;w1!=NULL; w1=w1->next){
    bool flag=true;
    bool *s=&flag;

    uint32_t nowvalue=expr(w1->exp,s);
    if(nowvalue!=w1->value){
      flag++;
      printf("the watchpoint num is %d\n",w1->NO);
      printf("the expr is %s\n",w1->exp);
      printf("before value is: %d\n",w1->value);
      printf("now    value is: %d\n",nowvalue);
    }
  }
  return flag;

}
void delete_watch(int i)
{
  WP *wpi=&wp_pool[i];
  free_wp(wpi);
}
