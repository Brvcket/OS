#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

struct stat fileStat;

void find_all_hlinks(const char *source, const char *watched_dir) {
    char *path = malloc(strlen(watched_dir) + strlen(source) + 2);
    strcpy(path, watched_dir);
    strcat(path, "/");
    strcat(path, source);
    if (stat(path, &fileStat) < 0) {
        printf("Path: %s\n", path);
        perror("fileStat");
        exit(1);
    }
    printf("hard links to %s (inode %lu):\n", source, (unsigned long)fileStat.st_ino);
    DIR *dir = opendir(watched_dir);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", watched_dir, entry->d_name);
            struct stat entryStat;
            lstat(path, &entryStat);
            if (entryStat.st_ino == fileStat.st_ino) {
                char resolved_path[PATH_MAX];
                realpath(path, resolved_path);
                printf("inode: %lu, Path: %s\n", (unsigned long)entryStat.st_ino, resolved_path);
            }
        }
    }
    closedir(dir);
}

void unlink_all(const char *source, const char *watched_dir) {
    struct stat fileStat;
    char *path = malloc(strlen(watched_dir) + strlen(source) + 2);
    strcpy(path, watched_dir);
    strcat(path, "/");
    strcat(path, source);
    stat(path, &fileStat);
    DIR *dir = opendir(watched_dir);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", watched_dir, entry->d_name);
            if (strcmp(entry->d_name, source) == 0) continue;
            struct stat entryStat;
            lstat(path, &entryStat);
            if (entryStat.st_ino == fileStat.st_ino && strcmp(path, source) != 0) {
                if (unlink(path) < 0) {
                    perror("Error unlinking file");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    closedir(dir);
}

void create_sym_link(const char *source, const char *link, const char *watched_dir) {
    symlink(source, link);
}

void print_stat_info(const char *file, const char *watched_dir) {
    char *path = malloc(strlen(watched_dir) + strlen(file) + 2);
    strcpy(path, watched_dir);
    strcat(path, "/");
    strcat(path, file);
    if (stat(path, &fileStat) < 0) {
        printf("-----------------------\n");
        return;
    }
    printf("File/Directory: %s\n", path);
    printf("Size: %ld bytes, Blocks: %ld, IO Block: %ld\n", fileStat.st_size, fileStat.st_blocks, fileStat.st_blksize);
    printf("Device: %ld, Inode: %ld, Links: %ld\n", fileStat.st_dev, fileStat.st_ino, fileStat.st_nlink);
    printf("Access: %o\n", fileStat.st_mode);
    printf("Access: %s", ctime(&fileStat.st_atime));
    printf("Modify: %s", ctime(&fileStat.st_mtime));
    printf("Change: %s", ctime(&fileStat.st_ctime));
    printf("---------------------\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <watched_directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *watched_dir = argv[1];
    char *file1 = "myfile1.txt";
    char *path1 = malloc(strlen(watched_dir) + strlen(file1) + 2);
    strcpy(path1, watched_dir);
    strcat(path1, "/");
    strcat(path1, file1);

    char *file12 = "myfile12.txt";
    char *path12 = malloc(strlen(watched_dir) + strlen(file12) + 2);
    strcpy(path12, watched_dir);
    strcat(path12, "/");
    strcat(path12, file12);

    char *file11 = "myfile11.txt";
    char *path11 = malloc(strlen(watched_dir) + strlen(file11) + 2);
    strcpy(path11, watched_dir);
    strcat(path11, "/");
    strcat(path11, file11);

    char *file13 = "myfile13.txt";
    char *path13 = malloc(strlen(watched_dir) + strlen(file13) + 2);
    strcpy(path13, watched_dir);
    strcat(path13, "/");
    strcat(path13, file13);

    int file_fd = open(path1, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    write(file_fd, "Hello world.", 12);
    close(file_fd);

    link(path1, path11);
    link(path1, path12);
    find_all_hlinks("myfile1.txt", watched_dir);
    rename(path1, "/tmp/myfile1.txt");
    int file11_fd = open(path11, O_WRONLY | O_APPEND);
    write(file11_fd, " Appended content.", 18);
    close(file11_fd);
    create_sym_link("/tmp/myfile1.txt", path13, watched_dir);
    int tmp_file_fd = open("/tmp/myfile1.txt", O_WRONLY | O_APPEND);
    write(tmp_file_fd, " Additional content.", 21);
    close(tmp_file_fd);
    unlink_all("myfile11.txt", watched_dir);
    find_all_hlinks("myfile11.txt", watched_dir);
    print_stat_info("myfile11.txt", watched_dir);
    return 0;
}
