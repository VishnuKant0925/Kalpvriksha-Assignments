#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BLOCK_SIZE 512
#define NAME_LENGTH 50
#define NUM_BLOCKS 1024

typedef enum{
    FILE_TYPE,
    DIRECTORY_TYPE
} NodeType;

typedef struct FreeBlock{
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct FileNode{
    char name[NAME_LENGTH];
    NodeType type;

    struct FileNode *parent;
    struct FileNode *children;
    struct FileNode *next;
    struct FileNode *prev;

    int *blockPointers;
    int blockCount;
    size_t contentSize;
} FileNode;

unsigned char (*virtualDisk)[BLOCK_SIZE] = NULL;

int totalBlocks = NUM_BLOCKS;
int usedBlocks = 0;

FreeBlock *freeHead = NULL;
FreeBlock *freeTail = NULL;

FileNode *root = NULL;
FileNode *cwd = NULL;

void initializeDisk(void)
{
    virtualDisk = calloc(NUM_BLOCKS, sizeof(*virtualDisk));

    if (virtualDisk == NULL)
    {
        printf("Memory allocation failed for virtual disk.\n");
        exit(EXIT_FAILURE);
    }
}

FreeBlock *createFreeBlock(int index)
{
    FreeBlock *newBlock = malloc(sizeof(FreeBlock));
    if (newBlock == NULL){
        printf("Memory allocation failed for FreeBlock.\n");
        return NULL;
    }
    newBlock->index = index;
    newBlock->next = NULL;
    newBlock->prev = NULL;
    return newBlock;
}

int allocateBlock(void){
    if (freeHead == NULL){
        return -1;
    }

    FreeBlock *temp = freeHead;
    int allocatedIndex = temp->index;

    freeHead = freeHead->next;

    if (freeHead == NULL){
        freeTail = NULL;
    }
    else{
        freeHead->prev = NULL;
    }
    free(temp);
    usedBlocks++;

    return allocatedIndex;
}

FileNode *createFileNode(const char *name, NodeType type)
{

    FileNode *newNode = calloc(1, sizeof(FileNode));

    if (newNode == NULL){
        return NULL;
    }

    strcpy(newNode->name, name);
    newNode->type = type;

    newNode->parent = NULL;
    newNode->children = NULL;
    newNode->next = NULL;
    newNode->prev = NULL;

    newNode->blockPointers = NULL;
    newNode->blockCount = 0;
    newNode->contentSize = 0;

    return newNode;
}

FileNode *findChild(FileNode *directory, const char *name)
{
    if (directory == NULL || directory->children == NULL){
        return NULL;
    }

    FileNode *current = directory->children;

    do{
        if (strcmp(current->name, name) == 0){
            return current;
        }
        current = current->next;
    } while (current != directory->children);

    return NULL;
}

bool insertChild(FileNode *parent, FileNode *child)
{
    if (parent->children == NULL)
    {
        child->next = child;
        child->prev = child;
        parent->children = child;
    }
    else{
        FileNode *head = parent->children;
        FileNode *tail = head->prev;
        tail->next = child;
        child->prev = tail;
        child->next = head;
        head->prev = child;
    }
       child->parent = parent;
       return true;
}

bool removeChild(FileNode *parent, FileNode *child){
    if (child->next == child){
        parent->children = NULL;
    }
    else{
        child->prev->next = child->next;
        child->next->prev = child->prev;

        if (parent->children == child){
            parent->children = child->next;
        }
    }

    child->parent = NULL;
    child->next = NULL;
    child->prev = NULL;
    return true;
}

void listDirectory(FileNode *directory){
    if (directory->children == NULL){
        printf("(empty)\n");
        return;
    }
FileNode *current = directory->children;

    do{
        if (current->type == DIRECTORY_TYPE){
            printf("%s/\n", current->name);
        }
        else{
            printf("%s\n", current->name);
        }

        current = current->next;
    } while (current != directory->children);
}

void printPath(FileNode *directory){
    if (directory == root){
        return;
    }
    printPath(directory->parent);
    printf("/%s", directory->name);
}

void showDiskUsage(void){
    int freeBlocks = totalBlocks - usedBlocks;
    double usage = (double)usedBlocks / totalBlocks * 100.0;
    printf("Total Blocks: %d\n", totalBlocks);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", freeBlocks);
    printf("Disk Usage: %.2f%%\n", usage);
}

void cleanupDirectory(FileNode *directory){
    if (directory == NULL || directory->children == NULL){
        return;
    }

    FileNode *first = directory->children;
    FileNode *last = first->prev;
    last->next = NULL;
    first->prev = NULL;
    directory->children = NULL;
    FileNode *current = first;

    while (current != NULL){
        FileNode *nextNode = current->next;
         if (current->type == DIRECTORY_TYPE){
            cleanupDirectory(current);
        }
        free(current->blockPointers);
        free(current);
        current = nextNode;
    }
}

bool initializeFreeBlockList(void){
    for (int i = 0; i < totalBlocks; i++){
        FreeBlock *newBlock = createFreeBlock(i);
        if (newBlock == NULL){
            FreeBlock *temp = freeHead;

            while (temp != NULL){
                FreeBlock *nextNode = temp->next;
                free(temp);
                temp = nextNode;
            }
            freeHead = NULL;
            freeTail = NULL;
            return false;
        }

        if (freeHead == NULL){
            freeHead = newBlock;
            freeTail = newBlock;
        }
        else{
            freeTail->next = newBlock;
            newBlock->prev = freeTail;
            freeTail = newBlock;
        }
    }

    return true;
}

bool freeBlock(int index)
{
    FreeBlock *newBlock = createFreeBlock(index);

    if (newBlock == NULL){
        return false;
    }
    if (freeHead == NULL){
        freeHead = newBlock;
        freeTail = newBlock;
    }
    else{
        freeTail->next = newBlock;
        newBlock->prev = freeTail;
        freeTail = newBlock;
    }
    usedBlocks--;
    return true;
}

void makeDirectory(const char *name)
{
    if (findChild(cwd, name) != NULL)
    {
        printf("An entry with this name already exists.\n");
        return;
    }

    FileNode *newDirectory = createFileNode(name, DIRECTORY_TYPE);

    if (newDirectory == NULL)
    {
        return;
    }

    insertChild(cwd, newDirectory);
    printf("Directory '%s' created successfully.\n", name);
}

void createFile(const char *name)
{
    if (findChild(cwd, name) != NULL)
    {
        printf("An entry with this name already exists.\n");
        return;
    }

    FileNode *newFile = createFileNode(name, FILE_TYPE);

    if (newFile == NULL)
    {
        return;
    }

    insertChild(cwd, newFile);
    printf("File '%s' created successfully.\n", name);
}

void readFile(const char *name)
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->type != FILE_TYPE)
    {
        printf("File '%s' not found.\n", name);
        return;
    }

    size_t remaining = file->contentSize;

    for (int i = 0; i < file->blockCount; i++)
    {
        size_t bytes = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

        fwrite(virtualDisk[file->blockPointers[i]], 1, bytes, stdout);
        remaining -= bytes;
    }

    printf("\n");
}

