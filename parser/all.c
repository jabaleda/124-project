#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "all.h"


/*
	Parser Notes/Plans/WIP: (coding ts without testing...)
		IMPORTANT: curType is compared against TOKENTYPES (prefixed with TOK_) and not SYMBOLS
				so sa mga pagcheck ng next token, make sure may TOK_
				opposite naman when creating node using createNode(Symbol s), 
				takes in SYMBOL (same as token names but without TOK_ prefix)
		ALSO IMPORTANT: sorry sabog 'to HAHAHAHHAAHHA, DI KO PINLANO NG MAIGI SINIMULAN KO LANG
						aayusin along the way
		
		1) pagsamahin na parts ng syntax and semantic analysis
			ex: encountered valid var_assign -> add to a symbol table
			pros: catch agad semantic errors, no need to do separate tree traversal, pdeng diretso run
			cons: hirap debug; sayang pag may error
		2) no verbose error message with line + parser terminates when encountering syntax error
			pros: 
			cons: premature exit that prevents flagging of other syntax errors further in
		3) magulo kung san nagiincrement ng cur in some places; switches between before calling function ng symbols or within (necessary for now)
		4) consider using variable length argument for adding child node to avoid 
			multiple addChild(node, child) statements (see expr() switch cases)
		5) does not check kung may linebreak after statements
			after a statement:
			- checks if there are more statements
			- checks for KTHXBYE (assumes no more statements then error pag may token pa past KTHXBYE)
		6) no infinite boolean yet
		8) something sa terminals
			ex: for a <var_val> that evaluates to INTEGER node should it have a child with no type and only a value
				example value = 5: para maproduce sa tree na <var_val> -> INTEGER -> 5
		9) on if-else: first of production rule looks for <expr> but checking
			for if-else atm does not look for expr and only looks for O RLY?
			possible workaround, if O RLY? is found, check if IT variable has value?? but
				this means O RLY? would work even if the expression it uses as a condition
				does not come directly before it and uses the result of some past
				expr
		

*/

/*
	Note on grammar: some production rules seem unnecessary
		ex: <break_choice> expanding just to TIL or WILE
		and may be implemented without the abstraction
	change to var_dec
	var_dec ::= I HAS A varident | I HAS A varident ITZ <var_value>
				I HAS A varident <var_dec> | I HAS A varident ITZ <var_value> <var_dec>
*/

#define curType (*cur)->type
#define nextType (*(cur+1))->type
const int EXPR_KEYWORDS[] = {TOK_SUM_OF, TOK_DIFF_OF, TOK_PRODUKT_OF, TOK_QUOSHUNT_OF, TOK_MOD_OF, TOK_NOT, TOK_BOTH_OF, TOK_EITHER_OF, TOK_WON_OF, TOK_ALL_OF, TOK_ANY_OF, TOK_BOTH_SAEM, TOK_DIFFRINT, TOK_BIGGR_OF, TOK_SMALLR_OF, TOK_SMOOSH};
int lastTokIdx;
/* Pointer to elements in token array */
Token** cur;

/* Total number of nodes in AST*/
int numNodes = 0;

ast_node* createNode(int type){
	ast_node* newNode = malloc(sizeof(ast_node));
	newNode->node_id = numNodes;
	newNode->type = type;
	newNode->children = NULL;
	newNode->numChildren = 0;
	numNodes++;
	return newNode;
}

/* For tracing adds */
void addTrace(ast_node* parent, ast_node* child){
	// if((*cur)->tok_id < lastTokIdx-1){
	// 	// printf("Added %s as child of %s. Current token: %s	Next token (Post-increment): %s\n", string_ver[child->type], string_ver[parent->type], string_ver[curType], string_ver[nextType]);
	// 	printf("Added %s as child of %s. Current token: %s\n", string_ver[child->type], string_ver[parent->type], string_ver[curType]);
	// } else {
	// 	printf("Added %s as child of %s.\n", string_ver[child->type], string_ver[parent->type]);
	// }
}

