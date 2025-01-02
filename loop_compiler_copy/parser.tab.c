/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include "AST.h"
#include "symbolTable.h"
#include "semantic.h"
#include "codeGenerator.h"


#define TABLE_SIZE 100

extern int yylex();        // Declare yylex, the lexer function
extern int yyparse(void);  // Declare yyparse, the parser function
extern FILE* yyin;         // Declare yyin, the file pointer for the input file
extern int yylineno;       // Declare yylineno, the line number counter from Bison
extern TAC* tacHead;       // Declare the head of the linked list of TAC entries
extern struct ASTNode* root;

void yyerror(const char* s);

ASTNode* root = NULL;
SymbolTable* symTab = NULL;  // Initialize the symbol table pointer
Symbol* symbol = NULL;
int parseErrorFlag = 0;      // Flag to indicate if an error occurred during parsing

// Global variable to hold the current function name
char* functionBeingParsed = NULL;

// You can keep function prototypes and helper functions here


#line 106 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TYPE = 3,                       /* TYPE  */
  YYSYMBOL_ID = 4,                         /* ID  */
  YYSYMBOL_SEMICOLON = 5,                  /* SEMICOLON  */
  YYSYMBOL_COMMA = 6,                      /* COMMA  */
  YYSYMBOL_EQ = 7,                         /* EQ  */
  YYSYMBOL_PLUS = 8,                       /* PLUS  */
  YYSYMBOL_MINUS = 9,                      /* MINUS  */
  YYSYMBOL_MULTIPLY = 10,                  /* MULTIPLY  */
  YYSYMBOL_DIVIDE = 11,                    /* DIVIDE  */
  YYSYMBOL_GT = 12,                        /* GT  */
  YYSYMBOL_LT = 13,                        /* LT  */
  YYSYMBOL_GEQ = 14,                       /* GEQ  */
  YYSYMBOL_LEQ = 15,                       /* LEQ  */
  YYSYMBOL_EQEQ = 16,                      /* EQEQ  */
  YYSYMBOL_NEQ = 17,                       /* NEQ  */
  YYSYMBOL_INTEGER = 18,                   /* INTEGER  */
  YYSYMBOL_FLOAT = 19,                     /* FLOAT  */
  YYSYMBOL_WRITE = 20,                     /* WRITE  */
  YYSYMBOL_LPAREN = 21,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 22,                    /* RPAREN  */
  YYSYMBOL_LBRACKET = 23,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 24,                  /* RBRACKET  */
  YYSYMBOL_FUNCTION = 25,                  /* FUNCTION  */
  YYSYMBOL_RETURN = 26,                    /* RETURN  */
  YYSYMBOL_LBRACE = 27,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 28,                    /* RBRACE  */
  YYSYMBOL_IF = 29,                        /* IF  */
  YYSYMBOL_ELSE = 30,                      /* ELSE  */
  YYSYMBOL_WHILE = 31,                     /* WHILE  */
  YYSYMBOL_UNRECOGNIZED = 32,              /* UNRECOGNIZED  */
  YYSYMBOL_YYACCEPT = 33,                  /* $accept  */
  YYSYMBOL_Program = 34,                   /* Program  */
  YYSYMBOL_FunctionDefList = 35,           /* FunctionDefList  */
  YYSYMBOL_FunctionDef = 36,               /* FunctionDef  */
  YYSYMBOL_FunctionHeader = 37,            /* FunctionHeader  */
  YYSYMBOL_FunctionFooter = 38,            /* FunctionFooter  */
  YYSYMBOL_ParamList = 39,                 /* ParamList  */
  YYSYMBOL_ParamListTail = 40,             /* ParamListTail  */
  YYSYMBOL_Param = 41,                     /* Param  */
  YYSYMBOL_ArgList = 42,                   /* ArgList  */
  YYSYMBOL_ArgListTail = 43,               /* ArgListTail  */
  YYSYMBOL_VarDeclList = 44,               /* VarDeclList  */
  YYSYMBOL_VarDecl = 45,                   /* VarDecl  */
  YYSYMBOL_StmtList = 46,                  /* StmtList  */
  YYSYMBOL_Stmt = 47,                      /* Stmt  */
  YYSYMBOL_ArrayAccess = 48,               /* ArrayAccess  */
  YYSYMBOL_Expr = 49                       /* Expr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 45 "parser.y"

    FunctionHeaderInfo* createFunctionHeader(const char* name) {
        FunctionHeaderInfo* header = (FunctionHeaderInfo*)malloc(sizeof(FunctionHeaderInfo));
        if (header == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for FunctionHeaderInfo.\n");
            exit(EXIT_FAILURE);
        }
        header->name = strdup(name);
        header->paramList = NULL; // Initially no parameters
        return header;
    }

#line 203 "parser.tab.c"

#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   198

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  33
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  17
/* YYNRULES -- Number of rules.  */
#define YYNRULES  48
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  110

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   287


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   100,   100,   112,   115,   125,   158,   190,   200,   203,
     217,   218,   227,   235,   238,   254,   255,   264,   267,   277,
     303,   323,   326,   336,   365,   409,   430,   443,   450,   457,
     461,   466,   476,   494,   519,   536,   553,   570,   574,   589,
     604,   619,   634,   649,   664,   685,   693,   701,   704
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TYPE", "ID",
  "SEMICOLON", "COMMA", "EQ", "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "GT",
  "LT", "GEQ", "LEQ", "EQEQ", "NEQ", "INTEGER", "FLOAT", "WRITE", "LPAREN",
  "RPAREN", "LBRACKET", "RBRACKET", "FUNCTION", "RETURN", "LBRACE",
  "RBRACE", "IF", "ELSE", "WHILE", "UNRECOGNIZED", "$accept", "Program",
  "FunctionDefList", "FunctionDef", "FunctionHeader", "FunctionFooter",
  "ParamList", "ParamListTail", "Param", "ArgList", "ArgListTail",
  "VarDeclList", "VarDecl", "StmtList", "Stmt", "ArrayAccess", "Expr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-90)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      -2,    14,    27,    26,    -2,    26,    11,   -90,    32,     8,
      26,   -90,     8,    35,    -3,     1,    22,    22,     8,    21,
      25,   -90,     8,    40,   -90,    41,    66,    49,    79,   -90,
      54,    22,    22,    22,    -7,   -90,   -90,    22,   -90,   120,
     133,    45,    22,    22,   -90,    22,   -90,   -90,   -90,    60,
      35,   -90,    64,   146,    68,   171,    50,    22,    67,   -90,
      22,    22,    22,    22,    22,    22,    22,    22,    22,    22,
     -90,   -90,    90,   105,   159,   -90,    79,    86,   -90,    87,
      22,   -90,   -90,    72,   -90,    34,    34,   -90,   -90,   181,
     181,   181,   181,   181,   181,     8,    69,   -90,   -90,   -90,
     -90,   171,   -90,    78,     8,   -90,     8,    81,   -90,   -90
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     0,     0,    17,     3,    17,     0,     1,     0,    21,
      17,     4,    21,     8,     0,     0,     0,     0,    21,     0,
       0,     2,    21,     0,    18,     0,     0,     0,    10,    19,
       0,     0,    13,     0,    44,    45,    46,     0,    47,     0,
       0,     0,     0,     0,    22,     0,     7,     5,    12,     0,
       0,     9,     0,     0,     0,    15,     0,    13,     0,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    29,     0,     0,     0,     6,    10,     0,    23,     0,
       0,    14,    32,     0,    37,    33,    34,    35,    36,    38,
      39,    40,    41,    42,    43,     0,     0,    24,    11,    20,
      26,    15,    48,    28,    21,    16,     0,     0,    27,    31
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -90,   -90,   106,   -90,   -90,   -90,   -90,    47,    61,    82,
      23,     5,   -90,   -11,   -89,    -9,   -12
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,     3,     4,     5,    47,    27,    51,    28,    54,
      81,     9,    10,    21,    22,    38,    55
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      23,    25,    29,    23,    39,    40,   103,    41,    31,    23,
      12,    44,    15,    23,    57,    24,    33,   108,     6,    53,
      30,    56,    32,     1,    33,    58,    34,     7,    16,     8,
      72,    73,    13,    74,    17,    18,    14,    19,    26,    20,
      35,    36,    42,    37,    62,    63,    43,    45,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,   101,    46,
      48,    49,    52,    71,    82,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    50,    23,    75,    77,    84,
      79,    99,   100,   107,   102,    23,   104,    23,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,   106,   109,
      11,    76,    95,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    98,   105,    59,     0,    96,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    83,
       0,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    78,     0,     0,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    97,     0,     0,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    80,     0,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    60,
      61,    62,    63,    -1,    -1,    -1,    -1,    -1,    -1
};

