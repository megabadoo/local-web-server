#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
main()
{
  int len;
  DIR *dirp;
  struct dirent *dp;

  dirp = opendir(".");
	printf("<html><body:");
  while ((dp = readdir(dirp)) != NULL)
    	//prepend requested resource
	printf("<a href=\"%s\">%s</a>\n", dp->d_name, dp->d_name);
	printf("</body.</html>");
  (void)closedir(dirp);
}
