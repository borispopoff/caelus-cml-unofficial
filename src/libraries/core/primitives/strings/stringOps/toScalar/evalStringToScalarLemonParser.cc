/*
** 2000-05-29
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Driver template for the LEMON parser generator.
**
** The "lemon" program processes an LALR(1) input grammar file, then uses
** this template to construct a parser.  The "lemon" program inserts text
** at each "%%" line.  Also, any "P-a-r-s-e" identifer prefix (without the
** interstitial "-" characters) contained in this template is changed into
** the value of the %name directive from the grammar.  Otherwise, the content
** of this template is copied straight through into the generate parser
** source file.
**
** The following is the concatenation of all %include directives from the
** input grammar file:
*/
#include <stdio.h>
#include <assert.h>
/************ Begin %include sections from the grammar ************************/
#line 1 "evalStringToScalarLemonParser.lyy"

/*--------------------------------*- C++ -*----------------------------------*\
Copyright (C) 2019 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of Caelus.

    Caelus is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Caelus is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with Caelus.  If not, see <http://www.gnu.org/licenses/>.

Description
    Lemon grammar of a simple string to scalar evaluation.

    The generated parser is localized in an anonymous namespace.
    Interface code wrapping is near the bottom of the file.

\*---------------------------------------------------------------------------*/

#include "evalStringToScalarDriver.hpp"
#include "evalStringToScalarParser.hpp"
#include "unitConversion.hpp"
#include "Random.hpp"
#include "error.hpp"

