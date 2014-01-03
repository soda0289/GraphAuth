CC=gcc

.PHONY: clean

pam_graph.so: pam_graph.o
	${CC} -g -shared -o $@ $? -lpam -lcrypt

pam_graph.o: pam_graph.c
	${CC} -g -fPIC -c $?

clean: 
	rm *.o
	rm *.so