static const yytype_int8 yycheck[] =
{
       9,    12,     5,    12,    16,    17,    95,    18,     7,    18,
       5,    22,     4,    22,    21,    10,    23,   106,     4,    31,
      23,    33,    21,    25,    23,    37,     4,     0,    20,     3,
      42,    43,    21,    45,    26,    27,     4,    29,     3,    31,
      18,    19,    21,    21,    10,    11,    21,     7,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    80,    28,
       4,    22,    18,    28,    24,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,     6,    95,    27,    24,    22,
      22,     5,     5,   104,    22,   104,    27,   106,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    30,    28,
       4,    50,    22,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    76,   101,     5,    -1,    22,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,     5,    57,
      -1,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,     5,    -1,    -1,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,     5,    -1,    -1,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,     6,    -1,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    25,    34,    35,    36,    37,     4,     0,     3,    44,
      45,    35,    44,    21,     4,     4,    20,    26,    27,    29,
      31,    46,    47,    48,    44,    46,     3,    39,    41,     5,
      23,     7,    21,    23,     4,    18,    19,    21,    48,    49,
      49,    46,    21,    21,    46,     7,    28,    38,     4,    22,
       6,    40,    18,    49,    42,    49,    49,    21,    49,     5,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
       5,    28,    49,    49,    49,    27,    41,    24,     5,    22,
       6,    43,    24,    42,    22,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    22,    22,     5,    40,     5,
       5,    49,    22,    47,    27,    43,    30,    46,    47,    28
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    33,    34,    35,    35,    36,    37,    38,    39,    39,
      40,    40,    41,    42,    42,    43,    43,    44,    44,    45,
      45,    46,    46,    47,    47,    47,    47,    47,    47,    47,
      47,    47,    48,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    49
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     0,     2,     4,     6,     1,     0,     2,
       0,     3,     2,     0,     2,     0,     3,     0,     2,     3,
       6,     0,     2,     4,     4,     3,     5,     7,     5,     3,
       3,     7,     4,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     1,     1,     1,     4
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* Program: FunctionDefList VarDeclList StmtList  */
#line 100 "parser.y"
                                         {
        (yyval.ast) = createNode(NodeType_Program);
        (yyval.ast)->program.functionDefList = (yyvsp[-2].ast);
        (yyval.ast)->program.varDeclList = (yyvsp[-1].ast);
        (yyval.ast)->program.stmtList = (yyvsp[0].ast);
        root = (yyval.ast); // Set the global root pointer to the root of the AST

        printf("\n[INFO] Program parsed successfully with functions.\n");
    }
#line 1242 "parser.tab.c"
    break;

  case 3: /* FunctionDefList: %empty  */
#line 112 "parser.y"
                {
        (yyval.ast) = NULL; // No functions defined
    }
#line 1250 "parser.tab.c"
    break;

  case 4: /* FunctionDefList: FunctionDef FunctionDefList  */
#line 115 "parser.y"
                                  {
        (yyval.ast) = createNode(NodeType_FunctionDefList);
        (yyval.ast)->functionDefList.functionDef = (yyvsp[-1].ast);
        (yyval.ast)->functionDefList.functionDefList = (yyvsp[0].ast);

        printf("[INFO] Function definition list recognized.\n");
    }
#line 1262 "parser.tab.c"
    break;

  case 5: /* FunctionDef: FunctionHeader VarDeclList StmtList FunctionFooter  */
#line 125 "parser.y"
                                                       {
        (yyval.ast) = createNode(NodeType_FunctionDef);
        (yyval.ast)->functionDef.name = strdup((yyvsp[-3].funcHeader)->name);
        (yyval.ast)->functionDef.varDeclList = (yyvsp[-2].ast);
        (yyval.ast)->functionDef.stmtList = (yyvsp[-1].ast);

        // Add function to symbol table
        addFunctionSymbol(symTab, (yyvsp[-3].funcHeader)->name);

        // Merge paramList (if any) into varDeclList
        if ((yyvsp[-3].funcHeader)->paramList != NULL) {
            if ((yyvsp[-2].ast) == NULL) {
                (yyval.ast)->functionDef.varDeclList = (yyvsp[-3].funcHeader)->paramList;
            } else {
                // Append $2 at the end of $1->paramList
                ASTNode* paramTail = (yyvsp[-3].funcHeader)->paramList;
                while (paramTail->varDeclList.varDeclList != NULL) {
                    paramTail = paramTail->varDeclList.varDeclList;
                }
                paramTail->varDeclList.varDeclList = (yyvsp[-2].ast);
                (yyval.ast)->functionDef.varDeclList = (yyvsp[-3].funcHeader)->paramList;
            }
        }

        printf("[INFO] Function defined: %s\n", (yyvsp[-3].funcHeader)->name);

        // Clean up
        free((yyvsp[-3].funcHeader)->name);
        free((yyvsp[-3].funcHeader));
    }
#line 1297 "parser.tab.c"
    break;

  case 6: /* FunctionHeader: FUNCTION ID LPAREN ParamList RPAREN LBRACE  */
#line 158 "parser.y"
                                               {
        (yyval.funcHeader) = createFunctionHeader((yyvsp[-4].string));

        // Set the global functionBeingParsed
        functionBeingParsed = (yyval.funcHeader)->name;

        // Enter a new scope for the function
        enterScope();

        // Add parameters to symbol table
        ASTNode* paramNode = (yyvsp[-2].ast); 
        (yyval.funcHeader)->paramList = (yyvsp[-2].ast); // Store paramList in FunctionHeaderInfo
        while (paramNode != NULL && paramNode->type == NodeType_VarDeclList) {
            ASTNode* singleParam = paramNode->varDeclList.varDecl;
            if (singleParam && singleParam->type == NodeType_VarDecl) {
                SymbolValue value;
                SymbolType st = TYPE_UNKNOWN;
                if (strcmp(singleParam->varDecl.varType, "int") == 0) {
                    st = TYPE_INT;
                    value.intValue = INT_MIN;
                } else if (strcmp(singleParam->varDecl.varType, "float") == 0) {
                    st = TYPE_FLOAT;
                    value.floatValue = FLT_MIN;
                }
                addSymbol(currentScope, singleParam->varDecl.varName, st, value);
            }
            paramNode = paramNode->varDeclList.varDeclList;
        }
    }
#line 1331 "parser.tab.c"
    break;

  case 7: /* FunctionFooter: RBRACE  */
#line 190 "parser.y"
           {
        // Exit the function's scope
        exitScope();

        // Reset the global functionBeingParsed
        functionBeingParsed = NULL;
    }
#line 1343 "parser.tab.c"
    break;

  case 8: /* ParamList: %empty  */
#line 200 "parser.y"
                {
        (yyval.ast) = NULL; // No params
    }
#line 1351 "parser.tab.c"
    break;

  case 9: /* ParamList: Param ParamListTail  */
#line 203 "parser.y"
                          {
        if ((yyvsp[0].ast) == NULL) {
            (yyval.ast) = createNode(NodeType_VarDeclList);
            (yyval.ast)->varDeclList.varDecl = (yyvsp[-1].ast); 
            (yyval.ast)->varDeclList.varDeclList = NULL;
        } else {
            (yyval.ast) = createNode(NodeType_VarDeclList);
            (yyval.ast)->varDeclList.varDecl = (yyvsp[-1].ast);
            (yyval.ast)->varDeclList.varDeclList = (yyvsp[0].ast);
        }
    }
#line 1367 "parser.tab.c"
    break;

  case 10: /* ParamListTail: %empty  */
#line 217 "parser.y"
                { (yyval.ast) = NULL; }
#line 1373 "parser.tab.c"
    break;

  case 11: /* ParamListTail: COMMA Param ParamListTail  */
#line 218 "parser.y"
                                {
        ASTNode* newList = createNode(NodeType_VarDeclList);
        newList->varDeclList.varDecl = (yyvsp[-1].ast);
        newList->varDeclList.varDeclList = (yyvsp[0].ast);
        (yyval.ast) = newList;
    }
#line 1384 "parser.tab.c"
    break;

  case 12: /* Param: TYPE ID  */
#line 227 "parser.y"
            {
        (yyval.ast) = createNode(NodeType_VarDecl);
        (yyval.ast)->varDecl.varType = strdup((yyvsp[-1].string));
        (yyval.ast)->varDecl.varName = strdup((yyvsp[0].string));
    }
#line 1394 "parser.tab.c"
    break;

  case 13: /* ArgList: %empty  */
#line 235 "parser.y"
                {
        (yyval.ast) = NULL;
    }
#line 1402 "parser.tab.c"
    break;

  case 14: /* ArgList: Expr ArgListTail  */
#line 238 "parser.y"
                       {
        // If there is just one expr
        if ((yyvsp[0].ast) == NULL) {
            (yyval.ast) = createNode(NodeType_StmtList); // Reuse StmtList or create a new node type for arguments
            (yyval.ast)->stmtList.stmt = (yyvsp[-1].ast);
            (yyval.ast)->stmtList.stmtList = NULL;
        } else {
            // $2 is another list, chain them
            (yyval.ast) = createNode(NodeType_StmtList);
            (yyval.ast)->stmtList.stmt = (yyvsp[-1].ast);
            (yyval.ast)->stmtList.stmtList = (yyvsp[0].ast);
        }
    }
#line 1420 "parser.tab.c"
    break;

  case 15: /* ArgListTail: %empty  */
#line 254 "parser.y"
                { (yyval.ast) = NULL; }
#line 1426 "parser.tab.c"
    break;

  case 16: /* ArgListTail: COMMA Expr ArgListTail  */
#line 255 "parser.y"
                             {
        ASTNode* newList = createNode(NodeType_StmtList);
        newList->stmtList.stmt = (yyvsp[-1].ast);
        newList->stmtList.stmtList = (yyvsp[0].ast);
        (yyval.ast) = newList;
    }
#line 1437 "parser.tab.c"
    break;

  case 17: /* VarDeclList: %empty  */
#line 264 "parser.y"
                {
        (yyval.ast) = NULL; // Handle case where there are no variable declarations
    }
#line 1445 "parser.tab.c"
    break;

  case 18: /* VarDeclList: VarDecl VarDeclList  */
#line 267 "parser.y"
                          {
        (yyval.ast) = createNode(NodeType_VarDeclList);
        (yyval.ast)->varDeclList.varDecl = (yyvsp[-1].ast);
        (yyval.ast)->varDeclList.varDeclList = (yyvsp[0].ast);

        printf("[INFO] Variable declaration list recognized.\n");
    }
#line 1457 "parser.tab.c"
    break;

  case 19: /* VarDecl: TYPE ID SEMICOLON  */
#line 277 "parser.y"
                      {
        SymbolValue value;
        if (strcmp((yyvsp[-2].string), "int") == 0) {
            value.intValue = INT_MIN; // Default value for uninitialized int
            addSymbol(currentScope, (yyvsp[-1].string), TYPE_INT, value);

            (yyval.ast) = createNode(NodeType_VarDecl);
            (yyval.ast)->varDecl.varType = strdup((yyvsp[-2].string)); // Set the variable type (e.g., "int", "float")
            (yyval.ast)->varDecl.varName = strdup((yyvsp[-1].string)); // Set the variable name
            //$$->dataType = TYPE_I;


        } else if (strcmp((yyvsp[-2].string), "float") == 0) {
            value.floatValue = FLT_MIN; // Default value for uninitialized float
            addSymbol(currentScope, (yyvsp[-1].string), TYPE_FLOAT, value);

            (yyval.ast) = createNode(NodeType_VarDecl);
            (yyval.ast)->varDecl.varType = strdup((yyvsp[-2].string)); // Set the variable type (e.g., "int", "float")
            (yyval.ast)->varDecl.varName = strdup((yyvsp[-1].string)); // Set the variable name
            //$$->dataType = TYPE_F;
        }

        

        printf("[INFO] Variable declared: %s %s;\n", (yyvsp[-2].string), (yyvsp[-1].string));
    }
#line 1488 "parser.tab.c"
    break;

  case 20: /* VarDecl: TYPE ID LBRACKET INTEGER RBRACKET SEMICOLON  */
#line 303 "parser.y"
                                                  {
        SymbolValue value;
        if (strcmp((yyvsp[-5].string), "int") == 0) {
            value.intArray = (int*)malloc((yyvsp[-2].integer) * sizeof(int));  // Allocate space for int array
            addArraySymbol(currentScope, (yyvsp[-4].string), TYPE_INT, value, (yyvsp[-2].integer));  // $4 is the array size
        } else if (strcmp((yyvsp[-5].string), "float") == 0) {
            value.floatArray = (float*)malloc((yyvsp[-2].integer) * sizeof(float));  // Allocate space for float array
            addArraySymbol(currentScope, (yyvsp[-4].string), TYPE_FLOAT, value, (yyvsp[-2].integer));  // $4 is the array size
        }

        (yyval.ast) = createNode(NodeType_ArrayDecl);
        (yyval.ast)->arrayDecl.varType = strdup((yyvsp[-5].string));  // Set the variable type (e.g., "int", "float")
        (yyval.ast)->arrayDecl.varName = strdup((yyvsp[-4].string));  // Set the variable name
        (yyval.ast)->arrayDecl.size = (yyvsp[-2].integer);             // Set the array size

        printf("[INFO] Array declared: %s %s[%d];\n", (yyvsp[-5].string), (yyvsp[-4].string), (yyvsp[-2].integer));
    }
#line 1510 "parser.tab.c"
    break;

  case 21: /* StmtList: %empty  */
#line 323 "parser.y"
                {
        (yyval.ast) = NULL; // Handle the case where there are no statements
    }
#line 1518 "parser.tab.c"
    break;

  case 22: /* StmtList: Stmt StmtList  */
#line 326 "parser.y"
                    {
        (yyval.ast) = createNode(NodeType_StmtList);
        (yyval.ast)->stmtList.stmt = (yyvsp[-1].ast);
        (yyval.ast)->stmtList.stmtList = (yyvsp[0].ast);

        printf("[INFO] Statement list recognized.\n");
    }
#line 1530 "parser.tab.c"
    break;

  case 23: /* Stmt: ID EQ Expr SEMICOLON  */
#line 336 "parser.y"
                         {
        Symbol* sym = lookupSymbol(currentScope, (yyvsp[-3].string));
        if (sym == NULL) {
            fprintf(stderr, "Error: Undeclared variable '%s' used at line %d.\n", (yyvsp[-3].string), yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        printf("[DEBUG] Assignment detected: %s = Expr\n", (yyvsp[-3].string)); // Debug print

        if (sym->type == TYPE_INT) {
            int value = (int)evaluateExpr((yyvsp[-1].ast), currentScope);  // Cast to int
            printf("[DEBUG] Assigning value to %s: %d\n", (yyvsp[-3].string), value); // Debug print
            sym->value.intValue = value;
        } else if (sym->type == TYPE_FLOAT) {
            float value = evaluateExpr((yyvsp[-1].ast), currentScope);  // Handle float values
            printf("[DEBUG] Assigning value to %s: %f\n", (yyvsp[-3].string), value); // Debug print
            sym->value.floatValue = value;
        }

        (yyval.ast) = createNode(NodeType_AssignStmt);
        (yyval.ast)->assignStmt.varName = strdup((yyvsp[-3].string));
        (yyval.ast)->assignStmt.operator = strdup((yyvsp[-2].operator));
        (yyval.ast)->assignStmt.expr = (yyvsp[-1].ast);
        (yyval.ast)->assignStmt.isArray = 0; // It's a normal variable
        (yyval.ast)->assignStmt.arrayIndex = NULL;

        printf("[INFO] Assignment statement recognized: %s = ...;\n", (yyvsp[-3].string));
    }
#line 1564 "parser.tab.c"
    break;

  case 24: /* Stmt: ArrayAccess EQ Expr SEMICOLON  */
#line 365 "parser.y"
                                    {
        printf("[DEBUG] Array assignment detected: %s[index] = Expr\n", (yyvsp[-3].ast)->arrayAccess.arrayName); // Debug print

        Symbol* sym = lookupSymbol(currentScope, (yyvsp[-3].ast)->arrayAccess.arrayName);
        if (sym == NULL) {
            fprintf(stderr, "Error: Undeclared array '%s' used at line %d.\n", (yyvsp[-3].ast)->arrayAccess.arrayName, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        int index = (int)evaluateExpr((yyvsp[-3].ast)->arrayAccess.index, currentScope);  // Evaluate index expression
        printf("[DEBUG] Array index evaluated: %d\n", index); // Debug print for index value

        if (index < 0 || index >= sym->size) {
            fprintf(stderr, "Error: Array index out of bounds for array '%s' at line %d.\n", (yyvsp[-3].ast)->arrayAccess.arrayName, yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        // Evaluate the value to be assigned and store in the array based on its type
        if (sym->type == TYPE_INT) {
            int value = (int)evaluateExpr((yyvsp[-1].ast), currentScope); // Evaluate and cast to int for int arrays
            printf("[DEBUG] Assigning int value to %s[%d]: %d\n", (yyvsp[-3].ast)->arrayAccess.arrayName, index, value); // Debug print
            sym->value.intArray[index] = value;  // Store in the int array
        } else if (sym->type == TYPE_FLOAT) {
            float value = evaluateExpr((yyvsp[-1].ast), currentScope); // Evaluate as float for float arrays
            printf("[DEBUG] Assigning float value to %s[%d]: %f\n", (yyvsp[-3].ast)->arrayAccess.arrayName, index, value); // Debug print
            sym->value.floatArray[index] = value;  // Store in the float array
        }

        // Generate TAC with the array name and index
        char arrayAccessStr[50];
        sprintf(arrayAccessStr, "%s[%d]", (yyvsp[-3].ast)->arrayAccess.arrayName, index);  // Create string "arr[0]" format

        // $1 is <ast>, from the ArrayAccess rule above, so $1->arrayAccess.index is ASTNode*.
        (yyval.ast) = createNode(NodeType_AssignStmt);
        (yyval.ast)->assignStmt.varName = strdup((yyvsp[-3].ast)->arrayAccess.arrayName);
        (yyval.ast)->assignStmt.operator = strdup((yyvsp[-2].operator));
        (yyval.ast)->assignStmt.expr = (yyvsp[-1].ast);
        (yyval.ast)->assignStmt.isArray = 1;
        (yyval.ast)->assignStmt.arrayIndex = (yyvsp[-3].ast)->arrayAccess.index; // $1->arrayAccess.index is ASTNode*, matches arrayIndex type.

        printf("[INFO] Array assignment recognized: %s[%d] = ...;\n", (yyvsp[-3].ast)->arrayAccess.arrayName, index);
    }
#line 1613 "parser.tab.c"
    break;

  case 25: /* Stmt: WRITE Expr SEMICOLON  */
#line 409 "parser.y"
                           {
        (yyval.ast) = createNode(NodeType_WriteStmt);
        char* id = NULL;

        if ((yyvsp[-1].ast)->type == NodeType_SimpleID) {
            Symbol* sym = lookupSymbol(currentScope, (yyvsp[-1].ast)->simpleID.name);
            if (sym == NULL) {
                fprintf(stderr, "Error: Undeclared variable '%s' used in write statement at line %d.\n", (yyvsp[-1].ast)->simpleID.name, yylineno);
                parseErrorFlag = 1;
                YYABORT;
            }
            id = strdup((yyvsp[-1].ast)->simpleID.name);
        } else if ((yyvsp[-1].ast)->type == NodeType_IntExpr) {
            id = strdup("intExpr");
        }

        printf("[DEBUG] Write statement detected: write %s\n", id); // Debug print

        (yyval.ast)->writeStmt.id = id;
        printf("[INFO] Write statement recognized: write %s;\n", id);
    }
#line 1639 "parser.tab.c"
    break;

  case 26: /* Stmt: ID LPAREN ArgList RPAREN SEMICOLON  */
#line 430 "parser.y"
                                         {
        Symbol* sym = lookupSymbol(currentScope, (yyvsp[-4].string));
        if (sym == NULL || sym->type != TYPE_FUNCTION) {
            fprintf(stderr, "Error: Undeclared function '%s' called at line %d.\n", (yyvsp[-4].string), yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }
        (yyval.ast) = createNode(NodeType_FunctionCall);
        (yyval.ast)->functionCall.name = strdup((yyvsp[-4].string));
        (yyval.ast)->functionCall.args = (yyvsp[-2].ast);
        // This represents a function call as a statement (no return value used)
        printf("[INFO] Function call statement recognized: %s(...);\n", (yyvsp[-4].string));
    }
#line 1657 "parser.tab.c"
    break;

  case 27: /* Stmt: IF LPAREN Expr RPAREN Stmt ELSE Stmt  */
#line 443 "parser.y"
                                           {
        (yyval.ast) = createNode(NodeType_IfStmt);
        (yyval.ast)->ifStmt.condition = (yyvsp[-4].ast);
        (yyval.ast)->ifStmt.thenStmt = (yyvsp[-2].ast);
        (yyval.ast)->ifStmt.elseStmt = (yyvsp[0].ast);
        printf("[INFO] If-Else statement recognized.\n");
    }
#line 1669 "parser.tab.c"
    break;

  case 28: /* Stmt: IF LPAREN Expr RPAREN Stmt  */
#line 450 "parser.y"
                                 {
        (yyval.ast) = createNode(NodeType_IfStmt);
        (yyval.ast)->ifStmt.condition = (yyvsp[-2].ast);
        (yyval.ast)->ifStmt.thenStmt = (yyvsp[0].ast);
        (yyval.ast)->ifStmt.elseStmt = NULL;
        printf("[INFO] If statement recognized.\n");
    }
#line 1681 "parser.tab.c"
    break;

  case 29: /* Stmt: LBRACE StmtList RBRACE  */
#line 457 "parser.y"
                             {
        (yyval.ast) = (yyvsp[-1].ast); // StmtList already represents the list of statements in the block
        printf("[INFO] Block statement recognized.\n");
    }
#line 1690 "parser.tab.c"
    break;

  case 30: /* Stmt: RETURN Expr SEMICOLON  */
#line 461 "parser.y"
                            {
        (yyval.ast) = createNode(NodeType_ReturnStmt);
        (yyval.ast)->returnStmt.expr = (yyvsp[-1].ast);
        printf("[INFO] Return statement recognized.\n");
    }
#line 1700 "parser.tab.c"
    break;

  case 31: /* Stmt: WHILE LPAREN Expr RPAREN LBRACE StmtList RBRACE  */
#line 466 "parser.y"
                                                      {
        (yyval.ast) = createNode(NodeType_WhileStmt);
        (yyval.ast)->whileStmt.condition = (yyvsp[-4].ast);
        (yyval.ast)->whileStmt.body = (yyvsp[-1].ast);

        printf("[INFO] While loop recognized.\n");
    }
#line 1712 "parser.tab.c"
    break;

  case 32: /* ArrayAccess: ID LBRACKET Expr RBRACKET  */
#line 476 "parser.y"
                              {
        // Lookup the array in the symbol table
        Symbol* sym = lookupSymbol(currentScope, (yyvsp[-3].string));
        if (sym == NULL) {
            fprintf(stderr, "Error: Undeclared array '%s' used at line %d.\n", (yyvsp[-3].string), yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        (yyval.ast) = createNode(NodeType_ArrayAccess);
        (yyval.ast)->arrayAccess.arrayName = strdup((yyvsp[-3].string));   // Store the array name
        (yyval.ast)->arrayAccess.index = (yyvsp[-1].ast);               // Store the index expression

        printf("[INFO] Array access recognized: %s[index]\n", (yyvsp[-3].string));
    }
#line 1732 "parser.tab.c"
    break;

  case 33: /* Expr: Expr PLUS Expr  */
#line 494 "parser.y"
                   {
        (yyval.ast) = createNode(NodeType_Expr);
        (yyval.ast)->expr.left = (yyvsp[-2].ast);
        (yyval.ast)->expr.right = (yyvsp[0].ast);
        //$$->expr.operator = strdup($2);

        printf("[INFO] left type: %u\n", (yyvsp[-2].ast)->dataType);
        printf("[INFO] right type: %u\n", (yyvsp[0].ast)->dataType);

        // If BOTH children are float => this is a float expression
        // Option A: store "+" as "f+"
        // Option B: store "+" as "+"

        if ((yyvsp[-2].ast)->dataType == TYPE_F && (yyvsp[0].ast)->dataType == TYPE_F) {
            (yyval.ast)->dataType = TYPE_F;
            (yyval.ast)->expr.operator = strdup("f+");
            printf("[INFO] Float Expression recognized: ... f+ ...\n");
        } else {
            // Otherwise, assume int
            (yyval.ast)->dataType = TYPE_I;
            (yyval.ast)->expr.operator = strdup("+");
            printf("[INFO] Int Expression recognized: ... + ...\n");
        }
        
    }
#line 1762 "parser.tab.c"
    break;

  case 34: /* Expr: Expr MINUS Expr  */
#line 519 "parser.y"
                      {
        (yyval.ast) = createNode(NodeType_Expr);
        (yyval.ast)->expr.left = (yyvsp[-2].ast);
        (yyval.ast)->expr.right = (yyvsp[0].ast);
        (yyval.ast)->expr.operator = strdup((yyvsp[-1].operator));

        if ((yyvsp[-2].ast)->dataType == TYPE_F && (yyvsp[0].ast)->dataType == TYPE_F) {
            (yyval.ast)->dataType = TYPE_F;
            (yyval.ast)->expr.operator = strdup("f-");
            printf("[INFO] Float Expression recognized: ... f- ...\n");
        } else {
            // Otherwise, assume int
            (yyval.ast)->dataType = TYPE_I;
            (yyval.ast)->expr.operator = strdup("-");
            printf("[INFO] Int Expression recognized: ... - ...\n");
        }
    }
#line 1784 "parser.tab.c"
    break;

  case 35: /* Expr: Expr MULTIPLY Expr  */
#line 536 "parser.y"
                         {
        (yyval.ast) = createNode(NodeType_Expr);
        (yyval.ast)->expr.left = (yyvsp[-2].ast);
        (yyval.ast)->expr.right = (yyvsp[0].ast);
        (yyval.ast)->expr.operator = strdup((yyvsp[-1].operator));

        if ((yyvsp[-2].ast)->dataType == TYPE_F && (yyvsp[0].ast)->dataType == TYPE_F) {
            (yyval.ast)->dataType = TYPE_F;
            (yyval.ast)->expr.operator = strdup("f*");
            printf("[INFO] Float Expression recognized: ... f* ...\n");
        } else {
            // Otherwise, assume int
            (yyval.ast)->dataType = TYPE_I;
            (yyval.ast)->expr.operator = strdup("*");
            printf("[INFO] Int Expression recognized: ... * ...\n");
        }
    }
#line 1806 "parser.tab.c"
    break;

  case 36: /* Expr: Expr DIVIDE Expr  */
#line 553 "parser.y"
                       {
        (yyval.ast) = createNode(NodeType_Expr);
        (yyval.ast)->expr.left = (yyvsp[-2].ast);
        (yyval.ast)->expr.right = (yyvsp[0].ast);
        (yyval.ast)->expr.operator = strdup((yyvsp[-1].operator));

        if((yyvsp[-2].ast)->dataType == TYPE_F && (yyvsp[0].ast)->dataType == TYPE_F) {
            (yyval.ast)->dataType = TYPE_F;
            (yyval.ast)->expr.operator = strdup("f/");
            printf("[INFO] Float Expression recognized: ... f/ ...\n");
        } else {
            // Otherwise, assume int
            (yyval.ast)->dataType = TYPE_I;
            (yyval.ast)->expr.operator = strdup("/");
            printf("[INFO] Int Expression recognized: ... / ...\n");
        }
    }
#line 1828 "parser.tab.c"
    break;

  case 37: /* Expr: LPAREN Expr RPAREN  */
#line 570 "parser.y"
                         {
        (yyval.ast) = (yyvsp[-1].ast);
        printf("[INFO] Parenthesized expression recognized: (...)\n");
    }
#line 1837 "parser.tab.c"
    break;

  case 38: /* Expr: Expr GT Expr  */
#line 574 "parser.y"
                   {
        (yyval.ast) = createNode(NodeType_Expr);
        (yyval.ast)->expr.left = (yyvsp[-2].ast);
        (yyval.ast)->expr.right = (yyvsp[0].ast);
        (yyval.ast)->expr.operator = strdup(">");
        
        if ((yyvsp[-2].ast)->dataType == TYPE_F && (yyvsp[0].ast)->dataType == TYPE_F) {
            (yyval.ast)->dataType = TYPE_F;
            (yyval.ast)->expr.operator = strdup("f>");
            printf("[INFO] Float Expression recognized: ... f> ... \n");
        } else {
            (yyval.ast)->dataType = TYPE_I;
            printf("[INFO] Int Expression recognized: ... > ...\n");
        }
    }
#line 1857 "parser.tab.c"
    break;

  case 39: /* Expr: Expr LT Expr  */
#line 589 "parser.y"
                   {
        (yyval.ast) = createNode(NodeType_Expr);
        (yyval.ast)->expr.left = (yyvsp[-2].ast);
        (yyval.ast)->expr.right = (yyvsp[0].ast);
        (yyval.ast)->expr.operator = strdup("<");
        
        if ((yyvsp[-2].ast)->dataType == TYPE_F && (yyvsp[0].ast)->dataType == TYPE_F) {
            (yyval.ast)->dataType = TYPE_F;
            (yyval.ast)->expr.operator = strdup("f<");
            printf("[INFO] Expression recognized: ... f< ... \n");
        } else {
            (yyval.ast)->dataType = TYPE_I;
            printf("[INFO] Expression recognized: ... < ...\n");
        }
    }
#line 1877 "parser.tab.c"
    break;

  case 40: /* Expr: Expr GEQ Expr  */
#line 604 "parser.y"
                    {
        (yyval.ast) = createNode(NodeType_Expr);
        (yyval.ast)->expr.left = (yyvsp[-2].ast);
        (yyval.ast)->expr.right = (yyvsp[0].ast);
        
        if ((yyvsp[-2].ast)->dataType == TYPE_F && (yyvsp[0].ast)->dataType == TYPE_F) {
            (yyval.ast)->dataType = TYPE_F;
            (yyval.ast)->expr.operator = strdup("f>=");
            printf("[INFO] Float Expression recognized: ... f>= ... \n");
        } else {
            (yyval.ast)->dataType = TYPE_I;
            (yyval.ast)->expr.operator = strdup(">=");
            printf("[INFO] Int Expression recognized: ... >= ...\n");
        }
    }
#line 1897 "parser.tab.c"
    break;

  case 41: /* Expr: Expr LEQ Expr  */
#line 619 "parser.y"
                    {
        (yyval.ast) = createNode(NodeType_Expr);
        (yyval.ast)->expr.left = (yyvsp[-2].ast);
        (yyval.ast)->expr.right = (yyvsp[0].ast);
        
        if ((yyvsp[-2].ast)->dataType == TYPE_F && (yyvsp[0].ast)->dataType == TYPE_F) {
            (yyval.ast)->dataType = TYPE_F;
            (yyval.ast)->expr.operator = strdup("f<=");
            printf("[INFO] Float Expression recognized: ... f<= ... \n");
        } else {
            (yyval.ast)->dataType = TYPE_I;
            (yyval.ast)->expr.operator = strdup("<=");
            printf("[INFO] Int Expression recognized: ... <= ...\n");
        }
    }
#line 1917 "parser.tab.c"
    break;

  case 42: /* Expr: Expr EQEQ Expr  */
#line 634 "parser.y"
                     {
        (yyval.ast) = createNode(NodeType_Expr);
        (yyval.ast)->expr.left = (yyvsp[-2].ast);
        (yyval.ast)->expr.right = (yyvsp[0].ast);
        
        if ((yyvsp[-2].ast)->dataType == TYPE_F && (yyvsp[0].ast)->dataType == TYPE_F) {
            (yyval.ast)->dataType = TYPE_F;
            (yyval.ast)->expr.operator = strdup("f==");
            printf("[INFO] Float Expression recognized: ... f== ... \n");
        } else {
            (yyval.ast)->dataType = TYPE_I;
            (yyval.ast)->expr.operator = strdup("==");
            printf("[INFO] Int Expression recognized: ... == ...\n");
        }
    }
#line 1937 "parser.tab.c"
    break;

  case 43: /* Expr: Expr NEQ Expr  */
#line 649 "parser.y"
                    {
        (yyval.ast) = createNode(NodeType_Expr);
        (yyval.ast)->expr.left = (yyvsp[-2].ast);
        (yyval.ast)->expr.right = (yyvsp[0].ast);
        
        if ((yyvsp[-2].ast)->dataType == TYPE_F && (yyvsp[0].ast)->dataType == TYPE_F) {
            (yyval.ast)->dataType = TYPE_F;
            (yyval.ast)->expr.operator = strdup("f!=");
            printf("[INFO] Float Expression recognized: ... f!= ... \n");
        } else {
            (yyval.ast)->dataType = TYPE_I;
            (yyval.ast)->expr.operator = strdup("!=");
            printf("[INFO] Int Expression recognized: ... != ...\n");
        }
    }
#line 1957 "parser.tab.c"
    break;

  case 44: /* Expr: ID  */
#line 664 "parser.y"
         {
        Symbol* sym = lookupSymbol(currentScope, (yyvsp[0].string));
        if (sym == NULL) {
            fprintf(stderr, "Error: Undeclared variable '%s' used at line %d.\n", (yyvsp[0].string), yylineno);
            parseErrorFlag = 1;
            YYABORT;
        }

        (yyval.ast) = createNode(NodeType_SimpleID);
        (yyval.ast)->simpleID.name = strdup((yyvsp[0].string));

        //check teh symboltable for the type of the variable
        if (sym->type == TYPE_INT) {
            (yyval.ast)->dataType = TYPE_I;
            printf("[INFO] INT Identifier recognized: %s\n", (yyvsp[0].string));
        } else if (sym->type == TYPE_FLOAT) {
            (yyval.ast)->dataType = TYPE_F;
            printf("[INFO] Float Identifier recognized: %s\n", (yyvsp[0].string));
        }
        
    }
#line 1983 "parser.tab.c"
    break;

  case 45: /* Expr: INTEGER  */
#line 685 "parser.y"
              {
        (yyval.ast) = createNode(NodeType_IntExpr);
        (yyval.ast)->dataType = TYPE_I; 
        (yyval.ast)->IntExpr.integer = (yyvsp[0].integer);

        printf("[INFO] Integer recognized: %d\n", (yyvsp[0].integer));
        printf("[INFO] Type : %u\n", (yyval.ast)->dataType);
    }
#line 1996 "parser.tab.c"
    break;

  case 46: /* Expr: FLOAT  */
#line 693 "parser.y"
            {
        (yyval.ast) = createNode(NodeType_FloatExpr);
        (yyval.ast)->dataType = TYPE_F;
        (yyval.ast)->FloatExpr.floatNum = (yyvsp[0].floatNum);

        printf("[INFO] FLOAT recognized: %f\n", (yyvsp[0].floatNum));
        printf("[INFO] Type : %u\n", (yyval.ast)->dataType);
    }
#line 2009 "parser.tab.c"
    break;

  case 47: /* Expr: ArrayAccess  */
#line 701 "parser.y"
                  {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 2017 "parser.tab.c"
    break;

  case 48: /* Expr: ID LPAREN ArgList RPAREN  */
#line 704 "parser.y"
                               {
        (yyval.ast) = createNode(NodeType_FunctionCall);
        (yyval.ast)->functionCall.name = strdup((yyvsp[-3].string));
        (yyval.ast)->functionCall.args = (yyvsp[-1].ast); // ArgList AST node
        printf("[INFO] Function call expression recognized: %s(...)\n", (yyvsp[-3].string));
    }
#line 2028 "parser.tab.c"
    break;


#line 2032 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 712 "parser.y"


int main() {
    // Initialize the global symbol table (global scope)
    symTab = createSymbolTable(NULL, TABLE_SIZE);
    if (symTab == NULL || symTab->table == NULL) {
        fprintf(stderr, "[ERROR] Failed to initialize global symbol table.\n");
        return EXIT_FAILURE;
    }

    // Set the global scope as the current scope
    currentScope = symTab;
    registerScope(currentScope);

    if (scopeListHead == NULL) {
        fprintf(stderr, "[ERROR] Scope list head is NULL after registering global scope.\n");
        return EXIT_FAILURE;
    }

    // Initialize file or input source
    yyin = fopen("testProg.cmm", "r");
    if (!yyin) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    printf("\n\n------- START COMPILER -------\n\n");

    int parseResult = yyparse();
    if (parseResult == 0) {
        printf("\n\n--- Parsing successful! ---\n\n");

        printf("\n----- ABSTRACT SYNTAX TREE -----\n");
        if (root != NULL) {
            traverseAST(root, 0);
        }

        // Print the symbol table for all scopes
        printSymbolTable(scopeListHead);

        // Generate TAC
        printf("\n----- GENERATED TAC -----\n");
        TAC* tacHead = generateTAC(root, NULL);
        printTAC(tacHead);

        // --- (2) Generate MIPS directly from the AST + symbol tables ---
        printf("\n----- GENERATING MIPS CODE -----\n");
        // Then generate MIPS from the TAC
        generateMIPSFromTAC(tacHead, "output.s");

        printf("[INFO] MIPS code generated in output.s\n");

        // Cleanup TAC
        freeTAC(tacHead);

        // Clean up the AST
        if (root != NULL) {
            freeAST(root);
        }
    } else {
        fprintf(stderr, "Parsing failed with errors\n");
    }

    // Cleanup the symbol tables
    ScopeNode* currentNode = scopeListHead;
    while (currentNode != NULL) {
        if (currentNode->scope != NULL) {
            freeSymbolTable(currentNode->scope);
        }
        ScopeNode* tempNode = currentNode;
        currentNode = currentNode->next;
        free(tempNode);
    }
    scopeListHead = NULL;

    fclose(yyin);
    return parseResult;
}


void yyerror(const char* s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
    parseErrorFlag = 1;
}
