# Currently working on
    - Fixing typecast parsing

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