#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

struct Directory {
    char* directory;
    char* name;
    int level;
    struct Directory* next;
};

typedef struct Directory Node;


char* getBaseDirectory(char* arg);

char* getBaseName(char* base);

Node* listSub(struct dirent *Dir, DIR *pdir, char* dir, int level);

Node* listSubb(Node* list, char* base, char* name);

void dirEntries(struct dirent *Dir, DIR *pdir, Node* list);


Node* end;


int main(int argc, char* argv[]) {
    struct dirent *Dir;
    DIR *pdir;

    if(argc < 2) {
        printf("No file path.");
        return 1;
    }

    char* base = getBaseDirectory(argv[1]);
    char* name = getBaseName(base);

    
    pdir = opendir(base);

    if(pdir == NULL) {
        printf("Cannot open directory.\n");
        return 1;
    }

    
    /* get inital list of base subfolders */
    Node* list = listSubb(listSub(Dir, pdir, base, 1), base, name);

    Node* head = list;

    closedir(pdir);

    
    /* skip over base folder */
    list = list->next;

    /* traverse through linked list and find more subfolders */
    while(list) {
        pdir = opendir(list->directory);
        
        Node* temp1 = list;

        Node* temp2 = listSub(Dir, pdir, list->directory, list->level);

        if(temp2 != NULL) {
            list = list->next;
            Node* temp3 = list;
            
            list = temp1;
            list->next = temp2;

            list = end;
            list->next = temp3;
            
            list = temp1;
        }

        list = list->next;
    }

    /*list = head;

    while(list) {
        printf("%s\n", list->directory);
        printf("%s\n", list->name);
        printf("%d\n", list->level);
        list = list->next;
    }*/

    list = head;

    dirEntries(Dir, pdir, list);

    return 0;
}

/* gets base directory from argv */
char* getBaseDirectory(char* arg) {
    if(((int) arg[strlen(arg)-1]) != 47) {
        char* tmp = arg;
        arg = malloc(strlen(tmp) + 3);
        strcpy(arg, tmp);
        strcat(arg, "/");
    }

    return arg;
}


/* gets name of folder from base directory */
char* getBaseName(char* base) {
    char* temp = malloc(strlen(base) + 2);
    strcpy(temp, base);
    
    char* named = temp;
    char* res = strtok(temp, "/");

    while(res != NULL) {
        res = strtok(NULL, "/");
        if(res != 0x0) {
            named = res;
        }
    }

    return named;
}

/* makes a linked list of folders */
Node* listSub(struct dirent *Dir, DIR *pdir, char* base, int level) {
    Node* curr, *head;
    char* fdir;

    int flag = TRUE;

    head = NULL;

    while((Dir = readdir(pdir)) != NULL) {
        if(Dir->d_type == 4)  {
            if(strncmp(Dir->d_name, ".", 1) != 0 && strncmp(Dir->d_name, "..", 2)) {
                curr = (Node*) malloc(sizeof(Node) + (strlen(Dir->d_name)*2) + sizeof(int) + 2);
                curr->next = head;

                if(flag == TRUE) {
                    end = (Node*) malloc(sizeof(curr));
                    end = curr;
                    flag = FALSE;
                }

                fdir = malloc(strlen(base) + strlen(Dir->d_name) + 3);
                strcpy(fdir, base);
                strcat(fdir, Dir->d_name);
                strcat(fdir, "/");

                curr->level = level + 1;
                curr->directory = fdir;
                curr->name = Dir->d_name;
                head = curr;
            }
        }
    }

    curr = head;

    return curr;
}

/* appending base to linked list */
Node* listSubb(Node* list, char* base, char* name) {
    Node* head = list;

    list = (Node*) malloc(sizeof(Node) + strlen(base) + strlen(name) + 2);
    list->next = head;
    list->directory = base;
    list->name = name;
    list->level = 1;
    head = list;
}

/* prints contents of base and subfolders */
void dirEntries(struct dirent *Dir, DIR *pdir, Node* list) {
    char* space;

    while(list) {
        pdir = opendir(list->directory);

        int level = list->level;
        space = malloc((strlen("    ") * level) + 2);

        while(level > 1) {
            strcat(space, "    ");
            level--;
        }
        
        printf("%s%s\n", space, list->name);

        while((Dir = readdir(pdir)) != NULL) {
            if(Dir->d_type != 4) {
                printf("    %s[%s]\n", space, Dir->d_name);
            }
        }

        free(space);

        closedir(pdir);
        list = list->next;
    }
}