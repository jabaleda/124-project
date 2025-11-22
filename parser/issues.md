# Currently working on/things to work on
    - if-else implementation
    - infinite boolean statements
    - semantic analysis, symbol tables
        - the token itself holds a value
    - running the actual thing (pls figure out especially for compound statement)
    - organize files, makefile (atm all together in one .c file, one .h file)

# Known Issues/WIP
    - Lexer/Tokenizer does not recognize '+' operator for VISIBLE's infinite arity (wrong grammar spec)

# Lexer
    - Dealing with unrecognized tokens if any (e.g. encountered random symbol like @#$%^)

# Tokenizer
    - Dealing with unrecognized tokens if any (e.g. encountered random symbol like @#$%^)

# Parser
    - Does not parse infinite arity boolean statements (ALL OF/ANY OF)
    - Issues with parsing R MAEK typecast statement
    - In some places, no checks if using nextType goes past last element of token array
    - Edit error messages

# Notes
    - Remove trace() calls and error prints when done
    - current implementation beginning from lexer throws away newlines, could be a problem?
        so after a statement, what parser currently does after parsing a statement is:
			- checks if there are more statements
			- checks for KTHXBYE (assumes no more statements then error pag may token pa past KTHXBYE)

# Notes on GRAMMAR
	- some production rules seem unnecessary
		ex: <break_choice> expanding just to TIL or WILE were implemented without the abstraction
	- change to var_dec
	    original: var_dec ::= I HAS A varident | I HAS A varident ITZ <var_value>
        added the ff:         I HAS A varident <var_dec> | I HAS A varident ITZ <var_value> <var_dec>
        to support further var_decs