my_shell: my_shell.o
	gcc -o my_shell my_shell.o
my_shell.o: my_shell.s
	gcc -c my_shell.s
my_shell.s: my_shell.c
	gcc -S my_shell.c
clean:
	rm -f my_shell *.o my_shell.s
