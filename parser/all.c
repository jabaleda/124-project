#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "all.h"


#define curType		(*cur)->type
#define nextType	(*(cur+1))->type
#define ITVAL		symTable->entries[0]->value

const int EXPR_KEYWORDS[] = {TOK_SUM_OF, TOK_DIFF_OF, TOK_PRODUKT_OF, TOK_QUOSHUNT_OF, TOK_MOD_OF, TOK_NOT, TOK_BOTH_OF, TOK_EITHER_OF, TOK_WON_OF, TOK_ALL_OF, TOK_ANY_OF, TOK_BOTH_SAEM, TOK_DIFFRINT, TOK_BIGGR_OF, TOK_SMALLR_OF, TOK_SMOOSH};
int lastTokIdx;
/* Pointer to elements in token array */
Token** cur;

/* Total number of nodes in AST*/
int numNodes = 0;

SymbolTable* symTable;

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

/* Alert syntax error and terminate interpreter */
void syntaxError(char *msg){
	printf("Syntax error: %s	Found: %s (line: %d)\n", msg, (*cur)->lexeme, (*cur)->line);
	exit(1);
}

/* Increment pointer up to the next non-newline (TOK_BREAK) token*/
void seekNextLine(){
	if(curType != TOK_BREAK){
		syntaxError("Expected newline.");
	}
	while(curType == TOK_BREAK){
		cur++;
	}
}


/* Use for debugging only, traces parse sequence to identify current function, token, lexeme, and line*/
void trace(char* msg, char* node){
	printf("Trace: %s	In: %s\n		Current Token: %s	Lexeme: %s	Line: %d\n\n", msg, node, string_ver[curType], (*cur)->lexeme,(*cur)->line);
}

/* Prints out formatted symbol table */
void print_table(SymbolTable* table){
    printf("=============== SYMBOL TABLE ===============\n");
    // Num     Identifier       SymbolType          VarType         Value
    printf("%-3s | %-20s | %-10s | %-10s | %-30s\n", "No.", "Identifier", "SymbolType", "VarType", "Value");        
    for(int i = 0; i < table->numEntries; i++){
        if(table->entries[i]->symType == SYM_VAR){
            switch(table->entries[i]->varType){
                case TYPE_INT:
                    printf("%-3d | %-20s | %-10s | %-10s | %-30d\n", i, table->entries[i]->id, symType_strings[table->entries[i]->symType], varType_strings[table->entries[i]->varType], table->entries[i]->value.intVal);        
                    break;
                case TYPE_FLOAT:
                    printf("%-3d | %-20s | %-10s | %-10s | %-30f\n", i, table->entries[i]->id, symType_strings[table->entries[i]->symType], varType_strings[table->entries[i]->varType], table->entries[i]->value.floatVal);        
                    break;
                case TYPE_STRING:
                    printf("%-3d | %-20s | %-10s | %-10s | %-30s\n", i, table->entries[i]->id, symType_strings[table->entries[i]->symType], varType_strings[table->entries[i]->varType], table->entries[i]->value.stringVal);        
                    break;
                case TYPE_BOOL:
                    printf("%-3d | %-20s | %-10s | %-10s | %-30s\n", i, table->entries[i]->id, symType_strings[table->entries[i]->symType], varType_strings[table->entries[i]->varType], table->entries[i]->value.intVal == 1? "True" : "False");        
                    break;
                case TYPE_TYPE:
                    printf("%-3d | %-20s | %-10s | %-10s | %-30s\n", i, table->entries[i]->id, symType_strings[table->entries[i]->symType], varType_strings[table->entries[i]->varType], table->entries[i]->value.stringVal);        
                    break;
            }
        // } else if (table->entries[i]->symType == SYM_FUN){
                    // printf("%-3d | %-20s | %-10s | %-10s | %-30s", i, table->entries[i]->id, symType_strings[table->entries[i]->symType], "N/A", "N/A");        
        } else {
                    printf("%-3d | %-20s | %-10s | %-10s | %-30s\n", i, table->entries[i]->id, symType_strings[table->entries[i]->symType], "N/A", "N/A");        

        }         
    }
}

SymbolTable* create_table(){
    SymbolTable* newTable = malloc(sizeof(SymbolTable));
    if(newTable == NULL){
        printf("Create symbol table malloc error.\n");
        exit(1);
    }
    newTable->numEntries = 0;
    newTable->entries = NULL;
    return newTable;
}

