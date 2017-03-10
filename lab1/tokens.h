#ifndef tokens_h
#define tokens_h
/* tokens.h -- List of labelled tokens and stuff
 *
 * Generated from: example1.g
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * ANTLR Version 1.33MR33
 */
#define zzEOF_TOKEN 1
#define WRITE 2
#define IF 3
#define THEN 4
#define ELSE 5
#define ENDIF 6
#define WHILE 7
#define DO 8
#define PLUS 9
#define TIMES 10
#define LP 11
#define RP 12
#define ASIG 13
#define COMP 14
#define TBOOL 15
#define FBOOL 16
#define NUM 17
#define ID 18
#define SPACE 19

#ifdef __USE_PROTOS
void program(AST**_root);
#else
extern void program();
#endif

#ifdef __USE_PROTOS
void instruction(AST**_root);
#else
extern void instruction();
#endif

#ifdef __USE_PROTOS
void expr(AST**_root);
#else
extern void expr();
#endif

#ifdef __USE_PROTOS
void term(AST**_root);
#else
extern void term();
#endif

#ifdef __USE_PROTOS
void subexpr(AST**_root);
#else
extern void subexpr();
#endif

#endif
extern SetWordType zzerr1[];
extern SetWordType zzerr2[];
extern SetWordType setwd1[];
