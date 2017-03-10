/*
 * A n t l r  T r a n s l a t i o n  H e a d e r
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 *
 *   antlr -gt legoChiv.g
 *
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
#define GENAST

#include "ast.h"

#define zzSET_SIZE 4
#include "antlr.h"
#include "tokens.h"
#include "dlgdef.h"
#include "mode.h"

/* MR23 In order to remove calls to PURIFY use the antlr -nopurify option */

#ifndef PCCTS_PURIFY
#define PCCTS_PURIFY(r,s) memset((char *) &(r),'\0',(s));
#endif

#include "ast.c"
zzASTgvars

ANTLR_INFO

#include <cstdlib>
#include <cmath>
#include <map>
#include <vector>
#include <list>
#include <unordered_set>
#include <stack>


//DEFINICIO ESTRUCTURES DE DADES:

//Bloc
typedef struct {
  int bHeight;//altura a la que es trova el bloc
  int x, y;//cantonada sup. esq. del bloc
  int h, w;//dimensions
} tblock;

//Taulell
typedef struct {
  int n, m;
  vector< vector<int> > height;//Altura de cada punt de la graella.
  vector< vector< vector<int> > > gLevels;//Nivells de la graella (gLevels <- vector de "graelles". Graella <- -1 si no hi ha res, X > 0 amb posVec on es el bloc que ocupa la posició)
} Graella;

//Conjunt de blocs Bloc
map<string, int> posIDs;//posicio a blocks segons ID del bloc
vector<tblock> blocks;//vector de blocs

//Info necessaria per tornar a montar blocs desempilats amb un altre.
typedef struct {
  int x, y;//posicio del bloc respecte el de sota (comença per 0,0).
  int posVec;//posicio del bloc en el vector de blocs.
} popedBlock;

/*
1. int -> posVec del bloc X
2. list -> llista de popedBlock, on cada item representa un bloc dels que tenia a sobre el bloc X.
*/
//map< int, list<popedBlock> > sPopedBlocks;

//Funcions
map<string,AST*> funcs;//nom funcio i node inicial de les instruccions.

//global structures
AST *root;
Graella g;

// function to fill token information
void zzcr_attr(Attrib *attr, int type, char *text) {
  /*if (type == ID) {
    attr->kind = "id";
    attr->text = text;
  }
  else {*/
    attr->kind = text;
    attr->text = "";
    //}
}

// function to create a new AST node
AST* createASTnode(Attrib* attr, int type, char* text) {
  AST* as = new AST;
  as->kind = attr->kind;
  as->text = attr->text;
  as->right = NULL; 
  as->down = NULL;
  return as;
}


/// create a new "list" AST node with one element
AST* createASTlist(AST *child) {
  AST *as=new AST;
  as->kind="list";
  as->right=NULL;
  as->down=child;
  return as;
}

/// get nth child of a tree. Count starts at 0.
/// if no such child, returns NULL
AST* child(AST *a,int n) {
  AST *c=a->down;
  for (int i=0; c!=NULL && i<n; i++) c=c->right;
  return c;
}



/// print AST, recursively, with indentation
void ASTPrintIndent(AST *a,string s)
{
  if (a==NULL) return;
  
  cout<<a->kind;
  if (a->text!="") cout<<"("<<a->text<<")";
  cout<<endl;
  
  AST *i = a->down;
  while (i!=NULL && i->right!=NULL) {
    cout<<s+"  \\__";
    ASTPrintIndent(i,s+"  |"+string(i->kind.size()+i->text.size(),' '));
    i=i->right;
  }
  
  if (i!=NULL) {
    cout<<s+"  \\__";
    ASTPrintIndent(i,s+"   "+string(i->kind.size()+i->text.size(),' '));
    i=i->right;
  }
}

/// print AST 
void ASTPrint(AST *a)
{
  while (a!=NULL) {
    cout<<" ";
    ASTPrintIndent(a,"");
    a=a->right;
  }
}

//HELP FUNCS
void writeGrid(){
  cout << endl;
  cout << "Grid Levels:" << endl;
  for(int l = 0; l < g.gLevels.size(); ++l){
    for(int i = 0; i < g.m; ++i){
      for(int j = 0; j < g.n; ++j){
        if(g.gLevels[l][i][j] < 10) cout << " ";
        if(g.gLevels[l][i][j] >= 0) cout << " " << g.gLevels[l][i][j];
        else cout << " " << '*';
      }
      cout << endl;
    }
    cout << endl;
  }
  cout << endl;
  cout << "Grid:" << endl;
  for(int i = 0; i < g.m; ++i){
    for(int j = 0; j < g.n; ++j){
      if(g.height[i][j] < 10) cout << " ";
      if(g.height[i][j] > 0) cout << " " << g.height[i][j];
      else cout << " " << '*';
    }
    cout << endl;
  }
  
}


//SEMANTICS

/*
Funcio init graella.
*/
void executeGrid(AST *a){
  g.n = stoi(child(a,0)->kind);
  g.m = stoi(child(a,1)->kind);
  g.height = vector< vector<int> >(g.m, vector<int>(g.n, 0));
  g.gLevels = vector< vector< vector<int> > >(1, vector< vector<int> >(g.m, vector<int>(g.n, -1)));
}

/*
Afegeix l'arrel de les funcions definides a funcs.
*/
void addFunctions(AST *a){
funcs[child(a,0)->kind] = child(a,1);
}


