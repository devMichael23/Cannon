#include <iostream>
#include <omp.h>
#include <ctime>

using namespace std;

// Создание матрицы на raw строк, column столбцов
int **create_matrix(int raw, int column, int code=0)
{
    int** matrix = new int*[raw];
    for(int i = 0; i < raw; i++)
    {
        matrix[i] = new int[column];
    }
    
    if (code == 1)
    {
    // Заполнение рандомными числами
    srand(time(0));
        for(int i = 0; i < raw; i++)
        {
            for(int j = 0; j < column; j++)
            {
                matrix[i][j] = 0 + rand() % 10;
            }
        }
    }
    
    return matrix;
}

// Вывод матрицы
void print_matrix(int** array, int raw, int column)
{
    for (int i = 0; i < raw; i++)
    {
        cout << "x[" << i << "] = ";
        for (int j = 0; j < column; j++)
        {
            cout << array[i][j] << " ";
        }
        cout << endl;
    }
}

void left(int** matrix, int k, int count, int length)
{
    for(int i = 0; i < count; i++)
    {
        int tmp = matrix[k][0];
        for(int j = 0; j < (length-1); j++)
        {
            matrix[k][j] = matrix[k][j+1];
        }
        matrix[k][length-1] = tmp;
    }
}

void up(int** matrix, int l, int count, int length)
{
    for(int i = 0; i < count; i++)
    {
        int tmp = matrix[0][l];
        for(int j = 0; j < (length-1); j++)
        {
            matrix[j][l] = matrix[j+1][l];
        }
        matrix[length-1][l] = tmp;
    }
}

int** multiplication(int** A, int** B, int raw, int column)
{
    int** result = new int*[raw];
    for (int i = 0; i < raw; i++)
    {
        result[i] = new int[raw];
    }
    for (int i = 0; i < raw; i++)
    {
        left(A, i, i, raw);
    }
    for (int i = 0; i < raw; i++)
    {
        up(B, i, i, raw);
    }
    
    for (int i = 0; i < raw; i++)
    {
        for (int j = 0; j < raw; j++)
        {
            for (int k = 0; k < raw; k++)
            {
                int tmp = (i + j + k) % raw;
                result[j][k] += A[j][tmp] * B[tmp][k];
                left(A, j, 1, raw);
                up(B, k, 1, raw);
            }
        }
    }
    return result;
}

int main()
{
    int** A = create_matrix(3, 3, 1);
    int** B = create_matrix(3, 3, 1);
    cout << "A:" << endl;
    print_matrix(A, 3, 3);
    cout << "B:" << endl;
    print_matrix(B, 3, 3);
    int** C = multiplication(A, B, 3, 3);
    cout << "C:" << endl;
    print_matrix(C, 3, 3);

    
    return 0;
}
