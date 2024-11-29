#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "globals.h"

/* SIZE is the size of the hash table */
#define SIZE 1000

#define SHIFT 4

ScopeList scopeStack[1000];
ScopeList scopes[1000];
int currentScope=0;
int currentScopeStack=0;
int location[1000];

static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/*
ScopeList sc_create_gscope(){
	ScopeList gscope=(ScopeList)malloc(sizeof(struct ScopeListRec);
	gscope->funName="global";
	gscope->nested=0;
	gscope->parent=NULL;
	scopes[currentScope]=gscope;
	currentScope++;
	return gscope;
}*/

//create scope
ScopeList sc_create(char * func){ 
  	ScopeList Scope = (ScopeList)malloc(sizeof(struct ScopeListRec));
  	Scope->funcName=func;
  	Scope->nested=currentScopeStack;
  	Scope->parent=sc_getsc();
 	scopes[currentScope]=Scope;
	currentScope++;
  	return Scope;
}	

//after sc_create, sc_insert
void sc_insert(ScopeList scope){
	scopeStack[currentScopeStack]=scope;
	location[currentScopeStack]=0;
	currentScopeStack++;
}


//get scope(current)
ScopeList sc_getsc()
{
	if(currentScopeStack>=1){
		return scopeStack[currentScopeStack-1];
	}
	return NULL;
}

//exit currentscope Stack pop
void sc_exit()
{
	if(currentScopeStack>=1){
		currentScopeStack-=1;
	}
}

//find symbol: if symbol not exist, shift parent
BucketList symbol_find(char* name)
{
	int h=hash(name);
	ScopeList currentScope=sc_getsc();
	while(currentScope!=NULL)
	{
		BucketList Temp=currentScope->hashTable[h];
		while(Temp!=NULL){
			if(strcmp(name,Temp->name)==1){
				return Temp;
			}
			Temp=Temp->next;
		}
		currentScope=currentScope->parent;
	}
	return NULL;
}

int addLocation(){
        return location[currentScopeStack-1]++;
}
/*
int st_addline(char *name,int lineno)
{
	BucketList TempBucket=get_bucket(name);
	LineList TempLine=TempBucket->lines;
	while(TempLine->next!=NULL){
		TempLine=TempLine->next;
	}
	TempLine->next=(LineList)malloc(sizeof(struct LineListRec));
	TempLine->next->lineno=lineno;
	TempLine->next->next=NULL;
}*/

//insert symbol or insert lineno 
void st_insert( char * name, int lineno, int loc, TreeNode* treeNode )
{ int h = hash(name);
  ScopeList currentScope=sc_getsc();
  BucketList l = currentScope->hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->treeNode=treeNode;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = currentScope->hashTable[h];
    currentScope->hashTable[h] = l; 
  }
   else
  { LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} 

// find symbol
int st_lookup ( char * name )
{ /*int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) return -1;
  else return l->memloc;*/
  BucketList l=symbol_find(name);
  if(l!=NULL){
	  return l->memloc;
  }
  return -1;
}

int st_lookup_top(char* name)
{
	int h=hash(name);
	ScopeList currentScope=sc_getsc();

	BucketList bl=currentScope->hashTable[h];
	while(bl!=NULL){
		if(strcmp(name,bl->name)!=0){
			return bl->memloc;
		}
		bl=bl->next;
	}
	return -1;
}

void printSymTab(FILE * listing)
{
  fprintf(listing,"< Symbol Table >\n");
  fprintf(listing,"Variable Name Variable Type Scope Name Location Line Numbers\n");
  fprintf(listing,"------------- ------------- ---------- -------- ------------\n");
  for(int i=0;i<currentScope;i++){
	  ScopeList nowScope=scopes[i];
	  BucketList* hashTable=nowScope->hashTable;
	  for (int j = 0; j < SIZE; j++)
    { if(hashTable[j] != NULL)
      { BucketList bl = hashTable[j];
        TreeNode * node = bl->treeNode;

        while(bl != NULL)
        { LineList ll = bl->lines;
          fprintf(listing,"%-15s",bl->name);

          switch (node->nodekind)
          { case DeclK:
              switch (node->kind.decl)
              { case FuncK:
                  fprintf(listing,"%-15s","Function");
                  break;
                case VariK:
                  switch (node->type)
                  { case Void:
                      fprintf(listing,"%-15s","Void");
                      break;
                    case Integer:
                      fprintf(listing,"%-15s","Integer");
                      break;
                    default:
                      break;
                  }
                  break;
                case ArrvK:
                  fprintf(listing,"%-15s","IntegerArray");
                  break;
                default:
                  break;
              }
              break;
            case ParaK:
              switch (node->kind.param)
              { case ArrpK:
                  fprintf(listing,"%-15s","IntegerArray");
                  break;
                case NonaK:
                  fprintf(listing,"%-15s","Integer");
                  break;
                default:
                  break;
              }
              break;
            default:
              break;
          }

          fprintf(listing,"%-12s",nowScope->funcName);
          fprintf(listing,"%-10d",bl->memloc);
          while(ll != NULL)
          { fprintf(listing,"%4d",ll->lineno);
            ll = ll->next;
          }
          fprintf(listing,"\n");

          bl = bl->next;
        }
      }
    }
  }
}
