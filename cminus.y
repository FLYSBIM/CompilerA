
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

#define YYSTYPE TreeNode*
static int savedNumber;
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* storesgg syntax tree for later return */
static int yylex(void);
int yyerror(char* message);
%}

%token IF ELSE WHILE RETURN VOID INT
%token ID NUM 
%token ASSIGN EQ NE LT LE GT GE PLUS MINUS TIMES OVER LPAREN RPAREN LBRACE RBRACE LCURLY RCURLY SEMI COMMA
%token ERROR


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
identifier
    : ID
    	{ savedName = copyString(tokenString);
    	  savedLineNo = lineno;
    	}
    ;
number
    : NUM
    	{ savedNumber = atoi(tokenString);
    	  savedLineNo = lineno;
    	}
    ;
var_declaration
    : type_specifier identifier SEMI
    { $$ = newDeclNode(VarK);
      $$->type=$1->type;
      $$->lineno = lineno;
      $$->attr.name = savedName;
    }
    | type_specifier identifier LBRACE number RBRACE SEMI
    { $$ = newDeclNode(ArrVarK);
      $$->type = $1->type;
      $$->lineno = lineno;
      $$->attr.arr.name = savedName;
      $$->attr.arr.size = savedNumber;
    }
    ;
type_specifier
    : INT
    {
      $$->type = Integer;
    }
    | VOID
    {
      $$->type = Void;
    }
    ;
fun_declaration
    : type_specifier identifier
    { $$ = newDeclNode(FunK);
      $$->lineno = lineno;
      $$->attr.name = savedName;
    } LPAREN params RPAREN compound_stmt{
      $$ = $3;
      $$->type=$1->type;
      $$->child[0] = $5;
      $$->child[1] = $7;
    }
    ;
params
    : param_list
    { $$ = $1; }
    | VOID
    { $$ = newParaNode(NonArrParaK); 
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
    : type_specifier identifier
    { $$ = newParaNode(NonArrParaK);
      //$$->child[0] = $1;
      $$->type = $1->type;
      $$->attr.name = savedName;
    }
    | type_specifier identifier LBRACE RBRACE
    { $$ = newParaNode(ArrParaK);
     // $$->child[0] = $1;
      $$->type = $1->type;
      $$->attr.name = savedName;
    }
    ;
compound_stmt
    : LCURLY local_declarations statement_list RCURLY
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
    { $$ = newStmtNode(RetuK); 
      $$->type = Void;
    }
    | RETURN expression SEMI
    { $$ = newStmtNode(RetuK);
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
    : identifier
    { $$ = newExpNode(IdK);
      $$->attr.name = savedName;
    }
    | identifier
    { $$ = newExpNode(ArrIdK);
      $$->attr.name = savedName;
    } LBRACE expression RBRACE{
      $$=$2;
      $$->child[0] = $4;
    }
    ;
simple_expression
    : additive_expression relop additive_expression
    { $$=newExpNode(OpK);
      $$->attr.op=$2->attr.op;
      $$->child[0] = $1;
      $$->child[1] = $3;
    }
    | additive_expression
    { $$ = $1; }
    ;
relop
    : LT
    { $$->attr.op= LT; }
    | LE
    { $$->attr.op= LE; }
    | GT
    { $$->attr.op= GT; }
    | GE
    { $$->attr.op= GE; }
    | EQ
    { $$->attr.op= EQ; }
    | NE
    { $$->attr.op= NE; }
    ;
additive_expression
    : additive_expression addop term
    { $$ = newExpNode(OpK);
      $$->child[0] = $1;
      $$->child[1] = $3;
      $$->attr.op = $2->attr.op;
    }
    | term
    { $$ = $1; }
    ;
addop
    : PLUS
    { $$->attr.op = PLUS; }
    | MINUS
    { $$->attr.op = MINUS; }
    ;
term
    : term mulop factor
    { $$ = newExpNode(OpK);
      $$->child[0] = $1;
      $$->child[1] = $3;
      $$->attr.op = $2->attr.op;
    }
    | factor
    { $$ = $1; }
    ;
mulop
    : TIMES
    { $$->attr.op = TIMES; }
    | OVER
    { $$->attr.op = OVER; }
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
    : identifier
    { $$ = newExpNode(CallK);
      $$->attr.name = savedName;
    } LPAREN args RPAREN{
      $$ = $2;
      $$->child[0] = $4;
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
