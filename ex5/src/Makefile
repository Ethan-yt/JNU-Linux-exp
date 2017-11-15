prog: a.o b.o c.o d.o assmb.o
	gcc a.o b.o c.o d.o assmb.o –L/home/user/lib –lm –o prog 
a.o: a.c
	gcc –c a.c 
b.o: b.c defs.h
	gcc –c b.c 
c.o: c.c
	gcc –c c.c 
d.o: d.c defs.h
	gcc –c d.c 
assmb.o: assmb.s
	as –o assmb.o assmb.s
clean:
	rm prog *.o