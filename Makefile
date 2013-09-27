warmup2: core.o input.o my402list.o
	gcc -o warmup2 -g core.o input.o my402list.o -lpthread -lm

core.o: core.c my402list.h warmup2.h core.h
	gcc -g -c -Wall core.c -lpthread -lm

input.o: input.c my402list.h  warmup2.h
	gcc -g -c -Wall input.c

my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c

clean:
	rm -f *.o input
