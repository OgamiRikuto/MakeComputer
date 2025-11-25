/*
    c言語でコンピュータ作ってみよう
    
    命令リストは "codelist.txt"を参照
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>

#define N /*Counter increment by*/1
//レジスター数
#define R_N 5

#define BIT2 2
#define BIT4 4
#define BIT8 8

//最大数
#define MAXLINES 100
#define MAXLEN 255

/*構造体*/
typedef struct  {
    int sum;
    int carry;
}ADDER;

typedef struct {
    int bit[BIT8];
}_8BIT;
typedef struct  {
    int bit[BIT4];
}_4BIT;

typedef struct {
    int bit[BIT2];
}_2BIT;

/**命令命名リスト**/
struct list {
    int bitcode;
    char *code;
    struct list *next;
};
typedef struct list LIST;

LIST *first;    //リストへの先頭ポインタ

//出入力用変数
int input=0;
int output=0;

//レジスター用変数
_8BIT REG = {};
int reg[R_N] = {};

//カウンター用変数
int cnt = 0;

/*関数プロトタイプ宣言*/
double logi(double);
int NOT(int);
int NAND(int, int);
int AND(int, int);
int NOR(int, int);
int OR(int, int);
int XOR(int, int);
int XNOR(int, int);
int on(int);
int off(int);
int or(int, int, int);
int and(int, int, int);
ADDER fa(int, int, int);
_8BIT to8(int);
int to10(_8BIT);
//int Switch(int, int);
//int byteSwitch(int, int);
int byteOR(int, int);
int byteNOT(int);
int byteNAND(int, int);
int byteNOR(int, int);
int byteAND(int, int);
ADDER byteADD(int, int, int);
ADDER byteSUB(int, int, int);
int byteXOR(int, int);
int byteXNOR(int, int);
int negative(int);
int mux(int, int, int);
int bitMemory(int, int);
void REGISTER(int, int, int, int *);
_2BIT bitDecorder(int);
_8BIT bit3Decorder(int, int, int, int);
_4BIT DEC(int);
int COND(int, int);
int ALU(int, int, int);
int counter(int, int);
void computer(int);

void error(char *);
void inputList(bool);
void showListAll();
void searchList();
void deleteList();
int searchCode();

void programExecution(int);
void execution(int);
void run();

void cat();

void print_reg();

//ロジスティック関数
double logi(double n)   {
    return 1 / (1+exp(-n));
}

