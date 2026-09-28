#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

int precedence(char ch)
{
    if (ch == '*' || ch == '/')
        return 2;

    if (ch == '+' || ch == '-')
        return 1;

    return 0;
}

int main()
{
    char input[500];
    int num[500];
    char op[500];

    int numTop = -1;
    int opTop = -1;
    int i = 0;
    int error = 0;

    bool expectNumber = true;

    printf("Enter expression: ");
    fgets(input, 500, stdin);

    while (input[i] != '\0')
    {
        if (isspace(input[i]))
        {
            i++;
            continue;
        }

        if (isdigit(input[i]))
        {
            if (!expectNumber)
            {
                error = 1;
                break;
            }

            int number = 0;

            while (isdigit(input[i]))
            {
                number = number * 10 + (input[i] - '0');
                i++;
            }

            num[++numTop] = number;
            expectNumber = false;
        }

        else if (input[i] == '+' ||
                 input[i] == '-' ||
                 input[i] == '*' ||
                 input[i] == '/')
        {
            if (expectNumber)
            {
                error = 1;
                break;
            }

            char currentOperator = input[i];

            while (opTop >= 0 &&
                   precedence(op[opTop]) >= precedence(currentOperator))
            {
                char currentOp = op[opTop--];

                int b = num[numTop--];
                int a = num[numTop--];

                if (currentOp == '+')
                {
                    num[++numTop] = a + b;
                }
                else if (currentOp == '-')
                {
                    num[++numTop] = a - b;
                }
                else if (currentOp == '*')
                {
                    num[++numTop] = a * b;
                }
                else if (currentOp == '/')
                {
                    if (b == 0)
                    {
                        error = 2;
                        break;
                    }

                    num[++numTop] = a / b;
                }
            }

            if (error != 0)
                break;

            op[++opTop] = currentOperator;
            expectNumber = true;
            i++;
        }

        else
        {
            error = 1;
            break;
        }
    }

    if (error == 0 && expectNumber)
        error = 1;

    while (error == 0 && opTop >= 0)
    {
        char currentOp = op[opTop--];

        int b = num[numTop--];
        int a = num[numTop--];

        if (currentOp == '+')
        {
            num[++numTop] = a + b;
        }
        else if (currentOp == '-')
        {
            num[++numTop] = a - b;
        }
        else if (currentOp == '*')
        {
            num[++numTop] = a * b;
        }
        else if (currentOp == '/')
        {
            if (b == 0)
            {
                error = 2;
                break;
            }

            num[++numTop] = a / b;
        }
    }

    if (error == 0 && numTop != 0)
        error = 1;

    if (error == 1)
    {
        printf("Error: Invalid expression.\n");
    }
    else if (error == 2)
    {
        printf("Error: Division by zero.\n");
    }
    else
    {
        printf("%d\n", num[numTop]);
    }

    return 0;
}