#include "globals.h"
#include "util.h"

void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { case ELSE:
    case IF:
    case INT:
    case RETURN:
    case VOID:
    case WHILE:
      fprintf(listing,
         "reserved word: %s\n",tokenString);
      break;
    case PLUS: fprintf(listing,"+\n"); break;
    case MINUS: fprintf(listing,"-\n"); break;
    case TIMES: fprintf(listing,"*\n"); break;
    case OVER: fprintf(listing,"/\n"); break;
    case LT: fprintf(listing,"<\n"); break;
    case LE: fprintf(listing,"<=\n"); break;
    case GT: fprintf(listing,">\n"); break;
    case GE: fprintf(listing,">=\n"); break;
    case EQ: fprintf(listing,"==\n"); break;
    case NE: fprintf(listing,"!=\n"); break;
    case ASSIGN: fprintf(listing,"=\n"); break;
    case SEMI: fprintf(listing,";\n"); break;
    case COMMA: fprintf(listing,",\n"); break;
    case LPAREN: fprintf(listing,"(\n"); break;
    case RPAREN: fprintf(listing,")\n"); break;
    case LBRACE: fprintf(listing,"[\n"); break;
    case RBRACE: fprintf(listing,"]\n"); break;
    case LCURLY: fprintf(listing,"{\n"); break;
    case RCURLY: fprintf(listing,"}\n"); break;
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
    default: 
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

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

TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExprK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

TreeNode * newDeclNode(DeclKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = DeclK;
    t->kind.decl = kind;
    t->lineno = lineno;
  }
  return t;
}

TreeNode * newParamNode(ParamKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ParaK;
    t->kind.param = kind;
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
    t->nodekind = TypeK;
    t->kind.type = kind;
    t->lineno = lineno;
  }
  return t;
}

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

static int indentno = 0;

#define INDENT indentno+=2
#define UNINDENT indentno-=2

static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

void printTree( TreeNode * tree )
{ int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK)
    { switch (tree->kind.stmt) {
        case CompK: 
          fprintf(listing,"Compound Statment:\n");
          break;
        case IfifK:
          fprintf(listing,"If Statement:\n");
          break;
	case IfelK:
	  fprintf(listing,"If-Else Statement:\n");
	  break;
        case IterK: 
          fprintf(listing,"While Statement:\n");
          break;
        case RetuK:
          fprintf(listing,"Return Statement:\n");
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExprK)
    { switch (tree->kind.exp) {
        case AssiK:
          fprintf(listing,"Assign:\n");
          break;
        case OperK:
          fprintf(listing,"Op: ");
          printToken(tree->attr.op,"\0");
          break;
        case ConsK:
          fprintf(listing,"Const: %d\n",tree->attr.val);
          break;
        case IdidK:
          fprintf(listing,"Variable: name = %s\n",tree->attr.name);
          break;
        case ArriK:
          fprintf(listing,"Variable: name = %s\n",tree->attr.name);
          break;
        case CallK:
          fprintf(listing,"Call: function name = %s\n", tree->attr.name);
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==DeclK)
    { switch (tree->kind.decl) {
        case FuncK:
          fprintf(listing,"Function Declaration: name = %s, return type = ",tree->attr.name);
	  if(tree->type==Integer){
		  printf("int\n");
	  }
	  else if(tree->type==Void){
		  printf("void\n");
  	  }
          break;
        case VariK:
          fprintf(listing,"Variable Declaration: name = %s, type = ",tree->attr.name);
	  if(tree->type==Integer){
		  printf("int\n");
	  }
	  else if(tree->type==Void){
		  printf("void\n");
	  }
          break;
        case ArrvK:
          fprintf(listing,"Variable Declaration: name = %s, type = int[]\n",tree->attr.arr.name);
          break;
	case NumbK:
	  fprintf(listing,"Const: %d\n",tree->attr.val);
	  break;
        default:
          fprintf(listing,"Unknown DeclNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ParaK)
    { switch (tree->kind.param) {
        case ArrpK:
          fprintf(listing,"Array Parameter: %s\n",tree->attr.name);
          break;
        case NonaK:
          fprintf(listing,"Parameter: name = %s, type = ",tree->attr.name);
	  if(tree->type==Integer)
	    fprintf(listing,"int\n");
	  else if(tree->type==Void)
	    fprintf(listing,"Void Parameter\n");
          break;
        default:
          fprintf(listing,"Unknown ParamNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==TypeK)
    { switch (tree->kind.type) {
        case TynaK:
          if(tree->attr.type==INT)
            fprintf(listing,"type: int\n");
          else if(tree->attr.type==VOID)
            fprintf(listing,"Void Parameter\n");
          break;
        default:
          fprintf(listing,"Unknown TypeNode kind\n");
          break;
      }
    }
    else fprintf(listing,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++) {
         printTree(tree->child[i]);
    }
    tree = tree->sibling;
  }
  UNINDENT;
}
