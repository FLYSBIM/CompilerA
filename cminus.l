%{
#include "globals.h"
#include "util.h"
#include "scan.h"

char tokenString[MAXTOKENLEN+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}({letter}|{digit})*
newline     \n
whitespace  [ \t]+

%%

"/*"            {
                    char c;
                    while (1) {
                        c = input();
                        if (c == EOF){
				break;
			}
			else if (c == '\n'){
				lineno++;
			}
                        else if (c == '*') {
                            c = input();
                            if (c == '/') break;
                        }
                    }
                }
"int"            {return INT;}
"void"          {return VOID;}
"if"          {return IF;}
"else"           {return ELSE;}
"while"        {return WHILE;}
"return"         {return RETURN;}
"=="            {return EQ;}
"!="             {return NE;}
"<"             {return LT;}
"<="             {return LE;}
">"             {return GT;}
">="             {return GE;}
"="             {return ASSIGN;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"		{return OVER;}
";"		{return SEMI;}
","		{return COMMA;}
"("		{return LPAREN;}
")"		{return RPAREN;}
"["		{return LBRACE;}
"]"		{return RBRACE;}
"{"		{return LCURLY;}
"}"		{return RCURLY;}
{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"{"             { char c;
                  do
                  { c = input();
                    if (c == EOF) break;
                    if (c == '\n') lineno++;
                  } while (c != '}');
                }
.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}
