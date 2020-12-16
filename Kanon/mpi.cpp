#include <iostream>
#include "mpi.h"

using namespace std;

// Создание матрицы на raw строк, column столбцов
int** create_matrix(int raw, int column, int code = 0)
{
    int** matrix = new int* [raw];
    for (int i = 0; i < raw; i++)
    {
        matrix[i] = new int[column];
    }

    if (code == 1)
    {
        // Заполнение рандомными числами
        srand(time(0));
        for (int i = 0; i < raw; i++)
        {
            for (int j = 0; j < column; j++)
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

    for (int i = 0; i < count; i++)
    {
        int tmp = matrix[k][0];
        for (int j = 0; j < (length - 1); j++)
        {
            matrix[k][j] = matrix[k][j + 1];
        }
        matrix[k][length - 1] = tmp;
    }
}

// Сдвинуть блок матрицы вверх
void up(int** matrix, int l, int count, int length)
{
    for (int i = 0; i < count; i++)
    {
        int tmp = matrix[0][l];
        for (int j = 0; j < (length - 1); j++)
        {
            matrix[j][l] = matrix[j + 1][l];
        }
        matrix[length - 1][l] = tmp;
    }
}

// Последовательное умножение матриц алгоритмом Кэнона
int** multiplication(int** A, int** B, int raw, int column)
{
    int** result = new int* [raw];
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

// Умножение матриц алгоритмом Кэнона
void mpi(int rank, int size, int n)
{
    int shift;
    int i, j, k;
    int dims[2];
    int periods[2];
    int left, right, up, down;
    double* A;
    double* B;
    double* C;
    double* buf;
    double* tmp = nullptr;
    unsigned int iseed = 0;
    int N;

    MPI_Comm comm;

    dims[0] = 0;
    dims[1] = 0;
    periods[0] = 1;
    periods[1] = 1;

    MPI_Dims_create(size, 2, dims);

    N = n / dims[0];

    A = (double*)malloc(N*N * sizeof(double));
    B = (double*)malloc(N*N * sizeof(double));
    buf = (double*)malloc(N*N * sizeof(double));
    C = (double*)calloc(N*N, sizeof(double));

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            A[i*N+j] = 5 - (int)(10 * rand() / (RAND_MAX + 1));
            B[i*N+j] = 5 - (int)(10 * rand() / (RAND_MAX + 1));
            C[i*N+j] = 0;
        }
    }

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &comm);
    MPI_Cart_shift(comm, 0, 1, &left, &right);
    MPI_Cart_shift(comm, 1, 1, &up, &down);

    double start = MPI_Wtime();

    for (shift = 0; shift < dims[0]; shift++)
    {
        for (i = 0; i < N; i++)
            for (k = 0; k < N; k++)
                for (j = 0; j < N; j++)
                    C[i*N+j] += A[i*N+k] * B[k*N+j];

        if (shift == (dims[0] - 1))
            break;

        MPI_Sendrecv(A, N * N, MPI_DOUBLE, left, 1, buf, N * N, MPI_DOUBLE, right, 1, comm, MPI_STATUS_IGNORE);
        tmp = buf;
        buf = A;
        A = tmp;

        MPI_Sendrecv(B, N * N, MPI_DOUBLE, up, 2, buf, N * N, MPI_DOUBLE, down, 2, comm, MPI_STATUS_IGNORE);
        tmp = buf;
        buf = B;
        B = tmp;
    }

    MPI_Barrier(comm);


    if (rank == 0)
    {
        double end = MPI_Wtime();
        double time = (end - start);
        cout << "n = " << n << endl;
        cout << "time = " << time << "sec; p = " << size << endl;
    }

    free(A); 
    free(B); 
    free(buf);
    free(C);
}

// Запуск вычислений
void run(int* n, int size, int rank, bool isMpi, int code = 0)
{
    if (!isMpi)
    {
        if (code == 2)
        {
            cout << endl;
            cout << "Последовательный метод вычисления:" << endl;
            for (int i = 0; i < 6; i++)
            {
                int** A = create_matrix(n[i], n[i], 1);
                int** B = create_matrix(n[i], n[i], 1);

                double start = clock();
                int** result = multiplication(A, B, n[i], n[i]);
                double end = clock();
                double sec = (end - start) / CLOCKS_PER_SEC;
                cout << "time = " << sec << " sec; n = " << n[i] << endl;
            }
        }
        else if (code == 1)
        {
            int n;
            cout << "\nВведите размер квадратной матрицы A:\n> ";
            cin >> n;
            int** A = create_matrix(n, n);
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
            int** B = create_matrix(n, n);
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
            print_matrix(B, n, n);

            clock_t start = clock();

            int** result = multiplication(A, B, n, n);

            clock_t end = clock();
            double sec = (double)(end - start) / CLOCKS_PER_SEC;

            cout << "Матрица С = AxB:" << endl;
            print_matrix(result, n, n);
            cout << endl;
            cout << "time = " << sec << " sec" << endl;
            cout << endl;
        }
    }
    else
    {
        if (rank == 0)
        {
            cout << endl;
            cout << "Распределенный метод вычисления:" << endl;
        }
        for (int i = 0; i < 6; i++)
        {
            mpi(rank, size, n[i]);
        }
    }
}



int main(int* argc, char** argv)
{
	int size, rank;
    setlocale(LC_ALL, "Russian");
    int* n = new int[] { 64, 100, 144, 196, 256, 324 };
    int** matrix;

	MPI_Init(argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        cout << "Практическая работа №5" << endl;
        cout << "Распределенное программирование с использованием основ технологии MPI" << endl;
        cout << "Петрушинин Михаил" << endl;
    }



    if (size == 1)
    {
        int choose;
        cout << "1 - Заполнить матрицу в ручную.\n2 - Заполнить матрицу автоматически\n> ";
        cin >> choose;
        run(n, size, rank, false, choose);
    }
    else
    {
        int choose = 0;
        if (rank == 0)
        {
            cout << "1 - Запустить процесс тестирования.\n> ";
            cin >> choose;
        }
        run(n, size, rank, true, choose);
    }

	MPI_Finalize();

	return 0;
}