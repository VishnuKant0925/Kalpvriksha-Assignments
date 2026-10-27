#include<stdio.h>
#include <stdlib.h>
#include <time.h>

void generateMatrix(int n, int matrix[n][n]){
    int min =0 , max =255;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            int num = (rand() % (max - min + 1)) + min;
            matrix[i][j] = num; 
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
   
    for(int i=0;i<n;i++){
        int low = 0;
        int high =n-1; 
        for(int j=n-1;j>n/2;j--){
            int *a = ptr + (i*n)+ low;
            int *b=  ptr +(i*n)+ high;
            swap(a,b);
            low++;
            high--;
        }
    }
}
void printMatrix(int n, int matrix[n][n]){
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%d ",matrix[i][j]);
        }
        printf("\n");
    }
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
    generateMatrix(n, matrix);
    printf("Original:\n");
    printMatrix(n,matrix);
    rotateMatrix(n,&matrix[0][0]);
    printf("Rotated:\n");
    printMatrix(n, matrix);
    return 0;
}