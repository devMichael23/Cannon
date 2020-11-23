#include <iostream>
#include <omp.h>
#include <ctime>

using namespace std;

// Создание матрицы на n строк, m столбцов
int **create_matrix(int n, int m, int code=0)
{
    int** array = new int*[n];
    for(int i = 0; i < n; i++)
    {
        array[i] = new int[m];
    }
    
    if (code == 1)
    {
    // Заполнение рандомными числами
    srand(time(0));
        for(int i = 0; i < n; i++)
        {
            for(int j=0; j < m; j++)
            {
                array[i][j] = 0 + rand() % 1000;;
            }
        }
    }
    
    return array;
}

// Вывод матрицы
void print_matrix(int** array)
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            std::cout << array[i][j] << " ";
        }
        std:: cout << std::endl;
    }
}

int main()
{
    
    
    return 0;
}
