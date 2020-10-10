#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ,
	NUMBER, TK_L, TK_R,
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"==", EQ},						// equal
	{"\\*", '*'},
	{"/", '/'},
	{"\\(", TK_L},
	{"\\)", TK_R},
	{"\\b[0-9]+\\b", NUMBER},
	{"-", '-'},
	
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

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
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

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;
strncpy(tokens[nr_token].str, substr_start, substr_len);
				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case 0:break;
					
					tokens[nr_token].type = rules[i].token_type;
					nr_token++;
					;
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}


bool check_parenthese(int p,int q){
	int i,tag=0;
	if(tokens[p].type!=5||tokens[q].type!=6)
		for(i=p;i<=q;i++){
			if(tokens[i].type==5) tag++;
			else if(tokens[i].type==6) tag--;
			if(tag==0&&i<q)return false;
		}
	if(tag!=0) return false;
	return true;
}

int pir(int type){
	if(type==1||type==8)return 1;
	else return 2;
}

int dominant_operator(int p, int q){
	int i, dom=p, left=0;
	int pr=-1;
	for(i=p;i<=q;i++){
		if(tokens[i].type==5){
			left+=1;
			i++;
			while(1){
				if(tokens[i].type==5)left+=1;
				else if(tokens[i].type==6)left--;
				i++;
				if(left==0) break;
			}
			if(i>q)break;
		}
		else if(tokens[i].type==7)continue;
		else if(pir(tokens[i].type)>pr){
			pr=pir(tokens[i].type);
			dom=i;
		}
	}
	return dom;
}

int eval(int p,int q){
        if(p>q){
        	printf("Bad expression");
        }
        else if(p==q){
		if(tokens[p].type==7){printf("%d",tokens[p].str[p]); return (int)tokens[p].str[p];}
        }
	else if(check_parenthese(p,q)==true){

		return eval(p+1,q-1);
	}
	else{
		int op = dominant_operator(p,q);
		int val1=eval(p,op-1);
		int val2=eval(op+1,q);

		switch(tokens[op].type){
			case 1:return val1+val2;
			case 8:return val1-val2;
			case 3:return val1*val2;
			case 4:return val1/val2;
			default:assert(0);
		}
	}
	return 0;
}


uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	else{
		*success=true;
		int p=0,q=nr_token;
		printf("%d", nr_token);
		return eval(p,q);
	}
}