#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#line 68 "evalStringToScalarLemonParser.cc"
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    YYNOCODE           is a number of type YYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    YYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    ParseTOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal
**                       symbols.
**    YYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_PARAM     Code to pass %extra_argument as a subroutine parameter
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    ParseCTX_*         As ParseARG_ except for %extra_context
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYNTOKEN           Number of terminal symbols
**    YY_MAX_SHIFT       Maximum value for shift actions
**    YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    YY_ERROR_ACTION    The yy_action[] code for syntax error
**    YY_ACCEPT_ACTION   The yy_action[] code for accept
**    YY_NO_ACTION       The yy_action[] code for no-op
**    YY_MIN_REDUCE      Minimum value for reduce actions
**    YY_MAX_REDUCE      Maximum value for reduce actions
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define YYCODETYPE unsigned char
#define YYNOCODE 41
#define YYACTIONTYPE unsigned char
#define ParseTOKENTYPE  CML::scalar
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  CML::scalar yy11;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL
#define ParseARG_PDECL
#define ParseARG_PARAM
#define ParseARG_FETCH
#define ParseARG_STORE
#define ParseCTX_SDECL  CML::parsing::evalStringToScalar::parseDriver* driver ;
#define ParseCTX_PDECL , CML::parsing::evalStringToScalar::parseDriver* driver 
#define ParseCTX_PARAM ,driver 
#define ParseCTX_FETCH  CML::parsing::evalStringToScalar::parseDriver* driver =yypParser->driver ;
#define ParseCTX_STORE yypParser->driver =driver ;
#define YYNSTATE             107
#define YYNRULE              40
#define YYNTOKEN             39
#define YY_MAX_SHIFT         106
#define YY_MIN_SHIFTREDUCE   144
#define YY_MAX_SHIFTREDUCE   183
#define YY_ERROR_ACTION      184
#define YY_ACCEPT_ACTION     185
#define YY_NO_ACTION         186
#define YY_MIN_REDUCE        187
#define YY_MAX_REDUCE        226
/************* End control #defines *******************************************/
#define YY_NLOOKAHEAD ((int)(sizeof(yy_lookahead)/sizeof(yy_lookahead[0])))

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= YY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between YY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and YY_MAX_SHIFTREDUCE           reduce by rule N-YY_MIN_SHIFTREDUCE.
**
**   N == YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
**   N between YY_MIN_REDUCE            Reduce by rule N-YY_MIN_REDUCE
**     and YY_MAX_REDUCE
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = yy_action[ yy_shift_ofst[S] + X ]
**    (B)   N = yy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X.
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define YY_ACTTAB_COUNT (455)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    74,  185,   38,   40,   36,   35,  145,   34,  182,  106,
 /*    10 */   104,  103,  102,  101,  100,   99,   71,   98,   97,   96,
 /*    20 */    95,   94,   93,   92,   91,   90,   89,   88,   87,   86,
 /*    30 */    85,   84,   83,   82,   81,   80,   79,   78,   77,   38,
 /*    40 */     1,   72,   41,  145,   34,  154,  106,  104,  103,  102,
 /*    50 */   101,  100,   99,   42,   98,   97,   96,   95,   94,   93,
 /*    60 */    92,   91,   90,   89,   88,   87,   86,   85,   84,   83,
 /*    70 */    82,   81,   80,   79,   78,   77,   38,   43,   44,   45,
 /*    80 */   145,   34,  153,  106,  104,  103,  102,  101,  100,   99,
 /*    90 */    46,   98,   97,   96,   95,   94,   93,   92,   91,   90,
 /*   100 */    89,   88,   87,   86,   85,   84,   83,   82,   81,   80,
 /*   110 */    79,   78,   77,   38,   47,   48,   49,  145,   34,   50,
 /*   120 */   106,  104,  103,  102,  101,  100,   99,   51,   98,   97,
 /*   130 */    96,   95,   94,   93,   92,   91,   90,   89,   88,   87,
 /*   140 */    86,   85,   84,   83,   82,   81,   80,   79,   78,   77,
 /*   150 */    39,   37,   36,   35,   52,   53,   54,  183,   55,   39,
 /*   160 */    37,   36,   35,   56,   57,   58,  181,   59,   39,   37,
 /*   170 */    36,   35,   60,   61,   62,  180,   63,   39,   37,   36,
 /*   180 */    35,   64,   65,   66,  179,   67,   39,   37,   36,   35,
 /*   190 */    68,   69,   70,  178,   73,   39,   37,   36,   35,  193,
 /*   200 */   192,   76,  177,  189,   39,   37,   36,   35,   75,    4,
 /*   210 */   186,  176,    5,   39,   37,   36,   35,   39,   37,   36,
 /*   220 */    35,    6,    7,  186,  175,    8,   10,   12,    9,   13,
 /*   230 */    39,   37,   36,   35,   39,   37,   36,   35,   14,   15,
 /*   240 */   186,  174,   17,   19,   20,   11,   21,   39,   37,   36,
 /*   250 */    35,   22,   23,  186,  173,   24,   39,   37,   36,   35,
 /*   260 */    25,  186,  186,  172,   26,   39,   37,   36,   35,  152,
 /*   270 */   186,  186,  171,   27,   39,   37,   36,   35,   39,   37,
 /*   280 */    36,   35,   28,   30,  186,  170,   31,   32,   33,   16,
 /*   290 */     2,   39,   37,   36,   35,   39,   37,   36,   35,    3,
 /*   300 */   105,  186,  169,  186,  186,  186,   18,  186,   39,   37,
 /*   310 */    36,   35,  186,  186,  186,  168,  186,   39,   37,   36,
 /*   320 */    35,  186,  186,  186,  167,  186,   39,   37,   36,   35,
 /*   330 */   186,  186,  186,  166,  186,   39,   37,   36,   35,  186,
 /*   340 */   186,  186,  165,  186,   39,   37,   36,   35,  186,  186,
 /*   350 */   186,  164,  186,   39,   37,   36,   35,  186,  186,  186,
 /*   360 */   163,  186,   39,   37,   36,   35,  186,  186,  186,  162,
 /*   370 */   186,   39,   37,   36,   35,  186,  186,  186,  161,  186,
 /*   380 */    39,   37,   36,   35,  186,  186,  186,  160,  186,   39,
 /*   390 */    37,   36,   35,   39,   37,   36,   35,  186,  186,  186,
 /*   400 */   159,  186,  186,  186,   29,  186,   39,   37,   36,   35,
 /*   410 */   186,  186,  186,  158,  186,   39,   37,   36,   35,  186,
 /*   420 */   186,  186,  157,  186,   39,   37,   36,   35,  186,  186,
 /*   430 */   186,  156,  186,   39,   37,   36,   35,  186,  186,  186,
 /*   440 */   155,  186,   39,   37,   36,   35,  186,  186,  186,  151,
 /*   450 */   187,   39,   37,   36,   35,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    39,   40,    2,   39,    3,    4,    6,    7,    8,    9,
 /*    10 */    10,   11,   12,   13,   14,   15,   39,   17,   18,   19,
 /*    20 */    20,   21,   22,   23,   24,   25,   26,   27,   28,   29,
 /*    30 */    30,   31,   32,   33,   34,   35,   36,   37,   38,    2,
 /*    40 */     7,   39,   39,    6,    7,    8,    9,   10,   11,   12,
 /*    50 */    13,   14,   15,   39,   17,   18,   19,   20,   21,   22,
 /*    60 */    23,   24,   25,   26,   27,   28,   29,   30,   31,   32,
 /*    70 */    33,   34,   35,   36,   37,   38,    2,   39,   39,   39,
 /*    80 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*    90 */    39,   17,   18,   19,   20,   21,   22,   23,   24,   25,
 /*   100 */    26,   27,   28,   29,   30,   31,   32,   33,   34,   35,
 /*   110 */    36,   37,   38,    2,   39,   39,   39,    6,    7,   39,
 /*   120 */     9,   10,   11,   12,   13,   14,   15,   39,   17,   18,
 /*   130 */    19,   20,   21,   22,   23,   24,   25,   26,   27,   28,
 /*   140 */    29,   30,   31,   32,   33,   34,   35,   36,   37,   38,
 /*   150 */     1,    2,    3,    4,   39,   39,   39,    8,   39,    1,
 /*   160 */     2,    3,    4,   39,   39,   39,    8,   39,    1,    2,
 /*   170 */     3,    4,   39,   39,   39,    8,   39,    1,    2,    3,
 /*   180 */     4,   39,   39,   39,    8,   39,    1,    2,    3,    4,
 /*   190 */    39,   39,   39,    8,   39,    1,    2,    3,    4,   39,
 /*   200 */    39,   39,    8,   39,    1,    2,    3,    4,   39,    7,
 /*   210 */    41,    8,    7,    1,    2,    3,    4,    1,    2,    3,
 /*   220 */     4,    7,    7,   41,    8,    7,    7,    7,   16,    7,
 /*   230 */     1,    2,    3,    4,    1,    2,    3,    4,    7,    7,
 /*   240 */    41,    8,    7,    7,    7,   16,    7,    1,    2,    3,
 /*   250 */     4,    7,    7,   41,    8,    7,    1,    2,    3,    4,
 /*   260 */     7,   41,   41,    8,    7,    1,    2,    3,    4,    8,
 /*   270 */    41,   41,    8,    7,    1,    2,    3,    4,    1,    2,
 /*   280 */     3,    4,    7,    7,   41,    8,    7,    7,    7,   16,
 /*   290 */     7,    1,    2,    3,    4,    1,    2,    3,    4,    7,
 /*   300 */     7,   41,    8,   41,   41,   41,   16,   41,    1,    2,
 /*   310 */     3,    4,   41,   41,   41,    8,   41,    1,    2,    3,
 /*   320 */     4,   41,   41,   41,    8,   41,    1,    2,    3,    4,
 /*   330 */    41,   41,   41,    8,   41,    1,    2,    3,    4,   41,
 /*   340 */    41,   41,    8,   41,    1,    2,    3,    4,   41,   41,
 /*   350 */    41,    8,   41,    1,    2,    3,    4,   41,   41,   41,
 /*   360 */     8,   41,    1,    2,    3,    4,   41,   41,   41,    8,
 /*   370 */    41,    1,    2,    3,    4,   41,   41,   41,    8,   41,
 /*   380 */     1,    2,    3,    4,   41,   41,   41,    8,   41,    1,
 /*   390 */     2,    3,    4,    1,    2,    3,    4,   41,   41,   41,
 /*   400 */     8,   41,   41,   41,   16,   41,    1,    2,    3,    4,
 /*   410 */    41,   41,   41,    8,   41,    1,    2,    3,    4,   41,
 /*   420 */    41,   41,    8,   41,    1,    2,    3,    4,   41,   41,
 /*   430 */    41,    8,   41,    1,    2,    3,    4,   41,   41,   41,
 /*   440 */     8,   41,    1,    2,    3,    4,   41,   41,   41,    8,
 /*   450 */     0,    1,    2,    3,    4,   41,   41,   41,   41,   41,
 /*   460 */    41,   41,   41,   41,   41,   41,   41,   41,   41,   41,
 /*   470 */    41,   41,   41,   41,   41,   41,   41,   41,   41,   41,
 /*   480 */    41,   41,   41,   41,   41,   41,   41,   41,   41,   41,
 /*   490 */    39,   39,   39,   39,
};
#define YY_SHIFT_COUNT    (106)
#define YY_SHIFT_MIN      (0)
#define YY_SHIFT_MAX      (450)
static const unsigned short int yy_shift_ofst[] = {
 /*     0 */   111,    0,   37,   74,  111,  111,  111,  111,  111,  111,
 /*    10 */   111,  111,  111,  111,  111,  111,  111,  111,  111,  111,
 /*    20 */   111,  111,  111,  111,  111,  111,  111,  111,  111,  111,
 /*    30 */   111,  111,  111,  111,  111,  111,  111,  111,  111,  111,
 /*    40 */   149,  158,  167,  176,  185,  194,  203,  212,  216,  229,
 /*    50 */   233,  246,  255,  264,  273,  277,  290,  294,  307,  316,
 /*    60 */   325,  334,  343,  352,  361,  370,  379,  388,  392,  405,
 /*    70 */   414,  423,  432,  441,  450,    1,    1,   33,  202,  205,
 /*    80 */   214,  215,  218,  219,  220,  222,  231,  232,  235,  236,
 /*    90 */   237,  239,  244,  245,  248,  253,  257,  266,  275,  276,
 /*   100 */   279,  280,  281,  283,  292,  261,  293,
};
#define YY_REDUCE_COUNT (39)
#define YY_REDUCE_MIN   (-39)
#define YY_REDUCE_MAX   (169)
static const short yy_reduce_ofst[] = {
 /*     0 */   -39,  -36,  -23,    2,    3,   14,   38,   39,   40,   51,
 /*    10 */    75,   76,   77,   80,   88,  115,  116,  117,  119,  124,
 /*    20 */   125,  126,  128,  133,  134,  135,  137,  142,  143,  144,
 /*    30 */   146,  151,  152,  153,  155,  160,  161,  162,  164,  169,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   184,  184,  184,  184,  184,  184,  184,  184,  184,  184,
 /*    10 */   184,  184,  184,  184,  184,  184,  184,  184,  184,  184,
 /*    20 */   184,  184,  184,  184,  184,  184,  184,  184,  184,  184,
 /*    30 */   184,  184,  184,  184,  184,  184,  184,  184,  184,  184,
 /*    40 */   184,  184,  184,  184,  184,  184,  184,  184,  184,  184,
 /*    50 */   184,  184,  184,  184,  184,  184,  184,  184,  184,  184,
 /*    60 */   184,  184,  184,  184,  184,  184,  184,  184,  184,  184,
 /*    70 */   184,  184,  184,  184,  184,  190,  191,  184,  184,  184,
 /*    80 */   184,  184,  184,  184,  184,  184,  184,  184,  184,  184,
 /*    90 */   184,  184,  184,  184,  184,  184,  184,  184,  184,  184,
 /*   100 */   184,  184,  184,  184,  184,  184,  184,
};
/********** End of lemon-generated parsing tables *****************************/