/* Add a child node and increment token array pointer*/
void addChild(ast_node* parent, ast_node* child){
	ast_node** temp = NULL;
	temp = realloc(parent->children, sizeof(ast_node*) * (parent->numChildren + 1));
	if(temp != NULL){
		parent->children = temp;
	} else {
		printf("Add child node realloc error.\n");
	}
	parent->children[parent->numChildren] = child;
	parent->numChildren++;
	addTrace(parent, child);
	cur++;
}

/* 
	Add a child node without incrementing. Used when adding a child node that
	does not directly consume a token (i.e. the second parameter is a function
	call to a non-terminal symbol). 
*/
void addChildNoIncrement(ast_node* parent, ast_node* child){
	ast_node** temp = NULL;
	temp = realloc(parent->children, sizeof(ast_node*) * (parent->numChildren + 1));
	if(temp != NULL){
		parent->children = temp;
	} else {
		printf("Add child node realloc error.\n");
	}
	parent->children[parent->numChildren] = child;
	parent->numChildren++;
	addTrace(parent, child);
}

void syntaxError(char *msg){
	printf("Syntax error: %s	Found: %s (line: %d)\n", msg, (*cur)->lexeme, (*cur)->line);
	exit(1);
}

/* Use for debugging only, traces parse sequence to identify current function, token, lexeme, and line*/
void trace(char* msg, char* node){
	printf("Trace: %s	In: %s\n		Current Token: %s	Lexeme: %s	Line: %d\n\n", msg, node, string_ver[curType], (*cur)->lexeme,(*cur)->line);
}

/*
	Finite Automata? to parse token list and build AST
*/
ast_node* program(TokenList* tokenList, int numTokens){
	ast_node* n;
	if(curType == TOK_HAI){
		n = createNode(PROG);
		// trace("Start", string_ver[n->type]);
		addChild(n, createNode(HAI));
		if(curType == TOK_WAZZUP){
			addChild(n, createNode(WAZZUP));
			if(curType == TOK_I_HAS_A){					// Found var dec statement
				// trace("Found var_dec", string_ver[n->type]);
				addChildNoIncrement(n, var_dec());
				// trace("End of var_decs", string_ver[n->type]);
					if(curType == TOK_BUHBYE){
						addChild(n, createNode(BUHBYE));
						// next token in list is not KTHXBYE, expect statement
						if(curType == TOK_KTHXBYE){			
							addChildNoIncrement(n, createNode(KTHXBYE));
						} else {
							// trace("Found statement, after in var_dec", string_ver[n->type]);
							addChildNoIncrement(n, stmt());
							addChildNoIncrement(n, createNode(KTHXBYE));
						}
					} else {
						syntaxError("Unexpected token");
					}
			} else if(curType == TOK_BUHBYE){		// No vars declared
					// trace("No var dec found", string_ver[n->type]);
					addChild(n, createNode(BUHBYE));
				if(curType == TOK_KTHXBYE){			// End of program keyword immediately encountered
													// Expect no further statements (all tokens consumed)
					addChildNoIncrement(n, createNode(KTHXBYE));
				} else {
					// trace("Found stmt after in no var_dec", string_ver[n->type]);
					addChildNoIncrement(n, stmt());
					addChildNoIncrement(n, createNode(KTHXBYE));
				}
			} else {
				syntaxError("Expected variable assignment or BUHBYE");
			}
		}
	} else {
		syntaxError("Expected HAI");
	}

	if((*cur)->tok_id < lastTokIdx){	// Not all tokens consumed after KTHXBYE keyword encountered
		syntaxError("Unexpected statement past end of program 'KTHXBYE'");
	}
	printf("Done parsing.\n");
	return n;
}

ast_node* var_dec(){
	ast_node* n = createNode(VAR_DEC);
	do{
		// trace("Found I HAS A", string_ver[n->type]);
		addChild(n, createNode(I_HAS_A));
		//  check for following identifier
		if(curType == TOK_IDENT){
			addChildNoIncrement(n, var_val());
			// trace("Returned from first ident in var_dec", string_ver[n->type]);
			// check for initialization
			if(curType == TOK_ITZ){		
				addChild(n, createNode(ITZ));
				addChildNoIncrement(n, var_val());
			}
		} else {
			syntaxError("Expected identifier after I HAS A");
		}
	} while(curType == TOK_I_HAS_A); // check for further variable declarations
	trace("Returning var_dec", string_ver[n->type]);
	return n;
}

