#include <assert.h>
#include <string.h>
#include "compiler.h"

//#define DEBUG_EBNF
#ifdef DEBUG_EBNF
#define debugPrint(x) printf(x)
#else
#define debugPrint(x)
#endif

static int E();
static void STMT();
static void IF();
static void BLOCK();
static char *file_data = NULL;

static char *lexPtr;
static lexer_token thisToken;
static lexer_token nextToken;
int tempIdx = 1, labelIdx = 1;

#define nextTemp() (tempIdx++)
#define nextLabel() (labelIdx++)

int isNext(char *set) {
  char eset[SMAX], etoken[SMAX];
  memset(etoken,' ',SMAX);
  memcpy(etoken+1,nextToken.start,nextToken.len);
  etoken[nextToken.len+2] = '\0';
  sprintf(eset, " %s ", set);
  return ((nextToken.len != 0) && (strstr(eset, etoken) != NULL));
}

int isNextType(TokenType type) {
  return (nextToken.Type == type);
}

int isEnd() {
  return (nextToken.len == 0);
}

char *next() {
  char strTmp[SMAX];
  thisToken = nextToken;
  // printf("%02d:token = %-10s type = %-10s\n", tokenIdx, tokens[tokenIdx], typeName[types[tokenIdx]]);
  nextToken = lexer_next(lexPtr);
  lexPtr = nextToken.start + nextToken.len;
  
  #ifdef DEBUG_EBNF
  memcpy(strTmp,nextToken.start,nextToken.len);
  strTmp[nextToken.len] = '\0';
  printf("token = %-10s type = %-10s\n", strTmp, typeName[nextToken.Type]);
  #endif

  return thisToken.start;
}

char *skip(char *set) {
  debugPrint("skip:");
  char strTmp[SMAX];
  memcpy(strTmp,nextToken.start,nextToken.len);
  strTmp[nextToken.len] = '\0';
  debugPrint(strTmp);
  debugPrint("\n");
  if (isNext(set)) {
    return next();
  } else {
    error("skip(%s) got %s fail!\n", set, strTmp);
  }
}

char *skipType(TokenType type) {
  char strTmp[SMAX];
  memcpy(strTmp,nextToken.start,nextToken.len);
  strTmp[nextToken.len] = '\0';
  if (isNextType(type)) {
    return next();
  } else {
    error("skipType(%s) got %s fail!\n", typeName[thisToken.Type], strTmp);
  }
}
//call function轉換為中間碼，參數最多可有100，bug:未做邊界判斷，同時irvm並無相對應之功能
// CALL(id) = (E)*
int CALL(char *id, int idLen) {
  assert(isNext("("));
  skip("(");
  int e[100], ei = 0;
  while (!isNext(")")) {
    e[ei++] = E();
    if (!isNext(")")) skip(",");
  }
  for (int i=0; i<ei; i++) {
    irEmitArg(e[i]);
  }
  skip(")");
  irEmitCall(id, ei,idLen);
  return 0;
}
//運算式基礎元素，括號內作為一元素，剩餘的元素為立即值，或是變數名稱
// F = (E) | Number | Id | CALL
int F() {
  debugPrint("F\n");
  int f;
  if (isNext("(")) { // '(' E ')'
    next(); // (
    f = E();
    next(); // )
  } else { // Number | Id | CALL
    f = nextTemp();
    char *item = next();
    irEmitAssignTs(f, item,thisToken.len);
    // emit("t%d = %s\n", f, item);
  }
  return f;
}
//缺乏連續賦值功能
//運行=運算元時因為未對運算元左邊元數暫存器或變數做判斷，造成中間碼出現t3 = t2 = t1的情況irvm運行時出現錯誤
//進行運算式解析
// E = F (op E)*
int E() {
  debugPrint("E\n");
  int i1 = F();
  while (isNext("+ - * / & | < > = <= >= == != && ||")) {
    char *op = next();
    int opLen = thisToken.len;
    int i2 = E();
    int i = nextTemp();
    irEmitOp2(i, i1, op, i2,opLen);
    // emit("t%d = t%d %s t%d\n", i, i1, op, i2);
    i1 = i;
  }
  return i1;
}