/*2input1output---------------------------------*/
//NAND関数
int NAND(int x1, int x2)  {
    const int bias = 30;
    const int w1 = -20;
    const int w2 = -20;
    if(logi(bias+x1*w1+x2*w2) >= 0.5) return 1;
    else return 0;
}
//NOT関数
int NOT(int in)  {
    return NAND(in, in);
}
//AND関数
int AND(int x1, int x2)   {
    return NOT(NAND(x1, x2));
}
//NOR関数
int NOR(int x1, int x2)   {
    return NOT(NAND(NOT(x1),NOT(x2)));
}
//OR関数
int OR(int x1, int x2)   {
    return NAND(NOT(x1), NOT(x2));
}
//XOR関数
int XOR(int x1, int x2) {
    return OR(NOR(NOT(x1), x2),NOR(x1, NOT(x2)));
}
//XNOR関数
int XNOR(int x1, int x2)    {
    return NOT(XOR(x1, x2));
}
/*----------------------------------------------*/
/*3input1output---------------------------------*/
//OR関数
int or(int x1, int x2, int x3)  {
    return OR(OR(x1, x2), x3);
}
//AND関数
int and(int x1, int x2, int x3) {
    return AND(AND(x1, x2), x3);
}
/*----------------------------------------------*/
//on
int on(int in)    {
    return NAND(in, NOT(in));
}
//off
int off(int in)   {
    return NOT(NAND(in, NOT(in)));
}
//FULL ADDER
ADDER fa(int x1, int x2, int x3) {
    ADDER out;
    out.sum = XOR(XOR(x1, x2), x3);
    out.carry = OR(AND(x1, x2), AND(XOR(x1, x2),x3));
    return out;
}
//10進→2進
_8BIT to8(int in)   {
    _8BIT out;
    for(int i=0; i<8; i++)  {
        out.bit[i] = in % 2;
        in/=2;
    }
    return out;
}
//2進→10進
int to10(_8BIT bits)    {
    int sum=0;
    for(int i=0; i<BIT8; i++)  {
        sum += bits.bit[i]*pow(2, i);
    }
    return sum;
}
//bit switch
int Switch(int enable, int in)  {
    if(enable)  return in;
    else return 0;
}
//byte switch
int byteSwitch(int enable, int in)   {
    if(enable) return in;
    else return 0;
}
/*Byte論理関数------------------------------------*/
//byte_or関数
int byteOR(int in1, int in2) {
    _8BIT n1 = to8(in1);
    _8BIT n2 = to8(in2);
    _8BIT out;
    for(int i=0; i<BIT8; i++)   {
        out.bit[i] = OR(n1.bit[i], n2.bit[i]);
    }
    return to10(out);
}
//byte_not関数
int byteNOT(int in)    {
    _8BIT n = to8(in);
    _8BIT out;
    for(int i=0; i<BIT8; i++)   {
        out.bit[i] = NOT(n.bit[i]);
    }
    return to10(out);
}
//byte_nand関数
int byteNAND(int in1, int in2)    {
    return byteOR((byteNOT(byteOR(in1, in1))),(byteNOT(byteOR(in2, in2))));
}
//byte_nor関数
int byteNOR(int in1, int in2) {
    return byteNOT(byteOR(in1,in2));
}
//byte_and関数
int byteAND(int in1, int in2) {
    return byteNOT(byteOR(byteNOT(byteOR(in1, in1)),(byteNOT(byteOR(in2, in2)))));
}
//ByteAdd関数
ADDER byteADD(int in, int in1, int in2)   {
    _8BIT n1 = to8(in1);
    _8BIT n2 = to8(in2);
    _8BIT sum;
    int ca;
    sum.bit[0] = fa(in, n1.bit[0], n2.bit[0]).sum;
    ca = fa(in, n1.bit[0], n2.bit[0]).carry;
    for(int i=1; i<BIT8; i++)   {
        sum.bit[i] = fa(ca, n1.bit[i], n2.bit[i]).sum;
        ca = fa(ca, n1.bit[i], n2.bit[i]).carry;
    }
    ADDER result;
    result.sum = to10(sum);
    result.carry = ca;
    return result;
}
//ByteSub関数
ADDER byteSUB(int in, int in1, int in2)   {
    _8BIT n1 = to8(in1);
    _8BIT n2 = to8(in2);
    ADDER out = byteADD(in, in1, negative(in2));
    return out;
}
int byteXOR(int in1, int in2)   {
    return byteOR(byteNOR(in1, byteNOT(in2)), byteNOR(byteNOT(in1), in2));
}
int byteXNOR(int in1, int in2)  {
    return byteNOT(byteXOR(in1, in2));
}
/*----------------------------------------------*/
//input selecter
int mux(int in, int n1, int n2)    {
    if(!in) return n1;
    else return n2;
}
//negative num
int negative(int n) {
    return byteADD(0, byteNOT(n), on(1)).sum;
}
/* c言語の使用上使えないレジスタ関数
//1bit memory
int bitMemory(int clk, int value)   {
    int tmp = reg;
    if(clk) reg = value;
    return tmp;
}

//byte memory
void REGISTER(int load, int save, int value, int *out)   {
    _8BIT in = to8(value);
    _8BIT tmp = REG;
    _8BIT empty = {};
    if(save)
        for(int i=0; i<BIT8; i++)   {
            REG.bit[i] = in.bit[i];
        }
}
*/
//1bit decorder
_2BIT bitDecorder(int in)   {
    _2BIT out;
    out.bit[0] = !in;
    out.bit[1] = in;
    return out;
}
//3bit decorder
_8BIT bit3Decorder(int enable, int n1, int n2, int n3)    {
    _8BIT out = {};
    if(enable) return out;
    out.bit[0] = NOR(NOT(NOR(n1,n2)),n3);
    out.bit[1] = AND(n1,NOR(n2,n3));
    out.bit[2] = AND(NOR(n1,n3),n2);
    out.bit[3] = AND(AND(n1,n2),NOT(n3));
    out.bit[4] = AND(NOR(n1,n2),n3);
    out.bit[5] = AND(AND(n1,n3),NOT(n2));
    out.bit[6] = AND(NOT(n1),AND(n2,n3));
    out.bit[7] = and(n1,n2,n3);
    return out;
}
//instruction decorder
_4BIT DEC(int in)  {
    _8BIT n = to8(in);
    _4BIT out;
    out.bit[0] = NOR(n.bit[6], n.bit[7]);
    out.bit[1] = AND(n.bit[6], NOT(n.bit[7]));
    out.bit[2] = AND(NOT(n.bit[6]), n.bit[7]);
    out.bit[3] = AND(n.bit[6], n.bit[7]);
    return out;
}
//条件分岐
int COND(int in1, int in2)   {
    _8BIT n1 = to8(in1);
    _8BIT n2 = to8(in2);
    _8BIT code = bit3Decorder(0, n1.bit[0], n1.bit[1], 0);
    _2BIT dec = bitDecorder(n1.bit[2]);
    int n2_or = or(or(n2.bit[0],n2.bit[1],n2.bit[2]),or(n2.bit[3],n2.bit[4],n2.bit[5]),OR(n2.bit[6],n2.bit[7]));
    if(dec.bit[0])  {
        for(int i=0; i<4; i++)  {
            if(code.bit[i]) {
                switch(i){
                    case 0:
                        return off(0);
                    case 1:
                        return NOT(n2_or);
                    case 2:
                        return n2.bit[7]; 
                    case 3:
                        return OR(NOT(n2_or), n2.bit[7]);
                }
            }
        }
    }
    else {
         for(int i=0; i<4; i++)  {
            if(code.bit[i]) {
                switch(i){
                    case 0:
                        return NOT(off(0));
                    case 1:
                        return NOT(NOT(n2_or));
                    case 2:
                        return NOT(n2.bit[7]); 
                    case 3:
                        return NOT(OR(NOT(n2_or), n2.bit[7]));
                }
            }
        }
    }
    return 0;
}
//ALU
int ALU(int n, int in1, int in2)    {
    _8BIT dec = bit3Decorder(0, to8(n).bit[0], to8(n).bit[1], to8(n).bit[2]);
    const int or = 0;
    const int nand = 1;
    const int nor = 2;
    const int and = 3;
    const int add = 4;
    const int sub = 5;
    for(int i=0; i<BIT8; i++)   {
        if(dec.bit[i])  {
            switch(i)  {
            case or:
                return byteOR(in1, in2);
            case nand:
                return byteNAND(in1, in2);
            case nor:
                return byteNOR(in1, in2);
            case and:
                return byteAND(in1, in2);
            case add:
                //printf("add(%d,%d)\n", in1, in2);
                return byteADD(0, in1, in2).sum;
            case sub:
                return byteSUB(0, in1, in2).sum;
            default:
                return 0;
            }
        }
    }
    return 0;
}
//カウンター
int counter(int action, int in) {
    if(action)  cnt = in;
    else cnt+=N;
    return cnt;
}