ast_node* var_val(){
	ast_node *n = createNode(VAR_VAL), *s;
	// trace("", string_ver[n->type]);
	switch(curType){
		case TOK_IDENT:
			s = createNode(IDENT);
			s->id_name = (*cur)->lexeme;
			addChild(n, s);
			break;
		case TOK_INTEGER:
			s = createNode(INTEGER);
			s->num_val = atoi((*cur)->lexeme);
			addChild(n, s);
			break;
		case TOK_FLOAT:
			s = createNode(FLOAT);
			s->num_val = atof((*cur)->lexeme);
			addChild(n, s);
			break;
		case TOK_STRING:
			s = createNode(STRING);
			s->string_val = (*cur)->lexeme;
			addChild(n, s);
			break;
		case TOK_BOOLEAN:
			s = createNode(BOOLEAN);
			s->string_val = (*cur)->lexeme;
			addChild(n, s);
			break;
		case TOK_TYPE:
			s = createNode(TYPE);
			s->string_val = (*cur)->lexeme;
			addChild(n, s);
			break;
		default:
			// trace("Reached default", string_ver[n->type]); // !!!
			// check if current token is a keyword to any of the expression statements that return values
			// else throw error
			int isExprKeyword = 0;
			for(int i = 0; i < sizeof(EXPR_KEYWORDS)/sizeof(int); i++){
				if(curType == EXPR_KEYWORDS[i]){
					isExprKeyword = 1;
					break;
				}
			}
			if(isExprKeyword){
				// trace("expr keyword found", string_ver[n->type]);
				addChildNoIncrement(n, expr());
			} else {
				if(curType != TOK_MAEK) syntaxError("Expected identifier/literal");
			}
	}
	// trace("Returning", string_ver[n->type]);
	return n;
}

ast_node* stmt(){
	ast_node* n;
	n = createNode(STMT);
	trace("", string_ver[n->type]);

	if(/*curType != TOK_O_RLY || */ curType == TOK_WTF || curType == TOK_IM_IN_YR || curType == TOK_HOW_IZ_I){
		addChildNoIncrement(n, compound_stmt());
	} else {
		addChildNoIncrement(n, single_stmt());
	}

	trace("Returning stmt", string_ver[n->type]);
	return n;
}

/* single_stmt> ::= <print> | <input> | <expr> | <assignment> | <function_call> | GTFO */
ast_node* single_stmt(){ 
	ast_node *n, *s;
	n = createNode(SINGLE_STMT);
	// trace("Single stmt check", string_ver[n->type]);

	switch(curType){
		case TOK_VISIBLE:	/* VISIBLE <var_val>*/
			// printf("VISIBLE FOUND\n");
			// trace("In visible", string_ver[n->type]);
			addChildNoIncrement(n, print());
			break;
		case TOK_GIMMEH:	/* GIMMEH varident*/
			addChildNoIncrement(n, input());
			break;
		case TOK_I_IZ: /* <function_call> ::= I IZ funident MKAY | I IZ funident <argument> MKAY*/
			addChildNoIncrement(n, function_call());
			break;
		case TOK_GTFO:
			addChild(n, createNode(GTFO));
			break;
		case TOK_IDENT: /* varident R <var_val>*/
			if((*cur)->tok_id < lastTokIdx){
				if(nextType == TOK_R){
					if((*cur)->tok_id < lastTokIdx-1){
						if((*(cur+2))->type != TOK_MAEK){
							addChildNoIncrement(n, assignment());
							break;
						} else {
						}
					} 
				} 
			}
		default:
			addChildNoIncrement(n, expr());
	}

	// statements being generated within a compound statement body
	// must eventually encounter keywords indicating either the start
	// of another block (ex: OMG in switch) or the end of a block (ex: IM OUTTA YR in loop)
	if(	curType == TOK_OMG 
		|| curType == TOK_OMGWTF
		|| curType == TOK_OIC
		|| curType == TOK_IM_OUTTA_YR
		|| curType == TOK_IF_U_SAY_SO
	){
		// trace("Start/End keyword of compound stmt block found", string_ver[n->type]);
		return n;
	}

	if(curType != TOK_KTHXBYE){	// end of program not encountered, check for <stmt> ::= <single_stmt> <stmt>
		// trace("KTHXBYE not found, expecting more statements", string_ver[n->type]);
		addChildNoIncrement(n, stmt());
	}
	// trace("Returning single statement", string_ver[n->type]);
	return n;
}