//FUNCIONS DE LES INSTRUCCIONS:

/*
Executa la funcio HEIGHT. Retorna, si existeix el bloc i es a la graella, l'altura del bloc apuntat per la ID donada.
(ergo: a quina altura esta el bloc + l'altura maxima del que tingui per sobre)
*/
int height(AST *a){
//*CHI*/cout << "HEIGHT " << child(a, 0)->kind << endl;
//Si existeix el bloc especificat
if(posIDs.find(child(a, 0)->kind) != posIDs.end()){
tblock b = blocks[posIDs[child(a, 0)->kind]];
//Si el bloc esta a la graella
if(b.x > 0 and b.y > 0){
int retVal = -1;
int mx = b.x+b.h;
int my = b.y+b.w;
for(int i = b.y-1; i < my-1; ++i){
for(int j = b.x-1; j < mx-1; ++j){
if(g.height[i][j] > retVal){
retVal = g.height[i][j];
}   
//*CHI*/cout << i << " " << j << endl;
}
}
return (retVal - b.bHeight + 1);
}
}
return -1;
}

/*
Comprova si es pot possar un bloc de cert tamany en una posicio concreta. Si es pot, retorna a quina altura es pot possar.
Si no es pot, retorna -1.
*/
int canPutBlock(int x, int y, int h, int w){
//*CHI*/cout << "INI CANPUT ";
int mx = x+h-1;
int my = y+w-1;
if(x <= 0 or y <= 0 or mx > g.n or my > g.m){
/*CHI*/cout << x << " " << y << " " << mx << " " << my << " FORA DEL TAULELL!" << endl;
return -1;
}
int iniHeigh = g.height[y-1][x-1];
for(int i = y-1; i < my; ++i){
for(int j = x-1; j < mx; ++j){
if(g.height[i][j] != iniHeigh){
/*CHI*/cout << "SOLAPAMENT!" << endl;
return -1;
}   
//*CHI*/cout << i << " " << j << endl;
}
}
/*CHI*/cout << "OK!";
/*CHI*/cout << " FI CANPUT " << iniHeigh << endl;
return iniHeigh;
}

/*
Crea un block nou a la graella
x, y -> coord pos
h, w -> tamany
bHeight -> altura on es possa el bloc
vecPos -> posicio que ocupara al vector de blocs
*/
tblock createBlock(int x, int y, int h, int w, int bHeight, int vecPos){  
tblock nblock;
nblock.x = x;
nblock.y = y;
nblock.h = h;
nblock.w = w;
nblock.bHeight = bHeight;
int mx = x+h-1;
int my = y+w-1;
for(int i = y-1; i < my; ++i){
for(int j = x-1; j < mx; ++j){
++g.height[i][j];
//*CHI*/cout << i << " " << j << endl;
}
}
if(g.gLevels.size() < bHeight) g.gLevels.push_back(vector< vector<int> >(g.m, vector<int>(g.n, -1)));
for(int i = y-1; i < my; ++i){
for(int j = x-1; j < mx; ++j){
g.gLevels[bHeight-1][i][j] = vecPos;
}
}
return nblock;
}

/*
Comprova que si hi ha blocs "superiors compartits". Si hi ha retorna true, si no false.
*/
bool supBlocks(tblock b){
bool some = true;//si no trovem cap bloc no cal seguir
unordered_set<int> blocsLvl;//blocs trovats en cada nivell
for(int l = b.bHeight; some and l < g.gLevels.size(); ++l){
some = false;
//conjunt de blocs per sobre del bloc
blocsLvl.clear();
for(int i = b.y-1; i < b.y+b.w-1; ++i){
for(int j = b.x-1; j < b.x+b.h-1; ++j){
if(g.gLevels[l][i][j] >= 0){
blocsLvl.insert(g.gLevels[l][i][j]);
some = true;
}
}
}
//repassar al voltant:
//fila superior:
//*CHI*/cout << "fs" << b.y-2 << endl;
//*CHI*/cout << "fsQ" << b.x-2 << endl;
if(b.y-2 >= 0){
if(b.x-2 >= 0){
if(blocsLvl.find(g.gLevels[l][b.y-2][b.x-2]) != blocsLvl.end()){
/*CHI*/cout << "POP/MOVE FAILED! Comparteix algun bloc superior! fsQ" << endl;
return true;
}
}
for(int j = b.x-1; j < b.x+b.h-1; ++j){
if(blocsLvl.find(g.gLevels[l][b.y-2][j]) != blocsLvl.end()){
/*CHI*/cout << "POP/MOVE FAILED! Comparteix algun bloc superior! fs" << endl;
return true;
}
}
}
//fila inferior:
//*CHI*/cout << "fi" << b.y-1+b.w << endl;
//*CHI*/cout << "fiQ" << b.x-1+b.h << endl;
if(b.y-1+b.w < g.m){
if(b.x-2 >= 0){
if(blocsLvl.find(g.gLevels[l][b.y-1+b.w][b.x-2]) != blocsLvl.end()){
/*CHI*/cout << "POP/MOVE FAILED! Comparteix algun bloc superior! fiQ" << endl;
return true;
}
}
for(int j = b.x-1; j < b.x+b.h-1; ++j){
if(blocsLvl.find(g.gLevels[l][b.y-1+b.w][j]) != blocsLvl.end()){
/*CHI*/cout << "POP/MOVE FAILED! Comparteix algun bloc superior! fi" << endl;
return true;
}
}
}
//columna esq:
//*CHI*/cout << "ce" << b.x-2 << endl;
//*CHI*/cout << "ceQ" << b.y-1+b.w << endl;
if(b.x-2 >= 0){
if(b.y-1+b.w < g.m){
if(blocsLvl.find(g.gLevels[l][b.y-1+b.w][b.x-2]) != blocsLvl.end()){
/*CHI*/cout << "POP/MOVE FAILED! Comparteix algun bloc superior! ceQ" << endl;
return true;
}
}
for(int i = b.y-1; i < b.y+b.w-1; ++i){
if(blocsLvl.find(g.gLevels[l][i][b.x-2]) != blocsLvl.end()){
/*CHI*/cout << "POP/MOVE FAILED! Comparteix algun bloc superior! ce" << endl;
return true;
}
}
}
//columna dret:
//*CHI*/cout << "cd" << b.x-1+b.h << endl;
//*CHI*/cout << "cdQ" << b.y-2 << endl;
if(b.x-1+b.h < g.n){
if(b.y-2 >= 0){
if(blocsLvl.find(g.gLevels[l][b.y-2][b.x-1+b.h]) != blocsLvl.end()){
/*CHI*/cout << "POP/MOVE FAILED! Comparteix algun bloc superior! cdQ" << endl;
return true;
}
}
for(int i = b.y-1; i < b.y+b.w-1; ++i){
if(blocsLvl.find(g.gLevels[l][i][b.x-1+b.h]) != blocsLvl.end()){
/*CHI*/cout << "POP/MOVE FAILED! Comparteix algun bloc superior! cd" << endl;
return true;
}
}
}
}
return false;
}

