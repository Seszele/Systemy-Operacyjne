void **reserveArray(int size);
int getFileInfo(char *fileName);                    // returns file descriptor of tmp result file
int saveBlock(int fd, void **array, int size);      // returns array index, -1 if array is full
int removeBlock(int index, void **array, int size); // removes block, returns 1 if succeds and -1 otherwise
