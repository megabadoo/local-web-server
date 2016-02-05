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
#include <algorithm>
#include <queue>
#include <pthread.h>
#include <semaphore.h>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         10000
#define MESSAGE             "This is the message I'm sending back and forth"
#define QUEUE_SIZE          10000

using namespace std;
std::queue<int> work;
sem_t work_to_do;
sem_t space_on_q;
sem_t mutex1;

struct thread_params {
        long thread_id;
        std::string dir;
	
	thread_params():thread_id(0),dir(""){};
};

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

void* serve(void* arg){
	void* myVoidPtr;	
	        //long thread_id = long(arg);
        struct thread_params* tp = (struct thread_params*) arg;
        std::cout << "I'm thread " << tp->thread_id << std::endl;
        std::cout << "\t" << tp->dir << std::endl;
        
	string dir = tp->dir;
	
//	for(;;){

        sem_wait(&work_to_do);
        sem_wait(&mutex1);

        int hSocket = work.front();
        work.pop();

        std::cout << tp->thread_id << " working on " << hSocket << std::endl;

        sem_post(&mutex1);
        sem_post(&space_on_q);

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
	return myVoidPtr;
}

string original_rs = rs;
//for now, hardcode the prepended path into rs
rs = dir + rs;

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
	if(rs.find_last_of("/")!=rs.length()-1)
		rs += "/";
	if(original_rs.find_last_of("/")!=original_rs.length()-1)
                original_rs += "/";
	cout << rs << " is a directory with a slash" << endl;
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
	string msg = "<html><body><h1>" + original_rs + "</h1><ul>";
	while ((dp = readdir(dirp)) != NULL){
	//prepend requested resource
        msg+="<li><a href=\"";
	msg += original_rs;
	msg+= dp->d_name;
	msg+= "\">";
	msg+=dp->d_name;
	msg+="</a></li>\n";
}
        msg+="</ul></body></html>";

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



//	shutdown(hSocket, SHUT_RDWR);
    printf("\nClosing the socket");
        // close socket 
        if(close(hSocket) == SOCKET_ERROR)
        {
         printf("\nCould not close socket\n");
         exit(0);
        }



//}
	return myVoidPtr;
}

int main(int argc, char* argv[])
{
    int hSocket,hServerSocket;  /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address; /* Internet socket address stuct */
    int nAddressSize=sizeof(struct sockaddr_in);
    char pBuffer[BUFFER_SIZE];
    int nHostPort;
	string dir;
	   int queue_size = 1;
        if(sem_init(&space_on_q, 0, queue_size)!=0){
		cout << "Error initializaing semaphore space_on_q" << endl;
		exit(0);
	}
        if(sem_init(&work_to_do, 0, 0)!=0){
		cout << "Error initializing semaphore work_to_do" << endl;
		exit(0);
	}
        if(sem_init(&mutex1, 0, 1)!=0){
		cout << "Error initializing semaphore mutex1" << endl;
		exit(0);
	}

        int num_threads = 1;
        std::cout << "threads hello!" << std::endl;
        pthread_t thread;

    


    if(argc < 3)
      {
        printf("\nUsage: server host-port directory\n");
        exit(0);
      }
    else
      {
        nHostPort=atoi(argv[1]);
	dir = argv[2];
      }
            struct thread_params* tp=new thread_params();
     
    for(long i =0; i < num_threads; i++){

               tp->thread_id = i;
                tp->dir = dir;
                int ret_val = pthread_create(&thread,
                NULL,
                serve,
		(void*) tp);

		if(ret_val!=0){
			cout << "Error forming thread" << endl;
		}
    }

cout << "ID: " << tp->thread_id << endl;
cout << "DIR: " << tp->dir << endl;



    printf("\nStarting server");

    printf("\nMaking socket");
    /* make a socket */
    hServerSocket=socket(AF_INET,SOCK_STREAM,0);

    if(hServerSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        exit(0);
    }

    /* fill address struct */
    Address.sin_addr.s_addr=INADDR_ANY;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;


int optval = 1;
setsockopt (hSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

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


  for(int i=0;;i++){
        
        printf("\nWaiting for a connection\n");
        /* get the connected socket */
        hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);
	if(hSocket == SOCKET_ERROR){
		cout << "Could not connect to socket" << endl;
		exit(0);
	}

//	serve(tp);
	
	cout << "tp->thread_id: " << tp->thread_id << endl;
	cout << "tp->dir: " << tp->dir << endl;
        //accept (returns an int, push that int on the queue)
                sem_wait(&space_on_q);
                sem_wait(&mutex1);

                sleep(1);
                work.push(hSocket);
                std::cout << "pushed " << hSocket << std::endl;

                sem_post(&mutex1);
                sem_post(&work_to_do);
        }

}


