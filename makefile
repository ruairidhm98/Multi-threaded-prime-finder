CC=clang # C compiler
EXE=mtprimes # Exectuable file
OBJ=bounded_buffer.o mtprimes.o isprime.o minheap.o # Object code
CFLAGS=-Wall -Werror -g # Compiler flags
# Compiles object code into executable file
$(EXE) : $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Compiles source code into object code
%.o : %.c
	$(CC) $(CFLAGS) -c $^

.PHONY: clean run

# Deletes object code and executable file
clean:
	rm *.o
	rm $(EXE)

# Runs the executable
run:
	./mtprimes -b 100 -l 1000 -t 4