ast_node* print(){
	ast_node *n;
	n = createNode(PRINT);
	addChild(n, createNode(PRINT));
	addChildNoIncrement(n, var_val());
	return n;
}

ast_node* input(){
	ast_node *n;
	n = createNode(INPUT);
	addChild(n, createNode(GIMMEH));
	if(curType == IDENT) {
		addChildNoIncrement(n, var_val());
	} else {
		syntaxError("Expected Identifier in GIMMEH");
	}
	return n;
}

ast_node* assignment(){
	ast_node *n;
	n = createNode(ASSIGNMENT);
	// trace("", string_ver[n->type]);
	addChildNoIncrement(n, var_val());
	if(curType == TOK_R){
		addChild(n, createNode(R));
		addChildNoIncrement(n, var_val());
	} else {
		syntaxError("Expected R");
	}
	return n;
}

ast_node* function_call(){
	ast_node* n = createNode(FUNCTION_CALL), *s;
	addChild(n, createNode(I_IZ));
	if(curType == TOK_IDENT){
		addChildNoIncrement(n, var_val());
		if(curType == TOK_MKAY){		// no args
			addChild(n, createNode(MKAY));
		} else if(curType == TOK_YR){	// has args
			addChild(n, createNode(YR));
			// expect MKAY to end function call statement
			// else assumes arguments are being passed
			if(curType != TOK_MKAY){
				addChildNoIncrement(n, var_val());
				do{
					addChild(n, createNode(AN));
					addChildNoIncrement(n, var_val());
					// trace("In function_call arg loop", string_ver[n->type]);
				}while(curType == TOK_AN);
			}
			// exited while loop, expect MKAY
			if(curType == TOK_MKAY){
				addChild(n, createNode(MKAY));
			} else {
				syntaxError("Expected end of function_call MKAY");
			}
		} else {
			syntaxError("Expected MKAY or arg declarations YR");
		}
	} else {
		syntaxError("Expected identifier after I IZ");
	}
	return n;
}

ast_node* expr(){
	ast_node *n, *s;
	n = createNode(EXPR);
	trace("Check expr", string_ver[n->type]);
	int type = curType;
	if((*cur)->tok_id < lastTokIdx){
		printf("Next token: %s\n", string_ver[nextType]);
	}

	if(curType == TOK_SMOOSH){
		addChildNoIncrement(n, concatenation());
	}

	if(  curType == TOK_SUM_OF 
			|| curType == TOK_DIFF_OF 
			|| curType == TOK_PRODUKT_OF 
			|| curType == TOK_QUOSHUNT_OF 
			|| curType == TOK_MOD_OF)
	{
		addChildNoIncrement(n, arithmetic());
	}

	if(curType == TOK_NOT 
			|| curType == TOK_BOTH_OF 
			|| curType == TOK_EITHER_OF
			|| curType == TOK_WON_OF 
			|| curType == TOK_ALL_OF 
			|| curType == TOK_ANY_OF)
	{
		addChildNoIncrement(n, boolean());
	}

	if(curType == TOK_BOTH_SAEM || curType == TOK_DIFFRINT){
		addChildNoIncrement(n, comparison());
	}
	if(curType == TOK_MAEK) {
		addChildNoIncrement(n, typecasting());
	}
		// extension of <typecasting>, does lookahead to distinguish from assignment which also has a rule starting with varident
		// does extra checks to prevent trying to lookahead past last element of token list
	if((*cur)->tok_id < lastTokIdx){
		if(curType == TOK_IDENT && nextType == TOK_IS_NOW_A) {
			addChildNoIncrement(n, typecasting());
		}
	}
	if((*cur)->tok_id < lastTokIdx-1){
		if(curType == TOK_IDENT && nextType == TOK_R && (*(cur+2))->type == TOK_MAEK){
			addChildNoIncrement(n, typecasting());
		}
	}
	if(curType == TOK_BIGGR_OF || curType == TOK_SMALLR_OF){
		addChildNoIncrement(n, relational());
	} else {
		if(curType == TOK_IDENT) syntaxError("Unexpected identifier/literal");
	}

	trace("Returning expression", string_ver[n->type]);
	return n;
}