/* The next table maps tokens (terminal symbols) into fallback tokens.
** If a construct like the following:
**
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
**
** This feature can be used, for example, to cause some keywords in a language
** to revert to identifiers if they keyword does not apply in the context where
** it appears.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  yyStackEntry *yytos;          /* Pointer to top element of the stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyhwm;                    /* High-water mark of the stack */
#endif
#ifndef YYNOERRORRECOVERY
  int yyerrcnt;                 /* Shifts left before out of the error */
#endif
  ParseARG_SDECL                /* A place to hold %extra_argument */
  ParseCTX_SDECL                /* A place to hold %extra_context */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
  yyStackEntry yystk0;          /* First stack entry */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
  yyStackEntry *yystackEnd;            /* Last entry in the stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/*
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
namespace  {
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
} // End namespace 
#endif /* NDEBUG */

#if defined(YYCOVERAGE) || !defined(NDEBUG)
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = {
  /*    0 */ "$",
  /*    1 */ "PLUS",
  /*    2 */ "MINUS",
  /*    3 */ "TIMES",
  /*    4 */ "DIVIDE",
  /*    5 */ "NEGATE",
  /*    6 */ "NUMBER",
  /*    7 */ "LPAREN",
  /*    8 */ "RPAREN",
  /*    9 */ "PI",
  /*   10 */ "DEG_TO_RAD",
  /*   11 */ "RAD_TO_DEG",
  /*   12 */ "EXP",
  /*   13 */ "LOG",
  /*   14 */ "LOG10",
  /*   15 */ "POW",
  /*   16 */ "COMMA",
  /*   17 */ "SQR",
  /*   18 */ "SQRT",
  /*   19 */ "CBRT",
  /*   20 */ "SIN",
  /*   21 */ "COS",
  /*   22 */ "TAN",
  /*   23 */ "ASIN",
  /*   24 */ "ACOS",
  /*   25 */ "ATAN",
  /*   26 */ "ATAN2",
  /*   27 */ "HYPOT",
  /*   28 */ "SINH",
  /*   29 */ "COSH",
  /*   30 */ "TANH",
  /*   31 */ "MIN",
  /*   32 */ "MAX",
  /*   33 */ "MAG",
  /*   34 */ "MAGSQR",
  /*   35 */ "FLOOR",
  /*   36 */ "CEIL",
  /*   37 */ "ROUND",
  /*   38 */ "RAND",
  /*   39 */ "exp",
  /*   40 */ "evaluate",
};
#endif /* defined(YYCOVERAGE) || !defined(NDEBUG) */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "evaluate ::= exp",
 /*   1 */ "exp ::= NUMBER",
 /*   2 */ "exp ::= MINUS exp",
 /*   3 */ "exp ::= exp PLUS exp",
 /*   4 */ "exp ::= exp MINUS exp",
 /*   5 */ "exp ::= exp TIMES exp",
 /*   6 */ "exp ::= exp DIVIDE exp",
 /*   7 */ "exp ::= LPAREN exp RPAREN",
 /*   8 */ "exp ::= PI LPAREN RPAREN",
 /*   9 */ "exp ::= DEG_TO_RAD LPAREN RPAREN",
 /*  10 */ "exp ::= RAD_TO_DEG LPAREN RPAREN",
 /*  11 */ "exp ::= DEG_TO_RAD LPAREN exp RPAREN",
 /*  12 */ "exp ::= RAD_TO_DEG LPAREN exp RPAREN",
 /*  13 */ "exp ::= EXP LPAREN exp RPAREN",
 /*  14 */ "exp ::= LOG LPAREN exp RPAREN",
 /*  15 */ "exp ::= LOG10 LPAREN exp RPAREN",
 /*  16 */ "exp ::= POW LPAREN exp COMMA exp RPAREN",
 /*  17 */ "exp ::= SQR LPAREN exp RPAREN",
 /*  18 */ "exp ::= SQRT LPAREN exp RPAREN",
 /*  19 */ "exp ::= CBRT LPAREN exp RPAREN",
 /*  20 */ "exp ::= SIN LPAREN exp RPAREN",
 /*  21 */ "exp ::= COS LPAREN exp RPAREN",
 /*  22 */ "exp ::= TAN LPAREN exp RPAREN",
 /*  23 */ "exp ::= ASIN LPAREN exp RPAREN",
 /*  24 */ "exp ::= ACOS LPAREN exp RPAREN",
 /*  25 */ "exp ::= ATAN LPAREN exp RPAREN",
 /*  26 */ "exp ::= ATAN2 LPAREN exp COMMA exp RPAREN",
 /*  27 */ "exp ::= HYPOT LPAREN exp COMMA exp RPAREN",
 /*  28 */ "exp ::= SINH LPAREN exp RPAREN",
 /*  29 */ "exp ::= COSH LPAREN exp RPAREN",
 /*  30 */ "exp ::= TANH LPAREN exp RPAREN",
 /*  31 */ "exp ::= MIN LPAREN exp COMMA exp RPAREN",
 /*  32 */ "exp ::= MAX LPAREN exp COMMA exp RPAREN",
 /*  33 */ "exp ::= MAG LPAREN exp RPAREN",
 /*  34 */ "exp ::= MAGSQR LPAREN exp RPAREN",
 /*  35 */ "exp ::= FLOOR LPAREN exp RPAREN",
 /*  36 */ "exp ::= CEIL LPAREN exp RPAREN",
 /*  37 */ "exp ::= ROUND LPAREN exp RPAREN",
 /*  38 */ "exp ::= RAND LPAREN RPAREN",
 /*  39 */ "exp ::= RAND LPAREN exp RPAREN",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int yyGrowStack(yyParser *p){
  int newSize;
  int idx;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  idx = p->yytos ? (int)(p->yytos - p->yystack) : 0;
  if( p->yystack==&p->yystk0 ){
    pNew = malloc(newSize*sizeof(pNew[0]));
    if( pNew ) pNew[0] = p->yystk0;
  }else{
    pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  }
  if( pNew ){
    p->yystack = pNew;
    p->yytos = &p->yystack[idx];
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows from %d to %d entries.\n",
              yyTracePrompt, p->yystksz, newSize);
    }
