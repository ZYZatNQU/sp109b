# 第三週  
## 上週回顧
    編譯器
        1.BNF/EBNF/生成/語法
        2. 編譯器
        3.lexer(詞彙解析器)
        4. if , while , ...

## 本周:詞彙解析器Lexer
    詞彙解析器Lexer
        |        
    語法解析器
## 本周範例程式
    lexer.c詞彙解析器
        創建新結構體儲存解析出來的詞彙，以便閱讀
        移除原本存放解析出來詞彙的暫存區
            優點:減少記憶體占用同時減少超出所以邊界的可能
            缺點:找到特定位置詞彙的時間複雜度增加了。
    compiler.c編譯程式
        從lexer取解析完資訊轉為中間碼，完成後
    ir.c產生中間碼
        中間碼有
        IrCall: call 函數名稱 參數數量 //call function
        IrArg:  arg 暫存變量 //產生傳入參數
        IrAssignSt: 變數名稱 = 暫存變量 //暫存便量賦值給變數
        IrAssignTs: 暫存變量 = 立即值或變數 //讀出立即值或者變數
        IrLabel: 標籤序號 //產生標籤 
        IrGoto: goto 標籤序號 //跳轉至標籤
        IrIfGoto: if 暫存變量 goto 標籤序號 //暫存變量不為0跳轉至標籤
        IrIfNotGoto: ifnot 暫存變量 goto 標籤序號 //暫存變量為0跳轉至標籤
        IrOp2: 暫存變量1 = 暫存變量2 運算元 暫存變量3 //將暫存變量2和暫存變量3進行運算後賦值給暫存變量1
    irvm.c運行中間碼，運行前須先call ir.c中的irPass2()將標籤位置先載入暫存區
        變數將被儲存於int v[VAR_MAX]中透過hash函數取數值
        暫存變量儲存於int t[VAR_MAX]中
        bug : varMap中tabley運行irRun時透過mapNew function取得記憶體，但從未被釋放，若重複呼叫irRun將會造成內存洩漏
    