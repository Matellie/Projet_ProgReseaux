server:
	cd Serveur && \
	gcc -Wall server.c awale.c -o server

run-server: server
	./Serveur/server

valgrind-server:
	cd Serveur && \
	gcc -Wall -g server.c awale.c -o server && \
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./server

client:
	cd Client && \
	gcc -Wall client.c -o client

alice:
	cd Client && \
	./client 127.0.0.1 Alice

bob:
	cd Client && \
	./client 127.0.0.1 Bob

charles:
	cd Client && \
	./client 127.0.0.1 Charles


awale:
	cd Serveur && \
	gcc -Wall awale.c -o awale

run-awale: awale
	cd Serveur && ./awale

valgrind-awale:
	cd Serveur && \
	gcc -Wall -g awale.c -o awale && \
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./awale