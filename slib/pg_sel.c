#include"pg_sel.h"
#include"pg_val.h"

//{{{int	ng_sel_crtcon(struct pg_struct *p)
/*函数说明：验证用户输入的信息，并将其格式化为连接字符串
  参数：	自定义的数据结构
  返回值：	=0	成功
  			=1	错误
  */
int	ng_sel_crtcon(struct pg_struct *p)
{
	int i;
	char *m;
	if(p==NULL)
	{
		printf("数据库连接信息不能为空\n");
		return 1;
	}
	if((strlen(p->dbname) <= 0) || (strlen(p->sql) <= 0))
	{
		printf("数据库名称和查询字符串不能为空\n");
		return 1;
	}
	memset(constr,0,SLEN);
	m=constr;i=SLEN;
	snprintf(m,i,"dbname=%s",p->dbname);
	m=constr;
	i=strlen(m);m+=i;i=SLEN-i;
	if(strlen(p->host) > 0)
	{
		snprintf(m,i," host=%s",p->host);
		m=constr;i=strlen(m);
		m+=i;i=SLEN-i;		
	}
	if(strlen(p->haddr) > 0)
	{
		snprintf(m,i," hostaddr=%s",p->haddr);
		m=constr;i=strlen(m);
		m+=i;i=SLEN-i;
	}
	if(strlen(p->user) > 0)
	{
		snprintf(m,i," user=%s",p->user);
		m=constr;i=strlen(m);
		m+=i;i=SLEN-i;
	}
	if(strlen(p->pwd) > 0)
	{
		snprintf(m,i," password=%s",p->pwd);
		m=constr;i=strlen(m);
		m+=i;i=SLEN-i;
	}
	if(strlen(p->port) > 0)
	{
		snprintf(m,i," port=%s",p->port);
		m=constr;i=strlen(m);
		m+=i;i=SLEN-i;
	}
	if(strlen(p->cur) <= 0)
		snprintf(p->cur,sizeof(p->cur),"aatiny");
	return 0;
}//}}}
//{{{int ng_sel_malloc()
/*函数说明：根据取得记录的字段数来确定分配内存的函数。
  参数：	无
  返回值：	=0	成功,全局变量lp中保存了分配的地址指针
  			=1	错误，lp=NULL
 */
int ng_sel_malloc()
{
	int i,j;
	char **m,*c;
	if(lp != NULL)
	{
		printf("当前缓冲区指针不为空\n");
		return 1;
	}
	if((conn == NULL) || (res == NULL))
	{
		printf("当前事务或者连接有错误\n");
		return 1;
	}
	nrow=PQntuples(res);
	ncol=PQnfields(res);
	lp=malloc(sizeof(char*)*ncol);
	m=(char **)lp;
	for(i=0;i<ncol;i++)
	{
		c=malloc(PERSIZE);
		if(c == NULL)
		{
			for(j=0;j<i;j++)
				free(m[j]);
			free(lp);
			printf("内存分配失败！\n");
			return 1;
		}
		m[i]=c;
	}
//	lp=(void*)m;
	return 0;
}//}}}
//{{{int pg_sel_begin(struct pg_struct *p)
/*函数说明：postgresql数据库查询操作的主要包装函数，简化了c语言对postgresql数据库的操作。
  			本函数主要实现了用户信息的录入检查，建立连接，开启事务，开始SQL查询，内存申请操作。
参数：		一个自定义的用于传输用户信息的结构指针
返回值：	0,本函数对于过程错误将调用包含exit()的退出函数，所以本函数的返回值没有意义
			而退出函数将根据本程序中设置的指示器的不同值采取不同的退出前清理工作。
 */
int	pg_sel_begin(struct pg_struct *p)
{
	indicator=3;
	if(ng_sel_crtcon(p))
		pg_sel_end();
	conn=PQconnectdb(constr);
	if(PQstatus(conn) != CONNECTION_OK)
	{
		printf("connect error\nconstr: %s",constr);
		pg_sel_end();
	}
	res=PQexec(conn,EX_STEP1);
	indicator=1;
	if(PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		printf("exec step1 error\n");
		pg_sel_end();
	}
	PQclear(res);
	memset(constr,0,sizeof(constr));
	snprintf(constr,sizeof(constr),EX_STEP2,p->cur,p->sql);
	res=PQexec(conn,constr);
	if(PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		printf("exec step2 error\n%s\n",constr);
		pg_sel_end();
	}
	PQclear(res);
	memset(constr,0,sizeof(constr));
	snprintf(constr,sizeof(constr),EX_STEP3,p->cur);
	res=PQexec(conn,constr);
	if(PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		printf("exec step3 error\n");
		pg_sel_end();
	}
	if(ng_sel_malloc())
	{
		printf("exec step4 error\n");
		pg_sel_end();
	}
	indicator=0;	
	return 0;
}//}}}
//{{{int	pg_sel_getval(int index)
/*函数说明：	取得一条指定序号的记录，在取得记录时，该函数是操作最频繁的函数。因此该函数应尽可能的简化
  参数：		待取得记录的索引号
  返回值：		=0成功
  				=1指定的行号无效
				=2默认的字段长度太小
 */
