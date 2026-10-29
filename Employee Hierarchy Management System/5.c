#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Employee {
    int id;
    char name[100];
    char designation[100];
    int managerIndex;
    int *reports;
    int reportCount;
    int reportCapacity;
    int active;
};

struct Employee *employees = NULL;
int employeeCount = 0;
int employeeSlots = 0;
int capacity = 0;
int ceoIndex = -1;

int readInt(char msg[]){
    char line[100];
    int n;
    char ch;

    while (1)
    {
        printf("%s", msg);

        if (fgets(line, sizeof(line), stdin) == NULL)
            exit(0);

        if (sscanf(line, " %d %c", &n, &ch) == 1)
            return n;

        printf("Enter a valid number.\n");
    }
}

void readString(char msg[], char str[], int size){
    printf("%s", msg);
    fgets(str, size, stdin);
    str[strcspn(str, "\n")] = '\0';
}

int findEmployee(int id){
    for (int i = 0; i < employeeSlots; i++){
        if (employees[i].active && employees[i].id == id) return i;
    }
    return -1;
}

int reserveSpace(){
    if (employeeSlots < capacity)  return 1;

    int newCapacity = capacity == 0 ? 4 : capacity * 2;

    struct Employee *p =  realloc(employees, newCapacity * sizeof(struct Employee));

    if (p == NULL){
        return 0;
    }

    employees = p;
    capacity = newCapacity;
    return 1;
}

int reserveReports(int manager, int extra){
    int needed = employees[manager].reportCount + extra;

    if (needed <= employees[manager].reportCapacity) return 1;

    int newCapacity = employees[manager].reportCapacity == 0 ? 2 : employees[manager].reportCapacity;

    while (newCapacity < needed){
        newCapacity *= 2;
    }
    int *p = realloc(employees[manager].reports,newCapacity * sizeof(int));

    if (p == NULL){
        return 0;
    }

    employees[manager].reports = p;
    employees[manager].reportCapacity = newCapacity;
    return 1;
}

void appendReport(int manager, int report){
    employees[manager].reports[employees[manager].reportCount] = report;

    employees[manager].reportCount++;
}

void removeReport(int manager, int report){
    for (int i = 0; i < employees[manager].reportCount; i++){
        if (employees[manager].reports[i] == report){
            for (int j = i; j < employees[manager].reportCount - 1; j++){
                employees[manager].reports[j] = employees[manager].reports[j + 1];
                }
            employees[manager].reportCount--;
            return;
        }
    }
}

void addEmployee(){
    int id = readInt("Enter Employee ID: ");
    if (id <= 0 || findEmployee(id) != -1){
        printf("Invalid or duplicate ID.\n");
        return;
    }

    char name[100], designation[100];

    readString("Enter Name: ", name, 100);
    readString("Enter Designation: ", designation, 100);

    if (name[0] == '\0' || designation[0] == '\0'){
        printf("Name and designation cannot be empty.\n");
        return;
    }

    int manager = -1;

    if (employeeCount > 0){
        int mid = readInt("Enter Reporting Manager ID: ");
        manager = findEmployee(mid);

        if (manager == -1){
            printf("Manager not found.\n");
            return;
        }
    }
    else{
        printf("This employee will be the CEO.\n");
    }

    if (!reserveSpace())  return;

    if (manager != -1 && !reserveReports(manager, 1))  return;

    int index = employeeSlots;

    employees[index].id = id;
    strcpy(employees[index].name, name);
    strcpy(employees[index].designation, designation);
    employees[index].managerIndex = manager;
    employees[index].reports = NULL;
    employees[index].reportCount = 0;
    employees[index].reportCapacity = 0;
    employees[index].active = 1;

    employeeSlots++;
    employeeCount++;

    if (manager == -1)
        ceoIndex = index;
    else
        appendReport(manager, index);

    printf("Employee added.\n");
}

int countTeam(int index)
{
    if (index < 0 || !employees[index].active)  return 0;

    int count = 1;

    for (int i = 0; i < employees[index].reportCount; i++){
        int report = employees[index].reports[i];
        count += countTeam(report);
    }

    return count;
}

void displayTeam(int index, int level){
    if (index < 0 || !employees[index].active)  return;
    
    for (int i = 0; i < level; i++)
        printf("    ");

    printf("%d - %s - %s\n",
           employees[index].id,
           employees[index].name,
           employees[index].designation);

    for (int i = 0; i < employees[index].reportCount; i++){
        int report = employees[index].reports[i];
        displayTeam(report, level + 1);
    }
}

int countDirectReports(int index){
    return employees[index].reportCount;
}

void removeEmployee(){
    if (ceoIndex == -1){
        printf("Organization is empty.\n");
        return;
    }

    int id = readInt("Enter Employee ID to remove: ");
    int index = findEmployee(id);

    if (index == -1){
        printf("Employee not found.\n");
        return;
    }

    if (index == ceoIndex){
        printf("CEO cannot be removed.\n");
        return;
    }

    int manager = employees[index].managerIndex;
    int target = -1;

    for (int i = 0; i < employees[manager].reportCount; i++){
        int sibling = employees[manager].reports[i];

        if (sibling == index){
            continue;
        }
        if (target == -1 ||countDirectReports(sibling) < countDirectReports(target)) {
            target = sibling;
        }
    }

    if (target == -1)
        target = manager;

    int n = employees[index].reportCount;
    int *saved = NULL;

    if (n > 0){
        saved = malloc(n * sizeof(int));

        if (saved == NULL){
            printf("Memory allocation failed.\n");
            return;
        }

        for (int i = 0; i < n; i++){
            saved[i] = employees[index].reports[i];
        }
            
    }

    if (!reserveReports(target, n)) {
        free(saved);
        return;
    }

    removeReport(manager, index);

    for (int i = 0; i < n; i++){
        int report = saved[i];
        employees[report].managerIndex = target;
        appendReport(target, report);
    }

    free(saved);
    free(employees[index].reports);

    employees[index].reports = NULL;
    employees[index].reportCount = 0;
    employees[index].reportCapacity = 0;
    employees[index].active = 0;

    employeeCount--;

    printf("Employee removed.\n");
}

void displayEmployeeHierarchy(){
    if (ceoIndex == -1){
        printf("Organization is empty.\n");
        return;
    }

    int id = readInt("Enter Employee ID: ");
    int index = findEmployee(id);

    if (index == -1){
        printf("Employee not found.\n");
        return;
    }

    displayTeam(index, 0);

    printf("Total Employees Under %d: %d\n",id, countTeam(index) - 1);
}

void displayCompleteHierarchy(){
    if (ceoIndex == -1){
        printf("Organization is empty.\n");
        return;
    }
    printf("\n===== ORGANIZATION HIERARCHY =====\n");
    displayTeam(ceoIndex, 0);

    printf("Total Employees: %d\n", countTeam(ceoIndex));
}

void freeMemory()
{
    for (int i = 0; i < employeeSlots; i++){
        free(employees[i].reports);
    }
    free(employees);

    employees = NULL;
    employeeCount = 0;
    employeeSlots = 0;
    capacity = 0;
    ceoIndex = -1;
}

int main()
{
    while (1)
    {
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");

        int choice = readInt("Enter your choice: ");

        switch (choice)
        {
        case 1:
            addEmployee();
            break;
        case 2:
            removeEmployee();
            break;
        case 3:
            displayEmployeeHierarchy();
            break;
        case 4:
            displayCompleteHierarchy();
            break;
        case 5:
            freeMemory();
            printf("Program terminated.\n");
            return 0;
        default:
            printf("Invalid choice.\n");
        }
    }
}