#endif
    p->yystksz = newSize;
  }
  return pNew==0;
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to ParseAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef YYMALLOCARGTYPE
# define YYMALLOCARGTYPE size_t
#endif

/* Initialize a new parser that has already been allocated.
*/
namespace  {
void ParseInit(void *yypRawParser ParseCTX_PDECL){
  yyParser *yypParser = (yyParser*)yypRawParser;
  ParseCTX_STORE
#ifdef YYTRACKMAXSTACKDEPTH
  yypParser->yyhwm = 0;
#endif
#if YYSTACKDEPTH<=0
  yypParser->yytos = NULL;
  yypParser->yystack = NULL;
  yypParser->yystksz = 0;
  if( yyGrowStack(yypParser) ){
    yypParser->yystack = &yypParser->yystk0;
    yypParser->yystksz = 1;
  }
#endif
#ifndef YYNOERRORRECOVERY
  yypParser->yyerrcnt = -1;
#endif
  yypParser->yytos = yypParser->yystack;
  yypParser->yystack[0].stateno = 0;
  yypParser->yystack[0].major = 0;
#if YYSTACKDEPTH>0
  yypParser->yystackEnd = &yypParser->yystack[YYSTACKDEPTH-1];
#endif
} // End namespace 
}

#ifndef Parse_ENGINEALWAYSONSTACK
/*
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
namespace  {
void *ParseAlloc(void *(*mallocProc)(YYMALLOCARGTYPE) ParseCTX_PDECL){
  yyParser *yypParser;
  yypParser = (yyParser*)(*mallocProc)( (YYMALLOCARGTYPE)sizeof(yyParser) );
  if( yypParser ){
    ParseCTX_STORE
    ParseInit(yypParser ParseCTX_PARAM);
  }
  return (void*)yypParser;
}
} // End namespace 
#endif /* Parse_ENGINEALWAYSONSTACK */


/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "yymajor" is the symbol code, and "yypminor" is
** a pointer to the value to be deleted.  The code used to do the
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH
  ParseCTX_FETCH
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are *not* used
    ** inside the C code.
    */
/********* Begin destructor definitions ***************************************/
/********* End destructor definitions *****************************************/
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void yy_pop_parser_stack(yyParser *pParser){
  yyStackEntry *yytos;
  assert( pParser->yytos!=0 );
  assert( pParser->yytos > pParser->yystack );
  yytos = pParser->yytos--;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yy_destructor(pParser, yytos->major, &yytos->minor);
}

/*
** Clear all secondary memory allocations from the parser
*/
namespace  {
void ParseFinalize(void *p){
  yyParser *pParser = (yyParser*)p;
  while( pParser->yytos>pParser->yystack ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  if( pParser->yystack!=&pParser->yystk0 ) free(pParser->yystack);
#endif
}
} // End namespace 

#ifndef Parse_ENGINEALWAYSONSTACK
/*
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
namespace  {
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
#ifndef YYPARSEFREENEVERNULL
  if( p==0 ) return;
#endif
  ParseFinalize(p);
  (*freeProc)(p);
}
} // End namespace 
#endif /* Parse_ENGINEALWAYSONSTACK */

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
namespace  {
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyhwm;
}
} // End namespace 
#endif

/* This array of booleans keeps track of the parser statement
** coverage.  The element yycoverage[X][Y] is set when the parser
** is in state X and has a lookahead token Y.  In a well-tested
** systems, every element of this matrix should end up being set.
*/
#if defined(YYCOVERAGE)
static unsigned char yycoverage[YYNSTATE][YYNTOKEN];
#endif

