CC=gcc

.PHONY: clean install

pam_graph.so: pam_graph.o
	${CC} -g -shared -o $@ $? -lpam -lcrypt

pam_graph.o: pam_graph.c
	${CC} -g -fPIC -c $?

install: pam_graph.so
	cp $< /usr/lib64/security/

clean: 
	-rm *.o
	-rm *.so
