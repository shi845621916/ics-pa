#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
float eval(int p,int q);
bool check_parentheses(int p,int q);
enum {
  TK_NOTYPE = 256, TK_NUM, TK_EQ

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"==", TK_EQ},         // equal
  {"^[0-9]+",TK_NUM},
  {"\\+", '+'},         // plus
  {"\\-", '-'},
  {"\\*", '*'},
  {"\\/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
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
    printf("str = %s\n", e+position);
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

        switch (rules[i].token_type) {
          default: 
            strncpy(tokens[nr_token].str,e+position-substr_len,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  for(int i = 0 ; i<nr_token;i++){
    printf("i:%d,type:%d,str:%s\n", i,tokens[i].type,tokens[i].str);
  }
  //TODO();
  printf("result= %f\n", eval(0,nr_token-1));
  return 0;
}

float eval(int p,int q){
  printf("p:%d,q:%d\n", p,q);
  if(p > q){
    printf("ERROR\n");
    assert(0);
  }
  else if(p == q){
    return atof(tokens[p].str);

  }
  else if(check_parentheses(p,q) == true){
    printf("Has parenthes\n");
    return eval(p+1,q-1);
  }
  else{
    int isParenthes = 0;
    int op = 0;
    for (int i = 0; i < nr_token; i++)
    {
      switch(tokens[nr_token].type){
        case '(':
          isParenthes++;
          break;
        case ')':
          isParenthes--;
          break;
        case '+':
        case '-':
          if(isParenthes == 0)
            op = nr_token;
          break;
        case '/':
        case '*':
          if(isParenthes == 0 && tokens[op].type != '+' && tokens[op].type != '-')
            op = nr_token;
          break;
      }
    }
    printf("op=%d\n", op);
    float val1 = eval(p, op-1);
    float val2 = eval(op+1, q);

    switch(tokens[op].type){
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default : assert(0);
    }
  }
}

bool check_parentheses(int p,int q){
  bool result = false;
  int isParenthes = 0;
  for (int i = p; i <= q; i++)
  {
    if(tokens[i].type == '('){
      isParenthes++;
    }else if(tokens[i].type == ')'){
      isParenthes--;
    }
  }

  if(isParenthes == 0 && tokens[p].type == '(' && tokens[q].type == ')')
    result = true;
  return result;
}