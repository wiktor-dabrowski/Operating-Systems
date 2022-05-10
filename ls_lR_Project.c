#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <assert.h>
#include <time.h>

#define B_TO_KB 1024

struct file_data{
    char* name;
    char type;
    char mode[10];
    int links_nr;
    char* user_name;
    char* group_name;
    int size;
    int allocated;
    char* month;
    int day;
    int hour;
    int minute;
    char minute_str[3];
    char hour_str[3];
};

void convert_mode_to_str(int mode, char* txt_mode){
    for(int i=0; i<3; i++){
        int hex = mode%8;

        if(hex%2 == 1)
            txt_mode[8-i*3] = 'x';
        else
            txt_mode[8-i*3] = '-';
        hex = hex/2;

        if(hex%2 == 1)
            txt_mode[7-i*3] = 'w';
        else
            txt_mode[7-i*3] = '-';
        hex = hex/2;

        if(hex%2 == 1)
            txt_mode[6-i*3] = 'r';
        else
            txt_mode[6-i*3] = '-';
        hex = hex/2;

        mode = mode/8;
    }
}

void is_folder_open(char* folder_name){
    DIR *pDIR;
    pDIR=opendir(folder_name);

    if(pDIR==NULL){
        fprintf(stderr, "%s %d:opendir() failed(%s)\n",
        __FILE__, __LINE__, strerror(errno));
        closedir(pDIR);
        exit(-1);
    }

    closedir(pDIR);
}

static int compare_str (const void *p, const void *q)
{
    const struct file_data *l= (struct file_data*)p;
    const struct file_data *r= (struct file_data*)q;
    int cmp;

    cmp= strcmp (l->name, r->name);
    return cmp;
}

void read_struct(struct file_data files[], char* folder_name, int* number_of_files){
    DIR *pDIR;
    pDIR=opendir(folder_name);

    char* month[12] = {"sty", "lut", "mar", "kwi", "maj", "cze", "lip", "sie", "wrz", "paz", "lis", "gru"};

    struct dirent *pDirEnt;
    pDirEnt=readdir(pDIR);
    int file_nr=0;
    while(pDirEnt!=NULL){
        struct file_data file = files[file_nr];
        struct stat st;
        char* name = pDirEnt->d_name;
        file.name=name;
        if(name[0] != '.'){
            char file_full_path[1024];
            snprintf(file_full_path, sizeof(file_full_path), "%s/%s", folder_name, name);
            /*strcat(file_full_path, folder_name);
            strcat(file_full_path, "/");
            strcat(file_full_path, name);*/
            stat(file_full_path, &st);
            file.size = st.st_size;
            int mode = st.st_mode;
            file.links_nr = st.st_nlink;

            int user_id = st.st_uid;
            int group_id = st.st_gid;
            struct passwd *pws;
            pws = getpwuid(user_id);
            struct group *grp;
            grp = getgrgid(group_id);
            file.user_name = pws->pw_name;
            file.group_name = grp->gr_name;
            int to_get_out = umask(mode);
            int good_mask = umask(mode);
            file.allocated = st.st_blocks * st.st_blksize / B_TO_KB / 8;

            convert_mode_to_str(good_mask, file.mode);

            struct tm *tm = localtime(&st.st_mtime);
            file.month=month[tm->tm_mon];
            file.day = tm->tm_mday;
            file.hour=tm->tm_hour;
            file.minute=tm->tm_min;
            file.hour_str[1]=(file.hour%10) + '0';
            file.hour_str[0]=((file.hour/10)%10) + '0';
            file.minute_str[1]=(file.minute%10) + '0';
            file.minute_str[0]=((file.minute/10)%10) + '0';
            
            if(S_ISREG(mode)){
                 file.type='-';
            }
            else if(S_ISDIR(mode)){
                file.type='d';
            }

            *number_of_files=*number_of_files+1;
            files[file_nr]=file;
            file_nr = file_nr+1;
        }
        
        pDirEnt=readdir(pDIR);
    }
    closedir(pDIR);
}

void print_file(struct file_data file){
    printf("%c%s %i %s %s %i %s %i %s:%s %s\n",
            file.type, file.mode, file.links_nr, file.user_name, file.group_name, file.size,
            file.month, file.day, file.hour_str, file.minute_str,
            file.name);
}

void rec_search(char* folder_name){
    printf("%s:\n", folder_name);
    is_folder_open(folder_name);
    struct file_data files[250];
    int number_of_files = 0;
    read_struct(files, folder_name, &number_of_files);
    int total_size=0;
    for(int i=0;i<number_of_files; i++){
        total_size=total_size+files[i].allocated;
    }
    if (number_of_files>1){
        qsort (files, number_of_files, sizeof files[0], compare_str);
    }
    printf("Razem %i\n", total_size);
    for(int i=0; i<number_of_files; i++){
        print_file(files[i]);
    }
    for(int i=0; i<number_of_files; i++){
        if(files[i].type=='d'){
            char new_path[256]="";
            strcat(new_path, folder_name);
            strcat(new_path, "/");
            strcat(new_path, files[i].name);
            printf("\n");
            rec_search(new_path);
        }
    }

}

int main(int argc, char *acgv[]){
    rec_search(".");

    return 0;
}