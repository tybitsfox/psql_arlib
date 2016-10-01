#include"pg_sel.h"

extern char *lp;
extern int	nrow;
extern int	ncol;

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
		case 5://open setup file error
			printf("open setup file error\n");
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











