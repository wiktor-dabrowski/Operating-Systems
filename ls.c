#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>

#define TIME_SIZE 80
#define BUF_SIZE 1024

void mode2str(mode_t mode, char *buf) {
  const char chars[] = "rwxrwxrwx";
  for (size_t i = 0; i < 9; i++)
    buf[i] = (mode & (1 << (8 - i))) ? chars[i] : '-';

  buf[9] = '\0';
}

void print_dir(const char *dir_name) {
  struct dirent *dir1;
  struct stat stat_;
  struct tm lt;
  struct passwd *pwd;

  DIR *dir2 = opendir(dir_name);

  if (dir2 == NULL) {
    printf("Directory: %s : %s \n", dir_name, strerror(errno));
    return;
  }

  printf("Directory : %s\n\n", dir_name);

  while ((dir1 = readdir(dir2))) {
    char buf[BUF_SIZE];
    char time[TIME_SIZE];
    char perm[10];
    DIR *dir3;
    snprintf(buf, sizeof(buf), "%s/%s", dir_name, dir1->d_name);

    if (stat(buf, &stat_) == 0)
      pwd = getpwuid(stat_.st_uid);
    else {
      printf("%s: %s\n", buf, strerror(errno));
      continue;
    }

    time_t t = stat_.st_mtime;
    localtime_r(&t, &lt);
    strftime(time, sizeof(time), "%c", &lt);
    mode2str(stat_.st_mode, perm);

    if (!(strcmp(dir1->d_name, ".") == 0 || strcmp(dir1->d_name, "..") == 0)) {
      if (pwd != 0)

        printf("%s \t %s \t %ld \t %s \t %s \n", perm, pwd->pw_name,
               (long)stat_.st_size, time, dir1->d_name);

      else
        printf("%s \t %d \t %ld \t %s \t %s\n\n", perm, stat_.st_uid,
               (long)stat_.st_size, time, dir1->d_name);

      dir3 = opendir(buf);
      if (dir3 != NULL) {
        closedir(dir3);
        print_dir(buf);
      }
    }
  }
  closedir(dir2);
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    print_dir(".");
  } else {
    for (int i = 1; i < argc; i += 1) print_dir(argv[i]);
  }
  return 1;
}
