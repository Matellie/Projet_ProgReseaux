

server:
	cd Serveur && \
	gcc server.c awale.c -o server

run-server: server
	./Serveur/server

client:
	cd Client && \
	gcc client.c -o client

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
	cd Awale && gcc awale.c -o awale

run-awale: awale
	cd Awale && ./awale

valgrind-awale:
	cd Awale && \
	gcc -Wall -g awale.c -o awale && \
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./awale