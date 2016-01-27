#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	struct stat filestat;

	if(stat(argv[1], &filestat)) {
		cout <<"ERROR in stat\n";
	}
	if(S_ISREG(filestat.st_mode)) {
		cout << argv[1] << " is a regular file \n";
		cout << "file size = "<<filestat.st_size <<"\n";
	}
	if(S_ISDIR(filestat.st_mode)) {
		cout << argv[1] << " is a directory \n";
	}
	
}

