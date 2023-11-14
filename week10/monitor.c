#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN 1024 * sizeof(struct inotify_event) + 1024 * 16
#define PATH_MAX 4096

struct stat fileStat;

void print_stat_info(const char *file, const char *watched_dir) {
    char *path = malloc(strlen(watched_dir) + strlen(file) + 2);
    strcpy(path, watched_dir);
    strcat(path, "/");
    strcat(path, file);
    if (stat(path, &fileStat) < 0) {
        printf("--------------\n");
        return;
    }
    printf("File/Directory: %s\n", path);
    printf("Size: %ld bytes, Blocks: %ld, IO Block: %ld\n", fileStat.st_size, fileStat.st_blocks, fileStat.st_blksize);
    printf("Device: %ld, Inode: %ld, Links: %ld\n", fileStat.st_dev, fileStat.st_ino, fileStat.st_nlink);
    printf("Access: %o\n", fileStat.st_mode);
    printf("Access: %s", ctime(&fileStat.st_atime));
    printf("Modify: %s", ctime(&fileStat.st_mtime));
    printf("Change: %s", ctime(&fileStat.st_ctime));
    printf("--------------\n");
}

void handle_sigint(int signum) {
    printf("\nReceived SIGINT. Printing stat info before termination.\n");
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        exit(1);
    }
    signal(SIGINT, handle_sigint);
    char *dir_path = argv[1];
    int fd = inotify_init();
    inotify_add_watch(fd, dir_path,IN_ACCESS | IN_CREATE | IN_DELETE | IN_MODIFY | IN_OPEN);
    char buffer[EVENT_BUF_LEN];
    for (;;) {
        int length = read(fd, buffer, EVENT_BUF_LEN);
        if (length < 0) {
            perror("read");
            exit(1);
        }
        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                if (event->mask & IN_ACCESS) {
                    printf("File accessed: %s\n", event->name);
                }
                if (event->mask & IN_CREATE) {
                    printf("File/directory created: %s\n", event->name);
                }
                if (event->mask & IN_DELETE) {
                    printf("File/directory deleted: %s\n", event->name);
                }
                if (event->mask & IN_MODIFY) {
                    printf("File modified: %s\n", event->name);
                }
                if (event->mask & IN_OPEN) {
                    printf("File/directory opened: %s\n", event->name);
                }
                char path[PATH_MAX];
                snprintf(path, PATH_MAX, "%s/%s", dir_path, event->name);
                print_stat_info(event->name, dir_path);
            }
            i += EVENT_SIZE + event->len;
        }
    }
}