/*
Fa el POP d'un block (i tots els seus superiors) si pot. Si no es pot 
treure (perque en te a sobre que es recolzen en altres blocs) retorna false.
*/
bool pop(string bID){

  //TODO guardar info de blocs que treiem (i tot el que tinguin a sobre) per si despres sen vol fer un PUSH

  //Si existeix el ID
if(posIDs.find(bID) != posIDs.end()){
int posB = posIDs[bID];
tblock b = blocks[posB];
//Si el bloc es a la graella
if(b.x > 0 and b.y > 0){
//Si el bloc no te altres blocs per sobre
if(!supBlocks(b)){
//Ja es pot treure i actualitzar les estructures de dades
int mx = b.x+b.h-1;
int my = b.y+b.w-1;
//Esborrat i guardar dades en temporals
for(int i = b.y-1; i < my; ++i){
for(int j = b.x-1; j < mx; ++j){
//*CHI*/cout << i << " " << j << endl;
g.height[i][j] = b.bHeight-1;
bool some = true;
for(int l = b.bHeight-1; some and l < g.gLevels.size(); ++l){
if(g.gLevels[l][i][j] != -1){
blocks[g.gLevels[l][i][j]].x = -1;
blocks[g.gLevels[l][i][j]].y = -1;
blocks[g.gLevels[l][i][j]].bHeight = -1;
g.gLevels[l][i][j] = -1;
}
else{
some = false;
}
}
}
}
blocks[posB].x = -1;
blocks[posB].y = -1;
blocks[posB].bHeight = -1;
posIDs.erase(bID);//bloc que ha sigut borrat ja no esta apuntat per el ID que l'apuntaba
return true;
}
}
}
return false;
}

