# Currently working on


# Known Issues/WIP
    - Lexer/Tokenizer does not recognize '+' operator for VISIBLE's infinite arity
# Lexer

# Tokenizer
    - Dealing with unrecognized tokens
# Parser
    - Inconsistent rule on when TokenList pointer (cur) is incremented
        - Possible Fix: only increment (cur++) when adding a NODE
    - Does not parse infinite arity boolean statements (ALL OF/ANY OF)
    - Issues with parsing R MAEK typecast statement
    - In some places, no checks if using nextType goes past last element of token array
    - Edit error messages

# Note
    - Remove trace() calls and error prints when done