/*
   这是一个postgresql数据库通用操作的c程序包装库，该库的作用就是
   能够简单、轻松的在c下使用postgresql数据库。

   				Author:tybitsfox Email:tybitsfox@163.com
 */
#ifndef CLS_OWNE
#include"clsscr.h"
#include<postgresql/libpq-fe.h>
#endif

//{{{常量的定义
#define		EX_STEP1	"BEGIN"
#define		EX_STEP2	"DECLARE %s CURSOR FOR %s"
#define		EX_STEP3	"FETCH ALL IN %s"
#define		EX_STEP4	"END"
#define		SLEN		4096
#define		PERSIZE		1024
#define		sfile		"./pgsql_set.ini"
//}}}

//{{{结构的定义 struct pg_struct
struct pg_struct
{
	char	host[64];			//主机名
	char	dbname[64];			//数据库名 --必填
	char	user[64];			//用户名
	char	pwd[64];			//密码
	char	haddr[64];			//主机地址
	char	port[64];			//端口
	char	sql[SLEN];			//SQL查询字符串
	char	cur[64];			//事物游标名
};
/*连接字符串格式：host=aa dbname=dd user=uu password=pwd hostaddr=127.0.0.1
 其中dbname是必填项，其他的项为可选，但是如果填了host，则必须填写user和password
 */
//}}}

//{{{查询操作相关函数的定义
//以pg_开头的函数是提供给用户调用的，以ng_开头的主要是子功能实现的函数，一般无需用户调用
int	pg_sel_begin(struct pg_struct *p);	//负责数据库连接，事务开始，发送SQL查询的函数
int	pg_sel_getval(int index);			//执行fetch取得一条记录数据的函数
void	pg_sel_end();					//负责事务结束，清理内存，关闭连接的函数
//----------------ng functions---------------------
int	ng_sel_crtcon(struct pg_struct *p);	//负责格式化连接字符串及用户输入检查的函数
int	ng_sel_malloc();					//负责内存申请分配的函数

//}}}
//{{{新建，编辑，删除相关操作的函数定义
int pg_cmd_begin(struct pg_struct *p);  //命令操作函数的初始化函数
void pg_cmd_end();
//}}}
//{{{2016-10-1添加的用于分析参数或配置文件来获取用户录入信息的函数
int parse_para(int argc,char **argv,struct pg_struct *p);
int from_ini(struct pg_struct *p);
//}}}