/*
Fa el push d'un bloc si es pot.
*/
bool push(string IDb, AST *secondBlockRoot){
int posVec = posIDs[IDb];
int sx, sy, sh, sw, sHeight;
tblock secBlock;
tblock b = blocks[posVec];
if(secondBlockRoot->kind != "list"){
//Si ens han donat una ID i no existia o que comparteix blocs superiors, no fem res
if(posIDs.find(secondBlockRoot->kind) == posIDs.end()){
/*CHI*/cout << "ID no exists!" << endl;
return false;
}
secBlock = blocks[posIDs[secondBlockRoot->kind]];
if(supBlocks(secBlock)){
/*CHI*/cout << "BLOCS SUP!!" << endl;
return false;
}
sx = secBlock.x;
sy = secBlock.y;
sh = secBlock.h;
sw = secBlock.w;
sHeight = secBlock.bHeight;
//Comprovar que el bloc que volem fer push no esta sota el bloc al que li fem el push!
if(sx >= b.x and sy >= b.y and sx+sh <= b.x+b.h and sy+sw <= b.y+b.w and sHeight <= b.bHeight){
/*CHI*/cout << secondBlockRoot->kind << endl;
/*CHI*/cout << "Bloc esta sota l'altre!! " << sx << " " << b.x << " " << sy << " " <<  b.y << " " << (sx+sh) << " " << (b.x+b.h) << " " << (sy+sw) << " " << (b.y+b.w) << " " << sHeight << " " << b.bHeight << endl;
return false;
}
}
else{
sx = g.n+1;
sy = g.m+1;
sh = stoi(child(secondBlockRoot, 0)->kind);
sw = stoi(child(secondBlockRoot, 1)->kind);
sHeight = 0;
}
//Comprovem que podem possar el bloc al lloc i mirem on
if(b.h < sh or b.w < sw){
/*CHI*/cout << "Aixo no i cap!" << endl;
return false;
}
int height;
int x, y;
bool trobat = false;
for(int i = b.y-1; i < b.y+b.w-sw+1; ++i){
for(int j = b.x-1; j < b.x+b.h-sh+1; ++j){
int actHeight = g.height[i][j];
if(j >= sx-1 and i >= sy-1 and j <= sx+sh-1 and i <= sy+sw-1){
cout << (j >= sx-1 and i >= sy-1 and j <= sx+sh-1 and i <= sy+sw-1) << endl;
actHeight = sHeight-1;
}
int conflict = false;
for(int ci = 0; !conflict and i+ci < b.y+b.w-1 and ci < sw; ++ci){
for(int cj = 0; !conflict and j+cj < b.x+b.h-1 and cj < sh; ++cj){
/*CHI*/cout << "c " << (j+cj) << " " << (i+ci) << endl;
int cHeight = g.height[i+ci][j+cj];
if(j+cj >= sx-1 and i+ci >= sy-1 and j+cj <= sx+sh-1 and i+ci <= sy+sw-1){
cHeight = sHeight-1;
}
if(cHeight != actHeight) conflict = true;
else if(ci == sw-1 and cj == sh-1){
if(!trobat or (trobat and height > actHeight)){
trobat = true;
x = j+1;
y = i+1;
height = actHeight;
}
}
}
}
}
}
if(!trobat){
/*CHI*/cout << "No trobat!" << endl;
return false;
}
else{
/*CHI*/cout << "Trobat a " << x << " " << y << endl;
}

  //Si existia, s'ha de treure d'on estaba i possar-lo!
if(secondBlockRoot->kind != "list"){
//Moure i actualitzar les estructures de dades
int mx = secBlock.x+secBlock.h-1;
int my = secBlock.y+secBlock.w-1;
cout << mx << " " << my << endl;
//Primer borrem. Guardem els valors a sumar en una llista
list<int> blocHeights;
list<int> blocPosGLvl;
unordered_set<int> posBlocTmp;
//Esborrat i guardar dades en temporals
for(int i = secBlock.y-1; i < my; ++i){
for(int j = secBlock.x-1; j < mx; ++j){
//*CHI*/cout << i << " " << j << endl;
blocHeights.push_back(g.height[i][j] - secBlock.bHeight+1);
g.height[i][j] = secBlock.bHeight-1;
bool some = true;
for(int l = secBlock.bHeight-1; some and l < g.gLevels.size(); ++l){
if(g.gLevels[l][i][j] != -1){
posBlocTmp.insert(g.gLevels[l][i][j]);
blocPosGLvl.push_back(g.gLevels[l][i][j]);
g.gLevels[l][i][j] = -1;
}
else{
some = false;
}
}
blocPosGLvl.push_back(-1);//Per saver on parar per cada parell i j.
}
}
int nx, ny;
nx = x - secBlock.x;
ny = y - secBlock.y;
//Augmentar valors graelles.
int startHeight = g.height[secBlock.y+ny-1][secBlock.x+nx-1];
for(int i = secBlock.y+ny-1; i < my+ny; ++i){
for(int j = secBlock.x+nx-1; j < mx+nx; ++j){
//*CHI*/cout << i << " " << j << endl;
g.height[i][j] += blocHeights.front();
blocHeights.pop_front();
int l = startHeight;
//*CHI*/cout << l << endl;
while(blocPosGLvl.front() != -1){
if(g.gLevels.size() <= l){
g.gLevels.push_back(vector< vector<int> >(g.m, vector<int>(g.n, -1)));
}
g.gLevels[l][i][j] = blocPosGLvl.front();              
blocPosGLvl.pop_front();
++l;
}
blocPosGLvl.pop_front();
}
}
//Actualitzar blocks.
for(unordered_set<int>::iterator itr = posBlocTmp.begin(); itr != posBlocTmp.end(); ++itr) {
blocks[*itr].x += nx;
blocks[*itr].y += ny;
blocks[*itr].bHeight += startHeight - secBlock.bHeight + 1;
}
}
//Si no existia el bloc que estem possant el creem
else{
cout << sx << " " << sy << " " << sh << " " << sw << " " << height << " " << blocks.size() << endl;
blocks.push_back(createBlock(x, y, sh, sw, height+1, blocks.size()));
}
return true;
}

/*
PLACE dun bloc a la graella. El podrem possar si hi ha una ESPLANADA (totes les posicions tenen la mateixa altura)
al lloc que ens demanin. En cas que no es pugui no el possarem, ni es creara el nou bloc, ni es guardara o sobreescriura
la seva ID.
*/
void place(AST *a){
AST *place = child(a, 1);
int x, y, h, w;//pos x y, tamany h w
x = stoi(child(child(place, 1), 0)->kind);
y = stoi(child(child(place, 1), 1)->kind);
h = stoi(child(child(place, 0), 0)->kind);
w = stoi(child(child(place, 0), 1)->kind);
if(h == 0 or w == 0){
/*CHI*/cout << "BLOC SENSE MIDA!?!" << endl;
return;
}
//Mirem a quina altura es pot possar el bloc, si retorna -1 es que no es pot.
int bHeight = canPutBlock(x, y, h, w) + 1;
if(bHeight >  0){
//*CHI*/cout << "block params: " << x << " " << y << " " << h << " " << w << " " << bHeight << endl;
//Com es un place, tenim un ID segur si esta ben format i sera un bloc nou, sobreescrivim o creem al map posIDs.
posIDs[child(a, 0)->kind] = blocks.size();//El bloc nou es possa al final del vector de blocs.
//Creem el bloc a les estructures de dades necessaries (graella i vector de blocs).
blocks.push_back(createBlock(x, y, h, w, bHeight, posIDs[child(a, 0)->kind]));
} 
}

