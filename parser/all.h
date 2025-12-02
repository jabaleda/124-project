#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

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
	TOK_IF_U_SAY_SO,

    TOK_PLUS,       // '+' char
    TOK_BREAK
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

/*
	Enum to represent non-terminal and terminal grammar symbols
	All token types are terminal
*/
typedef enum{
    // Literals
    INTEGER,
    FLOAT,
    STRING,
    BOOLEAN,
    TYPE,

    // Idenfier
    IDENT,

    // Keywords
    // single-word keywords
    HAI,
    KTHXBYE,
    WAZZUP,
    BUHBYE,
    ITZ,
    R,
    NOT,
    DIFFRINT,
    SMOOSH,
    MAEK,
    A,
    VISIBLE,
    GIMMEH,
    OIC,
    WTF,
    OMG,
    OMGWTF,
    UPPIN,
    NERFIN,
    YR,
    TIL,
    WILE,
    AN,
    GTFO,
    MKAY,

    // two-word keywords
    SUM_OF,
    DIFF_OF,
    PRODUKT_OF,
    QUOSHUNT_OF,
    MOD_OF,
    BIGGR_OF,
    SMALLR_OF,
    BOTH_OF,
    EITHER_OF,
    WON_OF,
    ANY_OF,
    ALL_OF,
    BOTH_SAEM,
    O_RLY,
    YA_RLY,
    NO_WAI,
    I_IZ,

    // three-word keywords
    I_HAS_A,
    IS_NOW_A,
    IM_IN_YR,
    IM_OUTTA_YR,
    HOW_IZ_I,

	//
	IF_U_SAY_SO,

    PLUS,
    BREAK,

	// Non-terminals
	PROG,
	VAR_DEC,
	VAR_VAL,
	LITERAL,
	STMT,
	SINGLE_STMT,
	COMPOUND_STMT,
	PRINT,
	INPUT,
	EXPR,
	ARITHMETIC,
	BOOLEAN_STMT,
	FIN_BOOLEAN,
	INF_BOOLEAN,
	INF_BOOLEAN_ARG,
	INF_LAST_ARG,
	CONCATENATION,
	CONCAT_OPERAND,
	COMPARISON,
	RELATIONAL,
	TYPECASTING,
	ASSIGNMENT,
	IF_ELSE,
	BRANCHES_BLOCK,
	SWITCH_CASE,
	CASE_BLOCK,
	DEFAULT_BLOCK,
	LOOP,
	LOOP_VAR_OPERATION,
	BREAK_CONDITION,
	BREAK_CHOICE,
	FUNCTION_DEFINITION,
	PARAMETER,
	FUNCTION_CALL,
	ARGUMENT

} Symbol;

// copied over from parser
// delete later
// para lang maprint ung token type
char* string_ver[] = {
	"INTEGER",
	"FLOAT",
	"STRING",
	"BOOLEAN",
	"TYPE",

	"IDENT",

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
	"WTF",
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

	"SUM_OF",
	"DIFF_OF",
	"PRODUKT_OF",
	"QUOSHUNT_OF",
	"MOD_OF",
	"BIGGR_OF",
	"SMALLR_OF",
	"BOTH_OF",
	"EITHER_OF",
	"WON_OF",
	"ANY_OF",
	"ALL_OF",
	"BOTH_SAEM",
	"O_RLY",
	"YA_RLY",
	"NO_WAI",
	"I_IZ",

	"I_HAS_A",
	"IS_NOW_A",
	"IM_IN_YR",
	"IM_OUTTA_YR",
	"HOW_IZ_I",

	"IF_U_SAY_SO",

    "PLUS",
    "BREAK",

	"PROG",
	"VAR_DEC",
	"VAR_VAL",
	"LITERAL",
	"STMT",
	"SINGLE_STMT",
	"COMPOUND_STMT",
	"PRINT",
	"INPUT",
	"EXPR",
	"ARITHMETIC",
	"BOOLEAN_STMT",
	"FIN_BOOLEAN",
	"INF_BOOLEAN",
	"INF_BOOLEAN_ARG",
	"INF_LAST_ARG",
	"CONCATENATION",
	"CONCAT_OPERAND",
	"COMPARISON",
	"RELATIONAL",
	"TYPECASTING",
	"ASSIGNMENT",
	"IF_ELSE",
	"BRANCHES_BLOCK",
	"SWITCH_CASE",
	"CASE_BLOCK",
	"DEFAULT_BLOCK",
	"LOOP",
	"LOOP_VAR_OPERATION",
	"BREAK_CONDITION",
	"BREAK_CHOICE",
	"FUNCTION_DEFINITION",
	"PARAMETER",
	"FUNCTION_CALL",
	"ARGUMENT"
};