/* Rules: <arithmetic> ::= (SUM OF | DIFF OF | PRODUKT OF | QUOSHUNT OF | MOD OF) <var_value> AN <var_value> */
ast_node* arithmetic(){
	ast_node* n;
	n = createNode(ARITHMETIC);
	switch(curType){
		case TOK_SUM_OF:
			addChild(n, createNode(SUM_OF));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, var_val());	
			} else {
				syntaxError("Expected arg separator AN in binary arithmetic");
			}
			break;
		case TOK_DIFF_OF:
			addChild(n, createNode(DIFF_OF));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, var_val());	
			} else {
				syntaxError("Expected arg separator AN in binary arithmetic");
			}
			break;
		case TOK_PRODUKT_OF:
			addChild(n, createNode(PRODUKT_OF));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, var_val());	
			} else {
				syntaxError("Expected arg separator AN in binary arithmetic");
			}
			break;
		case TOK_QUOSHUNT_OF:
			addChild(n, createNode(QUOSHUNT_OF));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, var_val());	
			} else {
				syntaxError("Expected arg separator AN in binary arithmetic");
			}
			break;
		case TOK_MOD_OF:
			addChild(n, createNode(MOD_OF));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, var_val());	
			} else {
				syntaxError("Expected arg separator AN in binary arithmetic");
			}
			break;
		default:
	}
	return n;
}

/* Rules: 
	<fin_boolean> ::= NOT <var_val> | (BOTH OF | EITHER OF | WON OF) <var_val> AN <var_val>

*/
ast_node* boolean(){
	ast_node* n;
	n = createNode(BOOLEAN);
	switch(curType){
		case TOK_NOT:
			addChild(n, createNode(NOT));
			addChildNoIncrement(n, var_val());
			break;
		case TOK_BOTH_OF:
			addChild(n, createNode(BOTH_OF));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, var_val());
			} else {
				syntaxError("Expected arg separator AN in binary boolean");
			}
			break;
		case TOK_EITHER_OF:
			addChild(n, createNode(EITHER_OF));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, var_val());
			} else {
				syntaxError("Expected arg separator AN in binary boolean");
			}
			break;
		case TOK_WON_OF:
			addChild(n, createNode(WON_OF));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, var_val());
			} else {
				syntaxError("Expected arg separator AN in binary boolean");
			}
			break;
	}
	return n;
}

/* Rules: <comparison> ::= (BOTH SAEM | BOTH DIFFRINT) <var_val> AN <var_val>*/
ast_node* comparison(){
	ast_node* n;
	n = createNode(COMPARISON);
	// trace("In comparison", string_ver[n->type]);
	switch(curType){
		case TOK_BOTH_SAEM:
			addChild(n, createNode(BOTH_SAEM));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, relational());
			} else {
				syntaxError("Expected arg separator AN in binary comparison");
			}
			break;
		case TOK_DIFFRINT:
			addChild(n, createNode(DIFFRINT));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, relational());
			} else {
				syntaxError("Expected arg separator AN in binary comparison");
			}
			break;
	}
	return n;
}

/* <relational> ::= <var_val> | (BIGGR OF | SMALLR OF) <valr_val> AN <var_val>*/
ast_node* relational(){
	ast_node* n;
	n = createNode(RELATIONAL);
	// trace("In ", string_ver[n->type]);
	switch(curType){
		case TOK_BIGGR_OF:
			addChild(n, createNode(BIGGR_OF));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, var_val());
			} else {
				syntaxError("Expected arg separator AN in binary relational");
			}
			break;
		case TOK_SMALLR_OF:
			addChild(n, createNode(SMALLR_OF));
			addChildNoIncrement(n, var_val());
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChildNoIncrement(n, var_val());
			} else {
				syntaxError("Expected arg separator AN in binary relational");
			}	
			break;
		default:
			addChildNoIncrement(n, var_val());
	}
	return n;
}


