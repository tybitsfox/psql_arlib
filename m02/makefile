vpath %.h ../include
tag	=	m02
src	=	m02.c
hd	=	pg_sel.h
inc	=	-I/workarea/cprogram/include -I../include
flg	=	-L../slib -lpq -lmypsql_s

$(tag):$(src) $(hd)
	gcc -o $@ $< $(inc) $(flg)
clean:
	rm m02