/*
ASSIGNACIONS
*/
void assig(AST *a){
/*CHI*/cout << "=" << endl;
/*CHI*/cout << child(a, 0)->kind << " " << child(a, 1)->kind << endl;
//ASSIGNACIONS AMB PLACE: crea un bloc, el col·loca on s'indiqui de la graella (si es pot)
if(child(a, 1)->kind == "PLACE"){
place(a);
}
//ASSIGNACIONS AMB PUSH/POP
else{
//TODO ARREGLAR PUSH I FITS!!!
string bID = child(a, 0)->kind;
stack<AST*> ppInst;
AST *nextInst = child(a, 1);
//Apilem les instruccions (necessitem arrivar a la fulla per poder executar el push/pop en l'ordre correcte)
while(nextInst){
if(nextInst->kind == "PUSH" or nextInst->kind == "POP"){
//*CHI*/cout << child(nextInst,0)->kind << endl;
//*CHI*/cout << nextInst->kind << endl;
ppInst.push(child(nextInst,0));
ppInst.push(nextInst);
nextInst = child(nextInst,1);
}
else{
//*CHI*/cout << nextInst->kind << endl;
ppInst.push(nextInst);
nextInst = NULL;
}
}

    string iniBID = (ppInst.top())->kind;
//Si la ID no existeix ja no cal fer res
if(posIDs.find(iniBID) != posIDs.end()){
ppInst.pop();
int posVecIniB = posIDs[iniBID];
//Si no es a la graella tampoc cal fer res
if(blocks[posVecIniB].x > 0 and blocks[posVecIniB].y > 0){
while(not ppInst.empty()){
AST *actInst = ppInst.top();//Instruccio a fer
ppInst.pop();
AST *secondBlock = ppInst.top();//Bloc amb que operar sobre iniB
ppInst.pop();
/*CHI*/cout << iniBID << " " << actInst->kind << " " << secondBlock->kind << endl;
//Si ens demanen POP
if(actInst->kind == "POP"){
pop(secondBlock->kind);
}
else if(actInst->kind == "PUSH"){
push(iniBID, secondBlock);
}
}
posIDs[bID] = posIDs[iniBID];
}
}
else{
//no existeix el bloc al que ens demanen que fem les operacions...
return;
}
}
}

void move(AST *a){
/*CHI*/cout << "INI MOVE " << child(a, 0)->kind << " " << child(a, 2)->kind << endl;
int m = stoi(child(a, 2)->kind);//posicions a moures
if(m <= 0){
/*CHI*/cout << "... si no te mueves!" << endl;
return;
}
//Si existeix la ID que ens demanen.
if(posIDs.find(child(a, 0)->kind) != posIDs.end()){
int bPos = posIDs[child(a, 0)->kind];
//*CHI*/cout << " posID " << posIDs[child(a, 0)->kind] << endl;
tblock b = blocks[bPos];
//Si el bloc esta a la graella
if(b.x > 0 and b.y > 0){
//el q hem d'afegir a x o y si movem els blocs m posicions en la direccio que demanin.
int nx, ny;
nx = 0; ny = 0;
string dirMove = child(a, 1)->kind;
if(dirMove == "NORTH"){
ny = -m;
}
else if(dirMove == "SOUTH"){
ny = m;
}
else if(dirMove == "WEST"){
nx = -m;          
}
else if(dirMove == "EAST"){
nx = m;
}
if(b.x+nx-1 < 0 or b.y+ny-1 < 0 or b.x+b.h+nx-1 > g.n or b.y+b.w+ny-1 > g.m){          
/*CHI*/cout << "MOVE FAILED! No podem sortir del taulell!" << endl;
return;
}
//Comprovem que el podrem possar on demanen
int cn, cs, cw, ce;//Punts per on talla el bloc
//(l'altura al treure el bloc canviaria,
//si la casella que mirem esta al bloc haurem de mirar amb l'altura del bloc).
cn = b.y-2; cs = b.y+b.w-1; cw = b.x-2; ce = b.x+b.h-1;
int iniHeight = g.height[b.y+ny-1][b.x+nx-1];
if(b.y+ny-1 > cn and b.y+ny-1 < cs and b.x+nx-1 > cw and b.x+nx-1 < ce){
iniHeight = b.bHeight - 1;//Altura sobre la que esta el bloc
}
//*CHI*/cout << cn << " " << cs << " " << cw << " " << ce << endl;
//*CHI*/cout << "p " << b.x+nx-1 << " " << b.y+ny-1 << endl; 
for(int i = b.y+ny-1; i < cs+ny; ++i){
for(int j = b.x+nx-1; j < ce+nx; ++j){
//*CHI*/cout << "p " << j << " " << i << endl;
int nh = g.height[i][j];
if(i > cn and i < cs and j > cw and j < ce){
nh =  b.bHeight - 1;
}
if(nh != iniHeight){
/*CHI*/cout << "MOVE FAILED! Desnivell! " << iniHeight << " != " << nh << " a " << j << " " << i << endl;
return;
}
}
}
//Comprovem que no hi ha blocs "superiors compartits"
if(supBlocks(b)) return;

        //Ja es pot moure i actualitzar les estructures de dades
int mx = b.x+b.h-1;
int my = b.y+b.w-1;
//Primer borrem. Guardem els valors a sumar en una llista
list<int> blocHeights;
list<int> blocPosGLvl;
unordered_set<int> posBlocTmp;
//Esborrat i guardar dades en temporals
for(int i = b.y-1; i < my; ++i){
for(int j = b.x-1; j < mx; ++j){
//*CHI*/cout << i << " " << j << endl;
blocHeights.push_back(g.height[i][j] - b.bHeight+1);
g.height[i][j] = b.bHeight-1;
bool some = true;
for(int l = b.bHeight-1; some and l < g.gLevels.size(); ++l){
if(g.gLevels[l][i][j] != -1){
posBlocTmp.insert(g.gLevels[l][i][j]);
blocPosGLvl.push_back(g.gLevels[l][i][j]);
g.gLevels[l][i][j] = -1;
}
else{
some = false;
}
}
blocPosGLvl.push_back(-1);//Per saver on parar per cada parell i j.
}
}
//Augmentar valors graelles.
int startHeight = g.height[b.y+ny-1][b.x+nx-1];
for(int i = b.y+ny-1; i < my+ny; ++i){
for(int j = b.x+nx-1; j < mx+nx; ++j){
//*CHI*/cout << i << " " << j << endl;
g.height[i][j] += blocHeights.front();
blocHeights.pop_front();
int l = startHeight;
//*CHI*/cout << l << endl;
while(blocPosGLvl.front() != -1){
if(g.gLevels.size() <= l){
g.gLevels.push_back(vector< vector<int> >(g.m, vector<int>(g.n, -1)));
}
g.gLevels[l][i][j] = blocPosGLvl.front();              
blocPosGLvl.pop_front();
++l;
}
blocPosGLvl.pop_front();
}
}
//Actualitzar blocks.
for(unordered_set<int>::iterator itr = posBlocTmp.begin(); itr != posBlocTmp.end(); ++itr) {
blocks[*itr].x += nx;
blocks[*itr].y += ny;
blocks[*itr].bHeight += startHeight - b.bHeight + 1;
}
//*CHI*/for(int i = 0; i < blocks.size(); ++i) cout << i << " " << blocks[i].x << " " << blocks[i].y << " " << blocks[i].bHeight << endl;
}
else{
/*CHI*/cout << "MOVE FAILED! It does not exist in our dimension..." << endl;
}
}
else{
/*CHI*/cout << "MOVE FAILED! ID does not exist..." << endl;
}
/*CHI*/cout << "FI MOVE OK!" << endl;
}

