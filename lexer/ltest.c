#include "lexer.h"
#include "token.h"

int main(){
    LexemeList *lexList = lex();
    TokenList *tokList = tokenize(lexList);
    printTokenList(tokList);
}