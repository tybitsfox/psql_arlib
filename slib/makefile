vpath %.h ../include
tag	=	libmypsql_s.a
src	=	pg_sel.c
hd	=	pg_sel.h pg_val.h
obj	=	$(patsubst %.c,%.o,$(src))
inc	=	-I/workarea/cprogram/include -I../include
flg	=	-c -lpq

$(tag):$(obj)
	ar -cur $@ $^
$(obj):%.o:%.c %.h
	gcc -o $@ $(flg) $(inc) $<
clean:
	rm ./*.o ./*.a


