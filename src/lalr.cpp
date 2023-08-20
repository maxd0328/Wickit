/* ---------------------------------------------------------- *
 * LALR Parse Table Generator for Wickit                      *
 * Max Derbenwick 2023                                        *
 *                                                            *
 * This is an auto-generated source file                      *
 * ---------------------------------------------------------- */

#include "buildw/tokenizer.h"
#include "buildw/parser.h"

using namespace wckt;
using namespace wckt::build;

uint32_t __ACTION_TABLE[MAX_TOKEN_PLUS_ONE][13] = {
	[Token::NUMBER] = {4, 0, 0, 4, 0, 0, 0, 0, 4, 4, 0, 0, 0},
	[Token::TIMES] = {0, 15, 7, 0, 0, 20, 13, 0, 0, 0, 5, 20, 3},
	[Token::LPAREN] = {8, 0, 0, 8, 0, 0, 0, 0, 8, 8, 0, 0, 0},
	[Token::RPAREN] = {0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0},
	[Token::END_OF_STREAM] = {0, 15, 7, 0, 1, 11, 13, 0, 0, 0, 5, 9, 3},
	[Token::PLUS] = {0, 15, 7, 0, 18, 11, 13, 18, 0, 0, 5, 9, 3},
};

uint32_t __GOTO_TABLE[3][13] = {
	[0] = {2, 0, 0, 2, 0, 0, 0, 0, 2, 12, 0, 0, 0},
	[1] = {4, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	[2] = {5, 0, 0, 5, 0, 0, 0, 0, 11, 0, 0, 0, 0},
};

production_t __PROD_TABLE[7] = {
	[0] = {.nterm = 2, .length = 3, .action = nullptr},
	[1] = {.nterm = 0, .length = 3, .action = nullptr},
	[2] = {.nterm = 2, .length = 1, .action = nullptr},
	[3] = {.nterm = 1, .length = 3, .action = nullptr},
	[4] = {.nterm = 1, .length = 1, .action = nullptr},
	[5] = {.nterm = 0, .length = 2, .action = nullptr},
	[6] = {.nterm = 0, .length = 1, .action = nullptr},
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