ast_node* concatenation(){
	ast_node* n = createNode(CONCATENATION);
	addChild(n, createNode(SMOOSH));
	addChildNoIncrement(n, var_val());
	addChildNoIncrement(n, concat_operand());
	// trace("Returning concatenation node", string_ver[n->type]);
	return n;
}

ast_node* concat_operand(){
	ast_node* n = createNode(CONCAT_OPERAND);
	// trace("In concat_operand", string_ver[n->type]);
	if(curType != TOK_AN){
		syntaxError("Expected arg separator AN in SMOOSH");
	}
	do{
		addChild(n, createNode(AN));
		addChildNoIncrement(n, var_val());
		// trace("In concat_operand loop", string_ver[n->type]);
	}while(curType == TOK_AN);
	// trace("Outside of concat_operand loop, returning concat_operand node", string_ver[n->type]);
	return n;
}

/* <typecasting> ::= MAEK varident A type | varident IS NOW A type | varident R MAEK varident type */
ast_node* typecasting(){
	ast_node* n = createNode(TYPECASTING), *s;
	// trace("",string_ver[n->type]);
	if(curType == TOK_MAEK){
		// trace("Entered MAEK format",string_ver[n->type]);
		addChild(n, createNode(MAEK));
		if(curType == TOK_IDENT){
			addChildNoIncrement(n, var_val());
			if(curType == TOK_A){
				addChild(n, createNode(A));
				if(curType == TOK_TYPE){
					addChildNoIncrement(n, var_val());
				} else {
					syntaxError("Expected type literal after A");
				}
			} else {
				syntaxError("Expected type specifier A in typecast statement");
			}
		} else {
			syntaxError("Expected identifier after MAEK");
		}
	} else if(curType == TOK_IDENT) { // must follow either of (varident IS NOW A type | varident R MAEK varident type)
		// trace("Entered IS NOW A/R MAEK format",string_ver[n->type]);
		addChildNoIncrement(n, var_val());
		if(curType == TOK_IS_NOW_A){
			addChild(n, createNode(IS_NOW_A));
			if(curType == TOK_TYPE){
				addChildNoIncrement(n, var_val());
			} else {
				syntaxError("Expected type literal after A");
			}
		} else { // must be following R MAEK format
			addChild(n, createNode(R));
			addChild(n, createNode(MAEK));
			if(curType == TOK_IDENT){
				addChildNoIncrement(n, var_val());
				if(curType == TOK_TYPE){
					addChildNoIncrement(n, var_val());
				} else {
					syntaxError("Expected type literal after identifier in typecast");
				}
			} else {
				syntaxError("Expected identifier after typecast R MAEK");
			}
		}
	}
	return n;
}

ast_node* compound_stmt(){
	ast_node* n;
	n = createNode(COMPOUND_STMT);

	switch(curType){
		case TOK_WTF:
			addChildNoIncrement(n, switch_case());
			break;
		case TOK_IM_IN_YR:
			addChildNoIncrement(n, loop());
			break;
		case TOK_HOW_IZ_I:
			addChildNoIncrement(n, function_definition());
			break;
		default:
	}

	if(curType != TOK_KTHXBYE){	// end of program not encountered, check for <stmt> ::= <single_stmt> <stmt>
		addChild(n, stmt());
	}
	// trace("Returning cmpd_statement", string_ver[n->type]);
	return n;
}

// switch

ast_node* switch_case(){
	ast_node* n = createNode(SWITCH_CASE);
	addChild(n, createNode(WTF));
	if(curType == TOK_OMG){
		addChildNoIncrement(n, case_block());
	}
	// trace("Exited tok_omg check",string_ver[n->type]);
	if(curType == TOK_OMGWTF){ // check for default block keyword
		// trace("Found omgwtf", string_ver[n->type]);
		addChildNoIncrement(n, default_block());
	}
	// trace("Returning switch_case", string_ver[n->type]);
	addChild(n, createNode(OIC));
	return n;
}

