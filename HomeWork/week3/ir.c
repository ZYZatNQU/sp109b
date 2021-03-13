#include <string.h>
#include "ir.h"

IR ir[IR_MAX];
int irTop = 0;
extern int L[]; // label => address

void irNew(IR p) {
  ir[irTop++] = p;
}

void irEmitCall(char *fname, int t1, int fnameLen) {
  irNew((IR) {.type=IrCall, .op=fname, .t1=t1, .opLen=fnameLen, .sLen=0});
}

void irEmitArg(int t1) {
  irNew((IR) {.type=IrArg, .t1=t1, .opLen=0, .sLen=0});
}

void irEmitAssignTs(int t, char *s, int sLen) {
  irNew((IR) {.type=IrAssignTs, .op="t=s", .t=t, .s=s, .opLen=3, .sLen=sLen});
}

void irEmitAssignSt(char *s, int t, int sLen) {
  irNew((IR) {.type=IrAssignSt, .op="s=t", .t=t, .s=s, .opLen=3, .sLen=sLen});
}

void irEmitOp2(int t, int t1, char *op, int t2, int opLen) {
  irNew((IR) {.type=IrOp2, .op=op, .t=t, .t1=t1, .t2=t2, .opLen=opLen, .sLen=0});
}

void irEmitLabel(int label) {
  // L[label] = irTop;
  irNew((IR) {.type=IrLabel, .op="label", .label=label, .opLen=5, .sLen=0});
}

void irEmitGoto(int label) {
  irNew((IR) {.type=IrGoto, .op="goto", .label=label, .opLen=4, .sLen=0});
}

void irEmitIfGoto(int t, int label) {
  irNew((IR) {.type=IrIfGoto, .op="if-goto", .t=t, .label=label, .opLen=7, .sLen=0});
}

void irEmitIfNotGoto(int t, int label) {
  irNew((IR) {.type=IrIfNotGoto, .op="ifnot-goto", .t=t, .label=label, .opLen=10, .sLen=0});
}

void irPrint(IR *p) {
  char opTmp[SMAX];
  char sTmp[SMAX];
  switch (p->type) {
    case IrCall: 
      memcpy(opTmp,p->op,p->opLen);
      opTmp[p->opLen] = '\0';
      printf("call %s %d", opTmp, p->t1); 
      break;
    case IrArg:  
      printf("arg t%d", p->t1); 
      break;
    case IrAssignSt:
      memcpy(sTmp,p->s,p->sLen);
      sTmp[p->sLen] = '\0';
      printf("%s = t%d", sTmp, p->t);
      break;
    case IrAssignTs: 
      memcpy(sTmp,p->s,p->sLen);
      sTmp[p->sLen] = '\0';
      printf("t%d = %s", p->t, sTmp); 
      break;
    case IrLabel: 
      printf("(L%d)", p->label); 
      break;
    case IrGoto: 
      printf("goto L%d", p->label); 
      break;
    case IrIfGoto: 
      printf("if t%d goto L%d", p->t, p->label); 
      break;
    case IrIfNotGoto: 
      printf("ifnot t%d goto L%d", p->t, p->label); 
      break;
    case IrOp2: 
      memcpy(opTmp,p->op,p->opLen);
      opTmp[p->opLen] = '\0';
      printf("t%d = t%d %s t%d", p->t, p->t1, opTmp, p->t2); 
      break;
    default: error("ir.type %d not found!", p->type);
  }
  printf("\n");
}

void irDump() {
  printf("=======irDump()==========\n");
  for (int i=0; i<irTop; i++) {
    printf("%02d: ", i);
    irPrint(&ir[i]);
  }
}

void irPass2() {
  // printf("==========irPass2()============\n");
  for (int i=0; i<irTop; i++) {
    int label = ir[i].label, type = ir[i].type;
    if (type == IrLabel) {
      assert(label != 0);
      L[label] = i;
      // printf("L%d=%d\n", label, L[label]);
    }
  }
}
