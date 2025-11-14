#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

/*
	Notes/Plans/WIP: 
		1) pagsamahin na parts ng syntax and semantic analysis
			ex: encountered valid var_assign -> add to a symbol table
		2) no verbose error message with line + parser terminates when encountering syntax error
		3) magulo kung san nagiincrement ng cur in some places; switches between before calling function ng symbols or within (necessary for now)
*/

#define curType (*cur)->type
#define nextType (*cur++)->type
Token** cur; // Pointer to token list, change type later to token pag nagana na lexer!!!

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

void syntaxError(char *msg){
	printf("Syntax error: %s\n", msg);
	exit(1);
}

/*
	Finite Automata? to parse token list and build AST
*/


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
			n = createNode(IDENT);
			n->id_name = (*cur)->lexeme;
			break;
		case TOK_INTEGER:
			n = createNode(INTEGER);
			n->num_val = atoi((*cur)->lexeme);
			break;
		case TOK_FLOAT:
			n = createNode(FLOAT);
			n->num_val = atof((*cur)->lexeme);
			break;
		case TOK_STRING:
			n = createNode(STRING);
			n->string_val = (*cur)->lexeme;
			break;
		case TOK_BOOLEAN:
			n = createNode(BOOLEAN);
			n->string_val = (*cur)->lexeme;
			break;
		case TOK_TYPE:
			n = createNode(TYPE);
			n->string_val = (*cur)->lexeme;
			break;
		default:
			printf("Reached default of var_val\n"); // !!!
			// try expr();
			// not expr
			// return null?
			return NULL;
	}
	return n;
}

ast_node* print(){
	ast_node *n;
	n = createNode(PRINT);
	addChild(n, createNode(VISIBLE));
	addChild(n, var_val());
	return n;
}

ast_node* input(){
	ast_node *n, *s;
	n = createNode(INPUT);
	addChild(n, createNode(GIMMEH));
	cur++;
	if(curType == IDENT) {
		s = createNode(IDENT);
		s->id_name = (*cur)->lexeme;
		addChild(n, s);
	} else {
		syntaxError("Expected Identifier in GIMMEH");
	}

	return n;
}

ast_node* assignment(){
	ast_node *n, *s;
	n = createNode(IDENT);
	cur++;
	if(curType == TOK_R){
		addChild(n, createNode(R));
	} else {
		syntaxError("Expected R");
	}
	s = var_val();
	if(s != NULL){
		addChild(n, s);
	} else {
		syntaxError("Expected type literal following R");
	}
	return n;
}

ast_node* function_call(){
	ast_node* n = createNode(I_IZ), *s;
	cur++;
	if(curType == TOK_IDENT){
		s = createNode(IDENT);
		s->id_name = (*cur)->lexeme;
		addChild(n, s);
		cur++;
		if(curType == TOK_MKAY){ // no args
			addChild(n, createNode(MKAY));
		} else if(curType == TOK_YR){ // has args
			addChild(n, createNode(YR));
			while(curType != TOK_MKAY){
				s = var_val();
				if(s != NULL){
					addChild(n, s);
				} else {
					// error
				}
				cur++;
				if(curType == TOK_AN){
					addChild(n, createNode(AN));
				} else {
					// error
				}
			}
			if(curType == TOK_MKAY){
				addChild(n, createNode(MKAY));
			} else {
				// error
			}

		} else {
			// error
		}
	} else {
		syntaxError("Expected identifier after I IZ");
	}
}

ast_node* single_stmt(){ 
	/*<single_stmt> ::= <print> | <input> | <expr> | <assignment> | <function_call> | GTFO */
	ast_node *n, *s;
	n = createNode(SINGLE_STMT);
	// cur++;
	printf("IN SINGLE_STMT, cur = %d\n", curType);

	switch(curType){
		case TOK_VISIBLE:	/* VISIBLE <var_val>*/
			printf("VISIBLE FOUND\n");
			addChild(n, print());
			break;
		case TOK_GIMMEH:	/* GIMMEH varident*/
			addChild(n, input());
			break;
		case TOK_IDENT: /* varident R <var_val>*/
			addChild(n, createNode(IDENT));
			break;
		case TOK_I_IZ: /* <function_call> ::= I IZ funident MKAY | I IZ funident <argument> MKAY*/
			addChild(n, createNode(I_IZ));
			break;
		default:
			printf("Reached end of single_stmt\n");
			


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
				syntaxError("Expected variable assignment or BUHBYE");
			}
		}
	} else {
		syntaxError("Expected HAI");
	}
	if(cur < &tokenList[numTokens-1]){ // Encountered KTHXBYE early (other stuff found after KTHXBYE)
		syntaxError("Statements past KTHXBYE");
	}
	return n;
}

// preorder traversal
void visit(ast_node* node){
	printf("%s ", string_ver[node->type]);
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

	printf("Post-order traversal of AST\n");
	visit(root);
	printf("\n");
	// printf("Root's Children:\n");
// 	listChildren(root);
// 	listChildren(root->children[2]);
}