int EXP() {
  tempIdx = 1; // 讓 temp 重新開始，才不會 temp 太多！
  return E();
}
//賦值結果給等號運算元左邊變數名稱
// ASSIGN = id '=' E
void ASSIGN(char *id, int idLen) {
  debugPrint("ASSIGN\n");
  // char *id = next();
  skip("=");
  int e = EXP();
  irEmitAssignSt(id, e, idLen);
  // emit("%s = t%d\n", id, e);
}
//以E結果做為while是否跳出迴圈之判斷
// while (E) STMT
void WHILE() {
  debugPrint("WHILE\n");
  int whileBegin = nextLabel();
  int whileEnd = nextLabel();
  irEmitLabel(whileBegin);
  // emit("(L%d)\n", whileBegin);
  skip("while");
  skip("(");
  int e = E();
  irEmitIfNotGoto(e, whileEnd);
  // emit("goif T%d L%d\n", whileEnd, e);
  skip(")");
  STMT();
  irEmitGoto(whileBegin);
  // emit("goto L%d\n", whileBegin);
  irEmitLabel(whileEnd);
  // emit("(L%d)\n", whileEnd);
}
//通過E結果選擇是否轉跳至else若無else則elseBegin為if區塊結尾
// if (E) STMT else STMT
void IF() {
  debugPrint("IF\n");
  int elseBegin = nextLabel();
  int ifEnd;
  skip("if");
  skip("(");
  int e = E();
  skip(")");
  irEmitIfNotGoto(e, elseBegin);
  STMT();
  if(isNext("else")){
    skip("else");
    ifEnd = nextLabel();
    irEmitGoto(ifEnd);
    irEmitLabel(elseBegin);
    STMT();
    irEmitLabel(ifEnd);
  }else{
    irEmitLabel(elseBegin);
  }
}
//將while之判斷式放置執行後，完成後測迴圈
//do STMT while (E) ;
void DO() {
  debugPrint("DO\n");
  int doBegin = nextLabel();
  irEmitLabel(doBegin);
  skip("do");
  STMT();
  skip("while");
  skip("(");
  int e = E();
  skip(")");
  skip(";");
  irEmitIfGoto(e, doBegin);
}
//for迴圈第一參數為僅剛進入迴圈時進行的動作，第二參數為判斷式，第三參數為迴遞增式
//因為照程式碼先後順訊轉換為ir的關係，所以造成較多的轉跳
//想解決的話可以將E所在的token位置記下，於完成for迴圈中STMT部分轉換後再回到E所在的token位置進行轉換，這樣一就可以減少for迴圈轉跳次數減少開銷
//for ( STMT ; E ; ASSIGN ) STMT
void FOR() {
  char *id;
  int e;
  debugPrint("FOR\n");
  int forBegin = nextLabel();
  int forCount = nextLabel();
  int forDecide = nextLabel();
  int forEnd = nextLabel();

  skip("for");
  skip("(");
  //id = next();
  //ASSIGN(id,thisToken.len);
  //skip(";");
  STMT();
  irEmitLabel(forDecide);
  e = E();
  skip(";");
  irEmitIfNotGoto(e, forEnd);
  irEmitGoto(forBegin);
  irEmitLabel(forCount);
  id = next();
  ASSIGN(id,thisToken.len);
  irEmitGoto(forDecide);
  skip(")");
  irEmitLabel(forBegin);
  STMT();
  irEmitGoto(forCount);
  irEmitLabel(forEnd);
}
void STMT() {
  debugPrint("STMT\n");
  if (isNext("while"))
    WHILE();
  else if (isNext("if"))
    IF();
  else if (isNext("for"))
    FOR();
  else if (isNext("do"))
    DO();
  else if (isNext("{"))
    BLOCK();
  else {
    char *id = next();
    /*
    if (eq(id, "int")) {
      skip("int");
      while (!isNext(";")) {
        char *var = skipType(Id);
        mapAdd(symMap, var, &symList[symTop++]);
      }
    }
    */
    if (isNext("(")) {
      CALL(id,thisToken.len);
    } else {
      ASSIGN(id,thisToken.len);
    }
    skip(";");
  }
}

void STMTS() {
  while (!isEnd() && !isNext("}")) {
    STMT();
  }
}

// { STMT* }
void BLOCK() {
  skip("{");
  STMTS();
  skip("}");
}

void PROG() {
  STMTS();
}

void parse(char *fileData) {
  // printf("============ parse =============\n");
  file_data = fileData;
  lexPtr = fileData;
  next();
  PROG();
}
