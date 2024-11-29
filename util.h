#ifndef _UTIL_H_
#define _UTIL_H_

void printToken( TokenType, const char* );

TreeNode * newStmtNode(StmtKind);

TreeNode * newExpNode(ExpKind);

TreeNode * newDeclNode(DeclKind);

TreeNode * newParamNode(ParamKind);

TreeNode * newTypeNode(TypeKind);

char * copyString( char * );

void printTree( TreeNode * );

#endif
