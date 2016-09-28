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
	m=(char **)malloc(sizeof(void)*ncol);
	for(i=0;i<ncol;i++)
	{
		c=malloc(PERSIZE);
		if(c == NULL)
		{
			for(j=0;j<i;j++)
				free(m[j]);
			printf("内存分配失败！\n");
			return 1;
		}
		m[i]=c;
	}
	lp=(void*)m;
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
				//free(lp);
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