//turing complete
void computer(int instruction)   {
    _8BIT to_from = to8(instruction);
    _4BIT ins = DEC(instruction);
    //for(int i=0; i<BIT4; i++)   printf("ins.bit[%d]=%d\n", i, ins.bit[i]);
    _8BIT from = bit3Decorder(!ins.bit[2],to_from.bit[3],to_from.bit[4],to_from.bit[5]);
    //for(int i=0; i<BIT8; i++)   printf("from.bit[%d]=%d\n", i, from.bit[i]);
    _8BIT to = bit3Decorder(!ins.bit[2],to_from.bit[0],to_from.bit[1],to_from.bit[2]);
    //for(int i=0; i<BIT8; i++)   printf("to.bit[%d]=%d\n", i, to.bit[i]);
    const int immediate = 0;
    const int compute = 1;
    const int copy = 2;
    const int condition = 3;
    int regFrom, regTo;
    for(int i=0; i<BIT4; i++)   {
        if(ins.bit[i])  {
            switch(i)  {
            case immediate:
                reg[0] = instruction;
                break;
            case compute:
                reg[3] = ALU(instruction, reg[1], reg[2]);
                break;
            case copy:
                for(int i=0; i<BIT8; i++)   if(from.bit[i]) regFrom = i;
                for(int i=0; i<BIT8; i++)   if(to.bit[i]) regTo = i;
                //printf("regFrom=%d\n", regFrom);
                //printf("regTo=%d\n", regTo);                
                if(regFrom==6)  {
                    if(regTo==6)    output = input;
                    else reg[regTo] = input;
                }
                else {
                    if(regTo==6)    output = reg[regFrom];
                    else reg[regTo] = reg[regFrom];
                }
                break;
            case condition:
                counter(COND(instruction, reg[3]), reg[0]);
                break;
            default:
                break;
            }
        }
    }
    print_reg();
}

