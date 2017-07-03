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
											 //ls -S Ŀ�ǵ� ������ ���ؼ� ���ϸ�� ����ũ�⸦ �����ϴ� ����ü�� �����Ѵ�.

void list_segment (char *cwd){			     // cwd ��ο� �ִ� file���� �̸��� ����ϱ� ���� �ۼ��� list_segment �Լ�
    DIR *dp;
    struct dirent *dir_buf;
    char name[NAME_MAX+1][NAME_MAX+1];       // limits.h�� ���ǵǾ��ִ� FileName�� �ִ���� + 1 (\0�� ���ؼ�) ��ŭ�� 2���� �迭�� ����
    char temp[NAME_MAX+1];					 // name�� �迭�� ����Ǿ��ִ� ���ϸ��� �ٲٱ� ���� �ӽ� �迭
    int i = 0, j = 0, k = 0;

    memset(name,0,sizeof(name));

    if ((dp = opendir(cwd)) == NULL) {		 // file���� ���� directory�� open. ���н� error ��� ���� process ����
        printf("open directory failure\n");
        exit(-1);
    }

    while ((dir_buf = readdir(dp))){         // directory�� ������� ���� �� name �迭�� ���� �����̸��� �����Ѵ�.
        strcpy(name[i], dir_buf->d_name);
        ++i;
    }

    for(; j < i; ++j){						 // ������ �̸��� ��ҹ��� ���о���(strcasecmp�Լ�) ���ĺ� ������� �����Ѵ�.
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
    } //�ܼ� ls ��ɾ�� hidden file�� ������� ������, hidden file�� '.'���� �����ϹǷ� ���ϸ��� .���� �����ϴ� ��� �ǳʶڴ�.

    if (closedir(dp)) {
        printf("close directory failure\n");
        exit(-1);
    } //DIR*�� �ݴ´�. (���� �߻� �� error�� ��� �� ��������)
}

void list_segment_a (char *cwd) { // ����������  ���� list_segemet �Լ��� ������ �����ϰ� �۵��Ѵ�. ��, �����̸� ��� �� �ణ �ٸ���.
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
     } // ������ list_segment�� hidden file�� ������� �ʱ� ���� �����̸��� ù ������ '.'���� Ȯ���ϴ� �۾��� �������� ls -a�� ���� ���� ����Ѵ�.

    if (closedir(dp)) {
        printf("close directory failure\n");
        exit(-1);
    }
}

void list_segment_S (char *cwd) {
    DIR *dp;
    struct dirent *dir_buf;
    struct stat stat_buf;
    filesize  *file_struct;			// file �̸��� size�� �����ϱ� ���� ������ ����ü
    int cnt_dir_file = 0;			// ls -S�� ��쿡�� hidden file�� ������� �����Ƿ�, hidden file �����Ͽ� �� ���� file�� �ִ��� ���� ���� ����
    int i = 0, j = 0, k = 0;
    off_t temp_size;				// file ũ�� ���� ������ ���� ������ size ����
    char buf[512], temp[NAME_MAX];	// directory�� ��θ� �����ϱ� ���� buf, ���ĺ� ���� ���� ������ ���� temp
    
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

    file_struct = (filesize*)malloc(sizeof(filesize)*cnt_dir_file); // ������ ���� ��ŭ(hidden file ����) filesize�� �����Ҵ�
    
    while(dir_buf = readdir(dp)){
        if(dir_buf->d_name[0] == '.') continue;
        sprintf(buf, "%s/%s", cwd, dir_buf->d_name); //stat�� �ޱ� ���ؼ� pathname�� �ʿ��ϹǷ� buf�� pathname�� ������.
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

            } // ���� ũ�� ���� file_struct ����
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
            }// ���� ũ�Ⱑ ���� ��� ���ĺ� ���� ����.
        }
    }

    for(i = 0; i < cnt_dir_file; ++i){
        printf("%s\n",file_struct[i].name);
    }

    free(file_struct); // �Ҵ��� file_struct�� free.

    if (closedir(dp)) {
        printf("close directory failure.\n");
		exit(-1);
	}// dp�� �����鼭 ���� �߻� �� error�� ��� ���� ����.
}
    

int main (int argc, char *argv[]) {
  char *cur_dir;

  if (argc < 2 || strcmp(argv[1], "ls")) {
	  printf("command error.\n");
	  exit(-1);
	  //input error : �۵� ��� ./a.out ls | ./a.out ls (option) | ./a.out ls (path) | ./a.out ls (option) (path)
  } else if (argc == 2) {								// there is only "./a.out ls" so, print current working directory's files.
      cur_dir = "./";
      list_segment(cur_dir);
      return 0;
  } else if (argc == 3) {							// 3��° ���ڷ� �ɼ��̳� ��ΰ� �������� �ǹ�.
       if(strcmp(argv[2], "-a") == 0) { 
           cur_dir = "./";
           list_segment_a(cur_dir);
       } else if(strcmp(argv[2], "-S") == 0) {
           cur_dir = "./";
           list_segment_S(cur_dir);
       } else if(argv[2][0] == '-') {				//-a�� -S�̿��� command�� ������ �����޽��� ��� �� ���� ����
           printf("unknown command.\n");
           exit(-1);
       } else {
           cur_dir = argv[2];						// 3��° ���ڰ� �ɼ��� �ƴ϶�� ����̹Ƿ�
           list_segment(cur_dir);
       }
  } else if (argc == 4) {							// ���ڰ� 4���� ���� �ɼ�(-a, -S)�� ��ΰ� �ִ� ��츸 ����.
       if(strcmp(argv[2], "-a") == 0) {
           cur_dir = argv[3];
           list_segment_a(cur_dir);
       } else if (strcmp(argv[2], "-S") == 0) {
           cur_dir = argv[3];
           list_segment_S(cur_dir);
       } else {										// �� �� ���� ����� �޾��� �� �����޽��� ��� �� ����
           printf("unknown command.\n");
           exit(-1);
       }
  } else {											// ������ ó���� ��� �̿��� ��Ȳ �߻� �� �����޽��� ��� �� ����
      printf("command error.\n");
      exit(-1);
  } 
  return 0;
}

