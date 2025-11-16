#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

/* enum of all types of tokens */
typedef enum{
	// Literals
	TOK_INTEGER,
	TOK_FLOAT,
	TOK_STRING,
	TOK_BOOLEAN,
	TOK_TYPE,

	// Identifier
	TOK_IDENT,

	// Single-word keywords
	TOK_HAI,
	TOK_KTHXBYE,
	TOK_WAZZUP,
	TOK_BUHBYE,
	TOK_ITZ,
	TOK_R,
	TOK_NOT,
	TOK_DIFFRINT,
	TOK_SMOOSH,
	TOK_MAEK,
	TOK_A,
	TOK_VISIBLE,
	TOK_GIMMEH,
	TOK_OIC,
	TOK_WTF,
	TOK_OMG,
	TOK_OMGWTF,
	TOK_UPPIN,
	TOK_NERFIN,
	TOK_YR,
	TOK_TIL,
	TOK_WILE,
	TOK_AN,
	TOK_GTFO,
	TOK_MKAY,

	// Double-word keywords
	TOK_SUM_OF,
	TOK_DIFF_OF,
	TOK_PRODUKT_OF,
	TOK_QUOSHUNT_OF,
	TOK_MOD_OF,
	TOK_BIGGR_OF,
	TOK_SMALLR_OF,
	TOK_BOTH_OF,
	TOK_EITHER_OF,
	TOK_WON_OF,
	TOK_ANY_OF,
	TOK_ALL_OF,
	TOK_BOTH_SAEM,
	TOK_O_RLY,
	TOK_YA_RLY,
	TOK_NO_WAI,
	TOK_I_IZ,

	// Triple-word keywords
	TOK_I_HAS_A,
	TOK_IS_NOW_A,
	TOK_IM_IN_YR,
	TOK_IM_OUTTA_YR,
	TOK_HOW_IZ_I,

	//
	TOK_IF_U_SAY_SO

} TokenType;

/* List of all keywords */
char *keywords[] = {
    // single-word keywords
    "HAI",
    "KTHXBYE",
    "WAZZUP",
    "BUHBYE",
    "ITZ",
    "R",
    "NOT",
    "DIFFRINT",
    "SMOOSH",
    "MAEK",
    "A",
    "VISIBLE",
    "GIMMEH",
    "OIC",
    "WTF?",
    "OMG",
    "OMGWTF",
    "UPPIN",
    "NERFIN",
    "YR",
    "TIL",
    "WILE",
    "AN",
    "GTFO",
    "MKAY",

    // two-word keywords
    "SUM OF",
    "DIFF OF",
    "PRODUKT OF",
    "QUOSHUNT OF",
    "MOD OF",
    "BIGGR OF",
    "SMALLR OF",
    "BOTH OF",
    "EITHER OF",
    "WON OF",
    "ANY OF",
    "ALL OF",
    "BOTH SAEM",
    "O RLY?",
    "YA RLY",
    "NO WAI",
    "I IZ",

    // three-word keywords
    "I HAS A",
    "IS NOW A",
    "IM IN YR",
    "IM OUTTA YR",
    "HOW IZ I",

    //
    "IF U SAY SO"
};

typedef struct {
    TokenType type;
    char* lexeme;
                    // add semantic value for literals OR leave it to parser to check Token's type
                    // then treating the lexeme accordingly (ex if type == INTEGER, use value of atoi(lexeme))
    int line;
} Token;

typedef struct {
    Token** tokens;
    int numTokens;
} TokenList;


Token* createToken(TokenType type, char* lexeme, int line){
    Token *newToken = malloc(sizeof(Token));
    newToken->type = type;
    newToken->lexeme = malloc(sizeof(char) * strlen(lexeme) + 1);
    strcpy(newToken->lexeme, lexeme);
    newToken->line = line;
    return newToken;
}

TokenList* createTokenList(){
    TokenList *newList = malloc(sizeof(TokenList));
    newList->tokens = NULL;
    newList->numTokens = 0;
    return newList;
}

int addToken(TokenList* list, Token* newToken){
    Token **temp = NULL;
    temp = realloc(list->tokens, sizeof(Token) * list->numTokens + 1);
    if(temp != NULL){
        list->tokens = temp;
    } else {
        printf("Token list realloc error\n");
        exit(1);
    }
    list->numTokens++;
}