void removeDirectory(const char *name)
{
    FileNode *directory = findChild(cwd, name);

    if (directory == NULL || directory->type != DIRECTORY_TYPE)
    {
        printf("Directory '%s' not found.\n", name);
        return;
    }

    if (directory->children != NULL)
    {
        printf("Directory is not empty.\n");
        return;
    }

    removeChild(cwd, directory);
    free(directory);

    printf("Directory removed successfully.\n");
}

void printWorkingDirectory(void)
{
    if (cwd == root)
    {
        printf("/");
    }
    else
    {
        printPath(cwd);
    }
}

void cleanupFileSystem(void)
{
    cleanupDirectory(root);
    free(root);
    root = NULL;
    cwd = NULL;

    FreeBlock *current = freeHead;

    while (current != NULL)
    {
        FreeBlock *nextNode = current->next;
        free(current);
        current = nextNode;
    }

    freeHead = NULL;
    freeTail = NULL;

    free(virtualDisk);
    virtualDisk = NULL;

    printf("Memory released. Exiting program...\n");
}

void writeFile(const char *name, const char *content)
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->type != FILE_TYPE)
    {
        printf("File '%s' not found.\n", name);
        return;
    }

    size_t newSize = strlen(content);
    int requiredBlocks = (int)((newSize + BLOCK_SIZE - 1) / BLOCK_SIZE);

    int *newPointers = NULL;

    if (requiredBlocks > 0)
    {
        newPointers = malloc(requiredBlocks * sizeof(int));

        if (newPointers == NULL)
        {
            printf("Memory allocation failed.\n");
            return;
        }
    }

    int reusableBlocks = file->blockCount;

    if (reusableBlocks > requiredBlocks)
    {
        reusableBlocks = requiredBlocks;
    }

    for (int i = 0; i < reusableBlocks; i++)
    {
        newPointers[i] = file->blockPointers[i];
    }

    int newlyAllocated = 0;

    for (int i = reusableBlocks; i < requiredBlocks; i++)
    {
        int index = allocateBlock();

        if (index == -1)
        {
            for (int j = 0; j < newlyAllocated; j++)
            {
                freeBlock(newPointers[reusableBlocks + j]);
            }

            free(newPointers);
            printf("Not enough free disk space.\n");
            return;
        }

        newPointers[i] = index;
        newlyAllocated++;
    }

    size_t written = 0;

    for (int i = 0; i < requiredBlocks; i++)
    {
        size_t remaining = newSize - written;
        size_t bytes = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

        memset(virtualDisk[newPointers[i]], 0, BLOCK_SIZE);

        if (bytes > 0)
        {
            memcpy(virtualDisk[newPointers[i]], content + written, bytes);
        }

        written += bytes;
    }

    for (int i = requiredBlocks; i < file->blockCount; i++)
    {
        freeBlock(file->blockPointers[i]);
    }

    free(file->blockPointers);

    file->blockPointers = newPointers;
    file->blockCount = requiredBlocks;
    file->contentSize = newSize;

    printf("Data written successfully (size=%zu bytes).\n", newSize);
}

