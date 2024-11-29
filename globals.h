#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER
#include "y.tab.h"
#define ENDFILE 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXRESERVED 6

typedef int TokenType;

extern FILE* source;
extern FILE* listing; 
extern FILE* code;

extern int lineno;

typedef enum {StmtK,ExprK,DeclK,ParaK,TypeK} NodeKind;
typedef enum {CompK,IfifK,IfelK,IterK,RetuK} StmtKind;
typedef enum {AssiK,OperK,ConsK,IdidK,ArriK,CallK} ExpKind;
typedef enum {FuncK,VariK,ArrvK,NumbK} DeclKind;
typedef enum {ArrpK,NonaK} ParamKind;
typedef enum {TynaK} TypeKind;

typedef struct arrayAttr {
    TokenType type;
    char * name;
    int size;
} ArrayAttr;

typedef enum {Void,Integer,IntegerArray} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
   { struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     NodeKind nodekind;

     union { StmtKind stmt;
             ExpKind exp;
             DeclKind decl;
             ParamKind param;
             TypeKind type; } kind;

     union { TokenType op;
             TokenType type;
             int val;
             char * name;
             ArrayAttr arr;
     	     struct Scope* scope;} attr;
     ExpType type;
     TokenType ty;
   } TreeNode;

extern int EchoSource;
extern int TraceScan;
extern int TraceParse;
extern int TraceAnalyze;
extern int TraceCode;
extern int Error;
#endif
