#pragma once

#define DEFAULT_DIR		"sdmc:/"
#define MAX_LIST			28
#define MAX_DIRS			20

struct dirList_t {
	char** 		files;
	int 			fileNum;

	char**		directories;
	int 			dirNum;

	char* 		currentDir;
};