int isInteger(char* lex){
    int c = 0;
    if(lex[c] == '-' || isdigit(lex[c]) != 0){
        c++;
        while(isdigit(lex[c]) != 0 && c <= strlen(lex)){
            c++;
        }
        if(c == strlen(lex)) return 1;
    }
    return 0;
}

int isFloat(char* lex){
    int c = 0;
    if(lex[c] == '-' || isdigit(lex[c]) != 0){
        c++;
        while(isdigit(lex[c]) != 0 && c <= strlen(lex)){
            c++;
        }
        if(lex[c] == '.'){
            c++;
            while(isdigit(lex[c]) != 0 && c <= strlen(lex)){
                c++;
            }
            if(c == strlen(lex)) return 1;
        }
    }
    return 0;
}

int isIdentifier(char* lex){
    int c = 0;
    if(isalpha(lex[c]) != 0){
        c++;
        while(( isalpha(lex[c]) != 0 || 
                isdigit(lex[c]) != 0 || 
                lex[c] == '_') && c <= strlen(lex))
        {
            c++;
        }
        if(c == strlen(lex)) return 1;
    }

    return 0;
}

TokenType isKeyword(LexemeList* list, int *pos){
    int len;                                // number of words in the keyword
    Lexeme* lptr = list->lexemes[*pos];     // pointer to lexeme
    char* cptr;                             // pointer to characters in lexeme
    char* sptr;                             // pointer to elements in keywords list
    for(int i = 0; i < 48; i++){
        // Set their initial values
        len = 0;
        sptr = keywords[i];
        // printf("\nMatching against \"%s\"\n",  sptr);
        lptr = list->lexemes[*pos];
        cptr = lptr->value;
        while(*sptr != '\0' && *cptr == *sptr){
            // printf("sptr = %c   cptr = %c   lptr = %s\n", *sptr, *cptr, lptr->value);
            sptr++;
            // printf("*sptr after increment: %c   len = %d\n", *sptr, len);
            if(*sptr == ' '){
                len++;
                sptr++;
                if(*pos + len < list->total_num){
                    lptr = list->lexemes[*pos + len];
                } else {
                    return -1; // Reached end of list
                }
                cptr = lptr->value;
                // printf("Skipping space. sptr = %c   cptr = %c   lptr = %s   len = %d\n", *sptr, *cptr, lptr->value, len);
            } else {
                cptr++;
            }
        }
        if(*sptr == '\0'){ // successfully parsed a keyword
            len++;
            *pos += len-1; // move lexemeList iterator
            return i+6;  // return corresponding TokenType
        }
    }
    return -1; // none matched
}

TokenList* tokenize(LexemeList* list){
    TokenList* tokenList = createTokenList();

    for(int i = 0; i < list->total_num; i++){
        printf("On lexeme number %d\n", i);
        Token* newToken = NULL;
        if(isInteger(list->lexemes[i]->value)){
            newToken = createToken(TOK_INTEGER, list->lexemes[i]->value, list->lexemes[i]->line); 
        } else if(isFloat(list->lexemes[i]->value)){
            newToken = createToken(TOK_FLOAT, list->lexemes[i]->value, list->lexemes[i]->line); 
        // }                    
        // else if(){
            // if string
        } else if(list->lexemes[i]->value == "WIN"){
            newToken = createToken(TOK_BOOLEAN, "WIN", list->lexemes[i]->line); 
        } else if(list->lexemes[i]->value == "FAIL"){
            newToken = createToken(TOK_BOOLEAN, "FAIL", list->lexemes[i]->line); 
        } else {
            // Check if keyword
            int type = isKeyword(list, &i);
            if(type != -1){ // if keyword matched
                newToken = createToken(type, list->lexemes[i]->value, list->lexemes[i]->line);
            } else if(isIdentifier(list->lexemes[i]->value)){
            //     newToken = createToken(TOK_IDENT, list->lexemes[i]->value, list->lexemes[i]->line);
            } 
        }

        if(newToken == NULL){
            printf("unrecognized\n");
        } else {
            addToken(tokenList, newToken);
        }
    }

    return tokenList;
}

void printTokenList(TokenList *list) {
    for(int i = 0; i < list->numTokens; i++) {
        printf("Token %d: %s (line %d)\n", i, list->tokens[i]->lexeme, list->tokens[i]->line);
    }
}