int	pg_sel_getval(int index)
{
	int i;char **m;
	if((index >= nrow) || (index <0))
		return 1;
	m=(char **)lp;
	for(i=0;i<ncol;i++)
	{
		memset(m[i],0,PERSIZE);
		strncpy(m[i],PQgetvalue(res,index,i),PERSIZE);
	}
	return 0;
}//}}}
//{{{void	pg_sel_end()
/*函数说明：清理及退出函数，该函数依据indicator的设置进行不同的清理退出工作。
  参数：	无
  返回值：	无	
 */
void pg_sel_end()
{
	char **p;
	int i;
	switch(indicator)
	{
		case 0:	//成功，需要执行释放内存，关闭事务，关闭连接的操作
			if(lp != NULL)
			{
				p=(char **)lp;
				for(i=0;i<ncol;i++)
					free(p[i]);
				free(lp);
				lp=NULL;
			}
			ncol=0;nrow=0;indicator=0;
		case 1: //申请内存失败，需要执行关闭事务和关闭连接的操作
			if(res != NULL)
			{
				PQclear(res);
				res=NULL;
			}
		case 2: //开始事务失败，需要执行关闭连接的操作
			if(conn != NULL)
			{
				PQfinish(conn);
				conn=NULL;
			}
		case 3:	//连接失败，或者初始化输入错误
			break;
	};
	if(indicator)
		exit(0);
	return;
}//}}}
//{{{---------下面的两个函数是用于命令操作的-----------
//}}}
//{{{int pg_cmd_begin(struct pg_struct *p)
int pg_cmd_begin(struct pg_struct *p)
{
	if(ng_sel_crtcon(p))
		pg_cmd_end();
	conn=PQconnectdb(constr);
	if(PQstatus(conn) != CONNECTION_OK)
	{
		printf("connect error\n%s\n",constr);
		pg_cmd_end();
	}
	res=PQexec(conn,p->sql);
	if(PQresultStatus(res) != PGRES_COMMAND_OK)
		printf("exec error\n");
	pg_cmd_end();
	return 0;
}//}}}
//{{{void pg_cmd_end()
void pg_cmd_end()
{
	if(res != NULL)
		PQclear(res);
	res=NULL;
	if(conn != NULL)
		PQfinish(conn);
	conn=NULL;
	exit(0);
}//}}}
//{{{-----------下面的两个函数是用于分析参数或配置文件来获取用户输入信息的----------
//}}}
//{{{int parse_para(int argc,char **argv,struct pg_struct *p)
/*函数说明：这是一个参数解析函数，为了使测试程序更加灵活，便于测试，采用基本信息由参数传递的形式。
  参数：	前两个参数argc,argv是主程序的参数，直接传入。
  			第三个参数为自定义的用于存储用户输入信息的结构指针。
  返回值：	0：	成功
  			1:	参数有重复
			2:	接受参数的缓冲区太小 ---这个错误应该很少出现
			3:	参数的数量不正确
			4:	缺少必须输入的数据库名称和SQL字符串
 */
