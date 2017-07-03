#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>


typedef struct _filesize{
    char *name;
    off_t size;
} filesize;
											 //ls -S 커맨드 구현을 위해서 파일명과 파일크기를 저장하는 구조체를 정의한다.

void list_segment (char *cwd){			     // cwd 경로에 있는 file들의 이름을 출력하기 위해 작성한 list_segment 함수
    DIR *dp;
    struct dirent *dir_buf;
    char name[NAME_MAX+1][NAME_MAX+1];       // limits.h에 정의되어있는 FileName의 최대길이 + 1 (\0을 위해서) 만큼의 2차원 배열을 생성
    char temp[NAME_MAX+1];					 // name의 배열에 저장되어있는 파일명을 바꾸기 위한 임시 배열
    int i = 0, j = 0, k = 0;

    memset(name,0,sizeof(name));

    if ((dp = opendir(cwd)) == NULL) {		 // file들을 읽을 directory를 open. 실패시 error 출력 이후 process 종료
        printf("open directory failure\n");
        exit(-1);
    }

    while ((dir_buf = readdir(dp))){         // directory를 순서대로 읽은 후 name 배열에 각각 파일이름을 저장한다.
        strcpy(name[i], dir_buf->d_name);
        ++i;
    }

    for(; j < i; ++j){						 // 파일의 이름을 대소문자 구분없이(strcasecmp함수) 알파벳 순서대로 정렬한다.
        for(k = 0; k < i - 1; ++k){
            if(strcasecmp(name[k], name[k+1]) > 0){
                strcpy(temp, name[k]);
                strcpy(name[k], name[k+1]);
                strcpy(name[k+1], temp);
                memset(temp, 0, sizeof(temp));
             }
         }
     }

    for(j = 0; j < i; ++j) {
        if(name[j][0] == '.') continue;       
        printf("%-10s \n", name[j]);
    } //단순 ls 명령어는 hidden file을 출력하지 않으며, hidden file은 '.'으로 시작하므로 파일명이 .으로 시작하는 경우 건너뛴다.

    if (closedir(dp)) {
        printf("close directory failure\n");
        exit(-1);
    } //DIR*을 닫는다. (문제 발생 시 error문 출력 후 강제종료)
}

void list_segment_a (char *cwd) { // 전반적으로  위의 list_segemet 함수와 완전히 동일하게 작동한다. 단, 파일이름 출력 시 약간 다르다.
    DIR *dp;
    struct dirent *dir_buf;
    char name[NAME_MAX+1][NAME_MAX+1];
    char temp[NAME_MAX+1];
    int i = 0, j = 0, k = 0;

    if ((dp = opendir(cwd)) == NULL) {
        printf("open directory failure\n");
        exit(-1);
    }

     while ((dir_buf = readdir(dp))){
         strcpy(name[i], dir_buf->d_name);
         ++i;
     }

     for(; j < i; ++j){
         for(k = 0; k < i - 1; ++k){
             if(strcasecmp(name[k], name[k+1]) > 0){
                 strcpy(temp, name[k]);
                 strcpy(name[k], name[k+1]);
                 strcpy(name[k+1], temp);
                 memset(temp, 0, sizeof(temp));
              }
          }
     }

     for(j = 0; j < i; ++j) {    
         printf("%-10s \n", name[j]);
     } // 기존의 list_segment는 hidden file을 출력하지 않기 위해 파일이름의 첫 시작이 '.'인지 확인하는 작업을 거쳤지만 ls -a의 경우는 전부 출력한다.

    if (closedir(dp)) {
        printf("close directory failure\n");
        exit(-1);
    }
}

