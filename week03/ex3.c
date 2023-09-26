#include <stdio.h>
#include <string.h>

// Constants
#define MAX_NAME_LENGTH 63
#define MAX_PATH_SIZE 2048
#define MAX_FILE_SIZE 1024

// Forward declarations
struct File;
struct Directory;

// File structure
struct File {
    unsigned int id;
    char name[MAX_NAME_LENGTH + 1];
    size_t size;
    char data[MAX_FILE_SIZE];
    struct Directory *directory;
};

// Directory structure
struct Directory {
    char name[MAX_NAME_LENGTH + 1];
    struct File *files[256];
    struct Directory *directories[256];
    unsigned char nf;
    unsigned char nd;
    char path[MAX_PATH_SIZE];
};

// Function that overwrites the content of a file
void overwrite_to_file(struct File *file, const char *str) {
    if (file) {
        strcpy(file->data, str);
        file->size = strlen(str) + 1;
    }
}

// Function that appends content to a file
void append_to_file(struct File *file, const char *str) {
    if (file) {
        size_t currentSize = file->size;
        if (currentSize + strlen(str) < MAX_FILE_SIZE) {
            strcat(file->data, str);
            file->size = strlen(file->data) + 1;
        } else {
            printf("File size limit exceeded!\n");
        }
    }
}


// Function that prints the path of a file
void printp_file(struct File *file) {
    if (file) {
        printf("%s/%s\n", file->directory->path, file->name);
    }
}


// Function that adds a file to a directory
void add_file(struct File *file, struct Directory *dir) {
    if (file && dir) {
        if (dir->nf < 256) {
            file->directory = dir;
            dir->files[dir->nf++] = file;
        } else {
            printf("Directory is full!\n");
        }
    }
}

// Prints the name of the file
void show_file(struct File *file) {
    printf("%s ", file->name);
}

// Displays the content of the directory
void show_dir(struct Directory *dir) {
    printf("\nDIRECTORY\n");
    printf("path: %s\n", dir->path);
    printf("files:\n");
    printf(" [ ");
    for (int i = 0; i < dir->nf; i++) {
        show_file(dir->files[i]);
    }
    printf("]\n");
    printf(" directories:\n");
    printf(" { ");
    for (int i = 0; i < dir->nd; i++) {
        show_dir(dir->directories[i]);
    }
    printf("}\n");
}

// Adds the subdirectory dir1 to the directory dir2
void add_dir(struct Directory *dir1, struct Directory *dir2) {
    if (dir1 && dir2) {
        dir2->directories[dir2->nd] = dir1;
        dir2->nd++;
        char tempPath[MAX_PATH_SIZE];
        strcpy(tempPath, dir2->path);
        if (strcmp(dir2->path, "/")) {
            strcat(tempPath, "/");
        }
        strcat(tempPath, dir1->name);
        strcpy(dir1->path, tempPath);
    }
}

// main function
int main() {
    // Create the root directory
    struct Directory root;
    strcpy(root.name, "/");
    root.nf = 0;
    root.nd = 0;
    strcpy(root.path, "");

    // Create the home and bin directories
    struct Directory home;
    strcpy(home.name, "home");
    home.nf = 0;
    home.nd = 0;
    add_dir(&home, &root);

    struct Directory bin;
    strcpy(bin.name, "bin");
    bin.nf = 0;
    bin.nd = 0;
    add_dir(&bin, &root);

    // Create files and add them to directories
    struct File bash;
    bash.id = 1;
    strcpy(bash.name, "bash");
    strcpy(bash.data, "Bourne Again Shell!!");
    bash.size = strlen(bash.data) + 1;
    add_file(&bash, &bin);

    struct File ex3_1;
    ex3_1.id = 2;
    strcpy(ex3_1.name, "ex3_1.c");
    strcpy(ex3_1.data, "int printf(const char * format, ...);");
    ex3_1.size = strlen(ex3_1.data) + 1;
    add_file(&ex3_1, &home);

    struct File ex3_2;
    ex3_2.id = 3;
    strcpy(ex3_2.name, "ex3_2.c");
    strcpy(ex3_2.data, "// This is a comment in C language");
    ex3_2.size = strlen(ex3_2.data) + 1;
    add_file(&ex3_2, &home);

    // Append content to files
    append_to_file(&ex3_1, "int main(){printf(\"Hello World!\");}");

    // Print the paths of all files
    printp_file(&ex3_1);
    printp_file(&ex3_2);
    printp_file(&bash);

    return 0;
}