int parse_para(int argc,char **argv,struct pg_struct *p)
{//5,7,9,11,13,15 enabled
	int i,j,k,l;
	memset((void*)p,0,sizeof(struct pg_struct));
	pp[0]=p->dbname;pp[1]=p->sql;
	pp[2]=p->host;pp[3]=p->user;
	pp[4]=p->pwd;pp[5]=p->port;
	pp[6]=p->haddr;
	plen[0]=sizeof(p->dbname);plen[1]=sizeof(p->sql);
	plen[2]=sizeof(p->host);plen[3]=sizeof(p->user);
	plen[4]=sizeof(p->pwd);plen[5]=sizeof(p->port);
	plen[6]=sizeof(p->haddr);
	switch(argc)
	{
		case 1:	//new,use setup-file
			return from_ini(p); 
		case 5:
		case 7:
		case 9:
		case 11:
		case 13:
		case 15:
			break;
		default:
			return 3;
	};
	k=0;
	for(i=1;i<(argc-1);i++)
	{
		if((i%2)==0)
			continue;
		for(l=0;l<7;l++)
		{
			if(memcmp(argv[i],pch[l],strlen(argv[i]))==0)
			{
				if(k & pin[l])
					return 1; //repeat!
				j=strlen(argv[i+1]);
				if(j>=plen[l])
					return 2;//buffer size too less
				memcpy(pp[l],argv[i+1],j);
				k|=pin[l];
				break;
			}
		}
	}
	if((strlen(p->dbname) <= 0) || (strlen(p->sql) <= 0))
		return 4;//must be input
	return 0;
}//}}}
//{{{int from_ini(struct pg_struct *p)
/*函数说明：	本函数用于从配置文件获取用户操作数据库的信息
  参数：		一个自定义的用于存储用户输入信息的文件。
  返回值：		本函数的返回值与函数parse_para的返回值是一致的，只是多了一个打开文件失败的错误返回
  				0:	成功
				1:	参数有重复
				2:	接受参数的缓冲区太小
				3:	参数数量不正确，这是parse_para函数的返回值，本函数没有这个返回值
				4：	缺少必须输入的数据库名称和SQL字符串
 */
