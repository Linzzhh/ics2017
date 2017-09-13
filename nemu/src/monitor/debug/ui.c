#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
//void make_token(char *e);
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

//Single Step
static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  if(arg!=NULL) {
    //printf("%s\n",arg);
    int num=atoi(arg);
    cpu_exec(num);
  }
  else{
    cpu_exec(1);
  }
  return 0;
}
//Print Registers
static void print_reg()
{
  for(int i=R_EAX;i<=R_EDI;i++){
    printf("%s: 0x%08x\n",regsl[i],cpu.gpr[i]._32);
  }
  printf("eip: 0x%08x\n",cpu.eip);
}
//Print Informations
static int cmd_info(char *args)
{
  char *arg = strtok(NULL, " ");
  if (strcmp(arg, "r") == 0) {
    print_reg();
  }
  return 0;
}
//Scan the memory
static int cmd_x(char *args)
{
	int length,address;
	sscanf(args,"%d 0x%x",&length,&address);
	for(int i=0;i<length;++i) {
		printf("\n0x%08x: ",address+i*4);
		printf("0x%08x",vaddr_read((address+i*4),4));
	}
	printf("\n"); 
	return 0; 
}
static int cmd_p(char *args)
{
	bool flag=true;
	bool *s=&flag;
	uint32_t value=expr(args,s);
	printf("%d\n 0x%08x \n",value,value);
	return 0;
}
#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))


static struct {
  char *name;
  char *description;
  int (*handler) (char *);
}  cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  /* TODO: Add more commands */
  { "si", "Single Step", cmd_si},
  { "info", "Print Infomations", cmd_info},
  { "x", "Scan the memory", cmd_x},
  { "p", "get the value of expr", cmd_p},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
