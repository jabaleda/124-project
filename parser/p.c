#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

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
		7) issues with infinite arity in general (smoosh only has 2 operands atm)
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

/*
	Try to implement bonuses:
		1) MEBBE clause (else if) in if-else
		2) I *think* gagana if-else nesting based sa grammar
*/

#define curType (*cur)->type
#define nextType (*cur+1)->type
Token** cur; // Pointer to token list, change type later to token pag nagana na lexer!!!
int numNodes = 0;
Token* createToken(TokenType type){
	Token* t = malloc(sizeof(Token));
	t->type = type;
	return t;
}

ast_node* createNode(int type){
	ast_node* newNode = malloc(sizeof(ast_node));
	newNode->node_id = numNodes;
	numNodes++;
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
	ast_node* n = createNode(VAR_DEC), *s;
	addChild(n, createNode(I_HAS_A));
	do{
		cur++;
		if(curType == TOK_IDENT){
			s = createNode(IDENT);
			s->string_val = (*cur)->lexeme;
			addChild(n, s);
			if(nextType == TOK_ITZ){
				cur++;
				addChild(n, createNode(ITZ));
				addChild(n, var_val());
			}
		}
		cur++;
	} while(curType == TOK_I_HAS_A);
}

ast_node* var_val(){
	ast_node* n, *s;
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
			addChild(n, expr());
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
	n = createNode(ASSIGNMENT);
	s = createNode(IDENT);
	s->string_val = (*cur)->lexeme; 
	addChild(n, s);
	cur++;
	if(curType == TOK_R){
		addChild(n, createNode(R));
		s = var_val();
		addChild(n, s);
	} else {
		syntaxError("Expected R");
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
					syntaxError("Expected valid function arg");
				}
				cur++;
				if(curType == TOK_AN){
					addChild(n, createNode(AN));
				} else {
					syntaxError("Expected AN arg separator");
				}
			}
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

/* Rules: <arithmetic> ::= (SUM OF | DIFF OF | PRODUKT OF | QUOSHUNT OF | MOD OF) <var_value> AN <var_value> */
ast_node* arithmetic(){
	ast_node* n;
	n = createNode(ARITHMETIC);
	switch(curType){
		case TOK_SUM_OF:
			addChild(n, createNode(SUM_OF));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, var_val());	
			} else {
				syntaxError("Expected arg separator AN in binary arithmetic");
			}
			break;
		case TOK_DIFF_OF:
			addChild(n, createNode(DIFF_OF));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, var_val());	
			} else {
				syntaxError("Expected arg separator AN in binary arithmetic");
			}
			break;
		case TOK_PRODUKT_OF:
			addChild(n, createNode(PRODUKT_OF));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, var_val());	
			} else {
				syntaxError("Expected arg separator AN in binary arithmetic");
			}
			break;
		case TOK_QUOSHUNT_OF:
			addChild(n, createNode(QUOSHUNT_OF));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, var_val());	
			} else {
				syntaxError("Expected arg separator AN in binary arithmetic");
			}
			break;
		case TOK_MOD_OF:
			addChild(n, createNode(MOD_OF));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, var_val());	
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
			addChild(n, var_val());
			break;
		case TOK_BOTH_OF:
			addChild(n, createNode(BOTH_OF));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, var_val());
			} else {
				syntaxError("Expected arg separator AN in binary boolean");
			}
			break;
		case TOK_EITHER_OF:
			addChild(n, createNode(EITHER_OF));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, var_val());
			} else {
				syntaxError("Expected arg separator AN in binary boolean");
			}
			break;
		case TOK_WON_OF:
			addChild(n, createNode(WON_OF));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, var_val());
			} else {
				syntaxError("Expected arg separator AN in binary boolean");
			}
			break;
		// MISSING INFINITE BOOLEAN CASES
	}
	return n;
}

/* <relational> ::= <var_val> | (BIGGR OF | SMALLR OF) <valr_val> AN <var_val>*/
ast_node* relational(){
	ast_node* n;
	n = createNode(RELATIONAL);
	cur++;
	switch(curType){
		case TOK_BIGGR_OF:
			addChild(n, createNode(BIGGR_OF));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, var_val());
			} else {
				syntaxError("Expected arg separator AN in binary relational");
			}
			break;
		case TOK_SMALLR_OF:
			addChild(n, createNode(SMALLR_OF));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, var_val());
			} else {
				syntaxError("Expected arg separator AN in binary relational");
			}	
			break;
	}
	return n;
}