void deleteFile(const char *name)
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->type != FILE_TYPE)
    {
        printf("File '%s' not found.\n", name);
        return;
    }

    for (int i = 0; i < file->blockCount; i++)
    {
        freeBlock(file->blockPointers[i]);
    }

    removeChild(cwd, file);

    free(file->blockPointers);
    free(file);

    printf("File deleted successfully.\n");
}

void changeDirectory(const char *name){
    if (strcmp(name, "..") == 0)
    {
        if (cwd->parent != NULL)
        {
            cwd = cwd->parent;
        }

        printf("Moved to ");
        printWorkingDirectory();
        printf("\n");
        return;
    }

    FileNode *directory = findChild(cwd, name);

    if (directory == NULL || directory->type != DIRECTORY_TYPE)
    {
        printf("Directory '%s' not found.\n", name);
        return;
    }

    cwd = directory;

    printf("Moved to ");
    printWorkingDirectory();
    printf("\n");
}

void processCommand(char *line)
{
    char command[20];
    char name[NAME_LENGTH + 1];

    if (sscanf(line, "%19s", command) != 1)
    {
        return;
    }

    if (strcmp(command, "mkdir") == 0)
    {
        if (sscanf(line, "%*s %50s", name) == 1){
            makeDirectory(name);
        }
        
    }
    else if (strcmp(command, "create") == 0)
    {
        if (sscanf(line, "%*s %50s", name) == 1){
            createFile(name);
        }
    }
    else if (strcmp(command, "write") == 0)
    {
        char *position = line;

        while (*position != '\0' && *position != ' ')
        {
            position++;
        }

        while (*position == ' ')
        {
            position++;
        }

        int i = 0;

        while (*position != '\0' && *position != ' ' && i < NAME_LENGTH)
        {
            name[i++] = *position++;
        }

        name[i] = '\0';

        while (*position == ' ')
        {
            position++;
        }

        if (name[0] == '\0' || *position == '\0'){
            return;
        }

        if (*position == '"' || *position == '\'')
        {
            char quote = *position;
            position++;

            char *endQuote = strrchr(position, quote);

            if (endQuote != NULL)
            {
                *endQuote = '\0';
            }
        }

        writeFile(name, position);
    }
    else if (strcmp(command, "read") == 0)
    {
        if (sscanf(line, "%*s %50s", name) == 1) {
            readFile(name);
        }
        
    }
    else if (strcmp(command, "delete") == 0)
    {
        if (sscanf(line, "%*s %50s", name) == 1){
            deleteFile(name);
        }
        
    }
    else if (strcmp(command, "rmdir") == 0)
    {
        if (sscanf(line, "%*s %50s", name) == 1){
            removeDirectory(name);
        }
        
    }
    else if (strcmp(command, "ls") == 0){
        listDirectory(cwd);
    }
    else if (strcmp(command, "cd") == 0){
        if (sscanf(line, "%*s %50s", name) == 1){
            changeDirectory(name);
        }
    
    }
    else if (strcmp(command, "pwd") == 0){
        printWorkingDirectory();
        printf("\n");
    }
    else if (strcmp(command, "df") == 0)
    {
        showDiskUsage();
    }
    else if (strcmp(command, "exit") != 0)
    {
        printf("Invalid command.\n");
    }
}

int main(void)
{
    initializeDisk();

    if (!initializeFreeBlockList()){
        printf("Failed to initialize free block list.\n");
        free(virtualDisk);
        return 1;
    }

    root = createFileNode("/", DIRECTORY_TYPE);

    if (root == NULL){
        cleanupFileSystem();
        return 1;
    }

    cwd = root;

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char line[4096];

    while (1){
        printWorkingDirectory();
        printf(" > ");

        if (fgets(line, sizeof(line), stdin) == NULL){
            break;
        }
        line[strcspn(line, "\n")] = '\0';
        char command[20];

        if (sscanf(line, "%19s", command) == 1 &&
            strcmp(command, "exit") == 0)
        {
            break;
        }

        processCommand(line);
    }

    cleanupFileSystem();
    return 0;
}