void list_segment_S (char *cwd) {
    DIR *dp;
    struct dirent *dir_buf;
    struct stat stat_buf;
    filesize  *file_struct;			// file 이름과 size를 저장하기 위한 임의의 구조체
    int cnt_dir_file = 0;			// ls -S의 경우에도 hidden file을 출력하지 않으므로, hidden file 제외하여 몇 개의 file이 있는지 세기 위한 변수
    int i = 0, j = 0, k = 0;
    off_t temp_size;				// file 크기 별로 정렬을 위한 임의의 size 변수
    char buf[512], temp[NAME_MAX];	// directory의 경로를 저장하기 위한 buf, 알파벳 별로 파일 정렬을 위한 temp
    
    memset(temp,0,sizeof(temp));

    if((dp = opendir(cwd)) == NULL) {
        printf("open directory failure\n");
        exit(-1);
    }

    while(dir_buf = readdir(dp)){
       if(dir_buf->d_name[0] == '.') continue;
       ++cnt_dir_file;
    }

    rewinddir(dp);

    file_struct = (filesize*)malloc(sizeof(filesize)*cnt_dir_file); // 파일의 갯수 만큼(hidden file 제외) filesize를 동적할당
    
    while(dir_buf = readdir(dp)){
        if(dir_buf->d_name[0] == '.') continue;
        sprintf(buf, "%s/%s", cwd, dir_buf->d_name); //stat을 받기 위해선 pathname이 필요하므로 buf에 pathname을 저장함.
        stat(buf, &stat_buf); 
        file_struct[i].name = dir_buf->d_name;
        file_struct[i].size = stat_buf.st_size;
        ++i;
    }
    
    for(i = 0; i < cnt_dir_file; ++i) {
        for(j = 0; j < cnt_dir_file - 1 - i; ++j) {
           if(file_struct[j].size < file_struct[j+1].size) {
            strcpy(temp, file_struct[j].name);
            strcpy(file_struct[j].name, file_struct[j+1].name);
            strcpy(file_struct[j+1].name, temp);

            temp_size = file_struct[j].size;
            file_struct[j].size = file_struct[j+1].size;
            file_struct[j+1].size = temp_size;

            } // 파일 크기 별로 file_struct 정렬
        }    
    }

    for(i = 0; i< cnt_dir_file; ++i){
        for(j = 0; j < cnt_dir_file - 1; ++j) {
            if(file_struct[j].size != file_struct[j+1].size) continue;
            if(strcasecmp(file_struct[j].name, file_struct[j+1].name) > 0) {
                strcpy(temp, file_struct[j].name);
                strcpy(file_struct[j].name, file_struct[j+1].name);
                strcpy(file_struct[j+1].name, temp);
                memset(temp, 0, sizeof(temp));    
            }// 파일 크기가 같은 경우 알파벳 별로 정렬.
        }
    }

    for(i = 0; i < cnt_dir_file; ++i){
        printf("%s\n",file_struct[i].name);
    }

    free(file_struct); // 할당한 file_struct를 free.

    if (closedir(dp)) {
        printf("close directory failure.\n");
		exit(-1);
	}// dp를 닫으면서 에러 발생 시 error문 출력 이후 종료.
}
    

int main (int argc, char *argv[]) {
  char *cur_dir;

  if (argc < 2 || strcmp(argv[1], "ls")) {
	  printf("command error.\n");
	  exit(-1);
	  //input error : 작동 방식 ./a.out ls | ./a.out ls (option) | ./a.out ls (path) | ./a.out ls (option) (path)
  } else if (argc == 2) {								// there is only "./a.out ls" so, print current working directory's files.
      cur_dir = "./";
      list_segment(cur_dir);
      return 0;
  } else if (argc == 3) {							// 3번째 인자로 옵션이나 경로가 존재함을 의미.
       if(strcmp(argv[2], "-a") == 0) { 
           cur_dir = "./";
           list_segment_a(cur_dir);
       } else if(strcmp(argv[2], "-S") == 0) {
           cur_dir = "./";
           list_segment_S(cur_dir);
       } else if(argv[2][0] == '-') {				//-a와 -S이외의 command가 들어오면 에러메시지 출력 후 강제 종료
           printf("unknown command.\n");
           exit(-1);
       } else {
           cur_dir = argv[2];						// 3번째 인자가 옵션이 아니라면 경로이므로
           list_segment(cur_dir);
       }
  } else if (argc == 4) {							// 인자가 4개인 경우는 옵션(-a, -S)에 경로가 있는 경우만 존재.
       if(strcmp(argv[2], "-a") == 0) {
           cur_dir = argv[3];
           list_segment_a(cur_dir);
       } else if (strcmp(argv[2], "-S") == 0) {
           cur_dir = argv[3];
           list_segment_S(cur_dir);
       } else {										// 알 수 없는 명령을 받았을 시 에러메시지 출력 후 종료
           printf("unknown command.\n");
           exit(-1);
       }
  } else {											// 위에서 처리한 경우 이외의 상황 발생 시 에러메시지 출력 후 종료
      printf("command error.\n");
      exit(-1);
  } 
  return 0;
}

