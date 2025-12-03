#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
// #include "token.h"


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
    // then free substr in caller after lexeme creation
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
    // // file contents
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
    // char *cwd = "C:/Users/Julianne/Documents/25-26/CMSC-124/project/test.lol";
    char *cwd = "sample.lol";

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
    int iter = 0, lines_read = 0;
    int chars_read;
    char *start = lines_read_buffer;
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

            // check if keyword for comment -> move ptr lexeme_end to line end, increment chars read
            if(strcmp(substr, "BTW") == 0) {
                while(*lexeme_end != '\n' && *lexeme_end != '\0') {
                    lexeme_end++;
                    chars_read++;
                }
                // also consume \n
                lexeme_end++;
                chars_read++;
                lines_read++;   // increment since \n is consumed
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
            
        // check if '\\' backslash
        if(*current_char == '\n') {
            // newline character
            lexeme_end++;
            chars_read++;
            // get substr
            char substr[2] = "\n";
            substr[2] = '\0';
            printf("Newline lexeme: %s\n", substr);
            // increment lines read count for every newline encountered
            lines_read++;
            // add to lexeme list
            current_char = lexeme_end;
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