#include<stdio.h>
#include <stdlib.h>
#include <time.h>

void generateMatrix(int n, int *ptr){
    int min =0 , max =255;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            int num = (rand() % (max - min + 1)) + min;
            *(ptr+(i*n)+j)=num;
        }
    }
}

void swap(int *a , int *b){
    int temp = *a;
    *a = *b;
    *b=temp;
}

void rotateMatrix(int n , int *ptr){
    for(int i=0;i<n;i++){
        for(int j=0;j<i;j++){
            int *a = ptr+(i*n)+j;
            int *b = ptr+(j*n)+i;
            swap(a,b);
        }
    }
    // reverse the rows

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n / 2; j++)
        {
            int *a = ptr + (i * n) + j;
            int *b = ptr + (i * n) + (n - 1 - j);
            swap(a, b);
        }
    }
}
void printMatrix(int n, int *ptr){

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%d ",*(ptr+(i*n)+j));
        }
        printf("\n");
    }
}

void filter(int n, int *ptr)
{
    int *above = (int *)malloc(n * sizeof(int));   
    int *current = (int *)malloc(n * sizeof(int)); 

    for (int i = 0; i < n; i++)
    {
    
        for (int j = 0; j < n; j++)
            *(current + j) = *(ptr + i * n + j);

        
        for (int j = 0; j < n; j++)
        {
            int sum = 0, count = 0;

            for (int r = i - 1; r <= i + 1; r++) 
            {
                for (int c = j - 1; c <= j + 1; c++) 
                {
                    if (r < 0 || r >= n || c < 0 || c >= n)
                        continue; 

                    if (r == i - 1)
                        sum += *(above + c); 
                    else if (r == i)
                        sum += *(current + c); 
                    else
                        sum += *(ptr + r * n + c); 

                    count++;
                }
            }
            *(ptr + i * n + j) = sum / count; 
        }

        for (int j = 0; j < n; j++)
            *(above + j) = *(current + j);
    }

    free(above);
    free(current);
}
int main(){
    int n;
    printf("Enter a number n : ");
    scanf("%d",&n);
    if(n<2 || n>10){
        printf("Invalid value of n :");
        return 1;
    }
    srand(time(NULL));
    int matrix[n][n];

    generateMatrix(n, &matrix[0][0]);
    printf("Original:\n");
    printMatrix(n,&matrix[0][0]);

    rotateMatrix(n,&matrix[0][0]);
    printf("Rotated:\n");
    printMatrix(n, &matrix[0][0]);

    printf("FinalOutput :\n");
    filter(n,&matrix[0][0]);
    printMatrix(n, &matrix[0][0]);
    return 0;
}