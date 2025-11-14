#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

#define curType (*cur)->type
#define nextType (*cur++)->type



Token* createToken(TokenType type){
	Token* t = malloc(sizeof(Token));
	t->type = type;
	return t;
}
ast_node* createNode(int type){
	ast_node* newNode = malloc(sizeof(ast_node));
	newNode->type = type;
	return newNode;
}

void addChild(ast_node* parent, ast_node* child){
	parent->children = realloc(parent->children, parent->numChildren + 1);
	parent->children[parent->numChildren] = child;
	parent->numChildren++;
}

void syntaxError(){
	printf("Syntax error\n");
	exit(1);
}

/*
	Finite Automata? to parse token list and build AST
*/
Token** cur; // Pointer to token list, change type later to token pag nagana na lexer!!!



ast_node* var_dec(){

}

ast_node* expr(){

}

ast_node* var_val(){
	ast_node* n;
	cur++;
	printf(" IN VAR_VAL, cur = %d\n", curType);
	switch(curType){
		case TOK_IDENT:
			printf("IDENT FOUND\n");
			n = createNode(IDENT);
			n->value = *((*cur)->lexeme);
			break;
		case TOK_INTEGER:
			break;
		case TOK_FLOAT:
			break;
		case TOK_STRING:
			break;
		case TOK_BOOLEAN:
			break;
		case TOK_TYPE:
			break;
		default:
			printf("Reached default of var_val\n"); // !!!
			// expr();
	}
	return n;
}

ast_node* single_stmt(){
	ast_node *n, *s;
	n = createNode(SINGLE_STMT);
	// cur++;
	printf("IN SINGLE_STMT, cur = %d\n", curType);

	switch(curType){
		case TOK_VISIBLE:	/* VISIBLE <var_val>*/
			printf("VISIBLE FOUND\n");
			addChild(n, createNode(VISIBLE));
			addChild(n, var_val());
			// if(nextType == TOK_IDENT){
			// 	addChild(n, var_val());
			// } else {
			// 	printf("typeof next: %d %d\n", nextType, TOK_IDENT);
			// 	syntaxError();
			// }
		case TOK_GIMMEH:
			addChild(n, createNode(GIMMEH));
			cur++;
			if(curType == IDENT) addChild(n, createNode(IDENT));
	}
	if((*cur++)->type != TOK_KTHXBYE){	// end of program not encountered, check for <stmt> ::= <single_stmt> <stmt>
		s = stmt();
		if(s != NULL){
			addChild(n, s);
		}
	}
	return n;
}

ast_node* compound_stmt(){
	ast_node* n;

	return NULL;
}

ast_node* stmt(){
	ast_node* n, *single, *compound;
	n = createNode(STMT);
	printf("IN STMT\n");
		single = single_stmt();
		compound = compound_stmt();
		if(single != NULL) addChild(n, single);
		if(compound != NULL) addChild(n, single);
	return n;

}

ast_node* program(Token** tokenList, int numTokens){
	ast_node* n;
	if((*cur)->type == TOK_HAI){
		n = createNode(PROG);
		cur++;
		if((*cur)->type == TOK_WAZZUP){
			addChild(n, createNode(WAZZUP));
			cur++;
			if((*cur)->type == TOK_I_HAS_A){	// Found var dec statement
				addChild(n, var_dec());
			} else if((*cur)->type == TOK_BUHBYE){		// No vars declared
				addChild(n, createNode(BUHBYE));
				if((*cur++)->type != TOK_KTHXBYE){		// Program end keyword KTHXBYE not encountered, check if statement
					printf("FOUND STATEMENT\n");
					addChild(n, stmt());
					addChild(n, createNode(KTHXBYE));
				} else {
					addChild(n, createNode(KTHXBYE));
					cur++;
				}
			} else {
				syntaxError();
			}
		}
	} else {
		syntaxError();
	}
	if(cur < &tokenList[numTokens-1]){ // Encountered KTHXBYE early (not at end of list)
		syntaxError();
	}
	return n;
}

void visit(ast_node* node){
	printf("%d\n", node->type);
	for(int i = 0; i < node->numChildren; i++){
		visit(node->children[i]);
	}
}

void listChildren(ast_node* node){
	for(int i = 0; i < node->numChildren; i++){
		printf("Type of child node: %d\n", node->children[i]->type);
	}
}


int main(){
	int numTokens = 8;
	Token** tokenList = malloc(sizeof(Token*) * numTokens);
	tokenList[0] = createToken(TOK_HAI);
	tokenList[1] = createToken(TOK_WAZZUP);
	tokenList[2] = createToken(TOK_BUHBYE);
	tokenList[3] = createToken(TOK_VISIBLE);
	tokenList[4] = createToken(TOK_IDENT);
	tokenList[4]->lexeme = "A";
	tokenList[5] = createToken(TOK_VISIBLE);
	tokenList[6] = createToken(TOK_IDENT);
	tokenList[6]->lexeme = "B";
	tokenList[7] = createToken(TOK_KTHXBYE);
	
	cur = tokenList;

	// for(int i = 0; i < numTokens; i++){
	// 	printf("Type of node: %d\n", (*cur)->type);
	// 	cur++;
	// }
	ast_node* root = program(tokenList, numTokens);

	visit(root);
	printf("Root's Children:\n");
	listChildren(root);
	listChildren(root->children[2]);
}