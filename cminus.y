/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedNumber;
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);
static ExpType savedType;
%}

/* reserved words */
%token ELSE IF INT RETURN VOID WHILE
/* multicharacter tokens */
%token ID NUM
/* special symbols */
%token PLUS MINUS TIMES OVER LT LTEQ GT GTEQ EQ NEQ ASSIGN SEMI COMMA
%token LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE
/* book-keeping tokens */
%token ERROR

%% /* Grammar for TINY */
program     : decl_list
                 { savedTree = $1;}
            ;
decl_list   : decl_list decl
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
                 }
            | decl  { $$ = $1; }
            ;
decl        : var_decl  { $$ = $1; }
            | fun_decl  { $$ = $1; }
            ;
saveName    : ID
                 { savedName = copyString(tokenString);
                   savedLineNo = lineno;
                 }
            ;
saveNumber  : NUM
                 { 
		   $$=newDeclNode(NumK);
		   savedNumber = atoi(tokenString);
		   $$->attr.val=savedNumber;
                   savedLineNo = lineno;
                 }
            ;
var_decl    : type_spec saveName SEMI
                 { $$ = newDeclNode(VarK);
                   $$->lineno = lineno;
                   $$->attr.name = savedName;
		   $$->type=$1->type;
                 }
            | type_spec saveName LBRACK saveNumber RBRACK SEMI
                 { $$ = newDeclNode(ArrVarK);
                   $$->lineno = lineno;
                   $$->attr.arr.name = savedName;
                   //$$->attr.arr.size = savedNumber;
		   $$->type=Integer;
		   $$->child[0]=$4;
                 }
            ;
type_spec   : INT
                 { $$ = newTypeNode(TypeNameK);
                   $$->attr.type = INT;
		   $$->type=Integer;
                 }
            | VOID
                 { $$ = newTypeNode(TypeNameK);
                   $$->attr.type = VOID;
                 }
            ;
fun_decl    : type_spec saveName {
                   $$ = newDeclNode(FuncK);
                   $$->lineno = lineno;
                   $$->attr.name = savedName;
		   $$->type=$1->type;
                 }
              LPAREN params RPAREN comp_stmt
                 {
                   $$ = $3;
                   $$->child[1] = $5;   
                   $$->child[2] = $7; 
                 }
            ;
params      : param_list  { $$ = $1; }
            | VOID
                 { $$ = newTypeNode(TypeNameK);
                   $$->attr.type = VOID;
                 }
param_list  : param_list COMMA param
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $3;
                     $$ = $1; }
                     else $$ = $3; 
                 }
            | param { $$ = $1; };
param       : type_spec saveName
                 { $$ = newParamNode(NonArrParamK);
                   $$->attr.name = savedName;
		   $$->type=$1->type;
                 }
            | type_spec saveName
              LBRACK RBRACK
                 { $$ = newParamNode(ArrParamK);
                   $$->attr.name = savedName;
		   $$->type=$1->type;
                 }
            ;
comp_stmt   : LBRACE local_decls stmt_list RBRACE
                 { $$ = newStmtNode(CompK);
                   $$->child[0] = $2; /* local variable declarations */
                   $$->child[1] = $3; /* statements */
                 }
            ;
local_decls : local_decls var_decl
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
                 }
            | /* empty */ { $$ = NULL; }
            ;
stmt_list   : stmt_list stmt
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
                 }
            | /* empty */ { $$ = NULL; }
            ;
stmt        : exp_stmt { $$ = $1; }
            | comp_stmt { $$ = $1; }
            | sel_stmt { $$ = $1; }
            | iter_stmt { $$ = $1; }
            | ret_stmt { $$ = $1; }
            ;
exp_stmt    : exp SEMI { $$ = $1; }
            | SEMI { $$ = NULL; }
            ;
sel_stmt    : IF LPAREN exp RPAREN stmt
                 { $$ = newStmtNode(IfK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                   $$->child[2] = NULL;
                 }
            | IF LPAREN exp RPAREN stmt ELSE stmt
                 { $$ = newStmtNode(IfelseK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                   $$->child[2] = $7;
                 }
            ;
iter_stmt   : WHILE LPAREN exp RPAREN stmt
                 { $$ = newStmtNode(IterK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                 }
            ;
ret_stmt    : RETURN SEMI
                 { $$ = newStmtNode(RetK);
                   $$->child[0] = NULL;
                 }
            | RETURN exp SEMI
                 { $$ = newStmtNode(RetK);
                   $$->child[0] = $2;
                 }
            ;
exp         : var ASSIGN exp
                 { $$ = newExpNode(AssignK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                 }
            | simple_exp { $$ = $1; }
            ;
var         : saveName
                 { $$ = newExpNode(IdK);
                   $$->attr.name = savedName;
                 }
            | saveName 
                 { $$ = newExpNode(ArrIdK);
		   $$->attr.name= savedName;
		 } LBRACK exp RBRACK
                 { 
		   $$=$2;
		   $$->child[0] = $4;
		 }
            ;
simple_exp  : add_exp LTEQ add_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = LTEQ;
                 }
            | add_exp LT add_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = LT;
                 }
            | add_exp GT add_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = GT;
                 }
            | add_exp GTEQ add_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = GTEQ;
                 }
            | add_exp EQ add_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = EQ;
                 }
            | add_exp NEQ add_exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = NEQ;
                 }
            | add_exp { $$ = $1; }
            ;
add_exp     : add_exp PLUS term
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = PLUS;
                 }
            | add_exp MINUS term
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = MINUS;
                 }
            | term { $$ = $1; }
            ;
term        : term TIMES factor
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = TIMES;
                 }
            | term OVER factor
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = OVER;
                 }
            | factor { $$ = $1; }
            ;
factor      : LPAREN exp RPAREN { $$ = $2; }
            | var { $$ = $1; }
            | call { $$ = $1; }
            | saveNumber //NUM
                 { $$=$1;
		   //$$ = newExpNode(ConstK);
                   //$$->attr.val = atoi(tokenString);
                 }
            ;
call        : saveName {
                 $$ = newExpNode(CallK);
                 $$->attr.name = savedName;
              }
              LPAREN args RPAREN
                 { $$ = $2;
                   $$->child[0] = $4;
                 }
            ;
args        : arg_list { $$ = $1; }
            | /* empty */ { $$ = NULL; }
            ;
arg_list    : arg_list COMMA exp
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $3;
                     $$ = $1; }
                     else $$ = $3;
                 }
            | exp { $$ = $1; }
            ;

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}