/*
** Write into out a description of every state/lookahead combination that
**
**   (1)  has not been used by the parser, and
**   (2)  is not a syntax error.
**
** Return the number of missed state/lookahead combinations.
*/
#if defined(YYCOVERAGE)
namespace  {
int ParseCoverage(FILE *out){
  int stateno, iLookAhead, i;
  int nMissed = 0;
  for(stateno=0; stateno<YYNSTATE; stateno++){
    i = yy_shift_ofst[stateno];
    for(iLookAhead=0; iLookAhead<YYNTOKEN; iLookAhead++){
      if( yy_lookahead[i+iLookAhead]!=iLookAhead ) continue;
      if( yycoverage[stateno][iLookAhead]==0 ) nMissed++;
      if( out ){
        fprintf(out,"State %d lookahead %s %s\n", stateno,
                yyTokenName[iLookAhead],
                yycoverage[stateno][iLookAhead] ? "ok" : "missed");
      }
    }
  }
  return nMissed;
}
} // End namespace 
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static YYACTIONTYPE yy_find_shift_action(
  YYCODETYPE iLookAhead,    /* The look-ahead token */
  YYACTIONTYPE stateno      /* Current state number */
){
  int i;

  if( stateno>YY_MAX_SHIFT ) return stateno;
  assert( stateno <= YY_SHIFT_COUNT );
#if defined(YYCOVERAGE)
  yycoverage[stateno][iLookAhead] = 1;
#endif
  do{
    i = yy_shift_ofst[stateno];
    assert( i>=0 );
    assert( i<=YY_ACTTAB_COUNT );
    assert( i+YYNTOKEN<=(int)YY_NLOOKAHEAD );
    assert( iLookAhead!=YYNOCODE );
    assert( iLookAhead < YYNTOKEN );
    i += iLookAhead;
    assert( i<(int)YY_NLOOKAHEAD );
    if( yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      assert( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0]) );
      iFallback = yyFallback[iLookAhead];
      if( iFallback!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        assert( yyFallback[iFallback]==0 ); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        assert( j<(int)(sizeof(yy_lookahead)/sizeof(yy_lookahead[0])) );
        if( yy_lookahead[j]==YYWILDCARD && iLookAhead>0 ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead],
               yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
      return yy_default[stateno];
    }else{
      assert( i>=0 && i<sizeof(yy_action)/sizeof(yy_action[0]) );
      return yy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static YYACTIONTYPE yy_find_reduce_action(
  YYACTIONTYPE stateno,     /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser){
   ParseARG_FETCH
   ParseCTX_FETCH
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
/******** End %stack_overflow code ********************************************/
   ParseARG_STORE /* Suppress warning about unused %extra_argument var */
   ParseCTX_STORE
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void yyTraceShift(yyParser *yypParser, int yyNewState, const char *zTag){
  if( yyTraceFILE ){
    if( yyNewState<YYNSTATE ){
      fprintf(yyTraceFILE,"%s%s '%s', go to state %d\n",
         yyTracePrompt, zTag, yyTokenName[yypParser->yytos->major],
         yyNewState);
    }else{
      fprintf(yyTraceFILE,"%s%s '%s', pending reduce %d\n",
         yyTracePrompt, zTag, yyTokenName[yypParser->yytos->major],
         yyNewState - YY_MIN_REDUCE);
    }
  }
}
#else
# define yyTraceShift(X,Y,Z)
#endif

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  YYACTIONTYPE yyNewState,      /* The new state to shift in */
  YYCODETYPE yyMajor,           /* The major token to shift in */
  ParseTOKENTYPE yyMinor        /* The minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yytos++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
    yypParser->yyhwm++;
    assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack) );
  }
#endif
#if YYSTACKDEPTH>0
  if( yypParser->yytos>yypParser->yystackEnd ){
    yypParser->yytos--;
    yyStackOverflow(yypParser);
    return;
  }
#else
  if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz] ){
    if( yyGrowStack(yypParser) ){
      yypParser->yytos--;
      yyStackOverflow(yypParser);
      return;
    }
  }
#endif
  if( yyNewState > YY_MAX_SHIFT ){
    yyNewState += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
  }
  yytos = yypParser->yytos;
  yytos->stateno = yyNewState;
  yytos->major = yyMajor;
  yytos->minor.yy0 = yyMinor;
  yyTraceShift(yypParser, yyNewState, "Shift");
}

/* For rule J, yyRuleInfoLhs[J] contains the symbol on the left-hand side
** of that rule */
static const YYCODETYPE yyRuleInfoLhs[] = {
    40,  /* (0) evaluate ::= exp */
    39,  /* (1) exp ::= NUMBER */
    39,  /* (2) exp ::= MINUS exp */
    39,  /* (3) exp ::= exp PLUS exp */
    39,  /* (4) exp ::= exp MINUS exp */
    39,  /* (5) exp ::= exp TIMES exp */
    39,  /* (6) exp ::= exp DIVIDE exp */
    39,  /* (7) exp ::= LPAREN exp RPAREN */
    39,  /* (8) exp ::= PI LPAREN RPAREN */
    39,  /* (9) exp ::= DEG_TO_RAD LPAREN RPAREN */
    39,  /* (10) exp ::= RAD_TO_DEG LPAREN RPAREN */
    39,  /* (11) exp ::= DEG_TO_RAD LPAREN exp RPAREN */
    39,  /* (12) exp ::= RAD_TO_DEG LPAREN exp RPAREN */
    39,  /* (13) exp ::= EXP LPAREN exp RPAREN */
    39,  /* (14) exp ::= LOG LPAREN exp RPAREN */
    39,  /* (15) exp ::= LOG10 LPAREN exp RPAREN */
    39,  /* (16) exp ::= POW LPAREN exp COMMA exp RPAREN */
    39,  /* (17) exp ::= SQR LPAREN exp RPAREN */
    39,  /* (18) exp ::= SQRT LPAREN exp RPAREN */
    39,  /* (19) exp ::= CBRT LPAREN exp RPAREN */
    39,  /* (20) exp ::= SIN LPAREN exp RPAREN */
    39,  /* (21) exp ::= COS LPAREN exp RPAREN */
    39,  /* (22) exp ::= TAN LPAREN exp RPAREN */
    39,  /* (23) exp ::= ASIN LPAREN exp RPAREN */
    39,  /* (24) exp ::= ACOS LPAREN exp RPAREN */
    39,  /* (25) exp ::= ATAN LPAREN exp RPAREN */
    39,  /* (26) exp ::= ATAN2 LPAREN exp COMMA exp RPAREN */
    39,  /* (27) exp ::= HYPOT LPAREN exp COMMA exp RPAREN */
    39,  /* (28) exp ::= SINH LPAREN exp RPAREN */
    39,  /* (29) exp ::= COSH LPAREN exp RPAREN */
    39,  /* (30) exp ::= TANH LPAREN exp RPAREN */
    39,  /* (31) exp ::= MIN LPAREN exp COMMA exp RPAREN */
    39,  /* (32) exp ::= MAX LPAREN exp COMMA exp RPAREN */
    39,  /* (33) exp ::= MAG LPAREN exp RPAREN */
    39,  /* (34) exp ::= MAGSQR LPAREN exp RPAREN */
    39,  /* (35) exp ::= FLOOR LPAREN exp RPAREN */
    39,  /* (36) exp ::= CEIL LPAREN exp RPAREN */
    39,  /* (37) exp ::= ROUND LPAREN exp RPAREN */
    39,  /* (38) exp ::= RAND LPAREN RPAREN */
    39,  /* (39) exp ::= RAND LPAREN exp RPAREN */
};

/* For rule J, yyRuleInfoNRhs[J] contains the negative of the number
** of symbols on the right-hand side of that rule. */
static const signed char yyRuleInfoNRhs[] = {
   -1,  /* (0) evaluate ::= exp */
   -1,  /* (1) exp ::= NUMBER */
   -2,  /* (2) exp ::= MINUS exp */
   -3,  /* (3) exp ::= exp PLUS exp */
   -3,  /* (4) exp ::= exp MINUS exp */
   -3,  /* (5) exp ::= exp TIMES exp */
   -3,  /* (6) exp ::= exp DIVIDE exp */
   -3,  /* (7) exp ::= LPAREN exp RPAREN */
   -3,  /* (8) exp ::= PI LPAREN RPAREN */
   -3,  /* (9) exp ::= DEG_TO_RAD LPAREN RPAREN */
   -3,  /* (10) exp ::= RAD_TO_DEG LPAREN RPAREN */
   -4,  /* (11) exp ::= DEG_TO_RAD LPAREN exp RPAREN */
   -4,  /* (12) exp ::= RAD_TO_DEG LPAREN exp RPAREN */
   -4,  /* (13) exp ::= EXP LPAREN exp RPAREN */
   -4,  /* (14) exp ::= LOG LPAREN exp RPAREN */
   -4,  /* (15) exp ::= LOG10 LPAREN exp RPAREN */
   -6,  /* (16) exp ::= POW LPAREN exp COMMA exp RPAREN */
   -4,  /* (17) exp ::= SQR LPAREN exp RPAREN */
   -4,  /* (18) exp ::= SQRT LPAREN exp RPAREN */
   -4,  /* (19) exp ::= CBRT LPAREN exp RPAREN */
   -4,  /* (20) exp ::= SIN LPAREN exp RPAREN */
   -4,  /* (21) exp ::= COS LPAREN exp RPAREN */
   -4,  /* (22) exp ::= TAN LPAREN exp RPAREN */
   -4,  /* (23) exp ::= ASIN LPAREN exp RPAREN */
   -4,  /* (24) exp ::= ACOS LPAREN exp RPAREN */
   -4,  /* (25) exp ::= ATAN LPAREN exp RPAREN */
   -6,  /* (26) exp ::= ATAN2 LPAREN exp COMMA exp RPAREN */
   -6,  /* (27) exp ::= HYPOT LPAREN exp COMMA exp RPAREN */
   -4,  /* (28) exp ::= SINH LPAREN exp RPAREN */
   -4,  /* (29) exp ::= COSH LPAREN exp RPAREN */
   -4,  /* (30) exp ::= TANH LPAREN exp RPAREN */
   -6,  /* (31) exp ::= MIN LPAREN exp COMMA exp RPAREN */
   -6,  /* (32) exp ::= MAX LPAREN exp COMMA exp RPAREN */
   -4,  /* (33) exp ::= MAG LPAREN exp RPAREN */
   -4,  /* (34) exp ::= MAGSQR LPAREN exp RPAREN */
   -4,  /* (35) exp ::= FLOOR LPAREN exp RPAREN */
   -4,  /* (36) exp ::= CEIL LPAREN exp RPAREN */
   -4,  /* (37) exp ::= ROUND LPAREN exp RPAREN */
   -3,  /* (38) exp ::= RAND LPAREN RPAREN */
   -4,  /* (39) exp ::= RAND LPAREN exp RPAREN */
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
**
** The yyLookahead and yyLookaheadToken parameters provide reduce actions
** access to the lookahead token (if any).  The yyLookahead will be YYNOCODE
** if the lookahead token has already been consumed.  As this procedure is
** only called from one place, optimizing compilers will in-line it, which
** means that the extra parameters have no performance impact.
*/
static YYACTIONTYPE yy_reduce(
  yyParser *yypParser,         /* The parser */
  unsigned int yyruleno,       /* Number of the rule by which to reduce */
  int yyLookahead,             /* Lookahead token, or YYNOCODE if none */
  ParseTOKENTYPE yyLookaheadToken  /* Value of the lookahead token */
  ParseCTX_PDECL                   /* %extra_context */
){
  int yygoto;                     /* The next state */
  YYACTIONTYPE yyact;             /* The next action */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH
  (void)yyLookahead;
  (void)yyLookaheadToken;
  yymsp = yypParser->yytos;
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    yysize = yyRuleInfoNRhs[yyruleno];
    if( yysize ){
      fprintf(yyTraceFILE, "%sReduce %d [%s], go to state %d.\n",
        yyTracePrompt,
        yyruleno, yyRuleName[yyruleno], yymsp[yysize].stateno);
    }else{
      fprintf(yyTraceFILE, "%sReduce %d [%s].\n",
        yyTracePrompt, yyruleno, yyRuleName[yyruleno]);
    }
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( yyRuleInfoNRhs[yyruleno]==0 ){
#ifdef YYTRACKMAXSTACKDEPTH
    if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
      yypParser->yyhwm++;
      assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack));
    }
#endif
#if YYSTACKDEPTH>0
    if( yypParser->yytos>=yypParser->yystackEnd ){
      yyStackOverflow(yypParser);
      /* The call to yyStackOverflow() above pops the stack until it is
      ** empty, causing the main parser loop to exit.  So the return value
      ** is never used and does not matter. */
      return 0;
    }
#else
    if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz-1] ){
      if( yyGrowStack(yypParser) ){
        yyStackOverflow(yypParser);
        /* The call to yyStackOverflow() above pops the stack until it is
        ** empty, causing the main parser loop to exit.  So the return value
        ** is never used and does not matter. */
        return 0;
      }
      yymsp = yypParser->yytos;
    }
#endif
  }

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        YYMINORTYPE yylhsminor;
      case 0: /* evaluate ::= exp */