ast_node* case_block(){
	ast_node* n = createNode(CASE_BLOCK);
	do{
		// trace("", string_ver[n->type]);
		addChild(n, createNode(OMG));
		switch(curType){
			case TOK_INTEGER:
				addChildNoIncrement(n, var_val());
				// trace("In INTEGER", string_ver[n->type]);
				addChildNoIncrement(n, stmt());
				break;
			case TOK_FLOAT:
				addChildNoIncrement(n, var_val());
				addChildNoIncrement(n, stmt());
				break;
			case TOK_STRING:
				addChildNoIncrement(n, var_val());
				addChildNoIncrement(n, stmt());
				break;
			case TOK_BOOLEAN:
				addChildNoIncrement(n, var_val());
				addChildNoIncrement(n, stmt());
				break;
			case TOK_TYPE:
				addChildNoIncrement(n, var_val());
				addChildNoIncrement(n, stmt());
				break;
			default:
				syntaxError("Expected literal after OMG");
		}
		if((*cur)->tok_id < lastTokIdx){
			if(nextType == TOK_GTFO){  // if case_block has break, consume and create node
				addChild(n, createNode(GTFO));
			}
		}
	} while(curType == TOK_OMG); // check for further case
	// trace("Returning case_block",string_ver[n->type]);
	return n;
}

ast_node* default_block(){
	ast_node* n = createNode(DEFAULT_BLOCK);
	addChild(n, createNode(OMGWTF));
	addChildNoIncrement(n, stmt());
	return n;
}

ast_node* loop(){
	ast_node* n = createNode(LOOP), *s;
	addChild(n, createNode(IM_IN_YR));
	if(curType == TOK_IDENT){
		addChildNoIncrement(n, var_val());
		if(curType == TOK_UPPIN){
			addChild(n, createNode(UPPIN));		
		} else if(curType == TOK_NERFIN){
			addChild(n, createNode(NERFIN));
		} else {
			syntaxError("Expected UPPIN/NERFIN after loop identifier");
		}
		if(curType == TOK_YR){
			addChild(n, createNode(YR));
		} else {
			syntaxError("Expected YR in loop after UPPIN/NERFIN");
		}
		if(curType == TOK_IDENT){
			addChildNoIncrement(n, var_val());
		} else {
			syntaxError("Expected loop iterator identifier after UPPIN/NERFIN");
		}
		// check for break_condition (TIL/WILE keyword)
		// if either is found, check if break condition is boolean or comparison expr
		if(curType == TOK_TIL){ 
			addChild(n, createNode(TIL));
			if(curType != TOK_BOTH_SAEM && curType != TOK_DIFFRINT){  // if not comparison
				addChildNoIncrement(n, comparison());
			} else { // must be boolean
				addChildNoIncrement(n, boolean());
			}
			addChildNoIncrement(n, stmt());
		} else if(curType == TOK_WILE){
			addChild(n, createNode(WILE));
			if(curType != TOK_BOTH_SAEM && curType != TOK_DIFFRINT){  // if not comparison
				addChildNoIncrement(n, comparison());
			} else { // must be boolean
				addChildNoIncrement(n, boolean());
			}
			addChildNoIncrement(n, stmt());
		} else{
			addChildNoIncrement(n, stmt());
		}
		if(curType == TOK_IM_OUTTA_YR){
			addChild(n, createNode(IM_OUTTA_YR));
		} else {
			syntaxError("Expected loop exit keyword IM OUTTA YR");
		}
		if(curType == TOK_IDENT){
			addChildNoIncrement(n, var_val());
		} else {
			syntaxError("Expected loop identifier after IM OUTTA YR");
		}
	} else {
		syntaxError("Expected loop identifier after IM IN YR");
	}
	return n;
}

ast_node* function_definition(){
	ast_node* n = createNode(FUNCTION_DEFINITION), *s;
	addChild(n, createNode(HOW_IZ_I));
	if(curType == TOK_IDENT){
		addChildNoIncrement(n, var_val());
	} else {
		syntaxError("Expected identifier after HOW IZ I");
	}
	
	// check for parameter
	if(curType == TOK_YR){
		addChildNoIncrement(n, parameter());
	}
	
	// If 'IF U SAY SO' not found, assume there are statements in function body
	if(curType != TOK_IF_U_SAY_SO){
		addChildNoIncrement(n, stmt());
	}
	
	// Expect 'IF U SAY SO'
	if(curType == TOK_IF_U_SAY_SO){
		addChild(n, createNode(IF_U_SAY_SO));
	} else {
		syntaxError("Expected function definition end IF U SAY SO");
	}
	return n;
}

