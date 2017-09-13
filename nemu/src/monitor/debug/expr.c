#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256,NUM,HEX,REG
	  , TK_EQ,TK_NEQ,LOGIC_AND,LOGIC_OR,NOT,DEREF

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},          // spaces
  {"\\+", '+'},               // plus
  {"\\-", '-'},               // minus
  {"\\*", '*'},               // mul
  {"\\/", '/'},               // div
  {"^0x[0-9a-fA-F]+", HEX},   // hex
  {"[0-9]+", NUM},            // decimal
  {"\\(", '('},               // lbracket
  {"\\)", ')'},               // rbracket
  {"==", TK_EQ},              // equal
  {"^\\$[a-zA-Z]+", REG},     // reg_name
  {"!=", TK_NEQ},             // not equal
  {"&&", LOGIC_AND},          // logic and
  {"\\|\\|", LOGIC_OR},       // logic or
  {"!", '!'},                 // not
 


};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
      //  printf("%s  %d",rules[i].regex,rules[i].token_type);
        switch (rules[i].token_type) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '(':
		case ')':
		case '!':
		case TK_EQ:
		case TK_NEQ:
		case LOGIC_AND:
		case LOGIC_OR:
	          tokens[nr_token++].type=rules[i].token_type;break;
		case NUM:
		case HEX:
		  tokens[nr_token].type=rules[i].token_type;
		  for(int j=0;j<substr_len;j++)
			tokens[nr_token].str[j]=substr_start[j];
	          nr_token++;
		break;
		case REG:
		  tokens[nr_token].type=rules[i].token_type;
		  for(int j=1;j<substr_len;j++)
			tokens[nr_token].str[j-1]=substr_start[j];
                  nr_token++;
		default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
//  for(int i=0;i<nr_token;i++)
//  {
//  printf("%d    ",tokens[i].type);
//  if(tokens[i].type>255) printf("%s  ",tokens[i].str); 
//  }
  return true;
}

static int get_level(int op)
{
	if(op=='!'||op==DEREF) return 1;
	if(op=='+'||op=='-') return 2;
	if(op=='*'||op=='/') return 3;
	if(op==TK_EQ||op==TK_NEQ) return 4;
	if(op==LOGIC_AND||op==LOGIC_OR) return 5;
	return 10000;
}
static int cmp_op(int op1,int op2)
{

   if(get_level(op1)==get_level(op2)) return 0;
   if(get_level(op1)<get_level(op2))  return -1;
   if(get_level(op1)>get_level(op2))  return 1;
   return -2;
}
static int get_dominant_optype(int p,int q){//get the index...
	int inbracket=0;
	int op=10000; 
	int index=p;
	for(int i=p;i<=q;i++){
		int tp=tokens[i].type;
		if(tp<TK_NOTYPE||tp>REG){ //the type is a operation
		if(tp=='(') {inbracket++;continue;}
		if(tp==')') {inbracket--;continue;}
		if(inbracket>0) continue; // in the bracket..
                if(cmp_op(op,tp)>=0) {op=tp;index=i;}
	}
	}
	return index;
}
static bool check_parentheses(int p, int q)
{
	if(tokens[p].type=='('&&tokens[q].type==')'){
		int cnt=0;
		for(int i=p;i<=q;i++){
		if(tokens[i].type=='(') cnt++;
		else if(tokens[i].type==')') cnt--;
		if(cnt<=0&&i<q) return false;
	}
	if(cnt>0) return false;
	}
	else{
//	printf("not surrounded!");
	return false;
	}
	return true;
}
static uint32_t eval(int p,int q)
{	if(p>q){
	  printf("bad expression!!"); return -1;
	}
	else if(p==q){
                if(tokens[p].type==NUM) //the value is the decimal
	          return atoi(tokens[p].str);
		else if(tokens[p].type==HEX) //the value is the hex
		  return strtol(tokens[p].str,NULL,16);
		else if(tokens[p].type==REG){// the value is register
		for(int i=R_EAX;i<=R_EDI;i++)
			if(strcmp(regsl[i],tokens[p].str)==0){
			return cpu.gpr[i]._32;
			}
		}
		  return -1;
	  // return the value of number
	}
	else if(check_parentheses(p,q)==true){
	//the expression is surrouded by a match
      		return eval(p+1,q-1);
	}else{
	  int op=get_dominant_optype(p,q);
	  int optype=tokens[op].type;
	  int val1,val2;
	  if(optype!='!'&&optype!=DEREF)
	    val1=eval(p,op-1);
	  val2=eval(op+1,q);
	  switch(optype){
		case '+':return val1+val2;
		case '-':return val1-val2;
		case '*':return val1*val2;
		case '/':return val1/val2;
		case TK_NEQ:return val1!=val2;
		case LOGIC_AND:return val1&&val2;
		case LOGIC_OR:return val1||val2;
		case TK_EQ:return val1==val2;
		case DEREF: vaddr_read(val2,4);
		case '!':return !val2;
		default :assert(0);
	}
	}
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
 // if( check_parentheses(0,nr_token)) printf("success!!");
 // else printf("error!!!!!!!");
 //int index =  get_dominant_optype(0,nr_token);
 //printf("%c %d",tokens[index].type,index);
  
  for(int i=0;i<nr_token;i++){
  	if(tokens[i].type=='*'&&(i==0||tokens[i-1].type!=NUM))
	  tokens[i].type=DEREF;
  }

  uint32_t value=eval(0,nr_token-1);
  printf("%d  0x%08x \n",value,value);
  return 0;
}
