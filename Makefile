bn: bn.c
	gcc -o bn bn.c -Wall -Wextra -Wshadow -g -fsanitize=address,undefined

twosum: TwoSum.c
	gcc -o twosum TwoSum.c -Wall -Wextra -Wshadow -g -fsanitize=address,undefined

fiber: fiber.c
	gcc -o fiber fiber.c -Wall -Wextra -Wshadow -g -fsanitize=address,undefined

clean:
	rm bn twosum fiber *.o
