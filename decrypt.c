#include <stdio.h>
#include <stdlib.h>
#include <magic.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
//[TO DO]In the future would be nice to make this algorithm more general
unsigned int getFileSize(char *fileName){
        unsigned int size;
        FILE *f = fopen(fileName, "r");
        fseek(f, 0L, SEEK_END);
        size = ftell(f);
        fclose(f);
        return size;
}
void xor(char* file, int buffLen, int key){
	for(int i=0; i < buffLen; i++)
	    file[i] = file[i] ^ key;
}

int main(int argc, char **argv){
    char foundFile[50], outPath[] = "./found_files/", *actual_file = "./program";
    if(argc<2){
	fprintf(stderr, "Usage: %s <path_to_the_file> [key]\n", argv[0]);
	exit(1); // I was to lazy to search the error code for missing file
    }
    FILE *fout, *fin = fopen(argv[1], "rb");
    const char *magic_full;
    magic_t magic_cookie;
    unsigned int fileSize = getFileSize(argv[1]);
    char *fileData = (char*) malloc(sizeof(char) * fileSize);
    char *fileData_bk = (char*) malloc(sizeof(char) * fileSize);
    
    if(argc==2){
	    DIR* dir = opendir(outPath);//check if outPath dir exists
	    if (dir)
		closedir(dir);/* Directory exists. */
	    else if (ENOENT == errno){
		fprintf(stderr, "Directory %s does not exist!\n", outPath);
		exit(1);
	    } else/* opendir() failed for some other reason. */
		perror("Error");
    }
    
    magic_cookie = magic_open(0);
	
    if (magic_cookie == NULL) {
        printf("unable to initialize magic library\n");
        return 1;
    }

    //Loading default magic database
    if (magic_load(magic_cookie, NULL) != 0) {
        printf("cannot load magic database - %s\n", magic_error(magic_cookie));
        magic_close(magic_cookie);
        return 1;
    }
    //load file
    fread(fileData, sizeof(char), fileSize, fin);
    memcpy(fileData_bk, fileData, fileSize);

    //if we receive the key as the second parameter avoid bruteforcing
    if(argc==3){//I'm not proud of this is is mretty messy, but id gets the job don
        xor(fileData, fileSize, atoi(argv[2]));
        magic_full = magic_buffer(magic_cookie, fileData, fileSize);
	printf("[computing] %s\n", magic_full);
	if(strcmp("data",magic_full)!=0){
   	    fout = fopen("f.out", "wb");
	    fwrite(fileData, sizeof(char), fileSize, fout);
	    fclose(fout);
	}
	magic_close(magic_cookie);
        return 0;
    }
    // bruteforce the file
    for(int key=0; key<256; key++){
	xor(fileData, fileSize, key);
	//change this function
	magic_full = magic_buffer(magic_cookie, fileData, fileSize);
	printf("[computing] %s\n", magic_full);
	//write to file
	if(strcmp("data",magic_full)!=0){
	    printf("found: %s\nwih key:%d\n",magic_full,key);
	    memset(foundFile, '\0', 50 * sizeof(char));
	    strcpy(foundFile,outPath);
	    strcpy(&foundFile[strlen(outPath)],"f");
	    sprintf(&foundFile[strlen(outPath) + 1], "%d", key);
	    printf("[SOL]%s",foundFile);
    	    
	    fout = fopen(foundFile, "wb");
	    fwrite(fileData, sizeof(char), fileSize, fout);
	    fclose(fout);
	}
        //reset the buffer for the next itration
	memcpy(fileData, fileData_bk, fileSize);
    }
    magic_close(magic_cookie);
    return 0;
}