Entry* create_var_entry_int(char *id, int val){
    Entry* newEntry = malloc(sizeof(Entry));
    if(newEntry == NULL){
        printf("Symbol table entry malloc error.\n");
        exit(1);
    }   newEntry->id = strdup(id);
    newEntry->symType = SYM_VAR;
    newEntry->varType = TYPE_INT;
    newEntry->value.intVal = val;
    return newEntry;
}
Entry* create_var_entry_float(char *id, float val){
    Entry* newEntry = malloc(sizeof(Entry));
    if(newEntry == NULL){
        printf("Symbol table entry malloc error.\n");
        exit(1);
    }    newEntry->id = strdup(id);
    newEntry->symType = SYM_VAR;
    newEntry->varType = TYPE_FLOAT;
    newEntry->value.floatVal = val;
    return newEntry;
}
Entry* create_var_entry_string(char *id, char *val){
    Entry* newEntry = malloc(sizeof(Entry));
    if(newEntry == NULL){
        printf("Symbol table entry malloc error.\n");
        exit(1);
    }   newEntry->id = strdup(id);
    newEntry->symType = SYM_VAR;
    newEntry->varType = TYPE_STRING;
    newEntry->value.stringVal = val;
    return newEntry;
}

Entry* create_var_entry_bool(char *id, int val){
    Entry* newEntry = malloc(sizeof(Entry));
    if(newEntry == NULL){
        printf("Symbol table entry malloc error.\n");
        exit(1);
    }
    newEntry->id = strdup(id);
    newEntry->symType = SYM_VAR;
    newEntry->varType = TYPE_BOOL;
    newEntry->value.intVal = val;
    return newEntry;
}

Entry* create_var_entry_type(char *id, char *val){
    Entry* newEntry = malloc(sizeof(Entry));
    if(newEntry == NULL){
        printf("Symbol table entry malloc error.\n");
        exit(1);
    }    newEntry->id = strdup(id);
    newEntry->symType = SYM_VAR;
    newEntry->varType = TYPE_STRING;
    newEntry->value.stringVal = val;
    return newEntry;
}

Entry* create_var_entry_no_type(char *id){
    Entry* newEntry = malloc(sizeof(Entry));
    if(newEntry == NULL){
        printf("Symbol table entry malloc error.\n");
        exit(1);
    }    newEntry->id = strdup(id);
    newEntry->symType = SYM_VAR;
    newEntry->varType = TYPE_NOOB;
    return newEntry;
}

Entry* create_function_entry(char *id){
    Entry* newEntry = malloc(sizeof(Entry));
    if(newEntry == NULL){
        printf("Symbol table entry malloc error.\n");
        exit(1);
    }
    newEntry->id = strdup(id);
    newEntry->symType = SYM_FUN;
    return newEntry;
}

Entry* create_param_entry(char *id){
    Entry* newEntry = malloc(sizeof(Entry));
    if(newEntry == NULL){
        printf("Symbol table entry malloc error.\n");
        exit(1);
    }
    newEntry->id = strdup(id);
    newEntry->symType = SYM_PARAM;
    return newEntry;
}

void addSymTableEntry(SymbolTable* table, Entry* e){
    Entry **temp = realloc(table->entries, sizeof(Entry*) * (table->numEntries+1));
    if(temp == NULL){
        printf("Add symbol table entry realloc error.\n");
        exit(1);
    }
    table->entries = temp;
    table->entries[table->numEntries] = e;
    table->numEntries++;
}

SymbolTable* initSymbolTable(){
    SymbolTable* newTable = create_table();
    Entry* itVar = create_var_entry_int("IT", 0);
    addSymTableEntry(newTable, itVar);
    return newTable;
}

