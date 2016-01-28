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
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <dirent.h>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         10000
#define MESSAGE             "This is the message I'm sending back and forth"
#define QUEUE_SIZE          10000

using namespace std;

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

int get_file_size(string path){
	struct stat filestat;
	if(stat(path.c_str(), &filestat)){
	return -1;
	}
	return filestat.st_size;
}

void serve(int hSocket){

	string line;
	vector<char*> headers;
	GetHeaderLines(headers, hSocket, false);
	
	for(int i=0; i<headers.size(); i++){
		string l(headers[i]);
		if(l.find("GET")!=std::string::npos){
			line = l;
		}
	}


	//set content type and length

//parse the get request use stat to determine type of request (folder, regular file, invalid 4040
//parse the GET line

//find the requested resource (/foo.html)
//store requested resource (file path)
//in variable rs
stringstream ss;
ss.str(line.substr(8));
string rs;
ss >> rs;

if(rs.compare("/favicon.ico")==0){
	return;
}


//for now, hardcode the prepended path into rs
string prepended = "/Users/meganarnell/Documents/CS360 Internet Programming/local-web-server/testDir";
rs = prepended + rs;

//determine file type of requested resource
struct stat filestat;

if(stat(rs.c_str(), &filestat)){
	cout << "ERROR in stat" << endl;
	rs = "/Users/meganarnell/Documents/CS360 Internet Programming/local-web-server/404.html";
	//return a canned 404 response
	//with 404 headers and body
	 char pBuffer[BUFFER_SIZE];
        memset(pBuffer, 0, sizeof(pBuffer));
        int file_size = get_file_size(rs);
        sprintf(pBuffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Lengh: %d\r\n\r\n", file_size);

        int write_result = write(hSocket, pBuffer, strlen(pBuffer));
        if(write_result==SOCKET_ERROR){
                cout << "Error writing" << endl;
                exit(0);
        }
        FILE* fp = fopen(rs.c_str(), "r");

        char* buffer = (char*)malloc(file_size);

        int file_read_result = fread(buffer, file_size, 1, fp);
        if(file_read_result == -1){
                cout << "Error reading from file" << endl;
                exit(0);
        }

        write_result = write(hSocket, buffer, file_size);
        if(write_result==SOCKET_ERROR){
                cout << "Error writing" << endl;
                exit(0);
        }
}
else if(S_ISREG(filestat.st_mode)){

//determine content-type
string extension = rs.substr(rs.find_last_of("."));
transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
string contentType="";

if(extension.compare(".html")==0){
        contentType = "text/html";
}
else if(extension.compare(".jpg")==0 || extension.compare(".jpeg")==0){
        contentType = "image/jpg";
}
else if(extension.compare(".gif")==0){
        contentType = "image/gif";
}
else{
        contentType = "text/plain";
}

	//format headers
	//read file
	//send it to client
	char pBuffer[BUFFER_SIZE];
   	memset(pBuffer, 0, sizeof(pBuffer));
	int file_size = get_file_size(rs);
        sprintf(pBuffer, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Lengh: %d\r\n\r\n", contentType.c_str(), file_size);
	
        int write_result = write(hSocket, pBuffer, strlen(pBuffer));
        if(write_result==SOCKET_ERROR){
		cout << "Error writing" << endl;
		exit(0);
	}
	FILE* fp = fopen(rs.c_str(), "r");

        char* buffer = (char*)malloc(file_size);
        
	int file_read_result = fread(buffer, file_size, 1, fp);
	if(file_read_result == -1){
		cout << "Error reading from file" << endl;
		exit(0);
	}

        write_result = write(hSocket, buffer, file_size);
	if(write_result==SOCKET_ERROR){
                cout << "Error writing" << endl;
                exit(0);
        }
}

else if(S_ISDIR(filestat.st_mode)){
	cout << rs << " is a directory" << endl;
	//look for index.html (run stat function again)
	if(stat((rs+"index.html").c_str(), &filestat)){
		//index doesn't exist!
		//read dir listing
		//generate html
		//send appropriate headers
		//and body to client

	int len;
  	DIR *dirp;
 	 struct dirent *dp;

  	dirp = opendir(rs.c_str());
        string msg = "<html><body>";
 	 while ((dp = readdir(dirp)) != NULL){
        //prepend requested resource
        msg+="<a href=\"";
	msg+= dp->d_name;
	msg+= "\">";
	msg+=dp->d_name;
	msg+="</a>\n";
        msg+="</body></html>";
}
  	(void)closedir(dirp);
	
	int i = msg.length();
	string content_length = to_string(i);

	msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + content_length + "\r\n\r\n" + msg;

	cout << "MESSAGE IS: " << msg << endl;

	int write_result = write(hSocket, msg.c_str(), msg.length());
	if(write_result == -1){
		cout << "Error writing directory" << endl;
		exit(0);
	}

	}
	else{
		cout << "Found index.html in directory" << endl;
		//format headers
		//read index.html
		//send all to client
		//send it to client
	rs = rs+"/index.html";
        char pBuffer[BUFFER_SIZE];
        memset(pBuffer, 0, sizeof(pBuffer));
        int file_size = get_file_size(rs);
        sprintf(pBuffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Lengh: %d\r\n\r\n", file_size);

        int write_result = write(hSocket, pBuffer, strlen(pBuffer));
        if(write_result==SOCKET_ERROR){
                cout << "Error writing" << endl;
                exit(0);
        }
        FILE* fp = fopen(rs.c_str(), "r");

        char* buffer = (char*)malloc(file_size);

        int file_read_result = fread(buffer, file_size, 1, fp);
        if(file_read_result == -1){
                cout << "Error reading from file" << endl;
                exit(0);
        }

        write_result = write(hSocket, buffer, file_size);
        if(write_result==SOCKET_ERROR){
                cout << "Error writing" << endl;
                exit(0);
        }
	}	
}







//
/*
	char pBuffer[BUFFER_SIZE];
	memset(pBuffer, 0, sizeof(pBuffer));
//change to path input variable + requested resource (prepend command-line param to the GET request resource i.e. "/foo.html"
	int file_size = get_file_size("maxresdefault.jpg");
	sprintf(pBuffer, "HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\nContent-Lengh: %d\r\n\r\n", file_size);
//alwasys check system calls
	write(hSocket, pBuffer, strlen(pBuffer));
	FILE* fp = fopen("maxresdefault.jpg", "r");
	
	char *buffer = (char*)malloc(file_size); 
	fread(buffer, file_size, 1, fp);
//check above
	
	write(hSocket, buffer, file_size);




	std::string msg = ss.str();
	std::cout << "write" << std:: endl;
//	write(hSocket, msg.c_str(), msg.size());
	std::cout << "finished writing" << std:: endl;


	shutdown(hSocket, SHUT-RDWR);
	close(hSocket);*/
//	string msg = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 8\r\n\r\nHeyGuys!";
        /* number returned by read() and write() is the number of bytes
        ** read or written, with -1 being that an error occured
        ** write what we received back to the server */
  //      write(hSocket,msg.c_str(),msg.length());



//	shutdown(hSocket, SHUT_RDWR);
    printf("\nClosing the socket");
        // close socket 
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
