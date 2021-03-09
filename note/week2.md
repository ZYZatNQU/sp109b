# 第二週  
## 課程重點
    編譯器
    虛擬機
    OS
## 編譯器  
        (main.c)
    compiler
       |(main.s)
    assembler
       |(main.o)
       |
    Machine language--------------VM
       |                          | 
      CPU                        CPU


    語法處理(Syntax)-Lexer
       |            \剖悉器(Rarser)
    語意處理(Semantie)
       | 
    code gemeration
       \IR->ASM->obj


    深層語法(Grammar)適用於程式語言
    BNF(backus naur form)
##EBNF
    於exp0中可將輸入值轉換成各個有先後順序的小項，同時可以驗證輸入的語法是否正確。在創建語法時可較方便的新增項目，同時在像是C++中的運算子重載可以在這步驟中動態新增幾個函數來完成重載的功能。
    於exp0hack中可發現若是已經將輸入變為有順序的小項，同時個小項剛好又對應到CPU的一些指令，這樣讓我們可以在生成每個小項中同時生成對應的機械碼，在完成後輸入進CPU基本上就能完成一開始輸入的運算
    於exp0var引入了a到z之變數，對應的到組合語言的D=M;由於沒有修過之前的課程不了解hack的組合語言，以程式碼來猜測a-z變數代表了儲存變數所在的記憶體地址，所以這程式其實並不完整其中缺乏記憶體地址分配的部分。
