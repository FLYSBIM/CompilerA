/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

typedef struct BucketListRec //node
   { char * name;
     LineList lines;
     int memloc ;
     struct BucketListRec * next;
     TreeNode * treeNode;
   } * BucketList;

typedef struct ScopeListRec
   { char * funcName;
     struct ScopeListRec * parent;
     int nested;
     BucketList hashTable[1000];
   } * ScopeList;

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno, int loc,TreeNode* treeNode );

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name );

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
int addLocation();
void printSymTab(FILE * listing);
int st_lookup_top(char* name);
ScopeList sc_getsc();
void sc_insert(ScopeList scope);
#endif
