#coding:utf-8
import socket
import threading
import ctypes
gDict={}#保存客户的sock

MySqlDll = ctypes.windll.LoadLibrary("..\\..\\MySqlDll.dll")
HandleSql = MySqlDll.MySql_LineMySql("127.0.0.1".encode("gbk"), "root".encode("gbk"), "admin".encode("gbk"),"virus".encode("gbk"), 3306)
if(HandleSql == 0):
    print("数据库连接失败！" )
print("数据库连接成功！")
gSocketSever = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
gSocketSever.bind(("127.0.0.1", 8888))
gSocketSever.listen(5)
print("服务端启动成功！")

def main():
    MySqlDll.MySql_Execute(HandleSql, "SET NAMES GBK".encode("gbk"))
    print("重置数据库成功！")
    nT = threading.Thread(target=AcceptProc)
    nT.start()
def AcceptProc():
    while True:
        nSocketClient,nAddrClient = gSocketSever.accept()
        print(nSocketClient,"连接成功！")
        nT = threading.Thread(target=RecvDataProc,args=(nSocketClient,nAddrClient))
        nT.start()
def RecvDataProc(*arg):
    nSocketClient=arg[0]
    nAddrClient=arg[1]
    while True:
        try:
            nRecvByte =  nSocketClient.recv(65535)
            nRecvByte=bytearray(nRecvByte)
            nRecvTemp =bytearray(nRecvByte)

            for x in range(len(nRecvByte)):
                nRecvByte[x]^=5

            nMessage = nRecvByte.decode("utf-16")

            if(nMessage==""):
                print(nAddrClient,"客户端退出！")
                try:
                    del gDict[nAddrClient]
                    nSocketClient.close()
                except:
                    print("处理删除sock异常")
                return

            print(nAddrClient, " - 解密前：", nRecvTemp.decode("utf-16")," - 解密后：" ,nMessage)

            nList = nMessage.split("|#|")
            HeadData=nList[0]
            if(HeadData == "上传病毒"):
                nFileName = nList[1]
                nFileMd5 = nList[2]
                AddVirusInfo(nFileName,nFileMd5)

            elif(HeadData == "下载病毒"):
                nVirusInfo = GetVirusInfo()
                print(nVirusInfo)
                nSocketClient.send(str("下载病毒|#|" + nVirusInfo).encode("gbk"))
        except:
            print("异常！")
            return

def AddVirusInfo(nFileName,nFileMd5):
    nStr = "select * from info where md5 = '" + nFileMd5 + "'"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nVal =  MySqlDll.MySql_GetField(nRecord,"name".encode("gbk"))
    if(nVal!=0):return
    nStr = "insert into info ( `name`, `md5`) values ('" + nFileName + "' , '"+nFileMd5 + "')"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))



def GetVirusInfo():
    nStr = "select * from info"
    MySqlDll.MySql_Execute(HandleSql,nStr.encode("gbk"))
    nRecord = MySqlDll.MySql_GetRecord(HandleSql)
    nRecordNum = MySqlDll.MySql_GetRecordNum(nRecord)

    nVirusInfo=""
    for x in range(nRecordNum):
        nVal = MySqlDll.MySql_GetField(nRecord, "name".encode("gbk"))
        nTempName = ctypes.string_at(nVal, -1).decode('gbk')

        nVal = MySqlDll.MySql_GetField(nRecord, "md5".encode("gbk"))
        nTempMd5 = ctypes.string_at(nVal, -1).decode('gbk')

        if(x == nRecordNum-1):
            nVirusInfo = nVirusInfo + nTempName + "|" + nTempMd5
        else:
            nVirusInfo = nVirusInfo + nTempName + "|" + nTempMd5 + "|*|"

        MySqlDll.MySql_NextRecord(nRecord)

    MySqlDll.MySql_FreeRecord(nRecord)
    return nVirusInfo

if __name__ == "__main__":
    main()