int setVarEntryValInt(char* id, int val){
	for(int i = 0; i < symTable->numEntries; i++){
		if(strcmp(symTable->entries[i]->id, id) == 0){
			symTable->entries[i]->value.intVal = val;
		}
	}
}
int setVarEntryValFloat(char* id, float val){
	for(int i = 0; i < symTable->numEntries; i++){
		if(strcmp(symTable->entries[i]->id, id) == 0){
			symTable->entries[i]->value.floatVal = val;
		}
	}
}
int setVarEntryValString(char* id, char* val){
	for(int i = 0; i < symTable->numEntries; i++){
		if(strcmp(symTable->entries[i]->id, id) == 0){
			symTable->entries[i]->value.stringVal = strdup(val);
		}
	}
}
int setVarEntryValBool(char* id, int val){
	for(int i = 0; i < symTable->numEntries; i++){
		if(strcmp(symTable->entries[i]->id, id) == 0){
			symTable->entries[i]->value.intVal = val;
		}
	}
}
int setVarEntryValType(char* id, char* val){
	for(int i = 0; i < symTable->numEntries; i++){
		if(strcmp(symTable->entries[i]->id, id) == 0){
			symTable->entries[i]->value.stringVal = strdup(val);
		}
	}
}

/*
	Finite Automata? to parse token list and build AST
*/
ast_node* program(TokenList* tokenList, int numTokens){
	ast_node* n;
	while(curType == TOK_BREAK) cur++;		// ignore any trailing newlines
	if(curType == TOK_HAI){
		n = createNode(PROG);
		// trace("Start", string_ver[n->type]);
		addChild(n, createNode(HAI));
		seekNextLine();
		if(curType == TOK_WAZZUP){
			addChild(n, createNode(WAZZUP));
			seekNextLine();
			if(curType == TOK_I_HAS_A){					// Found var dec statement
				// trace("Found var_dec", string_ver[n->type]);
				addChildNoIncrement(n, var_dec());
				// trace("End of var_decs", string_ver[n->type]);
					if(curType == TOK_BUHBYE){
						addChild(n, createNode(BUHBYE));
						seekNextLine();
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
					seekNextLine();
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

	// seekNextLine();

	if(curType != TOK_KTHXBYE){	// Not all tokens consumed after KTHXBYE keyword encountered
		syntaxError("Error. Parsing incomplete.");
	}
	printf("Done parsing.\n");
	return n;
}

ast_node* var_dec(){
	ast_node* n = createNode(VAR_DEC), *c1, *c2;
	do{
		// trace("Found I HAS A", string_ver[n->type]);
		addChild(n, createNode(I_HAS_A));
		//  check for following identifier
		if(curType == TOK_IDENT){
			c1 = var_val();
			addChildNoIncrement(n, c1);
			// trace("Returned from first ident in var_dec", string_ver[n->type]);
			// check for initialization
			if(curType == TOK_ITZ){		
				addChild(n, createNode(ITZ));
				c2 = var_val();
				addChildNoIncrement(n, c2);
				switch(c2->children[0]->type){
					case TOK_INTEGER:
						addSymTableEntry(symTable, create_var_entry_int(c1->children[0]->id_name, (int) c2->children[0]->num_val));
						// addSymTableEntry(symTable, create_var_entry_int(c1->children[0]->id_name, symTable->entries[0]->value.intVal));
						break;
					default:
				}
			} else {
			
			}
		} else {
			syntaxError("Expected identifier after I HAS A");
		}

		seekNextLine();
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
			setVarEntryValInt("IT", s->num_val);
			addChild(n, s);
			break;
		case TOK_FLOAT:
			s = createNode(FLOAT);
			s->num_val = atof((*cur)->lexeme);
			setVarEntryValFloat("IT", s->num_val);
			addChild(n, s);
			break;
		case TOK_STRING:
			s = createNode(STRING);
			s->string_val = (*cur)->lexeme;
			setVarEntryValString("IT", s->string_val);
			addChild(n, s);
			break;
		case TOK_BOOLEAN:
			s = createNode(BOOLEAN);
			s->string_val = (*cur)->lexeme;
			setVarEntryValInt("IT", s->num_val);
			addChild(n, s);
			break;
		case TOK_TYPE:
			s = createNode(TYPE);
			s->string_val = (*cur)->lexeme;
			setVarEntryValString("IT", s->string_val);
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

	seekNextLine();

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

	// string concatenation
	if(curType == TOK_SMOOSH){
		addChildNoIncrement(n, concatenation());
	}

	// arithmetic expr
	if(  curType == TOK_SUM_OF 
			|| curType == TOK_DIFF_OF 
			|| curType == TOK_PRODUKT_OF 
			|| curType == TOK_QUOSHUNT_OF 
			|| curType == TOK_MOD_OF)
	{
		addChildNoIncrement(n, arithmetic());
	}

	// bool
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

	seekNextLine();
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
// void visit(ast_node* node){
// 	printf("%s ", string_ver[node->type]);
// 	for(int i = 0; i < node->numChildren; i++){
// 		visit(node->children[i]);
// 	}
// }

void visit2(ast_node* node, Symbol pType, int pId, int pos){
	if(pos == 'F'){ // first child
		printf("(Children of %d: %s[%d] ", pId, string_ver[node->type], node->node_id);
	} else if(pos == 'L'){ // last child
		printf("%s[%d])\n", string_ver[node->type], node->node_id);
	} else if(pType == -1){  // node being visited is root
		printf("ROOT NODE: PROG[%d]\n", node->node_id);
	} else if(pos == 'S') { // only child
		printf("(Child of %d: %s[%d] )\n", pId, string_ver[node->type], node->node_id);
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


// * Julianne -----------------------------------------------------------
// --- helpers 

EvalData *createEvalData(){
	EvalData *new = malloc(sizeof(EvalData));
	new->expr_source_type = 0;
	new->float_flag = 0;
	return new;
}

// symbol table var_ident ref searcher
int find_ident_num(SymbolTable *table, char *var_ident) {
	for(int i=0; i<table->numEntries; i++){
		if(strcmp(table->entries[i]->id, var_ident) == 0){
			return i;
		}
	}
	return -1;
}
// ---

// Interpreting Business
// Walk the tree start
// 	on var_dec: ITZ, evaluate and check var_val subtree until literal OR expr is encountered, then enter to symbol table.
//  on expr: 
// 		check expression type: PRINT, ARITH, etc.
//			obtain operands and evaluate them first if operand is another expr
//			perform operation
// 			return result

// 	until statement or expr or ... is encountered

/*
If both operands evaluate to a NUMBR, the result of the operation is a NUMBR.
If at least one operand is a NUMBAR, the result of the operation is a NUMBAR.

*/
// SUM OF | DIFF OF | PRODUKT OF | QUOSHUNT OF
void *arith_evaluator(ast_node *node, EvalData *answer) {
	printf("%s \n", string_ver[node->type]);
	// check first child for operator
	ast_node *first_child = node->children[0];
	// operands
	ast_node *left_operand = node->children[1];
	ast_node *right_operand = node->children[3];
	
	TokenType operator = first_child->type;
	int left_float_flag = 0;
	int right_float_flag = 0;

	int left_int, right_int, result_int;
	float left_fl, right_fl, result_fl;

	// check if they are expr (NESTED) then evaluate first before returning here
	printf("%s \n", string_ver[left_operand->children[0]->type]);
	if(left_operand->children[0]->type == EXPR) {
		EvalData *evaled = createEvalData();
		arith_evaluator(left_operand->children[0]->children[0], evaled);
		// check flags and set operands accdgly
		left_float_flag = evaled->float_flag;
		switch (left_float_flag) {
		case 0:
			left_int = evaled->eval_data.int_Result;
			break;
		case 1:
			left_fl = evaled->eval_data.flt_Result;
			break;
		default:
			break;
		}
	}
	printf("%s \n", string_ver[right_operand->children[0]->type]);
	if(right_operand->children[0]->type == EXPR) {
		EvalData *evaled = createEvalData();
		arith_evaluator(right_operand->children[0]->children[0], evaled);
		// check flags and set operands accdgly
		right_float_flag = evaled->float_flag;
		switch (right_float_flag) {
		case 0:
			right_int = evaled->eval_data.int_Result;
			break;
		case 1:
			right_fl = evaled->eval_data.flt_Result;
			break;
		default:
			break;
		}
	}

	// 0 0 -> NUMBR
	// 0 1 || 1 0 -> NUMBAR
	// 11 -> NUMBAR
	if(left_operand->type == VAR_VAL) {
		ast_node *lo_val = left_operand->children[0];
	
		int lo_type = lo_val->type;
		switch(lo_type){
			case INTEGER:
				left_int = lo_val->num_val;
				break;
			case FLOAT: 
				left_fl = lo_val->num_val;
				left_float_flag = 1;
				break;
			// case IDENT: 
			// 	// cross-check with symbol table
			// case EXPR:
				// call expr evaluator
		}
		// evaluate and return here
	}
	if(right_operand->type == VAR_VAL) {
		ast_node *ro_val = right_operand->children[0];
	
		int ro_type = ro_val->type;
		switch(ro_type){
			case INTEGER:
				right_int = ro_val->num_val;
				break;
			case FLOAT: 
				right_fl = ro_val->num_val;
				right_float_flag = 1;
				break;

			// case IDENT: 
			// 	// cross-check with symbol table
			// case EXPR:
				// call expr evaluator
		}
		// evaluate and return here
	}

	// perform operation
	switch(operator) {
		case SUM_OF:
			// check if NUMBR or NUMBAR
			if(left_float_flag || right_float_flag == 1){
				if(left_float_flag && right_float_flag == 1){
					result_fl = left_fl + right_fl;
				} else {
					// eval as NUMBAR
					switch(left_float_flag){
						case 0:
							result_fl = (float)left_int + right_fl;
							break;
						case 1:
							result_fl = left_fl + (float)right_int;
							break;
					}
				}
				answer->eval_data.flt_Result = result_fl;
			} else {
				// resulted to 0, eval as NUMBR
				result_int = left_int + right_int;
				answer->eval_data.int_Result = result_int;
			}
			break;
		case DIFF_OF:
			// check if NUMBR or NUMBAR
			if(left_float_flag || right_float_flag == 1){
				// eval as NUMBAR
				if(left_float_flag && right_float_flag == 1){
					result_fl = left_fl - right_fl;
				} else {
					// eval as NUMBAR
					switch(left_float_flag){
						case 0:
							result_fl = (float)left_int - right_fl;
							break;
						case 1:
							result_fl = left_fl - (float)right_int;
							break;
					}
				}
				answer->eval_data.flt_Result = result_fl;
			} else {
				// resulted to 0, eval as NUMBR
				result_int = left_int - right_int;
				answer->eval_data.int_Result = result_int;
			} 
			break;
		case PRODUKT_OF:
			// check if NUMBR or NUMBAR
			if(left_float_flag || right_float_flag == 1){
				// eval as NUMBAR
				if(left_float_flag && right_float_flag == 1){
					result_fl = left_fl * right_fl;
				} else {
					// eval as NUMBAR
					switch(left_float_flag){
						case 0:
							result_fl = (float)left_int * right_fl;
							break;
						case 1:
							result_fl = left_fl * (float)right_int;
							break;
					}
				}
				answer->eval_data.flt_Result = result_fl;
			} else {
				// resulted to 0, eval as NUMBR
				result_int = left_int * right_int;
				answer->eval_data.int_Result = result_int;
			}
			break;
		case QUOSHUNT_OF:
			
			if(right_fl != 0) {
				// check if NUMBR or NUMBAR
				if(left_float_flag || right_float_flag == 1){
					// eval as NUMBAR
					if(left_float_flag && right_float_flag == 1){
						result_fl = left_fl / right_fl;
					} else {
						// eval as NUMBAR
						switch(left_float_flag){
							case 0:
								result_fl = (float)left_int / right_fl;
								break;
							case 1:
								result_fl = left_fl / (float)right_int;
								break;
						}
					}
					answer->eval_data.flt_Result = result_fl;
				} else {
					// resulted to 0, eval as NUMBR
					result_int = left_int / right_int;
					answer->eval_data.int_Result = result_int;
				}
			} else {
				printf("!!! Error: Dividing by 0!");
			}
			break;
		default: 
			printf("!!! Error. Unknown arithmetic operation!\n");
	}

	// pack evaluation, to return to print
	answer->expr_source_type = 1;
	answer->float_flag = left_float_flag || right_float_flag;
}


int expr_type_check(ast_node *node) {
	int type = node->type;
	switch(type){
		case ARITHMETIC: return 1;
		case BOOLEAN: return 2;
		case COMPARISON: return 3;
		case CONCATENATION: return 4;
		case TYPECASTING: return 5;
		default: return 0;
	}
}


EvalData *subtree_walk(ast_node *node) {
	printf("%s \n", string_ver[node->type]);
	if(node->type == VAR_VAL) {
		ast_node *child = node->children[0];
		if(child->type == EXPR) {
			ast_node *grandchild = child->children[0];
			int expr_type = expr_type_check(grandchild);
			switch(expr_type){
				case ARITHMETIC: 
					// arithmetic evaluator
					break;
				// case BOOLEAN:

				// case COMPARISON: 

				// case CONCATENATION: 

				// case TYPECASTING: 

				default:
					printf("Error, unknown expression!");
			}

		} else {
			return 0;
		}
	} else if(node->type == EXPR) {
		ast_node *child = node->children[0];
		printf("%s \n", string_ver[child->type]);
		int expr_type = expr_type_check(child);
		switch(expr_type){
			case 1: 
				// arithmetic evaluator
				EvalData *evaled = createEvalData();
				arith_evaluator(child, evaled);
				return evaled;
				// break;

			// case BOOLEAN:

			// case COMPARISON: 

			// case CONCATENATION: 

			// case TYPECASTING: 

			default:
				printf("Error, unknown expression!");
		}
	}
}





// taken from pre-order traversal
void var_dec_tree_walk(ast_node* node){
	printf("%s \n", string_ver[node->type]);

	if(node->type == ITZ) {
		printf(" --- ITZ found! ---\n");
		// int dec_type = subtree_walk(node);
		// // // UNNEEDED: automaticaly entered to symtable // //check literal type of var_val child, call lit. type returner
		// // // if child is expr
		// switch(dec_type){

		// }
			// call expr evaluator and get expr type
			// switch case expr type
			// call respective evaluators

	}

	for(int i = 0; i < node->numChildren; i++){
		var_dec_tree_walk(node->children[i]);
	}
}

// called from root node, preorder traversal
void interpret_walk(SymbolTable *table, ast_node *node) {
	printf("%s \n", string_ver[node->type]);
	// check if print statement encountered, since output is (required) and evaluation may be performed
	if(node->type == PRINT && node->children[0]->type == PRINT) {
		ast_node *child = node->children[1];
		ast_node *print_op = child->children[0];
		int print_op_type = print_op->type;			// print operand value type
		switch(print_op_type) {
			case STRING:
				// print string to terminal
				// TODO: Remove quotations in printing
				printf("%s\n", print_op->string_val);
				return;		// to go to next child of PRINT's parent (PRINT's sibling)
			case INTEGER:
				printf("%d\n", (int)print_op->num_val);
				return;
			case FLOAT:
				printf("%f\n", print_op->num_val);
				return;
			// case BOOLEAN:
				// print literal to terminal
				// return
			case IDENT:
				// check with symbol table
				int ident_num = find_ident_num(table, print_op->id_name);
				// check type before print
				VarType val_type = table->entries[ident_num]->varType;
				switch(val_type) {
					case TYPE_INT:
						printf("%d\n", table->entries[ident_num]->value.intVal);
						break;
					case TYPE_FLOAT:
						printf("%f\n", table->entries[ident_num]->value.floatVal);
						break;
					// case TYPE_BOOL:
					
					// default to string to output
					default:
						printf("%s\n", table->entries[ident_num]->value.stringVal);
				}
				return;

				// symTable->entries
				// print return val
			case EXPR:
				// TODO: Make a branch w/ expr case outside of this print-if-branch to evaluate expressions not in a print statement
				// call expr evaluator
				EvalData *result = subtree_walk(print_op);		// TODO: might also need symbol table here to evaluate with var_idents

				switch(result->expr_source_type){
					case 1:
						// from arithmetic
						if(result->float_flag == 1){
							printf("%f\n", result->eval_data.flt_Result);
						} else if(result->float_flag == 0) {
							printf("%d\n", result->eval_data.int_Result);
						} // else 
						
				}
				// print return val
				return;
		}
	} 
	// else if () {


	// }
	for(int i = 0; i < node->numChildren; i++){
		interpret_walk(table, node->children[i]);
	}
}

// * Julianne -----------------------------------------------------------




















int main(){
	LexemeList* lexList = lex();
	printf("lexing done...");
	TokenList* tokList = tokenize(lexList);
	lastTokIdx = tokList->numTokens-1;
	cur = tokList->tokens;
	symTable = initSymbolTable();
	// listTokens(tokList);

	cur = tokList->tokens;
	FILE *outfile = fopen("tree.txt","w");
	ast_node* root = program(tokList, tokList->numTokens);
	printf("ROOT HAS %d children\n", root->numChildren);
	print_ast_root_f(root, outfile);

	print_table(symTable);

	printf("%d\n", root->children[0]->type);
	// visit2(root, -1, -1, 'S');

	// * Julianne
	// var_dec_tree_walk(root);
	printf("---------- INTERPRETING... ----------\n");
	interpret_walk(symTable, root);
	// Julianne
}	