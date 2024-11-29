#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "util.h"

static int location = 0;

static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

static void insertNode( TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      { case CompK:
	  ScopeList scope=sc_create("aaa");
	  sc_insert(scope);
	  location++;
	  t->attr.scope=sc_getsc();
          break;
        default:
          break;
      }
      break;
    case ExprK:
      switch (t->kind.exp)
      { case IdidK:
	case ArriK:
	case CallK:
          if (st_lookup(t->attr.name) == -1)
	    fprintf(listing,"error");
            //st_insert(t->attr.name,t->lineno,location++);
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            st_insert(t->attr.name,t->lineno,0,t);
          break;
        default:
          break;
      }
      break;
    case DeclK:
      switch (t->kind.decl)
      {  
	 case FuncK:
	   { char* funcName=t->attr.name;
	     if(st_lookup_top(t->attr.name)!=-1){
		     printf("error\n");
		     break;
	     }

	     if(sc_getsc()!=NULL)
	     { printf("e1rror\n");
	       break;
	     }
	     st_insert(funcName,t->lineno,addLocation(),t);
	     sc_insert(sc_create(funcName));
	   }
	 break;	   
	 case VariK:
	 case ArrvK:
	   {
	     char* name;
	     if(t->kind.decl == VariK)
	     {  name=t->attr.name;
		t->type=Integer;
	     }
	     else
	     {
		name=t->attr.arr.name;
	        t->type=IntegerArray;
	     }

	     if(st_lookup_top(name)<0){
		     st_insert(name,t->lineno,addLocation(),t);
	     }
	     else{
		     printf("error\n");
	     }
	   }
      }
      break;
    default:
      break;
  }
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{ traverse(syntaxTree,insertNode,nullProc);
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { case ExprK:
      switch (t->kind.exp)
      { case OperK:
        case ConsK:
        case IdidK:
          break;
        default:
          break;
      }
      break;
    case StmtK:
      switch (t->kind.stmt)
      { case IfifK:
	case IfelK:
          if (t->child[0] == NULL)
	    fprintf(listing,"aan\n");
            //typeError(t->child[0],"if test is not Boolean");
          break;
        default:
          break;
      }
      break;
    default:
      break;

  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,nullProc,checkNode);
}
