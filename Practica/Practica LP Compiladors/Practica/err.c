/*
 * A n t l r  S e t s / E r r o r  F i l e  H e a d e r
 *
 * Generated from: legoChiv.g
 *
 * Terence Parr, Russell Quong, Will Cohen, and Hank Dietz: 1989-2001
 * Parr Research Corporation
 * with Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 */

#define ANTLR_VERSION	13333
#include "pcctscfg.h"
#include "pccts_stdio.h"

#include <string>
#include <iostream>
using namespace std;

// struct to store information about tokens
typedef struct {
  string kind;
  string text;
} Attrib;

// function to fill token information (predeclaration)
void zzcr_attr(Attrib *attr, int type, char *text);

// fields for AST nodes
#define AST_FIELDS string kind; string text;
#include "ast.h"

// macro to create a new AST node (and function predeclaration)
#define zzcr_ast(as,attr,ttype,textt) as=createASTnode(attr,ttype,textt)
AST* createASTnode(Attrib* attr,int ttype, char *textt);
#define zzSET_SIZE 4
#include "antlr.h"
#include "ast.h"
#include "tokens.h"
#include "dlgdef.h"
#include "err.h"

ANTLRChar *zztokens[26]={
	/* 00 */	"Invalid",
	/* 01 */	"@",
	/* 02 */	"GRID",
	/* 03 */	"FITS",
	/* 04 */	"PUSH",
	/* 05 */	"POP",
	/* 06 */	"HEIGHT",
	/* 07 */	"PLACE",
	/* 08 */	"AT",
	/* 09 */	"MOVE",
	/* 10 */	"DIRECTION",
	/* 11 */	"WHILE",
	/* 12 */	"DEF",
	/* 13 */	"ENDEF",
	/* 14 */	"LP",
	/* 15 */	"RP",
	/* 16 */	"LSB",
	/* 17 */	"RSB",
	/* 18 */	"COMA",
	/* 19 */	"COMP",
	/* 20 */	"AND",
	/* 21 */	"ASIG",
	/* 22 */	"NUM",
	/* 23 */	"ID",
	/* 24 */	"FUNC",
	/* 25 */	"SPACE"
};
SetWordType zzerr1[4] = {0x40,0xa,0x80,0x1};
SetWordType zzerr2[4] = {0x30,0x0,0x0,0x0};
SetWordType zzerr3[4] = {0x0,0x40,0x80,0x0};
SetWordType zzerr4[4] = {0x80,0x40,0x80,0x0};
SetWordType setwd1[26] = {0x0,0x7b,0x0,0x0,0x0,0x0,0x46,
	0x0,0x0,0x46,0x0,0x46,0x6a,0x48,0x80,
	0x0,0x0,0x48,0x0,0x0,0x0,0x0,0x0,
	0xc6,0x46,0x0};
SetWordType zzerr5[4] = {0x30,0x0,0x0,0x0};
SetWordType zzerr6[4] = {0x72,0x3a,0x82,0x1};
SetWordType zzerr7[4] = {0x30,0x0,0x0,0x0};
SetWordType zzerr8[4] = {0x0,0x40,0x80,0x0};
SetWordType zzerr9[4] = {0x40,0x0,0x40,0x0};
SetWordType zzerr10[4] = {0x40,0x0,0x40,0x0};
SetWordType zzerr11[4] = {0x48,0x0,0x40,0x0};
SetWordType zzerr12[4] = {0x0,0x80,0x10,0x0};
SetWordType setwd2[26] = {0x0,0xcd,0x0,0x0,0x82,0x82,0xdd,
	0x0,0x80,0xcd,0x0,0xcd,0xcd,0xcd,0x0,
	0x60,0x0,0xcd,0x0,0x40,0x40,0x0,0x10,
	0xcd,0xcd,0x0};
SetWordType setwd3[26] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x1,0x0,0x0,0x1,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0};
