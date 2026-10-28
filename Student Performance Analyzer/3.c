#include <stdio.h>

struct Student
{
    int roll;
    char name[100];
    int marks1;
    int marks2;
    int marks3;
};

int Total(int marks1, int marks2, int marks3)
{
    int total;                      
    total = marks1 + marks2 + marks3;
    return total;
}

float Average(int total)
{
    return total / 3.0f;
}

char Grade(float average)
{
    if (average >= 85)
        return 'A';
    else if (average >= 70)
        return 'B';
    else if (average >= 50)
        return 'C';
    else if (average >= 35)
        return 'D';
    else
        return 'F';
}

void printStars(char grade)
{
    int count = 0;

    switch (grade)
    {
        case 'A': count = 5; break;
        case 'B': count = 4; break;
        case 'C': count = 3; break;
        case 'D': count = 2; break;
    }

    for (int i = 0; i < count; i++)
        printf("*");

    printf("\n");
}

void printRollNumbers(struct Student students[], int i, int n)
{
    if (i == n)
        return;

    if (i > 0)
        printf(" ");

    printf("%d", students[i].roll);

    printRollNumbers(students, i + 1, n);
}

int main()
{
    struct Student students[100];
    int n;
    printf("Enter the number of students (1-100): ");                         
    scanf("%d", &n);
    if (n < 1 || n > 100){
        printf("Invalid number of students\n");
        return 1;
      }
    for (int i = 0; i < n; i++){
        printf("Enter roll number, name, marks1, marks2, marks3 for student %d: ", i + 1);
        scanf("%d %s %d %d %d",
              &students[i].roll,
              students[i].name,
              &students[i].marks1,
              &students[i].marks2,
              &students[i].marks3);
        if (students[i].marks1 < 1 || students[i].marks1 > 100){
            printf("Error! Marks must be between 1 and 100.\n");
            return 1;
        }
        if (students[i].marks2 < 1 || students[i].marks2 > 100){
            printf("Error! Marks must be between 1 and 100.\n");
            return 1;
        }
        if (students[i].marks3 < 1 || students[i].marks3 > 100){
            printf("Error! Marks must be between 1 and 100.\n");
            return 1;
        }
    }
    for (int i = 0; i < n; i++){
        int total = Total(students[i].marks1,students[i].marks2,students[i].marks3);
        float average = Average(total);
        char grade = Grade(average);

        
        printf("\n");
        printf("Roll: %d\n", students[i].roll);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);

        if (average < 35){
            printf("\n");
            continue;
        }
                         
        printf("Performance: ");
        printStars(grade);
    }

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(students, 0, n);
    printf("\n");

    return 0;
}