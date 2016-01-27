server: new_serv.cpp
	g++ -o server new_serv.cpp
run: server
	./server 3000