int numVal(AST *a){
if(a->kind == "HEIGHT") return height(a);
return stoi(a->kind);
}

/*
Funcio FITS B2 xy l
*/
bool fits(AST *a){
string IDb = child(a, 0)->kind;
//No cal evaluar res si no existeix el ID donat
if(posIDs.find(IDb) != posIDs.end()){
tblock b = blocks[posIDs[IDb]];
//Si el bloc es a la graella
if(b.x > 0 and b.y > 0){
int h, w, maxHeight;//tamany del bloc a comprovar
h = stoi(child(child(a, 1), 0)->kind);
w = stoi(child(child(a, 1), 1)->kind);
//Si el bloc que volem comprovar si es pot possar es mes petit que b
if(b.h >= h and b.w >= w){
int l = stoi(child(a, 2)->kind);//nivell on el volem possar
if(l == 1) return false;//el primer tram es el bloc mateix.
l += b.bHeight - 1;//Nivell "real"
/*CHI*/cout << "MIRAR FIT " << h << " " << w << " " << l << " " << IDb << " " << b.y+b.w << endl;
for(int i = b.y-1; i < b.y+b.w-w; ++i){
for(int j = b.x-1; j < b.x+b.h-h; ++j){
/*CHI*/cout << "p " << j << " " << i << endl;
//Nomes cal comprovar si es pot possar si estem per sota el nivell q volem, si no no es pot i punt
if(g.height[i][j] < l){
int actHeight = g.height[i][j];//tmp guardar altura actual
bool conflict = false;//conflicte trobat
bool trobat = false;//posicio trobada
/*CHI*/int tmpCi, tmpCj, tmpL;
for(int ci = 0; !conflict and i+ci < b.y+b.w-1 and ci < w; ++ci){
for(int cj = 0; !conflict and j+cj < b.x+b.h-1 and cj < h; ++cj){
/*CHI*/cout << "c " << (j+cj) << " " << (i+ci) << " " << actHeight << " =? " << g.height[i+ci][j+cj] << endl;
if(g.height[i+ci][j+cj] != actHeight) conflict = true;
else if(ci+1 == w and cj+1 == h) trobat = true;
}
}
if(!conflict and trobat){
/*CHI*/cout << "TROBAT FITS A " << (i) << " " << (i) << " " << actHeight << endl;
return true;//Si arriva al final sense conflictes, hem trovat un lloc on possarlo
}
}
}
}
}
}
}
cout << "FALSE!!" << endl;
return false;
}

/*
Evalua una expressio booleana (comparacions <,> d'enters; funcio FITS; unions AND entre expressions).
*/
bool evalBoolExp(AST *a){
/*CHI*/cout << a->kind << endl;
if(a->kind == "<"){
/*CHI*/cout << " " << numVal(child(a, 0)) << " " << numVal(child(a,1)) << endl;
return numVal(child(a, 0)) < numVal(child(a,1));
}
else if(a->kind == ">"){
/*CHI*/cout << " " << numVal(child(a, 0)) << " " << numVal(child(a,1)) << endl;
return numVal(child(a, 0)) > numVal(child(a,1));
}
else if(a->kind == "AND"){
return evalBoolExp(child(a, 0)) and evalBoolExp(child(a, 1));
}
else if(a->kind == "FITS"){
return fits(a);
}
return false;
}

