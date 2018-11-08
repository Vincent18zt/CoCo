%{
#include "scanner.h"
int COMMENT = 0;
%}
identifier [[:alpha:]_][[:alnum:]_]*
%%
#.* {return PRE;}
const {if(!COMMENT) return CONST;}
continue {if(!COMMENT) return CONTINUE;}
default {if(!COMMENT) return DEFAULT;}
do {if(!COMMENT) return DO;}
else {if(!COMMENT) return ELSE;}
extern {if(!COMMENT) return EXTERN;}
goto {if(!COMMENT) return GOTO;}
if {if(!COMMENT) return IF;}
register {if(!COMMENT) return REGISTER;}
return {if(!COMMENT) return RETURN;}
signed {if(!COMMENT) return SIGNED;}
sizeof {if(!COMMENT) return SIZEOF;}
static {if(!COMMENT) return STATIC;}
switch {if(!COMMENT) return SWITCH;}
typedef {if(!COMMENT) return TYPEDEF;}
unsigned {if(!COMMENT) return UNSIGNED;}
volatile {if(!COMMENT) return VOLATILE;}
while {if(!COMMENT) return KEYWORD;}
auto {if(!COMMENT) return AUTO;}
break {if(!COMMENT) return BREAK;}
case {if(!COMMENT) return CASE;}
void {if(!COMMENT) return VOID;}
main {if(!COMMENT) return MAIN;}
float {if(!COMMENT) return TYPE;}
char {if(!COMMENT) return TYPE;}
int {if(!COMMENT) return TYPE;}
long {if(!COMMENT) return TYPE;}
enum {if(!COMMENT) return TYPE;}
struct {if(!COMMENT) return TYPE;}
union {if(!COMMENT) return TYPE;}
short {if(!COMMENT) return TYPE;}
double {if(!COMMENT) return TYPE;}
for {if(!COMMENT) return FOR;}
"/*" {COMMENT = 1;}
"*/" {COMMENT = 0;}
{identifier}(\[[0-9]*\])? {if(!COMMENT) return ID;}
\".*\" {if(!COMMENT) return STR;}
[0-9]+ {if(!COMMENT) return NUMBER;}
[[:space:]]+ {}
\( {if(!COMMENT) return LP;}
\) {if(!COMMENT) return RP;}
\{ {if(!COMMENT) return LBRACE;}
\} {if(!COMMENT) return RBRACE;}
\; {if(!COMMENT) return SEMICOLON;}
\= {if(!COMMENT) return ASSIGN;}
\< {if(!COMMENT) return LESS;}
\+ {if(!COMMENT) return ADD;}
\- {if(!COMMENT) return SUB;}
\* {if(!COMMENT) return MUL;}
\/ {if(!COMMENT) return DIV;}
\% {if(!COMMENT) return MOD;}
\! {if(!COMMENT) return NOT;}
\<= {if(!COMMENT) return LE;}
\>= {if(!COMMENT) return ME;}
\== {if(!COMMENT) return EE;}
\& {if(!COMMENT) return AND;}
\? {if(!COMMENT) return QST;}
\> {if(!COMMENT) return OP;}
\, {if(!COMMENT) return COM;}
\[ {if(!COMMENT) return OB;}
\] {if(!COMMENT) return CB;}
%%
int yywrap(void) {
    return 1;
}