typedef struct {
    char *value;
    unsigned int line;  // for error handling
} Lexeme;

typedef struct {
    unsigned int total_num;
    Lexeme **lexemes;
} LexemeList;



// Lexexe Reading and Building
/* trims leading spaces
*/
void trimLeadingSpaces(char *s) {
    char *ptr = s;
    while (*s == ' ') s++;
    while (*ptr++ = *s++);
}

int isAlphaOrNumOrAlphaNum(char c) {
    if(isalpha(c) != 0 || isdigit(c) != 0) {
        return 1;
    } else {
        return 0;
    }
}

int isDigit(char c) {
    if(isdigit(c) == 0) {
        return 0;
    } else {
        return 1;
    }
}


// Lexeme List
LexemeList *createLexemeList() {
    LexemeList *list = malloc(sizeof(LexemeList));
    list->total_num = 0;
    list->lexemes = NULL;
    return list;
}

Lexeme *createLexeme(char *value, unsigned int line) {
    Lexeme *newlex = malloc(sizeof(Lexeme));
    newlex->value = malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(newlex->value, value);
    newlex->line = line;
    return newlex;
    // then free substr in caller after lexeme creations
}

Lexeme *addLexemeToList(LexemeList *list, Lexeme *lex) {
    // why use void?
    int newTotal = list->total_num + 1;
    void *listStorage = NULL;
    listStorage = realloc(list->lexemes, sizeof(Lexeme*) * newTotal);

    // point to newly alloc storage
    list->lexemes = listStorage;
    list->lexemes[list->total_num] = lex;   // put in last unchanged index
    list->total_num = newTotal;

    return lex;
}

void printLexemeList(LexemeList *list) {
    for(int i = 0; i < list->total_num; i++) {
        printf("Lexeme %d: %s (line %d)\n", i, list->lexemes[i]->value, list->lexemes[i]->line);
    }
}