//ここからは実行のための関数---------------------------
//デバッグ用
void print_reg()    {
    for(int i=0; i<6; i++)  {
        printf("reg[%d] = %d\n", i, reg[i]);
    }
}

void print8bit(_8BIT in)    {
    for(int i=7; i>=0; i--) printf("%d", in.bit[i]);
    printf("\n");
}

// 標準エラー出力に、エラーメッセージを出力して終了
void error(char *str)   {
    fprintf(stderr, "%s\n", str);
    exit(1);
}

//入力用関数
_8BIT inputBinary()  {
    _8BIT out;
    for(int i=BIT8-1; i>=0; i--)   {
        scanf("%d", &out.bit[i]);
    }
    return out;
}


//リスト用関数
void inputList(bool is10)    {
    LIST *pt;
    char buff[MAXLEN+1];

    pt = (LIST *)malloc(sizeof(LIST));
    if(pt==NULL)    error("LISTのmalloc失敗");
    pt->next = first;
    first = pt;

    printf("命令コード：");
    if(is10)    scanf("%d", &(pt->bitcode));
    else pt->bitcode = to10(inputBinary());
    printf("入力：%d\n", pt->bitcode);

    printf("命名：");
    scanf("%s", buff);
    pt->code = (char *)malloc(sizeof(char)*(strlen(buff)+1));
    if(pt->code==NULL)  error("命名領域のmalloc失敗");
    strcpy(pt->code, buff);
}

void showListAll()  {
    LIST *pt;
    int count = 0;

    pt = first;         
    while (pt) {        
        printf("%3d: %3d %s\n", ++count, pt->bitcode, pt->code);
        pt = pt->next;
    }
}

void searchList()   {
    LIST *pt;

    int in;
    printf("命令コード = ");
    scanf("%d", &in);
    
    
    pt = first;
    while(pt)   {
        if(pt->bitcode == in)  {
            printf("%s\n", pt->code);
            return;
        }
        pt = pt->next;
    }
}