#line 62 "evalStringToScalarLemonParser.lyy"
{
    driver->setValue(yymsp[0].minor.yy11);
}
#line 1163 "evalStringToScalarLemonParser.cc"
        break;
      case 1: /* exp ::= NUMBER */
#line 66 "evalStringToScalarLemonParser.lyy"
{ yylhsminor.yy11 = yymsp[0].minor.yy0; }
#line 1168 "evalStringToScalarLemonParser.cc"
  yymsp[0].minor.yy11 = yylhsminor.yy11;
        break;
      case 2: /* exp ::= MINUS exp */
#line 67 "evalStringToScalarLemonParser.lyy"
{ yymsp[-1].minor.yy11 = -yymsp[0].minor.yy11; }
#line 1174 "evalStringToScalarLemonParser.cc"
        break;
      case 3: /* exp ::= exp PLUS exp */
#line 71 "evalStringToScalarLemonParser.lyy"
{ yylhsminor.yy11 = yymsp[-2].minor.yy11 + yymsp[0].minor.yy11; }
#line 1179 "evalStringToScalarLemonParser.cc"
  yymsp[-2].minor.yy11 = yylhsminor.yy11;
        break;
      case 4: /* exp ::= exp MINUS exp */
#line 72 "evalStringToScalarLemonParser.lyy"
{ yylhsminor.yy11 = yymsp[-2].minor.yy11 - yymsp[0].minor.yy11; }
#line 1185 "evalStringToScalarLemonParser.cc"
  yymsp[-2].minor.yy11 = yylhsminor.yy11;
        break;
      case 5: /* exp ::= exp TIMES exp */
#line 73 "evalStringToScalarLemonParser.lyy"
{ yylhsminor.yy11 = yymsp[-2].minor.yy11 * yymsp[0].minor.yy11; }
#line 1191 "evalStringToScalarLemonParser.cc"
  yymsp[-2].minor.yy11 = yylhsminor.yy11;
        break;
      case 6: /* exp ::= exp DIVIDE exp */
#line 74 "evalStringToScalarLemonParser.lyy"
{ yylhsminor.yy11 = yymsp[-2].minor.yy11 / yymsp[0].minor.yy11; }
#line 1197 "evalStringToScalarLemonParser.cc"
  yymsp[-2].minor.yy11 = yylhsminor.yy11;
        break;
      case 7: /* exp ::= LPAREN exp RPAREN */
#line 75 "evalStringToScalarLemonParser.lyy"
{ yymsp[-2].minor.yy11 = yymsp[-1].minor.yy11; }
#line 1203 "evalStringToScalarLemonParser.cc"
        break;
      case 8: /* exp ::= PI LPAREN RPAREN */
#line 80 "evalStringToScalarLemonParser.lyy"
{ yymsp[-2].minor.yy11 = CML::constant::mathematical::pi; }
#line 1208 "evalStringToScalarLemonParser.cc"
        break;
      case 9: /* exp ::= DEG_TO_RAD LPAREN RPAREN */
#line 81 "evalStringToScalarLemonParser.lyy"
{ yymsp[-2].minor.yy11 = CML::degToRad(); }
#line 1213 "evalStringToScalarLemonParser.cc"
        break;
      case 10: /* exp ::= RAD_TO_DEG LPAREN RPAREN */
