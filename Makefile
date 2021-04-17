vector: vector.c
	gcc -o vector vector.c -Wall -Wextra -Wshadow -g -fsanitize=address,undefined

vector_test: vector_test.c
	gcc -o vector_test vector_test.c -Wall -Wextra -Wshadow -g -fsanitize=address,undefined

vector_test_cpp: vector_test.cpp
	g++ vector_test.cpp -o vector_test

clean:
	rm vector vector_test vector_test_cpp *.o
