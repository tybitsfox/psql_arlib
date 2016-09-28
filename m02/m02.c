#include"pg_sel.h"
extern char *lp;
extern int	nrow;
extern int	ncol;
int main(int argc,char **argv)
{
	char **m;
	int i,j;
	struct pg_struct p;
	memset((void*)&p,0,sizeof(p));
	strcpy(p.dbname,"ty004");
//	strcpy(p.sql,"select * from basemsg");
//	strcpy(p.sql,"insert into basemsg (uid,uname,pwd,tel) values (3,'田裕农','tianym95131','15621585051')");
	strcpy(p.sql,"drop table ta001");
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
	if(pg_cmd_begin(&p) == 0)
		pg_cmd_end();
	return 0;
}