/*
Executa les instruccions que pertoqui.
*/
void executeListInstrucctions(AST *a){
if (a == NULL) return;
//ASSIGNACIONS
else if (a->kind == "="){
assig(a);    
}
//MOVE: mou un bloc (i el que tingui al seu damunt) en una direcció concreta.
else if(a->kind == "MOVE"){
move(a);
}
//HEIGHT
else if(a->kind == "HEIGHT"){
int hB = height(a);
if(height > 0) cout << hB << endl;
}
//WHILE
else if(a->kind == "WHILE"){
/*CHI*/cout << "WHILE " << endl;
AST *instRoot = child(a,1);
if(instRoot){
while(evalBoolExp(child(a, 0))){
int i = 0;
while(child(instRoot, i)){
executeListInstrucctions(child(instRoot,i));
++i;
}
}
}
else{
cout << "WHILE BUIT..." << endl;
}
/*CHI*/cout << "FI WHILE" << endl;
}
//NOM DE FUNCIO
else{
//Si existeix la funcio
if(funcs.find(a->kind) != funcs.end()){
/*CHI*/cout << "FUNC " << a->kind << endl;
int i = 0;
while(child(funcs[a->kind], i)){
executeListInstrucctions(child(funcs[a->kind], i));
++i;
}
/*CHI*/cout << "FI FUNC " << a->kind << endl;
}
}
}

void executeLego(AST *a){
int n = 0;
while(child(a,n)){
//*CHI*/cout << child(a,n)->kind << endl;
n++;
}
//Si Grid
if(child(a,0)){
executeGrid(child(a,0));    
//Si instruccions (o func)
if(child(a,1)){
AST *instRoot = child(a,1);
//Instruccions a executar i DEF funcions (si hi ha instruccions a executar)
if(child(instRoot,0) and child(instRoot,0)->kind != "DEF"){
//*CHI*/cout << "WORK!" << endl;
AST *funcRoot = child(a,2);
if(funcRoot and child(funcRoot,0)){
/*CHI*/cout << "INI DEFs!" << endl;
int d = 0;
while(child(funcRoot,d)){
if(child(child(funcRoot,d),1)){
addFunctions(child(funcRoot,d));//PASSEM EL NODE QUE TE "DEF"
/*CHI*/cout << d << " " << child(child(funcRoot,d),0)->kind << " " << funcs[child(child(funcRoot,d),0)->kind]->kind << " added!" << endl;
}
++d;
}
/*CHI*/cout << "END DEFs!" << endl;
}
//Ejecutar las instrucciones
int i = 0;
/*CHI*/cout << "INSTS: " << endl;
while(child(instRoot,i)){
executeListInstrucctions(child(instRoot,i));
//*CHI*/cout << "inst " << child(instRoot,i)->kind << endl;
++i;
}
/*CHI*/cout << "END INSTs!" << endl;
}
else{
//*CHI*/cout << "NOTHING TO DO" << endl;
}
}
}
}

int main() {
root = NULL;
ANTLR(lego(&root), stdin);
ASTPrint(root);
executeLego(root);
/*CHI*/
writeGrid();
}

void
#ifdef __USE_PROTOS
lego(AST**_root)
#else
lego(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  grid(zzSTR); zzlink(_root, &_sibling, &_tail);
  ops(zzSTR); zzlink(_root, &_sibling, &_tail);
  defs(zzSTR); zzlink(_root, &_sibling, &_tail);
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x1);
  }
}

void
#ifdef __USE_PROTOS
grid(AST**_root)
#else
grid(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(GRID); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x2);
  }
}

void
#ifdef __USE_PROTOS
ops(AST**_root)
#else
ops(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd1[LA(1)]&0x4) ) {
      instruction(zzSTR); zzlink(_root, &_sibling, &_tail);
      (*_root)=createASTlist(_sibling);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x8);
  }
}

void
#ifdef __USE_PROTOS
defs(AST**_root)
#else
defs(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==DEF) ) {
      functionDef(zzSTR); zzlink(_root, &_sibling, &_tail);
      (*_root)=createASTlist(_sibling);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x10);
  }
}

void
#ifdef __USE_PROTOS
functionDef(AST**_root)
#else
functionDef(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(DEF); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(FUNC); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  ops(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(ENDEF);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x20);
  }
}