/* Rules: <comparison> ::= (BOTH SAEM | BOTH DIFFRINT) <var_val> AN <var_val>*/
ast_node* comparison(){
	ast_node* n;
	n = createNode(COMPARISON);
	switch(curType){
		case TOK_BOTH_SAEM:
			addChild(n, createNode(BOTH_SAEM));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, relational());
			} else {
				syntaxError("Expected arg separator AN in binary comparison");
			}
			break;
		case TOK_DIFFRINT:
			addChild(n, createNode(DIFFRINT));
			addChild(n, var_val());
			cur++;
			if(curType == TOK_AN){
				addChild(n, createNode(AN));
				addChild(n, relational());
			} else {
				syntaxError("Expected arg separator AN in binary comparison");
			}
			break;
	}
	return n;
}

ast_node* concatenation(){
	ast_node* n = createNode(CONCATENATION);
	addChild(n, createNode(SMOOSH));
	addChild(n, var_val());
	addChild(n, concat_operand());
	return n;
}

ast_node* concat_operand(){
	ast_node* n;
	if(nextType != TOK_AN){
		syntaxError("Expected arg separator AN in SMOOSH");
	}

	do{
		cur++;
		addChild(n, createNode(AN));
		addChild(n, var_val());
	}while(nextType == TOK_AN);

	return n;
}

