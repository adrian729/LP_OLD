#ifndef tokens_h
#define tokens_h
/* tokens.h -- List of labelled tokens and stuff
 *
 * Generated from: legoChiv.g
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * ANTLR Version 1.33MR33
 */
#define zzEOF_TOKEN 1
#define GRID 2
#define FITS 3
#define PUSH 4
#define POP 5
#define HEIGHT 6
#define PLACE 7
#define AT 8
#define MOVE 9
#define DIRECTION 10
#define WHILE 11
#define DEF 12
#define ENDEF 13
#define LP 14
#define RP 15
#define LSB 16
#define RSB 17
#define COMA 18
#define COMP 19
#define AND 20
#define ASIG 21
#define NUM 22
#define ID 23
#define FUNC 24
#define SPACE 25

#ifdef __USE_PROTOS
void lego(AST**_root);
#else
extern void lego();
#endif

#ifdef __USE_PROTOS
void grid(AST**_root);
#else
extern void grid();
#endif

#ifdef __USE_PROTOS
void ops(AST**_root);
#else
extern void ops();
#endif

#ifdef __USE_PROTOS
void defs(AST**_root);
#else
extern void defs();
#endif

#ifdef __USE_PROTOS
void functionDef(AST**_root);
#else
extern void functionDef();
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
void pexpr(AST**_root);
#else
extern void pexpr();
#endif

#ifdef __USE_PROTOS
void boolexpr(AST**_root);
#else
extern void boolexpr();
#endif

#ifdef __USE_PROTOS
void hexpr(AST**_root);
#else
extern void hexpr();
#endif

#ifdef __USE_PROTOS
void pxyp(AST**_root);
#else
extern void pxyp();
#endif

#ifdef __USE_PROTOS
void xy(AST**_root);
#else
extern void xy();
#endif

#endif
extern SetWordType zzerr1[];
extern SetWordType zzerr2[];
extern SetWordType zzerr3[];
extern SetWordType zzerr4[];
extern SetWordType setwd1[];
extern SetWordType zzerr5[];
extern SetWordType zzerr6[];
extern SetWordType zzerr7[];
extern SetWordType zzerr8[];
extern SetWordType zzerr9[];
extern SetWordType zzerr10[];
extern SetWordType zzerr11[];
extern SetWordType zzerr12[];
extern SetWordType setwd2[];
extern SetWordType setwd3[];
