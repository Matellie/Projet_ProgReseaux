awale:
	cd Awale && gcc awale.c -o awale

run-awale:
	cd Awale && ./awale

valgrind-awale:
	cd Awale && \
	gcc -Wall -g awale.c -o awale && \
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./all