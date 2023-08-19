/* ---------------------------------------------------------- *
 * LALR Parse Table Generator for Wickit                      *
 * Max Derbenwick 2023                                        *
 *                                                            *
 * This is an auto-generated source file                      *
 * ---------------------------------------------------------- */

#include "buildw/tokenizer.h"
#include "buildw/parser.h"

using namespace wckt::build;

uint32_t __ACTION_TABLE[MAX_TOKEN_PLUS_ONE][15] = {
	[Token::DELIM_DOT] = {0, 0, 0, 0, 0, 0, 7, 24, 13, 15, 0, 0, 0, 9, 0},
	[Token::KEYW_IMPORT] = {8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0},
	[Token::DELIM_SEMICOLON] = {0, 0, 0, 0, 0, 0, 7, 21, 13, 15, 26, 0, 0, 9, 5},
	[Token::IDENTIFIER] = {0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0},
	[Token::NO_NAME] = {0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0},
	[Token::END_OF_STREAM] = {19, 19, 1, 0, 3, 11, 0, 0, 0, 0, 0, 0, 17, 0, 0},
	[Token::OPERATOR_MUL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0},
};

uint32_t __GOTO_TABLE[6][15] = {
	[0] = {0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0},
	[1] = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	[2] = {2, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	[3] = {0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0},
	[4] = {0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	[5] = {4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

production_t __PROD_TABLE[10] = {
	[0] = {.nterm = 2, .length = 1},
	[1] = {.nterm = 0, .length = 2},
	[2] = {.nterm = 4, .length = 1},
	[3] = {.nterm = 4, .length = 3},
	[4] = {.nterm = 2, .length = 2},
	[5] = {.nterm = 3, .length = 1},
	[6] = {.nterm = 3, .length = 1},
	[7] = {.nterm = 1, .length = 4},
	[8] = {.nterm = 5, .length = 0},
	[9] = {.nterm = 0, .length = 0},
};

/* Parse table interface */

uint32_t lalraction(uint32_t __row, uint32_t __col)
{ return __ACTION_TABLE[__col][__row]; }
uint32_t lalrgoto(uint32_t __row, uint32_t __col)
{ return __GOTO_TABLE[__col][__row]; }
production_t lalrprod(uint32_t __row)
{ return __PROD_TABLE[__row]; }

/* ---------------------------------------------------------- *
 * End of auto-generated source file                          *
 * ---------------------------------------------------------- */