void
#ifdef __USE_PROTOS
instruction(AST**_root)
#else
instruction(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==ID) ) {
    zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    zzmatch(ASIG); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    expr(zzSTR); zzlink(_root, &_sibling, &_tail);
  }
  else {
    if ( (LA(1)==MOVE) ) {
      zzmatch(MOVE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(DIRECTION); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==WHILE) ) {
        zzmatch(WHILE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        zzmatch(LP);  zzCONSUME;
        boolexpr(zzSTR); zzlink(_root, &_sibling, &_tail);
        zzmatch(RP);  zzCONSUME;
        zzmatch(LSB);  zzCONSUME;
        ops(zzSTR); zzlink(_root, &_sibling, &_tail);
        zzmatch(RSB);  zzCONSUME;
      }
      else {
        if ( (LA(1)==HEIGHT) ) {
          hexpr(zzSTR); zzlink(_root, &_sibling, &_tail);
        }
        else {
          if ( (LA(1)==FUNC) ) {
            zzmatch(FUNC); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr1,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x40);
  }
}

void
#ifdef __USE_PROTOS
expr(AST**_root)
#else
expr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==PLACE) ) {
    zzmatch(PLACE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    pxyp(zzSTR); zzlink(_root, &_sibling, &_tail);
    zzmatch(AT);  zzCONSUME;
    pxyp(zzSTR); zzlink(_root, &_sibling, &_tail);
  }
  else {
    if ( (setwd1[LA(1)]&0x80) ) {
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        if ( (LA(1)==ID) ) {
          {
            zzBLOCK(zztasp3);
            zzMake0;
            {
            zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
            {
              zzBLOCK(zztasp4);
              zzMake0;
              {
              if ( (LA(1)==PUSH) ) {
                zzmatch(PUSH); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
              }
              else {
                if ( (LA(1)==POP) ) {
                  zzmatch(POP); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
                }
                else {zzFAIL(1,zzerr2,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
              }
              zzEXIT(zztasp4);
              }
            }
            zzEXIT(zztasp3);
            }
          }
        }
        else {
          if ( (LA(1)==LP) ) {
            {
              zzBLOCK(zztasp3);
              zzMake0;
              {
              pxyp(zzSTR); zzlink(_root, &_sibling, &_tail);
              zzmatch(PUSH); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
              zzEXIT(zztasp3);
              }
            }
          }
          else {zzFAIL(1,zzerr3,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp2);
        }
      }
      pexpr(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {zzFAIL(1,zzerr4,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x1);
  }
}

void
#ifdef __USE_PROTOS
pexpr(AST**_root)
#else
pexpr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==ID) ) {
    zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    {
      zzBLOCK(zztasp2);
      zzMake0;
      {
      if ( (setwd2[LA(1)]&0x2) ) {
        {
          zzBLOCK(zztasp3);
          zzMake0;
          {
          if ( (LA(1)==PUSH) ) {
            zzmatch(PUSH); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {
            if ( (LA(1)==POP) ) {
              zzmatch(POP); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
            }
            else {zzFAIL(1,zzerr5,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
          }
          zzEXIT(zztasp3);
          }
        }
        pexpr(zzSTR); zzlink(_root, &_sibling, &_tail);
      }
      else {
        if ( (setwd2[LA(1)]&0x4) ) {
        }
        else {zzFAIL(1,zzerr6,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
      zzEXIT(zztasp2);
      }
    }
  }
  else {
    if ( (LA(1)==LP) ) {
      pxyp(zzSTR); zzlink(_root, &_sibling, &_tail);
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        if ( (LA(1)==PUSH) ) {
          zzmatch(PUSH); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {
          if ( (LA(1)==POP) ) {
            zzmatch(POP); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr7,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp2);
        }
      }
      pexpr(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {zzFAIL(1,zzerr8,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x8);
  }
}

void
#ifdef __USE_PROTOS
boolexpr(AST**_root)
#else
boolexpr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==FITS) ) {
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
        zzmatch(FITS); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        zzmatch(LP);  zzCONSUME;
        zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
        zzmatch(COMA);  zzCONSUME;
        xy(zzSTR); zzlink(_root, &_sibling, &_tail);
        zzmatch(COMA);  zzCONSUME;
        zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
        zzmatch(RP);  zzCONSUME;
        zzEXIT(zztasp3);
        }
      }
    }
    else {
      if ( (setwd2[LA(1)]&0x10) ) {
        {
          zzBLOCK(zztasp3);
          zzMake0;
          {
          {
            zzBLOCK(zztasp4);
            zzMake0;
            {
            if ( (LA(1)==HEIGHT) ) {
              hexpr(zzSTR); zzlink(_root, &_sibling, &_tail);
            }
            else {
              if ( (LA(1)==NUM) ) {
                zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
              }
              else {zzFAIL(1,zzerr9,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
            }
            zzEXIT(zztasp4);
            }
          }
          zzmatch(COMP); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          {
            zzBLOCK(zztasp4);
            zzMake0;
            {
            if ( (LA(1)==HEIGHT) ) {
              hexpr(zzSTR); zzlink(_root, &_sibling, &_tail);
            }
            else {
              if ( (LA(1)==NUM) ) {
                zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
              }
              else {zzFAIL(1,zzerr10,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
            }
            zzEXIT(zztasp4);
            }
          }
          zzEXIT(zztasp3);
          }
        }
      }
      else {zzFAIL(1,zzerr11,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==AND) ) {
      zzmatch(AND); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      boolexpr(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {
      if ( (LA(1)==RP) ) {
      }
      else {zzFAIL(1,zzerr12,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x20);
  }
}

void
#ifdef __USE_PROTOS
hexpr(AST**_root)
#else
hexpr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(HEIGHT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(LP);  zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(RP);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x40);
  }
}

void
#ifdef __USE_PROTOS
pxyp(AST**_root)
#else
pxyp(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(LP);  zzCONSUME;
  xy(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(RP);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x80);
  }
}

void
#ifdef __USE_PROTOS
xy(AST**_root)
#else
xy(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(COMA);  zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail);
  (*_root)=createASTlist(_sibling);
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x1);
  }
}