ast_node* parameter(){
	ast_node* n = createNode(PARAMETER);
	addChild(n, createNode(YR));
	if(curType == TOK_IDENT){
		addChildNoIncrement(n, var_val());
	} else {
		syntaxError("Expected parameter identifier after YR in function definition");
	}
	
	if(curType == TOK_AN){
		do{
			addChild(n, createNode(AN));
			if(curType == TOK_IDENT){
				addChildNoIncrement(n, var_val());
			} else {
				syntaxError("Expected parameter identifier after AN in function definition");
			}
		}while(curType == TOK_AN);
	}

	return n;
}

// ast_node* if_else(){
// 	ast_node* n = createNode(IF_ELSE);
// 	addChild(n, createNode(O_RLY));
// 	addChild(n, branches_block());
// 	if(curType == TOK_OIC){
// 		addChild(n, createNode(OIC));
// 	} else {
// 		syntaxError("Expected if block end OIC");
// 	}
// }

ast_node* branches_block(){
	ast_node *n;
	cur++;
	// check for if clause
	if(curType == TOK_YA_RLY){
		addChild(n, createNode(YA_RLY));	
		addChild(n, stmt());
		cur++;
		// check for else clause
		if(curType == TOK_NO_WAI){
			addChild(n, createNode(NO_WAI));
			addChild(n, stmt());
		}	
	}
	cur++;

}


// preorder traversal
void visit(ast_node* node){
	printf("%s ", string_ver[node->type]);
	for(int i = 0; i < node->numChildren; i++){
		visit(node->children[i]);
	}
}

void visit2(ast_node* node, Symbol pType, int pId, int pos){
	if(pos == 'F'){ // first child
		printf("(Children of %d: %s[%d] ", pId, string_ver[node->type], node->node_id);
	} else if(pos == 'L'){ // last child
		printf("%s[%d])\n", string_ver[node->type], node->node_id);
	} else if(pos == 'S') { // only child
		printf("(Child of %d: %s[%d] )\n", pId, string_ver[node->type], node->node_id);
	} else if(pType == -1){  // node being visited is root
		printf("ROOT NODE: PROG[%d]\n", node->node_id);
	} else {
		printf(" %s[%d] ", string_ver[node->type], node->node_id);
	}
	
	if(node->numChildren == 1){
		visit2(node->children[0], node->type, node->node_id, 'S');
		return;
	}

	for(int i = 0; i < node->numChildren; i++){
		if(i == 0){
			visit2(node->children[i], node->type, node->node_id,'F');
		} else if(i == node->numChildren-1) {
			visit2(node->children[i], node->type, node->node_id, 'L');
		} else {
			visit2(node->children[i], 0, node->node_id, i);
		}
	}
}

void listChildren(ast_node* node){
	printf("Children of %s\n", string_ver[node->type]);
	for(int i = 0; i < node->numChildren; i++){
		printf("\tChild [%d] type: %s\n", i, string_ver[node->children[i]->type]);
	}
}

void listTokens(TokenList* list){
	for(int i = 0; i < list->numTokens; i++){
		printf("Token: %s\n", string_ver[curType]);
		cur++;
	}
}

int main(){
	LexemeList* lexList = lex();
	TokenList* tokList = tokenize(lexList);
	lastTokIdx = tokList->numTokens-1;
	cur = tokList->tokens;
	listTokens(tokList);

	cur = tokList->tokens;
	FILE *outfile = fopen("tree.txt","w");
	ast_node* root = program(tokList, tokList->numTokens);
	// printf("ROOT HAS %d children\n", root->numChildren);
	print_ast_root_f(root, outfile);

	// printf("%d\n", root->children[0]->type);1
	// visit2(root, -1, -1, 'S');
}