void deleteList()   {
    LIST *pt;
    LIST *temp = first;
    char buff[MAXLEN+1];

    char *key;
    printf("名前=");
    scanf("%s", buff);
    key = (char *)malloc(sizeof(char) * (strlen(buff)+1));
    if(key == NULL) error("名前領域のmalloc失敗");
    strcpy(key, buff);

    pt = first;
    while(pt)   {   
        if(!strcmp(pt->code, key))  {
            temp->next = pt->next;
            free(pt->code);
            free(pt);
            return;
        }
        temp = pt;
        pt = pt->next;
    }
    if(pt==NULL)    {
        first = NULL;
    }
}

int searchCode() {
    LIST *pt;
    char buff[MAXLEN+1];

    char *key;
    scanf("%s", buff);
    key = (char *)malloc(sizeof(char)*(strlen(buff)+1));
    if(key==NULL)   error("命名のmalloc失敗");
    strcpy(key, buff);

    pt = first;
    while(pt)   {
        if(!strcmp(pt->code, key))  return pt->bitcode;
        pt = pt->next;
    }
    return 255;
}

//実行関数
void programExecution(int way) {
    int instructions[MAXLINES+1];
    int count=1;
    while(1)    {
        printf("%d: ", count);
        switch(way) {
        case 0://2進数
            instructions[count] = to10(inputBinary());
            break;
        case 1://10進数
            scanf("%d", &instructions[count]);
            break;
        case 2://コード
            instructions[count] = searchCode();
            break;
        default: break;
        }
        if(instructions[count]==255) break;
        count++;
    }
    while(cnt<=count)   {
        computer(instructions[counter(0,0)]);
    }
}

void execution(int way) {
    int instruction;
    while(1){
        printf("命令:");
        switch(way) {
        case 0://2進数
            instruction = to10(inputBinary());
            break;
        case 1://10進数
            scanf("%d", &instruction);
            break;
        case 2://コード
            instruction = searchCode();
            break;
        default: break;
        }
        printf("%d\n", instruction);
        if(instruction == 255) break;
        computer(instruction);
    }
}

void run()   {
    int inputWay;
    int exeWay;

    while(1)    {
        printf("入力方法選択\n2進数:0, 10進数:1, コード:2\n->");
        scanf("%d", &inputWay);
        if(inputWay == 0 || inputWay == 1 || inputWay == 2) break;
        printf("Illegal command\n");
    }
    while(1)    {
        printf("順次命令入力型:0, プログラム型:1\nmode:");
        scanf("%d", &exeWay);
        if(exeWay == 0 || exeWay == 1)  break;
        printf("Illegal command\n");
    }
    if(exeWay) programExecution(inputWay);
    else    execution(inputWay);
    printf("%d\n", output);
}

void cat()  {
    FILE *f;
    f = fopen("codelist.txt", "r");

    char *str;
    while(fgets(str, MAXLEN, f)!=NULL) {
        printf("%s", str);
    }
    printf("\n");
    fclose(f);
}

int main()  {
    /*命令に命名して保存する機能*/
    /**命令の入力方法は2進数と10進数の2通り**/
    /**命令入力→命名入力の順**/

    /*毎回命令を入力して実行する方法*/
    /*命令を全て入力してから実行する方法*/
    /**それぞれ命名した命令で実行する方法と2進数で命令する方法、10進数で命令する方法の3通り**/
    int ret;
    char buff[MAXLEN+1];

    first = NULL;
    while(1)    {
        cnt = 0;
        printf("命名(2進数):b, 命名(10進数):t, 命名一覧:s, 命名検索:f, 命名削除:d, 実行:r, コード説明:c, 終了:e\nmode:");
        ret = scanf("%s", buff);
        if(ret == EOF)  break;
        switch(buff[0]) {
            case 'b':
                inputList(false);
                break;
            case 't':
                inputList(true);
                break;
            case 's':
                showListAll();
                break;
            case 'f':
                searchList();
                break;
            case 'd':
                deleteList();
                break;
            case 'r':
                run();
                break;
            case 'c':
                cat();
                break;
            case 'e':
                exit(0);
                break;
            default:
                printf("Illegal command\n");
                break;
        }
    }
    return 0;
}
