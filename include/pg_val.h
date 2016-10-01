/*
   这是一个postgresql数据库通用操作的c程序包装库，该库的作用就是
   能够简单、轻松的在c下使用postgresql数据库。

   				Author:tybitsfox Email:tybitsfox@163.com
 */
#ifndef	CLS_OWNE
#include"clsscr.h"
#include<postgresql/libpq-fe.h>
#endif
//这个头文件将定义一些库函数所用的变量
PGconn		*conn=NULL;
PGresult	*res=NULL;
char		*lp=NULL;
char		constr[SLEN];
int			indicator=0;
int			nrow=0;
int			ncol=0;
//下面的设置用于用户录入信息的取得
char ich[7][10]={"QDBNAME=","QSQL=","QHOST=","QUSER=","QPWD=","QPORT=","QHADDR="};
char pch[7][3]={"-d","-s","-h","-u","-p","-P","-a"};
char *pp[7];
int  pin[7]={1,2,4,8,16,32,64};
int  plen[7];




