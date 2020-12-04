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

// Сдвинуть блок матрицы влево
void left(int** matrix, int k, int count, int length)
{
#pragma omp parallel
    {
    #pragma omp parallel for
        for(int i = 0; i < count; i++)
        {
            int tmp = matrix[k][0];
        #pragma omp parallel for
            for(int j = 0; j < (length-1); j++)
            {
                matrix[k][j] = matrix[k][j+1];
            }
            matrix[k][length-1] = tmp;
        }
    }
}

// Сдвинуть блок матрицы вверх
void up(int** matrix, int l, int count, int length)
{
#pragma omp parallel
    {
    #pragma omp parallel for
        for(int i = 0; i < count; i++)
        {
            int tmp = matrix[0][l];
        #pragma omp parallel for
            for(int j = 0; j < (length-1); j++)
            {
                matrix[j][l] = matrix[j+1][l];
            }
            matrix[length-1][l] = tmp;
        }
    }
}

// Осуществить умножение матрицы
int** multiplication(int** A, int** B, int raw, int column, int threads)
{
    int ** result = new int*[raw];
    omp_set_num_threads(threads);
#pragma omp parallel
    {
        result = new int*[raw];
    #pragma omp parallel for
        for (int i = 0; i < raw; i++)
        {
            result[i] = new int[raw];
        }
    #pragma omp parallel for
        for (int i = 0; i < raw; i++)
        {
        left(A, i, i, raw);
        }
    #pragma omp parallel for
        for (int i = 0; i < raw; i++)
        {
        up(B, i, i, raw);
        }
    
    #pragma omp parallel for
        for (int i = 0; i < raw; i++)
        {
        #pragma omp parallel for
            for (int j = 0; j < raw; j++)
            {
            #pragma omp parallel for
                for (int k = 0; k < raw; k++)
                {
                    int tmp = (i + j + k) % raw;
                    result[j][k] += A[j][tmp] * B[tmp][k];
                    left(A, j, 1, raw);
                    up(B, k, 1, raw);
                }
            }
        }
    }
    return result;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    cout << "Практическая работа №3" << endl;
    cout << "Параллельное программирование с использованием технолгии OpenMP" << endl;
    cout << "Петрушинин Михаил\n" << endl;
    int choose;
    cout << "1 - Заполнить матрицу в ручную.\n2 - Заполнить матрицу автоматически\n> ";
    cin >> choose;
    int** A;
    int** B;
    int** result;
    int threads = 0;
    
    if (choose == 1)
    {
        int n;
        cout << "\nВведите размер квадратной матрицы A:\n> ";
        cin >> n;
        A = create_matrix(n, n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                cout << "Введите x" << i << "y" << j << "\n> ";
                cin >> A[i][j];
            }
        }
        cout << "\nВведите размер квадратной матрицы B:\n> ";
        cin >> n;
        B = create_matrix(n, n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                cout << "Введите x" << i << "y" << j << "\n> ";
                cin >> B[i][j];
            }
        }
        
        cout << "Матрица А:" << endl;
        print_matrix(A, n, n);
        cout << "Матрица B:" << endl;
        print_matrix(A, n, n);
        
        threads = 1;
        double start = clock();
        result = multiplication(A, B, n, n, threads);
        double end = clock();
        double sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Матрица С = AxB:" << endl;
        print_matrix(result, n, n);
        cout << endl;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
        
        threads = 2;
        start = clock();
        result = multiplication(A, B, n, n, threads);
        end = clock();
        sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
        
        threads = 4;
        start = clock();
        result = multiplication(A, B, n, n, threads);
        end = clock();
        sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
        
        threads = 8;
        start = clock();
        result = multiplication(A, B, n, n, threads);
        end = clock();
        sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
        
        threads = 16;
        start = clock();
        result = multiplication(A, B, n, n, threads);
        end = clock();
        sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
        
        threads = 32;
        start = clock();
        result = multiplication(A, B, n, n, threads);
        end = clock();
        sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
    }
    else if(choose == 2)
    {
        int n;
        cout << "\nВведите размер квадратной матрицы A и B:\n> ";
        cin >> n;
        int** A = create_matrix(n, n, 1);
        int** B = create_matrix(n, n, 1);
        cout << endl;
        
        threads = 1;
        double start = clock();
        result = multiplication(A, B, n, n, threads);
        double end = clock();
        double sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
        
        threads = 2;
        start = clock();
        result = multiplication(A, B, n, n, threads);
        end = clock();
        sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
        
        threads = 4;
        start = clock();
        result = multiplication(A, B, n, n, threads);
        end = clock();
        sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
        
        threads = 8;
        start = clock();
        result = multiplication(A, B, n, n, threads);
        end = clock();
        sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
        
        threads = 16;
        start = clock();
        result = multiplication(A, B, n, n, threads);
        end = clock();
        sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
        
        threads = 32;
        start = clock();
        result = multiplication(A, B, n, n, threads);
        end = clock();
        sec = (end - start) / CLOCKS_PER_SEC;
        cout << "Время = " << sec << "s; Количество потоков = " << threads << endl;
    }
    
    return 0;
}
