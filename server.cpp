#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "cs360utils.h"
#include <vector>
#include <fstream>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         1000
#define MESSAGE             "This is the message I'm sending back and forth"
#define QUEUE_SIZE          5

string get_file_contents(const char* filename){
	ifstream in(filename, std::ios::in | std::ios::binary);

	if(in){
	std::string contents;
	in.seekg(0,std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0],contents.size());
	in.close();
	return(contents);
	}
	else{
	//couldn't read file
	return "";
	}
}

//int get_file_size(stringpath){
//	struct state filestat;
//	if(stat(path.c_str(), &filestat)){
//	return -1;
//}
//	return size;
//}

void serve(int hSocket){


	vector<char*> headers;
	GetHeaderLines(headers, hSocket, false);
	
	for(int i=0; i<headers.size(); i++){
		cout << headers[i] << endl;
	}
	cout << "**********" << endl;


	//set content type and length

//parse the get request use stat to determine type of request (folder, regular file, invalid 4040

//

	char pBuffer[BUFFER_SIZE];
	memset(pBuffer, 0, sizeof(pBuffer));

	int file_size = get_fiel_size("maxresdefault.jpg");
	sprintf(pBuffer, "HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\nContent-Lengh: %d\r\n\r\n", file_size);
//alwasys check system calls
	write(hSocket, pBuffer, strlen(pBuffer));
	FILE* fp = fopen("maxresdefault.jpg", "r");
	
	char *buffer = (char*)malloc(file_size); 
	fread(buffer, file_size, 1, fp);
//check above
	
	write(hSocket, buffer, file_size);


dfd

	std::string msg = ss.str();
	std::cout << "write" << std:: endl;
//	write(hSocket, msg.c_str(), msg.size());
	std::cout << "finished writing" << std:: endl;


	shutdown(hSocket, SHUT-RDWR);
	close(hSocket);
	string pBuffer = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 9\r\n\r\nHeyGuys!";
        /* number returned by read() and write() is the number of bytes
        ** read or written, with -1 being that an error occured
        ** write what we received back to the server */
        write(hSocket,pBuffer.c_str(),pBuffer.length());


	cout << "*******************************" << endl;


    printf("\nClosing the socket");
        /* close socket */
        if(close(hSocket) == SOCKET_ERROR)
        {
         printf("\nCould not close socket\n");
         exit(0);
        }
}

int main(int argc, char* argv[])
{
    int hSocket,hServerSocket;  /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address; /* Internet socket address stuct */
    int nAddressSize=sizeof(struct sockaddr_in);
    char pBuffer[BUFFER_SIZE];
    int nHostPort;

    if(argc < 2)
      {
        printf("\nUsage: server host-port\n");
        return 0;
      }
    else
      {
        nHostPort=atoi(argv[1]);
      }

    printf("\nStarting server");

    printf("\nMaking socket");
    /* make a socket */
    hServerSocket=socket(AF_INET,SOCK_STREAM,0);

    if(hServerSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* fill address struct */
    Address.sin_addr.s_addr=INADDR_ANY;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;

    printf("\nBinding to port %d",nHostPort);

    /* bind to a port */
    if(bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address)) 
                        == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }
 /*  get port number */
    getsockname( hServerSocket, (struct sockaddr *) &Address,(socklen_t *)&nAddressSize);
    printf("opened socket as fd (%d) on port (%d) for stream i/o\n",hServerSocket, ntohs(Address.sin_port) );

        printf("Server\n\
              sin_family        = %d\n\
              sin_addr.s_addr   = %d\n\
              sin_port          = %d\n"
              , Address.sin_family
              , Address.sin_addr.s_addr
              , ntohs(Address.sin_port)
            );


    printf("\nMaking a listen queue of %d elements",QUEUE_SIZE);
    /* establish listen queue */
    if(listen(hServerSocket,QUEUE_SIZE) == SOCKET_ERROR)
    {
        printf("\nCould not listen\n");
        return 0;
    }

    for(;;)
    {
        printf("\nWaiting for a connection\n");
        /* get the connected socket */
        hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);
	serve(hSocket);
	}
}
