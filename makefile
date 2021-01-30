CFLAGS=-std=c11 -g -static

ycc: ycc.c

test:ycc
	./test.sh
clean:
	rm -f ycc *.o *~ ycc*
.PHONY:	test clean
