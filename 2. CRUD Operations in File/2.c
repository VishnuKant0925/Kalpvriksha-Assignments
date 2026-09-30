#include <stdio.h>
#include <stdbool.h>
#include <string.h>

struct User
{
    int id;
    char name[100];
    int age;
};

void createFiles()
{
    FILE *fp;
    fp = fopen("user.txt", "a");
    if (fp != NULL)
        fclose(fp);
    fp = fopen("id.txt", "a");
    if (fp != NULL)
        fclose(fp);
}

int getNextId()
{
    FILE *fp;
    int id = 1;
    fp = fopen("id.txt", "r");

    if (fp != NULL)
    {
        if (fscanf(fp, "%d", &id) != 1) id = 1;
            
        fclose(fp);
    }

    fp = fopen("id.txt", "w");

    if (fp != NULL)
    {
        fprintf(fp, "%d", id + 1);
        fclose(fp);
    }

    return id;
}

void addUser()
{
    FILE *fp;
    struct User user;
    fp = fopen("user.txt", "a");
    if (fp == NULL)
    {
        printf("Error opening user.txt\n");
        return;
    }

    user.id = getNextId();

    printf("Enter name: ");
    fgets(user.name, sizeof(user.name), stdin);
    user.name[strcspn(user.name, "\n")] = '\0';

    printf("Enter age: ");
    scanf("%d", &user.age);
    getchar();

    fprintf(fp, "%d %s %d\n",
            user.id, user.name, user.age);

    fclose(fp);

    printf("User added successfully with ID: %d\n", user.id);
}

void displayUsers()
{
    FILE *fp;
    char line[300];
    fp = fopen("user.txt", "r");
    if (fp == NULL)
    {
        printf("0 users found\n");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        printf("%s", line);
    }

    printf("\n");
    fclose(fp);
}

void updateUser()
{
    FILE *fp;
    FILE *temp;
    struct User user;
    char line[1000];

    int searchId;
    bool found = false;
    fp = fopen("user.txt", "r");
    if (fp == NULL)
    {
        printf("0 users found\n");
        return;
    }

    temp = fopen("temp.txt", "w");

    if (temp == NULL)
    {
        printf("Error creating temporary file\n");
        fclose(fp);
        return;
    }

    printf("Enter user's ID to update: ");
    scanf("%d", &searchId);
    getchar();

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        sscanf(line, "%d %99[^0-9] %d",
               &user.id,
               user.name,
               &user.age);

        if (user.id == searchId)
        {
            found = true;
            int choice;
            printf("\n1. Update name\n");
            printf("2. Update age\n");
            printf("3. Update both\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);
            getchar();

            if (choice == 1)
            {
                printf("Enter new name: ");

                fgets(user.name, sizeof(user.name), stdin);
                user.name[strcspn(user.name, "\n")] = '\0';
            }
            else if (choice == 2)
            {
                printf("Enter new age: ");
                scanf("%d", &user.age);
                getchar();
            }
            else if (choice == 3)
            {
                printf("Enter new name: ");
                fgets(user.name, sizeof(user.name), stdin);
                user.name[strcspn(user.name, "\n")] = '\0';

                printf("Enter new age: ");
                scanf("%d", &user.age);
                getchar();
            }
            else
            {
                printf("Invalid choice. User was not updated.\n");
            }
        }

        fprintf(temp, "%d %s %d\n",
                user.id,
                user.name,
                user.age);
    }

    fclose(fp);
    fclose(temp);

    remove("user.txt");
    rename("temp.txt", "user.txt");

    if (found)
        printf("User updated successfully\n");
    else
        printf("User with ID %d not found\n", searchId);
}

void deleteUser()
{
    FILE *fp;
    FILE *temp;

    struct User user;

    char line[300];

    int searchId;
    bool found = false;

    fp = fopen("user.txt", "r");

    if (fp == NULL)
    {
        printf("0 users found\n");
        return;
    }
    temp = fopen("temp.txt", "w");

    if (temp == NULL)
    {
        printf("Error creating temporary file\n");
        fclose(fp);
        return;
    }

    printf("Enter user's ID to delete: ");
    scanf("%d", &searchId);
    getchar();

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        sscanf(line, "%d %99[^0-9] %d",
               &user.id,
               user.name,
               &user.age);

        if (user.id == searchId)
        {
            found = true;
            continue;
        }

        fprintf(temp, "%d %s %d\n",
                user.id,
                user.name,
                user.age);
    }

    fclose(fp);
    fclose(temp);
    remove("user.txt");
    rename("temp.txt", "user.txt");

    if (found)
        printf("User deleted successfully\n");
    else
        printf("User with ID %d not found\n", searchId);
}

void printMenu()
{
    printf("1. Add User\n");
    printf("2. Display Users\n");
    printf("3. Update User\n");
    printf("4. Delete User\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

int main()
{
    int choice;

    createFiles();

    while (true)
    {
        printMenu();

        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
            case 1:
                addUser();
                break;
            case 2:
                displayUsers();
                break;
            case 3:
                updateUser();
                break;
            case 4:
                deleteUser();
                break;
            case 5:
                printf("Program ended.\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
