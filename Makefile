warmup2: warmup2.o thr_func.o my402list.o
	gcc -o warmup2 -g warmup2.o thr_func.o my402list.o -lpthread -lm -lrt
warmup2.o: warmup2.c thr_func.h my402list.h
	gcc -g -c -Wall warmup2.c
thr_func.o: thr_func.c thr_func.h my402list.h
	gcc -g -c -Wall thr_func.c
my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c
clean:
	rm -f *.o warmup2