int from_ini(struct pg_struct *p)
{
	int i,j,k;
	char *c1,*c2,ch[PERSIZE];
	FILE *file=fopen(sfile,"r");
	if(file==NULL)
		return 5;	//open file error
	memset(ch,0,sizeof(ch));
	c1=ch;
	while(fgets(ch,PERSIZE,file))
	{
		for(i=0;i<7;i++)
		{
			if(strstr(ch,ich[i])==c1)
			{
				k=strlen(ch)-1;
				if(ch[k] == '\n')
					ch[k]=0;
				if(ch[k-1]=='\r') //no use in linux
					ch[k-1]=0;
				c2=c1+strlen(ich[i]);
				if(strlen(c2) >= plen[i])
				{
					fclose(file);
					return 2;//buffer size too less
				}
				if(strlen(pp[i])>0)
				{
					fclose(file);
					return 1;//input repeat
				}
				memcpy(pp[i],c2,strlen(c2));
				break;
			}
		}
		memset(ch,0,sizeof(ch));
	}
	fclose(file);
	if(strlen(pp[0])<=0 || strlen(pp[1])<=0)
		return 4;//must be input
	return 0;
}//}}}
//{{{本模块的帮助说明程序
void help()
{
	CLS();
	printf("本模块是为了方便c程序使用postgresql数据库而将postgresql的c接口进行打包操作的模块库\n");
	printf("为方便模块的使用，由本人编写的所有静态模块库都遵从如下约定：在所有的模块中都必须有一个\n");
	printf(COLOR_RED,"help()");
	printf("函数，以方便使用者掌握该模块中所有可使用的函数及函数功能和自定义的数据结构。\n");
	printf("本模块包含的用户调用函数有：\n");
	printf(COLOR_GREEN,"int pg_sel_begin(struct pg_struct *p)\n");
	printf("函数说明：	postgresql数据库查询操作的主要包装函数，简化了c语言对postgresql数据库的操作。\n\
		本函数用于查询操作，主要实现了用户信息的录入检查，建立连接，开启事务，开始SQL查询，内存申请操作。\n\
参数：		一个自定义的用于传输用户信息的结构指针\n\
返回值：	0,本函数对于过程错误将调用包含exit()的退出函数，所以本函数的返回值没有意义\n\
		而退出函数将根据本程序中设置的指示器的不同值采取不同的退出前清理工作。\n");
	printf(COLOR_GREEN,"int pg_sel_getval(int index)\n");
	printf("函数说明：	本函数用户查询操作，取得一条指定序号的记录，在取得记录时，该函数是操作最频繁的函数。因此该函数应尽可能的简化\n\
参数：		待取得记录的索引号\n\
返回值：	=0成功\n\
  		=1指定的行号无效\n\
		=2默认的字段长度太小\n");
	printf(COLOR_GREEN,"void pg_sel_end()\n");
	printf("函数说明：\t本函数用于查询操作的清理及退出函数，该函数依据indicator的设置进行不同的清理退出工作。\n参数：\t\t无\n返回值：\t无\n");
	printf(COLOR_GREEN,"int pg_cmd_begin(struct pg_struct *p)\n");
	printf("函数说明：\t本函数用于查询之外的数据库操作，执行本函数不会有返回的记录结果\n参数：\t\t一个自定义的用于传输用户信息的结构指针\n\
返回值：\t0：执行成功，然后执行pg_cmd_end函数退出即可，不为0的话会自动调用pg_cmd_end函数自动退出。\n");
	printf(COLOR_GREEN,"void pg_cmd_end()\n");
	printf("函数说明：\t本函数配合pg_cmd_begin()函数使用，用于查询之外操作的退出清理工作\n参数：\t\t无\n\
返回值：\t无，本函数会调用exit()函数直接退出。\n");
	printf(COLOR_GREEN,"int parse_para(int argc,char **argv,struct pg_struct *p)\n");
	printf("函数说明：\t这是一个参数解析函数，为了使程序更加灵活，采用基本信息由参数传递的形式。新的版本更支持\n\t\t\
配置文件传递信息，本程序会自动调用下面的配置文件分析函数: int from_ini(struct pg_struct *p)\n\
参数：\t\t前两个参数argc,argv是主程序的参数，直接传入。\n\t\t第三个参数为自定义的用于存储用户输入信息的结构指针。\n\
返回值：\t0：	成功\n\t\t1:	参数有重复\n\t\t2:	接受参数的缓冲区太小 ---这个错误应该很少出现\n\
\t\t3:	参数的数量不正确\n\t\t4:	缺少必须输入的数据库名称和SQL字符串\n");
	printf(COLOR_GREEN,"int from_ini(struct pg_struct *p)\n");
	printf("函数说明：\t本函数用于从配置文件获取用户操作数据库的信息\n参数：\t\t一个自定义的用于存储用户输入信息的文件。\n\
返回值：\t本函数的返回值与函数parse_para的返回值是一致的，只是多了一个打开文件失败的错误返回\n\t\t0:\t成功\n\
\t\t1:\t参数有重复\n\t\t2:\t接受参数的缓冲区太小\n\t\t4：\t缺少必须输入的数据库名称和SQL字符串\n\t\t5：\t打开配置文件错误\n");
	printf(COLOR_GREEN,"int	ng_sel_crtcon(struct pg_struct *p)\n");
	printf("函数说明：\t验证用户输入的信息，并将其格式化为连接字符串，该函数为内部功能性函数，不建议用户直接调用\n\
参数：\t\t自定义的数据结构\n返回值：\t\t=0\t成功\n\t\t=1\t错误\n");
	printf(COLOR_GREEN,"int ng_sel_malloc()\n");
	printf("函数说明：根据取得记录的字段数来确定分配内存的函数。该函数为内部功能性函数，不建议用户直接调用\n\
参数：\t\t无\n返回值：\t=0\t成功,全局变量lp中保存了分配的地址指针\n\t\t=1\t错误，lp=NULL\n");
	printf(COLOR_RED,"本函数定义的结构：struct pg_struct\n");
	printf("struct pg_struct\n{\n\tchar	host[64];\t\t//主机名\n\tchar	dbname[64];\t\t//数据库名 --必填\n\
\tchar	user[64];\t\t//用户名\n\tchar	pwd[64];\t\t//密码\n\tchar	haddr[64];\t\t//主机地址\n\tchar	port[64];\t\t//端口\n\
\tchar	sql[SLEN];\t\t//SQL查询字符串,SLEN=4096\n\tchar	cur[64];\t\t//事物游标名\n};\n");
	printf(COLOR_BLUE,"配置文件的说明\n");
	printf("文件名：./pgsql_set.ini \n设置及示例：\n#this is a config file for my postgresql lib\n#\tauthor:tybitsfox	2016-10-1\n\
#char ich[7][10]={\"QDBNAME=\",\"QSQL=\",\"QHOST=\",\"QUSER=\",\"QPWD=\",\"QPORT=\",\"QHADDR=\"};\n\
#those are const fields in upline do not modified it!\n\
QDBNAME=ty004\nQSQL=select * from basemsg\nQHOST=localhost\nQUSER=root\nQPWD=fuckyou\nQPORT=5432\nQHADDR=127.0.0.1\n");
	printf(COLOR_GREEN,"其他说明：\n");
	printf("由于本模块现为测试阶段，所有定义的指针、数据结构等多为全局变量，暂不具备可重入性。待正式发布后在予以改进\n");
	printf(COLOR_GREEN,"\tAuthor:tybitsfox\tBug report: tyyyyt@163.com\t2016-10-2\n\n");
}//}}}


