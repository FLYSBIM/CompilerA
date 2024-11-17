
/****************************************************/
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
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* storesgg syntax tree for later return */
%}

%token IF ELSE WHILE RETURN VOID INT
%token ID NUM 
%token ASSIGN EQ NE LT LE GT GE PLUS MINUS TIMES OVER LPAREN RPAREN LBRACE RBRACE LCURLY RCURLY SEMI COMMA
%token ERROR ENDFILE

%nonassoc ELSE

%% /* Grammar for TINY */

program
    : declaration_list
    { savedTree = $1; }
    ;
declaration_list
    : declaration_list declaration
    { YYSTYPE t = $1;
      if (t != NULL){
	  while (t->sibling != NULL)
              t = t->sibling;
          t->sibling = $2;
	  $$ = $1; }
      else $$ = $2;
      }
    | declaration
    { $$ = $1; }
    ;
declaration
    : var_declaration
    { $$ = $1; }
    | fun_declaration
    { $$ = $1; }
    ;
var_declaration
    : type_specifier ID SEMI
    { $$ = newDeclNode(VarK);
      $$->child[0] = $1;
      $$->lineno = lineno;
      $$->attr.name = copystring($2);
    }
    | type_specifier ID LBRACE NUM RBRACE SEMI
    { $$ = newDeclNode(ArrVarK);
      $$->child[0] = $1;
      $$->lineno = lineno;
      $$->attr.arr.name = savedName;
      $$->attr.arr.size = atoi(tokenString);
    }
    ;
type_specifier
    : INT
    { $$ = newTypeNode(TypeNameK);
      $$->attr.type = INT;
    }
    | VOID
    { $$ = newTypeNode(TypeNameK);
      $$->attr.type = VOID;
    }
    ;
fun_declaration
    : type_specifier ID LPAREN params RPAREN compound_stmt
    { $$ = newDeclNode(FunK);
      $$->lineno = lineno;
      $$->child[0] = $1;
      $$->attr.name = copystring($2);
      $$->child[1] = $4;
      $$->child[2] = $6;
    }
    ;
params
    : param_list
    { $$ = $1; }
    | VOID
    { $$ = newTypeNode(TypeK); 
      $$->attr.type = VOID;
    }
    ;
param_list
    : param_list COMMA param
    { YYSTYPE t = $1;
      if(t != NULL)
      { while (t->sibling != NULL)
           t = t->sibling;
        t->sibling = $3;
        $$ = $1; }
      else $$ = $3;
    }
    | param
    { $$ = $1; }
    ;
param
    : type_specifier ID
    { $$ = newDeclNode(NonArrParamK);
      $$->child[0] = $1;
      $$->attr.name = copystring($2);
    }
    | type_specifier ID LBRACE RBRACE
    { $$ = newDeclNode(ArrParamK);
      $$->child[0] = $1;
      $$->attr.name = copystring($2);
    }
    ;
compound_stmt
    : LBRACE local_declarations statement_list RBRACE
    { $$ = newStmtNode(CompK);
      $$->child[0] = $2;
      $$->child[1] = $3;
    }
    ;
local_declarations
    : local_declarations var_declaration
    { YYSTYPE t=  $1;
      if(t != NULL)
      { while (t->sibling != NULL)
           t = t->sibling;
      t->sibling = $2;
      $$ = $1; }
      else $$ = $2;
    }
    | /* empty */
    { $$ = NULL; }
    ;
statement_list
    : statement_list statement
    { YYSTYPE t = $1;
      if(t != NULL)
      { while (t->sibling != NULL)
          t = t->sibling;
      t->sibling = $2;
      $$ = $1; }
      else $$ = $2;
    }
    | /* empty */
    { $$ = NULL; }
    ;
statement
    : expression_stmt
    { $$ = $1; }
    | compound_stmt
    { $$ = $1; }
    | selection_stmt
    { $$ = $1; }
    | iteration_stmt
    { $$ = $1; }
    | return_stmt
    { $$ = $1; }
    ;
expression_stmt
    : expression SEMI
    { $$ = $1; }
    | SEMI
    { $$ = NULL; }
    ;
selection_stmt
    : IF LPAREN expression RPAREN statement
    { $$ = newStmtNode(IfK);
      $$->child[0] = $3;
      $$->child[1] = $5;
    }
    | IF LPAREN expression RPAREN statement ELSE statement
    { $$ = newStmtNode(IfK);
      $$->child[0] = $3;
      $$->child[1] = $5;
      $$->child[2] = $7;
    }
    ;
iteration_stmt
    : WHILE LPAREN expression RPAREN statement
    { $$ = newStmtNode(IterK);
      $$->child[0] = $3;
      $$->child[1] = $5;
    }
    ;
return_stmt
    : RETURN SEMI
    { $$ = newStmtNode(RetuK); }
    | RETURN expression SEMI
    { $$ = newStmtNode(ReturnK);
      $$->child[0] = $2;
    }
    ;
expression
    : var ASSIGN expression
    { $$ = newExpNode(AssignK);
      $$->child[0] = $1;
      $$->child[1] = $3;
    }
    | simple_expression
    { $$ = $1; }
    ;
var
    : ID
    { $$ = newExpNode(IdK);
      $$->attr.name = copystring(tokenstring);
    }
    | ID LBRACE expression RBRACE
    { $$ = newExpNode(ArrIdK);
      $$->attr.name = copystring($1);
      $$->child[0] = $3;
    }
    ;
simple_expression
    : additive_expression relop additive_expression
    { $$ = newExpNode(OpK);
      $$->child[0] = $1;
      $$->child[1] = $3;
      $$->attr.op = $2;
    }
    | additive_expression
    { $$ = $1; }
    ;
relop
    : LT
    { $$ = LT; }
    | LE
    { $$ = LE; }
    | GT
    { $$ = GT; }
    | GE
    { $$ = GE; }
    | EQ
    { $$ = EQ; }
    | NE
    { $$ = NE; }
    ;
additive_expression
    : additive_expression addop term
    { $$ = newExpNode(OpK);
      $$->child[0] = $1;
      $$->child[1] = $3;
      $$->attr.op = $2;
    }
    | term
    { $$ = $1; }
    ;
addop
    : PLUS
    { $$ = PLUS; }
    | MINUS
    { $$ = MINUS; }
    ;
term
    : term mulop factor
    { $$ = newExpNode(OpK);
      $$->child[0] = $1;
      $$->child[1] = $3;
      $$->attr.op = $2;
    }
    | factor
    { $$ = $1; }
    ;
mulop
    : TIMES
    { $$ = TIMES; }
    | OVER
    { $$ = OVER; }
    ;
factor
    : LPAREN expression RPAREN
    { $$ = $2; }
    | NUM
    { $$ = newExpNode(ConstK);
      $$->attr.val = atoi(tokenString);
    }
    | var
    { $$ = $1; }
    | call
    { $$ = $1; }
    ;
call
    : ID LPAREN args RPAREN
    { $$ = newExpNode(CallK);
      $$->attr.name = copystring($1);
      $$->child[0] = $3;
    }
    ;
args
    : arg_list
    { $$ = $1; }
    | /* empty */
    { $$ = NULL; }
    ;
arg_list
    : arg_list COMMA expression
    { YYSTYPE t = $1;
      if(t != NULL)
      { while (t->sibling != NULL)
          t = t->sibling;
      t->sibling = $3;
      $$ = $1; }
      else $$ = $3;
    }
    | expression
    { $$ = $1; }
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
