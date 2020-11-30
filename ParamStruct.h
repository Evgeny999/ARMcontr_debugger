#ifndef PARAMSTRUCT_H
#define PARAMSTRUCT_H
#include <QMainWindow>
#include <QTcpSocket>
const int CheckBoxMaxNum=20;
const int MaxNumberOfCostomName=10;

typedef struct{
    QString IpAddr;
    int Port;
    int NumberOfChannal;
    QTcpSocket IpPortSocket;
    int VirtualCounter;
    QString CustomName;
    bool Flag;
}ParamStruct;

typedef struct{
    QString Address;
    int HexDecBinCount;
    int Count8_16_32;
    bool ExitFlag;
}Row_Param_Struct;

typedef struct{
    QString ColumnIpAddress;
    int ColumnPORT;
    int ColumnChannel;
    bool ExitFlag;
    int ColumnVirtualCounter;
}Column_Param_Struct;

typedef struct{
    int Checked[CheckBoxMaxNum];
    bool ExitFlag;
}Check_Box_Struct;

typedef struct{
    QString Address;
    QString Code;
    QString CommandAndInstruction;
}Process_Window_Row_Struct;

typedef struct{
    QString Registr;
    QString RegInfo;
    QString RegName;
    short Size;
}Reg_WindTable_Struct;
#endif // PARAMSTRUCT_H
