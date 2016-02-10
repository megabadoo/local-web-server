server: new_serv.cpp
	g++ -std=c++11 -o server new_serv.cpp -lpthread
run: server
	./server 3000 10 ~/htdocs/CS360/basic-web-server/testDir/
debug: server
	gdb ./server 3000 10 ~/htdocs/CS360/basic-web-server/testDir/