#line 82 "evalStringToScalarLemonParser.lyy"
{ yymsp[-2].minor.yy11 = CML::radToDeg(); }
#line 1218 "evalStringToScalarLemonParser.cc"
        break;
      case 11: /* exp ::= DEG_TO_RAD LPAREN exp RPAREN */
#line 88 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::degToRad(yymsp[-1].minor.yy11);
}
#line 1225 "evalStringToScalarLemonParser.cc"
        break;
      case 12: /* exp ::= RAD_TO_DEG LPAREN exp RPAREN */
#line 93 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::radToDeg(yymsp[-1].minor.yy11);
}
#line 1232 "evalStringToScalarLemonParser.cc"
        break;
      case 13: /* exp ::= EXP LPAREN exp RPAREN */
#line 98 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::exp(yymsp[-1].minor.yy11);
}
#line 1239 "evalStringToScalarLemonParser.cc"
        break;
      case 14: /* exp ::= LOG LPAREN exp RPAREN */
#line 103 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::log(yymsp[-1].minor.yy11);
}
#line 1246 "evalStringToScalarLemonParser.cc"
        break;
      case 15: /* exp ::= LOG10 LPAREN exp RPAREN */
#line 108 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::log10(yymsp[-1].minor.yy11);
}
#line 1253 "evalStringToScalarLemonParser.cc"
        break;
      case 16: /* exp ::= POW LPAREN exp COMMA exp RPAREN */
#line 113 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-5].minor.yy11 = CML::pow(yymsp[-3].minor.yy11, yymsp[-1].minor.yy11);
}
#line 1260 "evalStringToScalarLemonParser.cc"
        break;
      case 17: /* exp ::= SQR LPAREN exp RPAREN */
#line 118 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::sqr(yymsp[-1].minor.yy11);
}
#line 1267 "evalStringToScalarLemonParser.cc"
        break;
      case 18: /* exp ::= SQRT LPAREN exp RPAREN */
#line 123 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::sqrt(yymsp[-1].minor.yy11);
}
#line 1274 "evalStringToScalarLemonParser.cc"
        break;
      case 19: /* exp ::= CBRT LPAREN exp RPAREN */
#line 128 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::cbrt(yymsp[-1].minor.yy11);
}
#line 1281 "evalStringToScalarLemonParser.cc"
        break;
      case 20: /* exp ::= SIN LPAREN exp RPAREN */
#line 133 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::sin(yymsp[-1].minor.yy11);
}
#line 1288 "evalStringToScalarLemonParser.cc"
        break;
      case 21: /* exp ::= COS LPAREN exp RPAREN */
#line 138 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::cos(yymsp[-1].minor.yy11);
}
#line 1295 "evalStringToScalarLemonParser.cc"
        break;
      case 22: /* exp ::= TAN LPAREN exp RPAREN */
#line 143 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::tan(yymsp[-1].minor.yy11);
}
#line 1302 "evalStringToScalarLemonParser.cc"
        break;
      case 23: /* exp ::= ASIN LPAREN exp RPAREN */
#line 148 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::asin(yymsp[-1].minor.yy11);
}
#line 1309 "evalStringToScalarLemonParser.cc"
        break;
      case 24: /* exp ::= ACOS LPAREN exp RPAREN */
#line 153 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::acos(yymsp[-1].minor.yy11);
}
#line 1316 "evalStringToScalarLemonParser.cc"
        break;
      case 25: /* exp ::= ATAN LPAREN exp RPAREN */
#line 158 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::atan(yymsp[-1].minor.yy11);
}
#line 1323 "evalStringToScalarLemonParser.cc"
        break;
      case 26: /* exp ::= ATAN2 LPAREN exp COMMA exp RPAREN */
#line 163 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-5].minor.yy11 = CML::atan2(yymsp[-3].minor.yy11, yymsp[-1].minor.yy11);
}
#line 1330 "evalStringToScalarLemonParser.cc"
        break;
      case 27: /* exp ::= HYPOT LPAREN exp COMMA exp RPAREN */
#line 168 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-5].minor.yy11 = CML::hypot(yymsp[-3].minor.yy11, yymsp[-1].minor.yy11);
}
#line 1337 "evalStringToScalarLemonParser.cc"
        break;
      case 28: /* exp ::= SINH LPAREN exp RPAREN */
#line 173 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::sinh(yymsp[-1].minor.yy11);
}
#line 1344 "evalStringToScalarLemonParser.cc"
        break;
      case 29: /* exp ::= COSH LPAREN exp RPAREN */
#line 178 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::cosh(yymsp[-1].minor.yy11);
}
#line 1351 "evalStringToScalarLemonParser.cc"
        break;
      case 30: /* exp ::= TANH LPAREN exp RPAREN */
#line 183 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::tanh(yymsp[-1].minor.yy11);
}
#line 1358 "evalStringToScalarLemonParser.cc"
        break;
      case 31: /* exp ::= MIN LPAREN exp COMMA exp RPAREN */
#line 188 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-5].minor.yy11 = CML::min(yymsp[-3].minor.yy11, yymsp[-1].minor.yy11);
}
#line 1365 "evalStringToScalarLemonParser.cc"
        break;
      case 32: /* exp ::= MAX LPAREN exp COMMA exp RPAREN */
#line 193 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-5].minor.yy11 = CML::max(yymsp[-3].minor.yy11, yymsp[-1].minor.yy11);
}
#line 1372 "evalStringToScalarLemonParser.cc"
        break;
      case 33: /* exp ::= MAG LPAREN exp RPAREN */
#line 198 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::mag(yymsp[-1].minor.yy11);
}
#line 1379 "evalStringToScalarLemonParser.cc"
        break;
      case 34: /* exp ::= MAGSQR LPAREN exp RPAREN */
#line 203 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::magSqr(yymsp[-1].minor.yy11);
}
#line 1386 "evalStringToScalarLemonParser.cc"
        break;
      case 35: /* exp ::= FLOOR LPAREN exp RPAREN */
#line 208 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = std::floor(yymsp[-1].minor.yy11);
}
#line 1393 "evalStringToScalarLemonParser.cc"
        break;
      case 36: /* exp ::= CEIL LPAREN exp RPAREN */
#line 213 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = std::ceil(yymsp[-1].minor.yy11);
}
#line 1400 "evalStringToScalarLemonParser.cc"
        break;
      case 37: /* exp ::= ROUND LPAREN exp RPAREN */
#line 218 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = std::round(yymsp[-1].minor.yy11);
}
#line 1407 "evalStringToScalarLemonParser.cc"
        break;
      case 38: /* exp ::= RAND LPAREN RPAREN */
#line 223 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-2].minor.yy11 = CML::Random().sample01<CML::scalar>();
}
#line 1414 "evalStringToScalarLemonParser.cc"
        break;
      case 39: /* exp ::= RAND LPAREN exp RPAREN */
#line 228 "evalStringToScalarLemonParser.lyy"
{
    yymsp[-3].minor.yy11 = CML::Random(yymsp[-1].minor.yy11).sample01<CML::scalar>();
}
#line 1421 "evalStringToScalarLemonParser.cc"
        break;
      default:
        break;