LexemeList* lex(/*FILE* fp*/) {

    // * ----- File reading test ----- 
    // // File reading
    // FILE *fileptr;
    // fileptr = fopen("test.lol", "r");
    // // file 
    // char storage[100];
    // // ? fgets vs fread?
    // // fgets(storage, 100, fileptr);   // reads one line
    // while(fgets(storage, 100, fileptr)){
    //     printf("%s", storage);
    // }
    // // printf("%s", storage);
    // fclose(fileptr);
    // * ----- End test -----

    // * ----- File Reading into Buffer -----
    FILE *fileptr = NULL;
    char line[1000] = "";
    char *lines_read_buffer = calloc(1, sizeof(char)); // initialize to empty string
    int line_length = 0;
    int lines = 0;
    int size = 0;

    // ? vary filename as needed. hardcoded file naming munaaa d2
    char *cwd = "C:/Users/Julianne/Documents/25-26/CMSC-124/lolcode-testcases-main/lolcode-testcases-main/project-testcases-fixed/01_variables.lol";
    // char *cwd = "sample.lol";

    printf("File path: %s\n", cwd);

    fileptr = fopen(cwd, "r");

    while(fgets(line, 1000, fileptr)){
        lines++;
        trimLeadingSpaces(line);
        line_length = strlen(line) + 1;
        size += line_length;
        // printf("\nLine length: %d\n", line_length);
        // reallocate memomy for buffer
        lines_read_buffer = realloc(lines_read_buffer, sizeof(char) * size + 1);
        // join new line to buffer
        strcat(lines_read_buffer, line);
        // printf("Current buffer size: %d\n", size);
        // printf("%s", lines_read_buffer);
    }

    // add null terminater to end of buffer
    int len = strlen(lines_read_buffer);
    lines_read_buffer[len] = '\0'; // null-terminate buffer for last eme 
    printf("%c", lines_read_buffer[len-1]); // what this for? !!!

    printf("\nTotal lines read: %d\n", lines);
    // printf("Total chars read: %d\n", size);
    printf("\n%s\n", lines_read_buffer);    // Prints all lines in source file !!!
    
    fclose(fileptr);
    // * ----- End File Reading into Buffer -----

    // * ----- Lexeme Buildeing -----
    int iter = 0, lines_read = 1;
    int chars_read;
    char *substrword;
    char *start_of_line = lines_read_buffer;
    char *current_char = lines_read_buffer;
    char *lexeme_end = lines_read_buffer;

    LexemeList *lexemeList = createLexemeList();

    // scan over buffer - Lexeme Builder
    // iter - for accessing string index, checking of loop termination
    // start - ptr to buffer start - unused as of now
    // current_char - ptr to current char being checked
    // lexeme_end - ptr to end of current lexeme being built

    // TODO?: Maybe refactor if series to swtich case

    while(iter < len){
        // note of chars read. for iterating iter. reset to 0 every loop turn. only increment when lexeme_end is incremented
        chars_read = 0;
        // check if whitespace
        if(*current_char == ' ') {
            // incremement iter + current read in this loop turn
            lexeme_end++;
            current_char = lexeme_end;
            chars_read++;
            iter += chars_read;
            // printf("Chars read this turn: %d\n", chars_read);
            continue;
        }
        // check char if alphabetic or digit
        if(isAlphaOrNumOrAlphaNum(*current_char) == 1) {
            // move ptr end until whitespace, increment chars read
            while(*lexeme_end != ' ' && *lexeme_end != '\n' && *lexeme_end != '\0') {
                lexeme_end++;
                chars_read++;
            }
            // get substr
            char substr[chars_read+1];
            strncpy(substr, current_char, chars_read);
            substr[chars_read] = '\0'; // null-terminate the substring

            substrword = substr;
            // check if keyword for comment -> move ptr lexeme_end to line end, increment chars read
            if(strcmp(substr, "BTW") == 0) {
                while(*lexeme_end != '\n' && *lexeme_end != '\0') {
                    lexeme_end++;
                    chars_read++;
                }
                // check if current char is same as start of line. T -> consume \n; else -> do not consume \n, inline comment
                if(current_char == start_of_line) {
                    // DO NOT CONSUME \n for inline comments
                    lexeme_end++;
                    chars_read++;
                    lines_read++;   // increment since \n is consumed, to keep line numbers consistent
                    // move line start to new line
                    start_of_line = lexeme_end;
                }
                
            } else if(strcmp(substr, "OBTW") == 0) {
                // toggle multiline comment flag -> 1, keep reading but not recording until flag is reset by T
                // make sure obtw is found at start of line, else throw error and exits
                if(current_char != start_of_line){
                    printf("Error: Comment at line %d is not allowed.", lines_read);
                    exit(-1);
                } else {
                    int temp_counter = 0;
                    while(1){
                        temp_counter = 0;
                        while(*lexeme_end != 'T' && *lexeme_end != '\n' && *lexeme_end != '\0') {
                            lexeme_end++;
                            chars_read++;
                        }
                        if(*lexeme_end == '\n'){
                            lines_read++;
                            lexeme_end++;
                            chars_read++;
                            current_char = lexeme_end;
                            // move line start to new line
                            start_of_line = lexeme_end;
                        }

                        char *temp = lexeme_end;
                        // check if TLDR appears
                        while(*temp != ' ' && *temp != '\n' && *temp != '\0') {
                            temp++;
                            temp_counter++;
                        }
                        if(temp_counter == 4){
                            char temp_str[temp_counter+1];
                            strncpy(temp_str, lexeme_end, temp_counter);
                            temp_str[temp_counter] = '\0';
                            
                            if(strcmp(temp_str, "TLDR") == 0){
                                lexeme_end = temp;
                                if(*lexeme_end == '\n'){
                                    lexeme_end++;
                                    lines_read++;
                                    chars_read+=temp_counter;
                                    current_char = lexeme_end;
                                    // move line start to new line
                                    start_of_line = lexeme_end;
                                    break;
                                } else {
                                    printf("Error: Comment at line %d is not allowed.", lines_read);
                                    exit(-1);
                                }
                                
                            }
                        }
                    }
                }
                
                
                

            } else {
                // // else add to lexeme list
                printf("Alphabetic/Digit/AlphaNum lexeme: %s\n", substr);
                // create lexeme and add to list
                Lexeme *newLex = createLexeme(substr, lines_read);
                addLexemeToList(lexemeList, newLex);
                // test
                // printLexemeList(lexemeList);

            }
            // update current char to lexeme end
            current_char = lexeme_end;
            // incremement iter + chars read in this loop turn
            iter += chars_read;
            // printf("Chars read this turn: %d\n", chars_read);
            continue;
        }
                
        // check char if symbol "-"
        if(*current_char == '-') {
            // check next if digits
            if(isDigit(*(current_char+1)) == 1) {
                // attach to digit lexeme if succeeding char is digit
                // move ptr end until whitespace, increment chars read
                while(*lexeme_end != ' '  && *lexeme_end != '\n') {
                    lexeme_end++;
                    chars_read++;
                }
                // get substr
                char substr[chars_read+1];
                strncpy(substr, current_char, chars_read);
                substr[chars_read] = '\0'; // null-terminate the substring
                
                printf("Signed digit lexeme: %s\n", substr);
                // add to lexeme list
                // create lexeme and add to list
                Lexeme *newLex = createLexeme(substr, lines_read);
                addLexemeToList(lexemeList, newLex);
            } else {
                // error. just "-" is invalid lexeme
                printf("Error: Invalid lexeme '-'\n");
                // break;
            }
            // update current char to lexeme end
            current_char = lexeme_end;
            // incremement iter + chars read in this loop turn
            iter += chars_read;
            // printf("Chars read this turn: %d\n", chars_read);
            continue;
        }
        
        // check char if "
        if(*current_char == '"') {
            // move to next first after " to start reading literal
            lexeme_end++;
            chars_read++;
            // move ptr end until next ", increment chars read
            while(*lexeme_end != '"') {
                lexeme_end++;
                chars_read++;
            }
            lexeme_end++;
            chars_read++;
            // get substr
            char substr[chars_read+1];
            strncpy(substr, current_char, chars_read);
            substr[chars_read] = '\0'; // null-terminate the substring
            printf("String literal lexeme: %s\n", substr);
            // add to lexeme list
            // create lexeme and add to list
            Lexeme *newLex = createLexeme(substr, lines_read);
            addLexemeToList(lexemeList, newLex);
            // update current char to lexeme end
            current_char = lexeme_end;
            // incremement iter + chars read in this loop turn
            iter += chars_read;
            // printf("Chars read this turn: %d\n", chars_read);
            continue;
        }

        if(*current_char == '+') {
            // '+' char for concatenating in VISIBLE statements
            lexeme_end++;
            chars_read++;
            // get substr
            char substr[2] = "+";
            substr[1] = '\0';
            printf("Print concat lexeme: %s\n", substr);
            Lexeme *newLex = createLexeme(substr, lines_read);
            addLexemeToList(lexemeList, newLex);
            // add to lexeme list
            current_char = lexeme_end;
            // incremement iter + chars read in this loop turn
            iter += chars_read;
            // printf("Chars read this turn: %d\n", chars_read);
            continue;
        }
            
        // check if '\\' backslash
        if(*current_char == '\n') {
            // newline character
            lexeme_end++;
            chars_read++;
            // ! TODO: check prev. substr if KTHNXBYE
            if(strcmp(substrword, "KTHXBYE") == 0) {
                // set iter to len to terminate loop after this turn
                iter = len;
            }

            // get substr
            char substr1[2] = "\n";
            substr1[1] = '\0';
            printf("Newline lexeme: %s\n", substr1);
            Lexeme *newLex = createLexeme(substr1, lines_read);
            addLexemeToList(lexemeList, newLex);
            // increment lines read count for every newline encountered
            lines_read++;
            // add to lexeme list
            current_char = lexeme_end;
            // move line start to new line
            start_of_line = lexeme_end;
            // incremement iter + chars read in this loop turn
            iter += chars_read;
            // printf("Chars read this turn: %d\n", chars_read);
            continue;
        }
            
        // else throw error?

    } // while end


    printLexemeList(lexemeList);

    // tokenize(lexemeList);

    /* 
    TODO: Build Lexeme List
    TODO: Tokenize Lexemes
        TODO: Create Structs for Token Types, Funcs, etc.
    */
    
    return lexemeList;
}

