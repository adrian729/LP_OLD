/*
 * A n t l r  S e t s / E r r o r  F i l e  H e a d e r
 *
 * Generated from: example1.g
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
AST* createASTnode(Attrib* attr, int ttype, char *textt);
#define zzSET_SIZE 4
#include "antlr.h"
#include "ast.h"
#include "tokens.h"
#include "dlgdef.h"
#include "err.h"

ANTLRChar *zztokens[20]={
	/* 00 */	"Invalid",
	/* 01 */	"@",
	/* 02 */	"WRITE",
	/* 03 */	"IF",
	/* 04 */	"THEN",
	/* 05 */	"ELSE",
	/* 06 */	"ENDIF",
	/* 07 */	"WHILE",
	/* 08 */	"DO",
	/* 09 */	"PLUS",
	/* 10 */	"TIMES",
	/* 11 */	"LP",
	/* 12 */	"RP",
	/* 13 */	"ASIG",
	/* 14 */	"COMP",
	/* 15 */	"TBOOL",
	/* 16 */	"FBOOL",
	/* 17 */	"NUM",
	/* 18 */	"ID",
	/* 19 */	"SPACE"
};
SetWordType zzerr1[4] = {0x4,0x8,0x6,0x0};
SetWordType zzerr2[4] = {0x0,0x8,0x6,0x0};
SetWordType setwd1[20] = {0x0,0x7a,0x79,0x0,0x0,0x0,0x0,
	0x0,0x0,0x60,0x40,0x7d,0x70,0x0,0x70,
	0x0,0x0,0x7d,0x7d,0x0};
