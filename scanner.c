#include <stdio.h>
#include "scanner.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
int count;

char *names[] = {NULL, "CONST", "CONTINUE", 
"DEFAULT", "DO", "ELSE", "EXTERN", "GOTO", "IF", "REGISTER", "RETURN", 
"SIGNED", "SIZEOF", "STATIC", "SWITCH", "TYPEDEF", "UNSIGNED", "VOLATILE", 
"AUTO", "BREAK", "CASE", "VOID", "MAIN", "TYPE", "KEYWORD", "FOR", "ID", "STR", 
"NUMBER", "LP", "RP", "LBRACE", "RBRACE", "SEMICOLON", "ASSIGN", "LESS", "ADD",
 "SUB", "MUL", "DIV", "MOD", "NOT", "LE", "ME", "EE", "AND", "QST", "OP", 
 "COM", "OB", "CB", "PRE"};

struct symbol {
    char name[50];
    int type;
    int position;
};

struct symbol symbol_tb[1000];

count = 0;

int is_in_tb(char name[50]) {
    int i = 0;
    int position = 0;
    for(i = 1; i <= count; i++) {
        if(strcmp(name, symbol_tb[i].name) == 0) {
            position = i;
        }
    }
    return position;

}

int insert_into_tb(char name[50], int type, int position) {
    if(is_in_tb(name) != 0) {
        return is_in_tb(name);
    } else {
        count++;
        symbol_tb[count].type = type;
        strncpy(symbol_tb[count].name, name, 50);
        position = count;
        return count;
    }
}

int main(void) {
    int ntoken, vtoken;

    ntoken = yylex();
    while(ntoken) {
        if(ntoken == 26) { 
            int position;   
            position = insert_into_tb(yytext, ntoken, count);  
            printf("<%s, %s, %d>\n", names[ntoken], yytext, position);
        } else {
            printf("<%s, %s>\n", names[ntoken], yytext);
        }
        ntoken = yylex();
    }

}