typedef struct {
    TokenType type;
    int tok_id;
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
    newToken->tok_id = -1;
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
    temp = realloc(list->tokens, sizeof(Token*) * (list->numTokens + 1));
    if(temp != NULL){
        list->tokens = temp;
        list->tokens[list->numTokens] = newToken;
    } else {
        printf("Token list realloc error\n");
        exit(1);
    }
    newToken->tok_id = list->numTokens;
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

int isString(char* lex){
    int c = 0;
    if(lex[c] == '"'){
        c++;
        while(isprint(lex[c]) != 0 && c <= strlen(lex)){
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
        if(*sptr == *cptr && *sptr == '\0'){ // successfully parsed a keyword
            len++;
            *pos += len-1; // move lexemeList iterator
            return i+6;  // return corresponding TokenType
        }
    }
    return -1; // none matched
}

void printTokenList(TokenList *list) {
    for(int i = 0; i < list->numTokens; i++) {
        printf("Line: %-4d Token %-3d Type: %-12s Lexeme[%d]: %-20s\n", list->tokens[i]->line, i+1, string_ver[list->tokens[i]->type], i, list->tokens[i]->lexeme);
    }
}

TokenList* tokenize(LexemeList* list){
    TokenList* tokenList = createTokenList();
    printf("\n==========TOKENIZER==========\n");
    for(int i = 0; i < list->total_num; i++){
        // printf("On lexeme number %d\n", i);
        Token* newToken = NULL;
        if(isInteger(list->lexemes[i]->value)){
            newToken = createToken(TOK_INTEGER, list->lexemes[i]->value, list->lexemes[i]->line); 
        } else if(isFloat(list->lexemes[i]->value)){
            newToken = createToken(TOK_FLOAT, list->lexemes[i]->value, list->lexemes[i]->line); 
        } else if(isString(list->lexemes[i]->value)){
            newToken = createToken(TOK_STRING, list->lexemes[i]->value, list->lexemes[i]->line);
        } else if(strcmp(list->lexemes[i]->value, "WIN") == 0){
            newToken = createToken(TOK_BOOLEAN, "WIN", list->lexemes[i]->line); 
        } else if(strcmp(list->lexemes[i]->value, "FAIL") == 0){
            newToken = createToken(TOK_BOOLEAN, "FAIL", list->lexemes[i]->line); 
        // Type literals
        } else if(strcmp(list->lexemes[i]->value, "NUMBR") == 0){
            newToken = createToken(TOK_TYPE, "NUMBR", list->lexemes[i]->line);
        } else if(strcmp(list->lexemes[i]->value, "NUMBAR") == 0){
            newToken = createToken(TOK_TYPE, "NUMBAR", list->lexemes[i]->line);
        } else if(strcmp(list->lexemes[i]->value, "YARN") == 0){
            newToken = createToken(TOK_TYPE, "YARN", list->lexemes[i]->line);
        } else if(strcmp(list->lexemes[i]->value, "TROOF") == 0){
            newToken = createToken(TOK_TYPE, "TROOF", list->lexemes[i]->line);
        } else if(strcmp(list->lexemes[i]->value, "+\0") == 0) {   // newline
            newToken = createToken(TOK_PLUS, "+", list->lexemes[i]->line);
        } else if(strcmp(list->lexemes[i]->value, "\n\0") == 0) {   // newline
            newToken = createToken(TOK_BREAK, "\\n", list->lexemes[i]->line);
        } else {
            // Check if keyword
            int type = isKeyword(list, &i);
            if(type != -1){ // if keyword matched
                newToken = createToken(type, list->lexemes[i]->value, list->lexemes[i]->line);
            } else if(isIdentifier(list->lexemes[i]->value)){
                newToken = createToken(TOK_IDENT, list->lexemes[i]->value, list->lexemes[i]->line);
            } 
        }

        if(newToken == NULL){
            printf("unrecognized\n");
        } else {
            addToken(tokenList, newToken);
        }
    }
    printf("\n========TOKENS GENERATED========\n\n");
    printTokenList(tokenList);
    return tokenList;
}



typedef struct ast_node{
	int node_id;
	TokenType type;
	union {
		float num_val; // for numeric value ng literal?
		char* id_name; 	// for ident
		char* string_val;	// for string literal
        int bool_val;   // for boolean literal
        struct ast_node** children; // for non-terminal's/root child nodes
	};
	int numChildren;
} ast_node;

// states forward dec
ast_node* program(TokenList* tokenList, int numTokens);
ast_node* stmt();
ast_node* var_val();
ast_node* var_dec();
ast_node* single_stmt();
ast_node* print();
ast_node* input();
ast_node* assignment();
ast_node* function_call();
ast_node* expr();
ast_node* arithmetic();
ast_node* boolean();
ast_node* comparison();
ast_node* relational();
ast_node* concatenation();
ast_node* concat_operand();
ast_node* typecasting();
ast_node* compound_stmt();
ast_node* switch_case();
ast_node* case_block();
ast_node* default_block();
ast_node* loop();
ast_node* function_definition();
ast_node* parameter();


// not yet implemented
ast_node* if_else();
ast_node* branches_block();

/*////////////////////////////////////////////////////////////////////////*/
// void print_ast(const ast_node *node, const char *prefix, int is_last) {
//     if (!node) return;

//     // Print prefix + branch characters
//     printf("%s%s%s[%d]\n",
//         prefix,
//         is_last ? "└── " : "├── ",
//         string_ver[node->type],
//         node->node_id
//     );

//     // Build next prefix for children
//     char new_prefix[512];
//     snprintf(
//         new_prefix,
//         sizeof(new_prefix),
//         "%s%s",
//         prefix,
//         is_last ? "    " : "│   "
//     );

//     // Recurse for each child
//     assert(node->numChildren >= 0 && node->numChildren < 1000);
//     for (int i = 0; i < node->numChildren; i++) {
//         print_ast(node->children[i], new_prefix, i == node->numChildren - 1);
//     }
// }

// void print_ast_root(const ast_node* root) {
//     if (!root) return;
//     printf("%s[%d]\n", string_ver[root->type], root->node_id);

//     for (int i = 0; i < root->numChildren; i++) {
//         print_ast(root->children[i], "", i == root->numChildren - 1);
//     }
// }

#include <stdio.h>

void print_ast_f(const ast_node *node, const char *prefix, int is_last, FILE *out) {
    if (!node) return;

    // Print prefix + branch characters
    fprintf(out, "%s%s%s[%d]\n",
        prefix,
        is_last ? "└── " : "├── ",
        string_ver[node->type],
        node->node_id
    );

    // Build next prefix for children
    char new_prefix[512];
    snprintf(
        new_prefix,
        sizeof(new_prefix),
        "%s%s",
        prefix,
        is_last ? "    " : "│   "
    );

    // Recurse for each child
    assert(node->numChildren >= 0 && node->numChildren < 1000);
    for (int i = 0; i < node->numChildren; i++) {
        print_ast_f(node->children[i], new_prefix, i == node->numChildren - 1, out);
    }
}

void print_ast_root_f(const ast_node *root, FILE *out) {
    if (!root) return;

    fprintf(out, "%s[%d]\n", string_ver[root->type], root->node_id);

    for (int i = 0; i < root->numChildren; i++) {
        print_ast_f(root->children[i], "", i == root->numChildren - 1, out);
    }
}


/*Semantic stuff*/

typedef enum{
    SYM_VAR,
    SYM_FUN,
    SYM_PARAM
}  SymbolType;

char* symType_strings[] = {
    "Variable",
    "Function",
    "Parameter"
};

/* Data type of variable identifier*/
typedef enum{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_TYPE,
    TYPE_NOOB   // for uninitialized
} VarType;

char* varType_strings[] = {
    "Integer",
    "Float",
    "String",
    "Boolean",
    "Type",
    "Noob"
};

typedef enum{
    GLOBAL_SCOPE,
    LOCAL_SCOPE
} SymbolScope;

typedef struct Entry{
    
    // int line;
    char* id;                   // string identifier/name
    SymbolType symType;            // is the symbol a function or var id
    VarType varType;
    // SymbolScope scope;
    union {                     // value of var symbols
        int intVal;
        float floatVal;
        char* stringVal;
    } value;
    // struct table_entry *prev;
    // struct table_entry *next;

} Entry;

typedef struct SymbolTable{
    int numEntries;
    Entry** entries;
} SymbolTable;


// * ----------
union Data {
    int int_Result;
    float flt_Result;
    char *string_Result;
    // int bool_Result;
};

// 0 - arith, comparison -> int or float
// 1 - concat -> string
// 2 - boolean
// Data type used to pass around result of evaluated expressions
typedef struct {
    int expr_source_type;
    int float_flag;
    union Data eval_data;
} EvalData;

EvalData *createEvalData();
EvalData *subtree_walk(ast_node *node);

// * ----------


void print_table(SymbolTable* table);
Entry* create_var_entry_int(char *id, int val);
Entry* create_var_entry_float(char *id, float val);
Entry* create_var_entry_string(char *id, char *val);
Entry* create_var_entry_bool(char *id, int val);
Entry* create_var_entry_type(char *id, char *val);
Entry* create_var_entry_no_type(char *id);
Entry* create_function_entry(char *id);
Entry* create_param_entry(char *id);
int setVarEntryValInt(char* id, int val);
int setVarEntryValFloat(char* id, float val);
int setVarEntryValString(char* id, char* val);
int setVarEntryValBool(char* id, int val);
int setVarEntryValType(char* id, char* val);
void addSymTableEntry(SymbolTable* table, Entry* e);
SymbolTable* initSymbolTable();