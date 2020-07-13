#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

// copy file
void do_copy_file(char* src, char* dst) {
  FILE *fd, *dest;
	char buf[257];
	int len;

	fd = fopen(src, "rb"); // no binary mode may produce bug (see pwnable.kr dos4fun)
	dest = fopen(dst, "wb");
	
	while ( len = fread(buf, 1, 256, fd) )
	{
		// buf[len] = '\0'; // not needed
		fwrite(buf, 1, len, dest);
		printf("Length : %d\n", len);
		// printf("%s\n", buf); // may contain null in file data
	}

	fclose(dest);
	fclose(fd);
}

// return 1 when file, else 0
// https://gist.github.com/cubicdaiya/5271121
// https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
int checkFile(char* fname) {
  struct stat path_stat;
  stat(fname, &path_stat);
  return S_ISREG(path_stat.st_mode);
}
int checkDir(char* dname) {
  struct stat path_stat;
  stat(dname, &path_stat);
  return S_ISDIR(path_stat.st_mode);
}

// base code from cpy.c in lecture
int main(int argc, char *argv[])
{
	int i = 0;
  char* path; // for path gen

	printf("ARGC : %d\n", argc);
	
  // argc check
	if ( argc < 3 ) {
		printf("Usage : %s <filename1> <filename2>\n", argv[0]);
		printf("or\nUsage : %s <filename1> <filename2> ... <directory name>\n", argv[0]);
		return 0;
	}

  // first mode : copy single file
  if(argc == 3) {
    if(!checkFile(argv[1])) {
      printf("Wrong Argument : %s is not a file", argv[1]);
      return 1;
    }
    if(!checkFile(argv[2])) {
      printf("Wrong Argument : %s is not a file", argv[2]);
      return 1;
    }
    do_copy_file(argv[1], argv[2]);
  }

  // second mode : copy multiple files
  else {
    for(i = 1; i < argc-1; i++) {
      if(!checkFile(argv[i])) {
        printf("Wrong Argument : %s is not a file", argv[i]);
        return 1;
      }
    }
    if(!checkDir(argv[argc-1])) {
      printf("Wrong Argument : %s is not a directory", argv[argc-1]);
      return 1;
    }
    for(i = 1; i < argc-1; i++) {
      path = calloc(2+strlen(argv[i])+1+strlen(argv[argc-1])+1, 1);
      strcpy(path, "./");
      strcpy(path+2, argv[argc-1]); // may be unsafe, but no oob
      path[strlen(argv[argc-1])+2] = '/';
      strcpy(path+2+strlen(argv[argc-1])+1, argv[i]);
      do_copy_file(argv[i], path);
      free(path);
    }
  }

  return 0;
}
