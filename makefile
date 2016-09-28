obj	=	slib m02
.PHONY:	all $(obj)
all:$(obj)

$(obj):
	$(MAKE) -C $@ $(target)