/********** End reduce actions ************************************************/
  };
  assert( yyruleno<sizeof(yyRuleInfoLhs)/sizeof(yyRuleInfoLhs[0]) );
  yygoto = yyRuleInfoLhs[yyruleno];
  yysize = yyRuleInfoNRhs[yyruleno];
  yyact = yy_find_reduce_action(yymsp[yysize].stateno,(YYCODETYPE)yygoto);

  /* There are no SHIFTREDUCE actions on nonterminals because the table
  ** generator has simplified them to pure REDUCE actions. */
  assert( !(yyact>YY_MAX_SHIFT && yyact<=YY_MAX_SHIFTREDUCE) );

  /* It is not possible for a REDUCE to be followed by an error */
  assert( yyact!=YY_ERROR_ACTION );

  yymsp += yysize+1;
  yypParser->yytos = yymsp;
  yymsp->stateno = (YYACTIONTYPE)yyact;
  yymsp->major = (YYCODETYPE)yygoto;
  yyTraceShift(yypParser, yyact, "... then shift");
  return yyact;
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH
  ParseCTX_FETCH
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
#line 45 "evalStringToScalarLemonParser.lyy"
 driver->reportFatal("Parse failure, giving up..."); 
#line 1467 "evalStringToScalarLemonParser.cc"
/************ End %parse_failure code *****************************************/
  ParseARG_STORE /* Suppress warning about unused %extra_argument variable */
  ParseCTX_STORE
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  ParseTOKENTYPE yyminor         /* The minor type of the error token */
){
  ParseARG_FETCH
  ParseCTX_FETCH
#define TOKEN yyminor
/************ Begin %syntax_error code ****************************************/
#line 46 "evalStringToScalarLemonParser.lyy"
 driver->reportFatal("Syntax error"); 
#line 1488 "evalStringToScalarLemonParser.cc"
/************ End %syntax_error code ******************************************/
  ParseARG_STORE /* Suppress warning about unused %extra_argument variable */
  ParseCTX_STORE
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH
  ParseCTX_FETCH
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
#ifndef YYNOERRORRECOVERY
  yypParser->yyerrcnt = -1;
#endif
  assert( yypParser->yytos==yypParser->yystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  ParseARG_STORE /* Suppress warning about unused %extra_argument variable */
  ParseCTX_STORE
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
namespace  {
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  YYACTIONTYPE yyact;   /* The parser action. */
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  int yyendofinput;     /* True if we are at the end of input */
#endif
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser = (yyParser*)yyp;  /* The parser */
  ParseCTX_FETCH
  ParseARG_STORE

  assert( yypParser->yytos!=0 );
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  yyendofinput = (yymajor==0);
#endif

  yyact = yypParser->yytos->stateno;
#ifndef NDEBUG
  if( yyTraceFILE ){
    if( yyact < YY_MIN_REDUCE ){
      fprintf(yyTraceFILE,"%sInput '%s' in state %d\n",
              yyTracePrompt,yyTokenName[yymajor],yyact);
    }else{
      fprintf(yyTraceFILE,"%sInput '%s' with pending reduce %d\n",
              yyTracePrompt,yyTokenName[yymajor],yyact-YY_MIN_REDUCE);
    }
  }
#endif

  do{
    assert( yyact==yypParser->yytos->stateno );
    yyact = yy_find_shift_action((YYCODETYPE)yymajor,yyact);
    if( yyact >= YY_MIN_REDUCE ){
      yyact = yy_reduce(yypParser,yyact-YY_MIN_REDUCE,yymajor,
                        yyminor ParseCTX_PARAM);
    }else if( yyact <= YY_MAX_SHIFTREDUCE ){
      yy_shift(yypParser,yyact,(YYCODETYPE)yymajor,yyminor);
#ifndef YYNOERRORRECOVERY
      yypParser->yyerrcnt--;
#endif
      break;
    }else if( yyact==YY_ACCEPT_ACTION ){
      yypParser->yytos--;
      yy_accept(yypParser);
      return;
    }else{
      assert( yyact == YY_ERROR_ACTION );
      yyminorunion.yy0 = yyminor;
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminor);
      }
      yymx = yypParser->yytos->major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
        yymajor = YYNOCODE;
      }else{
        while( yypParser->yytos >= yypParser->yystack
            && (yyact = yy_find_reduce_action(
                        yypParser->yytos->stateno,
                        YYERRORSYMBOL)) > YY_MAX_SHIFTREDUCE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yytos < yypParser->yystack || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
          yypParser->yyerrcnt = -1;
#endif
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          yy_shift(yypParser,yyact,YYERRORSYMBOL,yyminor);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
      if( yymajor==YYNOCODE ) break;
      yyact = yypParser->yytos->stateno;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor, yyminor);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      break;
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor, yyminor);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
        yypParser->yyerrcnt = -1;
#endif
      }
      break;
#endif
    }
  }while( yypParser->yytos>yypParser->yystack );
#ifndef NDEBUG
  if( yyTraceFILE ){
    yyStackEntry *i;
    char cDiv = '[';
    fprintf(yyTraceFILE,"%sReturn. Stack=",yyTracePrompt);
    for(i=&yypParser->yystack[1]; i<=yypParser->yytos; i++){
      fprintf(yyTraceFILE,"%c%s", cDiv, yyTokenName[i->major]);
      cDiv = ' ';
    }
    fprintf(yyTraceFILE,"]\n");
  }
#endif
  return;
}
} // End namespace 

/*
** Return the fallback token corresponding to canonical token iToken, or
** 0 if iToken has no fallback.
*/
namespace  {
int ParseFallback(int iToken){
#ifdef YYFALLBACK
  assert( iToken<(int)(sizeof(yyFallback)/sizeof(yyFallback[0])) );
  return yyFallback[iToken];
#else
  (void)iToken;
#endif
  return 0;
}
} // End namespace 
#line 236 "evalStringToScalarLemonParser.lyy"


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CML::parsing::evalStringToScalar::parser::stop()
{
    if (lemon_)
    {
        ParseFree(lemon_, ::operator delete);
        lemon_ = nullptr;
    }
}


void CML::parsing::evalStringToScalar::parser::start(parseDriver& driver)
{
    this->stop();
    lemon_ = ParseAlloc(::operator new, &driver);
}


void CML::parsing::evalStringToScalar::parser::parse
(
    int tokenId,
    CML::scalar val    /* The value for the token */
)
{
    Parse(lemon_, tokenId, val);
}


CML::word CML::parsing::evalStringToScalar::parser::nameOfToken
(
    int tokenId
) const
{
    #ifndef NDEBUG
    if
    (
        tokenId > 0
     && unsigned(tokenId) < (sizeof(yyTokenName) / sizeof(char*))
    )
    {
        return yyTokenName[tokenId];
    }
    return "<invalid>";
    #else
    return word();
    #endif
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#line 1780 "evalStringToScalarLemonParser.cc"
