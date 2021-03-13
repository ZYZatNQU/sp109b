#ifndef __IR_H__
#define __IR_H__

#include <stdio.h>
#include "map.h"
#include "util.h"

#define emit printf

extern void irEmitArg(int t1) ;
extern void irEmitCall(char *fname, int t1, int fnameLen);
extern void irEmitAssignTs(int t, char *s, int sLen);
extern void irEmitAssignSt(char *s, int t, int sLen);
extern void irEmitOp2(int t, int p1, char *op, int p2, int opLen);
extern void irEmitLabel(int label);
extern void irEmitGoto(int label);
extern void irEmitIfGoto(int t, int label);
extern void irEmitIfNotGoto(int t, int label);
extern void irDump();

// IR Virtual Machine
#define trace printf
#define VAR_MAX 10000
#define IR_MAX  10000

typedef enum { IrAssignSt, IrAssignTs, IrOp2, IrLabel, IrCall1, IrGoto, IrIfGoto, IrIfNotGoto, IrCall, IrArg } IrType;

typedef struct {
  IrType type;
  int t, t1, t2, label;
  char *s, *op;
  int sLen, opLen;
} IR;
//因為我token取法，為節省記憶體，不將字串重新複製到新位置，而是指向字串起始點，並儲存字串長度
//所以在其他地方要讀取字串時都將知道字串長度，所以結構體中多儲存字串長度訊息


extern IR ir[];
extern int irTop;
extern void irPass2();
extern int irExec(int i);
extern void irRun();
extern void irPrint(IR *p);

#endif
