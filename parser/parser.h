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

} TokenType;


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


// REMOVE LATER!!!
typedef struct {
    TokenType type;
    char* lexeme;
                    // add semantic value for literals
    int line;
} Token;

typedef struct ast_node{
	TokenType type;
	union {
		float num_val; // for numeric value ng literal?
		char* id_name; 	// for ident
		char* string_val;	// for string literal
        struct ast_node** children; // for non-terminal's/root child nodes
	};
	int numChildren;
} ast_node;

// states
ast_node* program();
ast_node* stmt();
ast_node* single_stmt();
ast_node* compound_stmt();
ast_node* expr();
ast_node* var_val();
ast_node* var_dec();
// ast_node* ;
// ast_node* ;
// ast_node* ;
// ast_node* ;
// ast_node* ;
// ast_node* ;
// ast_node* ;