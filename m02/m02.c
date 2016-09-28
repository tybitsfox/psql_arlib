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
	strcpy(p.sql,"select * from basemsg");
	pg_sel_begin(&p);
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
	pg_sel_end();
	return 0;
}



