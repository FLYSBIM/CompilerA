/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char* tokenString )
{ 
  switch (token)
  { 
    case IF:
    case ELSE:
    case WHILE:
    case RETURN:
    case INT:
    case VOID:
      fprintf(listing,
         "reserved word: %s\n",tokenString);
      break;    
    case ASSIGN: fprintf(listing, "=\n"); break;
    case EQ: fprintf(listing, "==\n"); break;
    case NE: fprintf(listing, "!=\n"); break;
    case LT: fprintf(listing, "<\n"); break;
    case LE: fprintf(listing, "<=\n"); break;
    case GT: fprintf(listing, ">\n"); break;
    case GE: fprintf(listing, ">=\n"); break;
    case PLUS: fprintf(listing,"+\n"); break;
    case MINUS: fprintf(listing, "-\n"); break;
    case TIMES: fprintf(listing, "*\n"); break;
    case OVER: fprintf(listing, "/\n"); break;
    case LPAREN: fprintf(listing, "(\n"); break;
    case RPAREN: fprintf(listing, ")\n"); break;
    case LBRACE: fprintf(listing, "[\n"); break;
    case RBRACE: fprintf(listing, "]\n"); break;
    case LCURLY: fprintf(listing, "{\n"); break;
    case RCURLY: fprintf(listing, "}\n"); break;
    case SEMI: fprintf(listing, ";\n"); break;
    case COMMA: fprintf(listing, ",\n"); break;
    case ENDFILE: fprintf(listing,"EOF\n"); break;
    case NUM:
      fprintf(listing,
          "NUM, val= %s\n",tokenString);
      break;
    case ID:
      fprintf(listing,
          "ID, name= %s\n",tokenString);
      break;
    case ERROR:
      fprintf(listing,
          "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

TreeNode * newDeclNode(DeclKind kind) 
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n",lineno);
  else {
    for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = DeclK;
    t->kind.decl = kind;
    t->lineno = lineno;
  }
  return t;
}

TreeNode * newParaNode(ParaKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ParaK;
    t->kind.para = kind;
    t->lineno = lineno;
  }
  return t;
}

TreeNode * newTypeNode(TypeKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = TypK;
    t->kind.type = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree )
{ int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK)
    { switch (tree->kind.stmt) {
        case IfK: 
          fprintf(listing,"If\n");
          break;
        case IterK: //While
          fprintf(listing,"While Statement:\n");
          break;
        case RetuK: //Return
          fprintf(listing,"Return Statement:\n");
          break;
	case CompK: //Compound
	  fprintf(listing,"Compound Statement:\n");
	  break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK)
    { switch (tree->kind.exp) {
        case OpK: //Operator
          fprintf(listing,"Op: ");
          printToken(tree->attr.op,"\0");
          break;
        case ConstK: //Constant
          fprintf(listing,"Const: %d\n",tree->attr.val);
          break;
        case IdK: //Identifier
          fprintf(listing,"Variable: name = %s\n",tree->attr.name);
          break;
	case CallK: //Function Call
	  fprintf(listing,"Call Function name = %s\n",tree->attr.name);
	  break;
	case AssignK: //Assign
	  fprintf(listing,"Assign:\n");
	  break;
	case ArrIdK: //
	  fprintf(listing,"Variable: name = %s\n",tree->attr.name);
	  break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==DeclK)
    { switch (tree->kind.decl) {
        case FunK: //v
	  fprintf(listing,"Function Declaration: name = %s, return type = ",tree->attr.name);
	  if(tree->type == Void){
	    fprintf(listing,"void\n");
	  }
	  else if(tree->type == Integer){
	    fprintf(listing,"int\n");
	  }
	  break;
	case VarK: //v
	  fprintf(listing,"Variable Declaration: name = %s type = ",tree->attr.name);
	  fprintf(listing,"int\n");
	  break;
	case ArrVarK: //v
	  fprintf(listing,"Variable Declaration: name = %s type = int[]\n",tree->attr.name);
	  fprintf(listing,"int[]\n");
	  break;
	default:
	  fprintf(listing,"Unknown DeclNode kind\n");
	  break;
      }
    }
    else if (tree->nodekind==ParaK)
    { switch (tree->kind.type) {
	case ArrParaK:
	  fprintf(listing,"Parameter: name = %s, type = int[]",tree->attr.name);
	  break;
	case NonArrParaK:
	  if(tree->type==Integer){
	  	fprintf(listing,"parameter: name = %s type = int",tree->attr.name);
	  }
	  else if(tree->type==Void){
		fprintf(listing,"Void Parameter\n");
	  }
	  break;
	default:
          fprintf(listing,"Unknown ParaNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==TypK)
    { switch (tree->kind.type) {
	case TypeK:
	  if(tree->attr.type == VOID){
	      fprintf(listing,"type = void\n");
	  }
	  else if(tree->attr.type == INT){
	      fprintf(listing,"type = int\n");
	  }
	  break;
	default:
          fprintf(listing,"Unknown TypeNode kind\n");
          break;
      }
    }
///
    else fprintf(listing,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++)
         printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
