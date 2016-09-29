#include"pg_sel.h"
extern char *lp;
extern int	nrow;
extern int	ncol;

char pch[7][3]={"-d\0","-s\0","-h\0","-u\0","-p\0","-P\0","-a\0"};
char *pp[7];
int  pin[7]={1,2,4,8,16,32,64};
int  plen[7];

int parse_para(int argc,char **argv,struct pg_struct *p);
//{{{int main(int argc,char **argv)
int main(int argc,char **argv)
{
	char **m;
	int i,j;
	struct pg_struct p;
	i=parse_para(argc,argv,&p);
	switch(i)
	{
		case 0://success!
			break;
		case 1://para repeat
			printf("parameters repeat error\n");
			return 0;
		case 2://buffer size
			printf("buffer size too less\n");
			return 0;
		case 3://help
			printf("Usage:\n%s -d dbname [-hupPa para] -s SQLstr\n\
Example:\n%s -d ty004 [-h localhost -u root -p password -P 5432 -a 192.168.1.2] -s 'select * from basemsg'\n",argv[0],argv[0]);
			return 0;
		case 4://must be input
			printf("The datebase name and SQL string must be input!\n");
			return 0;
	};
	printf("dbname=%s user=%s password=%s host=%s hostaddr=%s port=%s\nconnstr:%s\n",p.dbname,p.user,p.pwd,\
			p.host,p.haddr,p.port,p.sql);
/*	pg_sel_begin(&p);
	m=(char **)lp;
	for(i=0;i<nrow;i++)
	{
		pg_sel_getval(i);
		for(j=0;j<ncol;j++)
		{
			printf("%s\t",m[j]);
		}
		printf("\n");
	}
	pg_sel_end(); */
//	if(pg_cmd_begin(&p) == 0)
//		pg_cmd_end();
	return 0;
}//}}}
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
	switch(argc)
	{
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
	memset((void*)p,0,sizeof(struct pg_struct));
	pp[0]=p->dbname;pp[1]=p->sql;
	pp[2]=p->host;pp[3]=p->user;
	pp[4]=p->pwd;pp[5]=p->port;
	pp[6]=p->haddr;
	plen[0]=sizeof(p->dbname);plen[1]=sizeof(p->sql);
	plen[2]=sizeof(p->host);plen[3]=sizeof(p->user);
	plen[4]=sizeof(p->pwd);plen[5]=sizeof(p->port);
	plen[6]=sizeof(p->haddr);
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


