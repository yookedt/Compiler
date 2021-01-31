#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//トークンの種類
typedef enum{
	TK_RESERVED, //記号
	TK_NUM,       //整数トークン
	TK_EOF,		 //入力の終わりを表すトークン
}TokenKind;

typedef struct Token Token;

//トークン型
struct Token{
	TokenKind kind; //トークンの種類
	Token *next;	//次の入力トークン
	int val;		//kindがTK_NUMの場合、その数値
	char *str; 		//トークンの文字列
};

Token *token;

//エラーを報告するための関数
//printfと同じ引数を取る
void error(char *fmt, ...){
	va_list ap;
	va_start(ap,fmt);
	vfprintf(stderr,fmt,ap);
	fprintf(stderr,"\n");
	exit(1);
}

//次のトークンが期待してる記号の時には、トークンを一つ読み進めて
//真を返す。それ以外の場合には偽を返す
bool consume(char op){ //引数には+や-と言った記号が入る
	if(token->kind != TK_RESERVED || token->str[0] != op) //この次のtokenが記号でであるか、この次の文字が引数と同じであるかを確認している.
		return false;//違うのであればfalseを返す
	token = token->next;//trueであるならば、tokenを次に移す.
	return true;
}

//次のトークンが期待している記号のときには、トークンを一つ読み進める。
//それ以外の場合にはエラーを表示する
void expect(char op){
	if(token->kind != TK_RESERVED || token->str[0] != op)
		error("'%c'ではありません",op);
	token = token->next;
}

//次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す
//それ以外の場合にはエラーを報告する.
int expect_number(){
	if(token->kind != TK_NUM) 
		error("数ではありません");
	int val = token->val;	
	token = token->next;
	return val;
}

bool at_eof(){
	return token->kind == TK_EOF;
}

//新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind,Token *cur,char *str){
	Token *tok = calloc(1,sizeof(Token)); //型を読み込んでメモリを確保する
	tok->kind = kind; //kind...トークン型
	tok->str = str; //strトークン文字列
	cur->next = tok; 
	return tok;
}

//入力文字pをトークナイズしてそれを返す
Token *tokenize(char *p){ //char  *p...argv[1]
	Token head; //tokenの一番最初
	head.next = NULL; //次のリストを初期化している
	Token *cur = &head; //curとtokenizeは同じものを指す

	while(*p){//while文なのでp++しなくても次の文字へ進
	//空白文字をスキップ
	//issapaceは文字が空白文字かチェックする関数,空白なら真
		if(isspace(*p)){ //空白文字なら
			p++; //ポインタを次に移す
			continue; //if文を抜ける
		}

		if(*p == '+' || *p == '-'){ //記号なら
			cur = new_token(TK_RESERVED,cur,p++); //new_tokenによってtokenを整理してcurにつなげる
			continue;	//if文を抜ける
		}
	
		if(isdigit(*p)){ //isdigitは数字であるかチェックする
			cur = new_token(TK_NUM,cur,p); //new tokenによってtokenを整理してcurにつなげる
			cur->val = strtol(p,&p,10); //strtpl(変換したい文字列,スペースがある手前のポインタ,基数)
			continue;
		}
	
	error("トークナイズできません");
	}

	new_token(TK_EOF,cur,p); 
	return head.next; //次のリストへ移す,curも同じく移る
}


int main(int argc,char **argv){
	if(argc != 2){
		error("引数の個数が正しくありません");
		return 1;
	}

	//トークナイズする
	token = tokenize(argv[1]);

	//アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	
	//式の最初を数でなければならないので、それをチェックして
	//最初のmov命令を出力
	printf("  mov rax,%d\n",expect_number());

	//'+<数>'あるいは'-<数>'というトークンの並びを消費しつつ
	//アセンブリを出力
	while(!at_eof()){
		if(consume('+')){
			printf("  add rax, %d\n",expect_number());
			continue;
		}

		expect('-');
		printf("  sub rax, %d\n",expect_number());
	}

	printf("  ret\n");
	return 0;
} 
	
