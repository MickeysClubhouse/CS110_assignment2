#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

int pathname_lookup_inumber(struct unixfilesystem *fs, char *pathname, int* inumber);

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {

  char path[strlen(pathname)];
  strcpy(path, pathname);   //copy the string pathname
  int root_inumber = ROOT_INUMBER;
  int* inumber;
  inumber =&root_inumber;
  if(strcmp(path, "/") == 0) return root_inumber;    //if the pathname is root pathname ("/")

  int value = pathname_lookup_inumber(fs, path, inumber);

  return value;
}

int pathname_lookup_inumber(struct unixfilesystem *fs, char *path, int* inumber) {

  const char *delim = "/";   // delimiter
  struct direntv6 currDir;    //current directory
  char mypath[strlen(path) - 1];
  strcpy(mypath, path + 1);   // copy string name after the first delimiter
  char* currPathname= strtok(mypath, delim);  //first string that occurs before next delimiter
  path += strlen(currPathname) + 1;     // after first delimiter and first nondelimiter string
  if (directory_findname(fs, currPathname, *inumber, &currDir)< 0) {

    return -1;        // if error occurred when reading the current directory
  }
  *inumber = currDir.d_inumber;      // inumber of current directory
  if (strlen(path) != 0) {     //if the rest of the path name is not empty
    int value = pathname_lookup_inumber(fs, path, inumber);     // recursively search the inumber
  }

  return *inumber;     // if the rest of the path name is empty, return the current directory's inumber
}
