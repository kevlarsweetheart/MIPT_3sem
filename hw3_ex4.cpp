#include <fstream>
#include <vector>
#include <time.h>
#include <omp.h>

using std::vector;
using std::ifstream;
using std::ofstream;
using std::endl;

/* The program finds the result of multiplication of two square matrixes
   using N threads. Size of each matrix is aliquot to N. The infile structure:
   size of the matrixes -> number of threads -> matrix A -> matrix B 
 
   The outfile contains the result of multiplication and time in seconds
   the processor needed to execute the program */

/*
 * Без openmp в один поток в 4,5 раза быстрее выходит. 
 * Так и задумывалось?
 *
 * - А это только на "маленьких" циклах такое происходит?
 * Если нет, то это странно. Если же только на них, то вроде
 * всё нормально: из-за синхронизации и прочих затрат параллелизма.
 */

void init_matrix(vector<vector<int> > &matrix, int matrix_size)
{
	matrix.resize(matrix_size);
	for (int i = 0; i < matrix_size; ++i)
		matrix[i].resize(matrix_size);
}

void get_matrix(vector<vector<int> > &matrix, int matrix_size, ifstream &infile)
{
	for (int i = 0; i < matrix_size; ++i)
		for (int j = 0; j < matrix_size; ++j)
			infile >> matrix[i][j];
}

int get_sum(vector<vector<int> > &a, vector<vector<int> > &b, int i, int j, int vec_size)
{
	int res = 0;
	for (int k = 0; k < vec_size; ++k)
		res += a[i][k] * b[k][j];
	return res;
}

void mult_matrixes(vector<vector<int> > &a, vector<vector<int> > &b, vector<vector<int> > &c, int matrix_size, int n_threads)
{
  #pragma omp parallel for num_threads(n_threads)
	for (int i = 0; i < matrix_size; ++i)
		for (int j = 0; j < matrix_size; ++j)
			c[i][j] = get_sum(a, b, i, j, matrix_size);
}

void print_matrix(vector<vector<int> > &matrix, int matrix_size, ofstream &outfile)
{
	for (int i = 0; i < matrix_size; ++i)
	{
		for (int j = 0; j < matrix_size; ++j)
			outfile << matrix[i][j] << " ";
		outfile << endl;
	}
}

int main()
{
	ifstream infile;
	infile.open("in.txt");
	ofstream outfile;
	outfile.open("out.txt");

	int matrix_size, n;
	infile >> matrix_size >> n;
	vector<vector<int> > a, b, c;

	init_matrix(a, matrix_size);
	init_matrix(b, matrix_size);
	init_matrix(c, matrix_size);

	get_matrix(a, matrix_size, infile);
	get_matrix(b, matrix_size, infile);

	mult_matrixes(a, b, c, matrix_size, n);
	print_matrix(c, matrix_size, outfile);
	outfile << "Time spent: " << ((double)clock()) / CLOCKS_PER_SEC << endl;
	infile.close();
	outfile.close();
	return 0;
}