/* <typecasting> ::= MAEK varident A type | varident IS NOW A type | varident R MAEK varident type */
ast_node* typecasting(){
	ast_node* n = createNode(TYPECASTING), *s;
	
	if(curType == TOK_MAEK){
		addChild(n, createNode(MAEK));
		cur++;
		if(curType == TOK_IDENT){
			s = createNode(IDENT);
			s->string_val = (*cur)->lexeme;
			addChild(n, s);
			cur++;
			if(curType == TOK_A){
				addChild(n, createNode(A));
				cur++;
				if(curType == TOK_TYPE){
					s = createNode(TYPE);
					s->string_val = (*cur)->lexeme;
					addChild(n, s);
				} else {
					syntaxError("Expected type literal after A");
				}
			} else {
				syntaxError("Expected type specifier A in typecast statement");
			}
		} else {
			syntaxError("Expected identifier after MAEK");
		}
	} else { // must follow either of (varident IS NOW A type | varident R MAEK varident type)
		s = createNode(IDENT);
		s->string_val = (*cur)->lexeme;
		addChild(n, s);
		cur++;
		if(curType == TOK_IS_NOW_A){
			addChild(n, createNode(IS_NOW_A));
			cur++;
			if(curType == TOK_TYPE){
				s = createNode(TYPE);
				s->string_val = (*cur)->lexeme;
				addChild(n, s);
			} else {
				syntaxError("Expected type literal after A");
			}
		} else { // must be following R MAEK format
			addChild(n, createNode(R));
			cur++;
			addChild(n, createNode(MAEK));
			cur++;
			if(curType == TOK_IDENT){
				s = createNode(IDENT);
				s->string_val = (*cur)->lexeme;
				addChild(n, s);
				cur++;
				if(curType == TOK_TYPE){
					s = createNode(TYPE);
					s->string_val = (*cur)->lexeme;
					addChild(n, s);
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


ast_node* expr(){
	ast_node *n, *s;
	n = createNode(EXPR);
	// cur++;

	// <arithmetic>
	if(curType == TOK_SUM_OF || curType == TOK_DIFF_OF || curType == TOK_PRODUKT_OF || curType == TOK_QUOSHUNT_OF || curType == TOK_MOD_OF){
		addChild(n, arithmetic());
	}

	// <boolean>
	if(curType == TOK_NOT || curType == TOK_BOTH_OF || curType == TOK_EITHER_OF || curType == TOK_WON_OF || curType == TOK_ALL_OF || curType == TOK_ANY_OF){
		addChild(n, boolean());
	}

	// <comparison>
	if(curType == TOK_BOTH_SAEM|| curType == TOK_DIFFRINT){
		addChild(n, comparison());
	}

	// <concatenation>
	if(curType == TOK_SMOOSH){
		addChild(n, concatenation());
	}

	// <typecasting>, does lookahead to distinguish from assignment which also has a rule starting with varident
	if(curType == TOK_MAEK || (curType == TOK_IDENT && nextType == TOK_IS_NOW_A) || (curType == TOK_IDENT && nextType == TOK_R && (*cur+2)->type == TOK_MAEK)){
		addChild(n, typecasting());
	}

	return 0;
}

ast_node* single_stmt(){ 
	/*<single_stmt> ::= <print> | <input> | <expr> | <assignment> | <function_call> | GTFO */
	ast_node *n, *s;
	n = createNode(SINGLE_STMT);
	// cur++;
	printf("IN SINGLE_STMT, cur = %d, curlex: %s\n", curType, (*cur)->lexeme);

	switch(curType){
		case TOK_VISIBLE:	/* VISIBLE <var_val>*/
			printf("VISIBLE FOUND\n");
			addChild(n, print());
			break;
		case TOK_GIMMEH:	/* GIMMEH varident*/
			addChild(n, input());
			break;
		case TOK_IDENT: /* varident R <var_val>*/
			printf("ASSIGN FOUND\n");
			if(nextType == TOK_R){
				addChild(n, assignment());
			} else {
				printf("NOT ASSIGN\n");
			}
			break;
		case TOK_I_IZ: /* <function_call> ::= I IZ funident MKAY | I IZ funident <argument> MKAY*/
			addChild(n, function_call());
			break;
		case TOK_GTFO:
			addChild(n, createNode(GTFO));
			break;
		default:
			printf("Reached end of single_stmt\n");
			addChild(n, expr());
			// try compound addChild()
	}

	cur++;
	if(curType != TOK_KTHXBYE){	// end of program not encountered, check for <stmt> ::= <single_stmt> <stmt>
		s = stmt();
		if(s != NULL){
			addChild(n, s);
		}
	}
	return n;
}

ast_node* compound_stmt(){
	ast_node* n;
	n = createNode(COMPOUND_STMT);

	if(nextType == TOK_O_RLY){ // check for if
		switch(curType){

		}
	} else {}



	return NULL;
}

// switch

ast_node* switch_case(){
	ast_node* n = createNode(SWITCH_CASE);
	addChild(n, createNode(WTF));
	cur++;
	if(curType == TOK_OMG){
		addChild(n, case_block());
	}
	cur++;
	if(curType == TOK_OMGWTF){ // check for default block keyword
		addChild(n, default_block());
	}
	addChild(n, createNode(OIC));
	return n;
}

ast_node* case_block(){
	ast_node* n = createNode(CASE_BLOCK), *s;
	do{
		addChild(n, createNode(OMG));
		cur++;
		switch(curType){
			case TOK_INTEGER:
				s = createNode(INTEGER);
				s->num_val = atoi((*cur)->lexeme);
				addChild(n, s);
				cur++;
				addChild(n, stmt());
				break;
			case TOK_FLOAT:
				s = createNode(FLOAT);
				s->num_val = atof((*cur)->lexeme);
				addChild(n, s);
				cur++;
				addChild(n, stmt());
				break;
			case TOK_STRING:
				s = createNode(INTEGER);
				s->string_val = (*cur)->lexeme;
				addChild(n, s);
				cur++;
				addChild(n, stmt());
				break;
			case TOK_TYPE:
				s = createNode(INTEGER);
				s->string_val = (*cur)->lexeme;
				addChild(n, s);
				cur++;
				addChild(n, stmt());
				break;
			default:
				syntaxError("Expected literal after OMG");
		}
		if(nextType == TOK_GTFO){  // if case_block has break, consume and create node
			cur++;
			addChild(n, createNode(GTFO));
		}
		cur++;	
	} while(curType == TOK_OMG); // check for further case

	return n;
}

ast_node* default_block(){
	ast_node* n = createNode(DEFAULT_BLOCK);
	addChild(n, createNode(OMGWTF));
	addChild(n, stmt());
	return n;
}

ast_node* loop(){
	ast_node* n = createNode(LOOP), *s;
	addChild(n, createNode(IM_IN_YR));
	cur++;
	if(nextType == TOK_IDENT){
		s = createNode(IDENT);
		s->string_val = (*cur)->lexeme;
		addChild(n, s);
		// slight change to grammar, removing loop_var_operation
		cur++;
		if(curType == TOK_UPPIN){
			addChild(n, createNode(UPPIN));		
		} else if(curType == TOK_NERFIN){
			addChild(n, createNode(NERFIN));
		} else {
			syntaxError("Expected UPPIN/NERFIN after loop identifier");
		}
		cur++;
		if(curType == TOK_YR){
			addChild(n, createNode(YR));
		} else {
			syntaxError("Expected YR in loop after UPPIN/NERFIN");
		}
		cur++;
		if(curType == TOK_IDENT){
			s = createNode(IDENT);
			s->string_val = (*cur)->lexeme;
			addChild(n, s);
		} else {
			syntaxError("Expected loop iterator identifier after UPPIN/NERFIN");
		}
		cur++;
		// check for break_condition (TIL/WILE keyword)
		// if either is found, check if break condition is boolean or comparison expr
		if(curType == TOK_TIL){ 
			addChild(n, createNode(TIL));
			if(curType != TOK_BOTH_SAEM && curType != TOK_DIFFRINT){  // if not comparison
				addChild(n, comparison());
			} else { // must be boolean
				addChild(n, boolean());
			}
			addChild(n, stmt());
		} else if(curType == TOK_WILE){
			addChild(n, createNode(WILE));
			if(curType != TOK_BOTH_SAEM && curType != TOK_DIFFRINT){  // if not comparison
				addChild(n, comparison());
			} else { // must be boolean
				addChild(n, boolean());
			}
			addChild(n, stmt());
		} else{
			addChild(n, stmt());
		}
		cur++;
		if(curType == TOK_IM_OUTTA_YR){
			addChild(n, createNode(IM_OUTTA_YR));
		} else {
			syntaxError("Expected loop exit keyword IM OUTTA YR");
		}
		cur++;
		if(curType == TOK_IDENT){
			s = createNode(IDENT);
			s->string_val = (*cur)->lexeme;
			addChild(n, s);
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
	cur++;
	if(curType == TOK_IDENT){
		s = createNode(IDENT);
		s->string_val = (*cur)->lexeme;
		addChild(n, s);
	} else {
		syntaxError("Expected identifier after HOW IZ I");
	}
	if(nextType == TOK_YR){ // check for parameter
		addChild(n, parameter());
	}
	addChild(n, stmt());
	cur++;
	if(curType == TOK_IF_U_SAY_SO){
		addChild(n, createNode(IF_U_SAY_SO));
	} else {
		syntaxError("Expected function definition end IF U SAY SO");
	}
	return n;
}

ast_node* parameter(){
	ast_node* n = createNode(PARAMETER), *s;
	do{
		cur++;
		addChild(n, createNode(YR));
		cur++;
		if(curType == TOK_IDENT){
			s = createNode(IDENT);
			s->string_val = (*cur)->lexeme;
			addChild(n, s);
		} else {
			syntaxError("Expected param identifier after YR in function definition");
		}
	}while(nextType == TOK_YR);

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

ast_node* stmt(){
	ast_node* n, *s;
	n = createNode(STMT);
	printf("IN STMT\n");
	printf("stmt curlex: %s\n", string_ver[(*cur)->type]);

	if(curType != TOK_KTHXBYE) cur++;

	if(/*curType != TOK_O_RLY || */ curType != TOK_WTF || curType != TOK_IM_IN_YR|| curType != TOK_HOW_IZ_I){
		addChild(n, single_stmt());
	} else {
		addChild(n, compound_stmt());
	}

	// single = single_stmt();
	// compound = compound_stmt();
	// if(single != NULL) addChild(n, single);
	// if(compound != NULL) addChild(n, single);
	printf("RETURNING STMT\n");
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
			if(curType == TOK_I_HAS_A){	// Found var dec statement
				addChild(n, var_dec());
					if(curType == TOK_BUHBYE){
					addChild(n, createNode(BUHBYE));
					if((*cur+1)->type != TOK_KTHXBYE){		// next token in list is not KTHXBYE, check if statement
						printf("FOUND STATEMENT\n");
						addChild(n, stmt());
						addChild(n, createNode(KTHXBYE));
					} else {
						addChild(n, createNode(KTHXBYE));
						cur++;
					}
				}
			} else if((*cur)->type == TOK_BUHBYE){		// No vars declared
				addChild(n, createNode(BUHBYE));
				if((*cur+1)->type != TOK_KTHXBYE){		// next token in list is not KTHXBYE, check if statement
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


int main(){
	int numTokens = 6;
	Token** tokenList = malloc(sizeof(Token*) * numTokens);
	tokenList[0] = createToken(TOK_HAI);
	tokenList[1] = createToken(TOK_WAZZUP);
	tokenList[2] = createToken(TOK_BUHBYE);
	tokenList[3] = createToken(TOK_VISIBLE);
	tokenList[4] = createToken(TOK_IDENT);
	tokenList[4]->lexeme = "A";
	tokenList[5] = createToken(TOK_KTHXBYE);

	// int numTokens = 8;
	// Token** tokenList = malloc(sizeof(Token*) * numTokens);
	// tokenList[0] = createToken(TOK_HAI);
	// tokenList[1] = createToken(TOK_WAZZUP);
	// tokenList[2] = createToken(TOK_BUHBYE);
	// tokenList[3] = createToken(TOK_VISIBLE);
	// tokenList[4] = createToken(TOK_IDENT);
	// tokenList[4]->lexeme = "A";
	// tokenList[5] = createToken(TOK_VISIBLE);
	// tokenList[6] = createToken(TOK_IDENT);
	// tokenList[6]->lexeme = "B";
	// tokenList[7] = createToken(TOK_KTHXBYE);
	

	cur = tokenList;
	ast_node* root = program(tokenList, numTokens);

	listChildren(root);
	listChildren(root->children[0]);
	listChildren(root->children[1]);
	listChildren(root->children[2]); //stmt
	printf("stmt has %d c",root->children[2]->numChildren);
	listChildren(root->children[2]->children[0]);
	
	printf("\n===========================\n");
	printf("Post-order traversal of AST\n");
	printf("===========================\n");

	// visit(root);
	// printf("\n");

	// printf("\n");
	visit2(root, -1, -1,'X');
	// // visit(root);

	// printf("\n");
	
	// printf("Root's Children:\n");
// 	listChildren(root);
// 	listChildren(root->children[2]);
}