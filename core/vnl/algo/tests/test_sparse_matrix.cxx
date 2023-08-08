// This is core/vnl/algo/tests/test_sparse_matrix.cxx
#include <ctime>
#include <iostream>
#include <algorithm>
#include "vnl/vnl_sparse_matrix.h"
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_generalized_eigensystem.h>
#include "testlib/testlib_test.h"

// Test the sparse matrix operations.
void
doTest1()
{
  constexpr unsigned int n = 20;

  vnl_sparse_matrix<double> m1(n, n);
  for (unsigned i = 0; i < n; i++)
  {
    m1(i, i) = 2.0;
    m1(i, (i + 3) % n) = 1.0;
  }

  std::cout << "m1:\n";
  for (unsigned i = 0; i < n; i++)
  {
    for (unsigned j = 0; j < n; j++)
      std::cout << m1(i, j) << ' ';
    std::cout << std::endl;
  }

  vnl_sparse_matrix<double> m2(n, n);
  for (unsigned i = 0; i < n; i++)
  {
    m2(i, i) = 2.0;
    m2(i, (i + n - 3) % n) = 1.0;
  }

  std::cout << "m2:\n";
  for (unsigned i = 0; i < n; i++)
  {
    for (unsigned j = 0; j < n; j++)
      std::cout << m2(i, j) << ' ';
    std::cout << std::endl;
  }

  vnl_sparse_matrix<double> prod = m1 * m2;

  std::cout << "prod:\n";
  for (unsigned i = 0; i < n; i++)
  {
    for (unsigned j = 0; j < n; j++)
      std::cout << prod(i, j) << ' ';
    std::cout << std::endl;
  }

  vnl_sparse_matrix<double> sum = m1 + m2;

  std::cout << "sum:\n";
  for (unsigned i = 0; i < n; i++)
  {
    for (unsigned j = 0; j < n; j++)
      std::cout << sum(i, j) << ' ';
    std::cout << std::endl;
  }

  vnl_sparse_matrix<double> diff = m1 - m2;

  std::cout << "diff:\n";
  for (unsigned i = 0; i < n; i++)
  {
    for (unsigned j = 0; j < n; j++)
      std::cout << diff(i, j) << ' ';
    std::cout << std::endl;
  }
}

void
doTest2()
{
  std::clock_t t = std::clock();
  for (unsigned int n = 1000; n < 4000; n += 1000)
  {
    vnl_sparse_matrix<double> m1(n, n);
    for (unsigned i = 0; i < n; i++)
    {
      m1(i, i) = 2.0;
      m1(i, (i + 3) % n) = 1.0;
    }

    vnl_sparse_matrix<double> m2(n, n);
    for (unsigned i = 0; i < n; i++)
    {
      m2(i, i) = 2.0;
      m2(i, (i + n - 3) % n) = 1.0;
    }

    vnl_sparse_matrix<double> prod = m1 * m2;

    std::clock_t tn = std::clock();
    std::cout << n << ' ' << tn - t << std::endl;
    t = tn;
  }
}

void
doTest3()
{
  constexpr unsigned int n = 20;

  vnl_sparse_matrix<double> ms(n, n);
  vnl_matrix<double> md(n, n);
  md = 0.0; // Initialise to all zeros
  // The matrix must be symmetric
  for (unsigned i = 0; i < n; i++)
  {
    ms(i, i) = md(i, i) = i + 1.0;
    ms(i, (i + 3) % n) = md(i, (i + 3) % n) = 1.0;
    ms(i, (i + n - 3) % n) = md(i, (i + n - 3) % n) = 1.0;
    // ms(i,i) = md(i,i) = 1.0*(i+1)*(i+1);
  }

  std::cout << "ms:\n";
  for (unsigned i = 0; i < n; i++)
  {
    for (unsigned j = 0; j < n; j++)
      std::cout << ms(i, j) << ' ';
    std::cout << std::endl;
  }
  std::cout << "md:\n" << md << std::endl;
}

void
doTest4()
{
  constexpr unsigned int n = 20;

  vnl_sparse_matrix<double> ms(n, n);
  vnl_matrix<double> md(n, n);
  md = 0.0; // Initialise to all zeros
  // The matrix must be symmetric
  for (unsigned i = 0; i < n; i++)
  {
    ms(i, i) = md(i, i) = i + 1.0;
    ms(i, (i + 3) % n) = md(i, (i + 3) % n) = 1.0;
    ms(i, (i + n - 3) % n) = md(i, (i + n - 3) % n) = 1.0;
    // ms(i,i) = md(i,i) = 1.0*(i+1)*(i+1);
  }
}

void
doTest5()
{
  constexpr unsigned int n = 20;

  // testing the generaized case of CalculateNPairs
  // A * x = lambda * B * x
  // test that B = identity produces same answers as
  // A * x = lambda * x
  vnl_sparse_matrix<double> ms(n, n), bIdentity(n, n);
  vnl_matrix<double> md(n, n);
  md = 0.0; // Initialise to all zeros
  // The matrix must be symmetric
  for (unsigned i = 0; i < n; i++)
  {
    bIdentity(i, i) = 1.0;
    ms(i, i) = md(i, i) = i + 1.0;
    ms(i, (i + 3) % n) = md(i, (i + 3) % n) = 1.0;
    ms(i, (i + n - 3) % n) = md(i, (i + n - 3) % n) = 1.0;
    // ms(i,i) = md(i,i) = 1.0*(i+1)*(i+1);
  }
}

void
doTest6()
{
  constexpr int matOrd = 6;
  constexpr double Sdata[matOrd * matOrd] = {
    30.0000, -3.4273, 13.9254, 13.7049, -2.4446, 20.2380, -3.4273, 13.7049, -2.4446, 1.3659,  3.6702,  -0.2282,
    13.9254, -2.4446, 20.2380, 3.6702,  -0.2282, 28.6779, 13.7049, 1.3659,  3.6702,  12.5273, -1.6045, 3.9419,
    -2.4446, 3.6702,  -0.2282, -1.6045, 3.9419,  2.5821,  20.2380, -0.2282, 28.6779, 3.9419,  2.5821,  44.0636,
  };

  constexpr double Cdata[matOrd * matOrd] = {
    -1, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 0, -1, 0, 0, 2, 2, 0, 0, -1, 0, 2, 2, 0, 0, 0, -1, 0, 2, 2, 2, 2, 0, -1,
  };
  vnl_matrix<double> S(matOrd, matOrd); std::copy_n(Sdata,matOrd*matOrd,S.begin());
  vnl_matrix<double> C(matOrd, matOrd); std::copy_n(Cdata,matOrd*matOrd,C.begin());

  // set up spare matrices same as dense ones...
  vnl_sparse_matrix<double> sparseS(matOrd, matOrd);
  vnl_sparse_matrix<double> sparseC(matOrd, matOrd);
  for (int i = 0; i < matOrd; i++)
  {
    for (int j = 0; j < matOrd; j++)
    {
      if (S(i, j) != 0.0)
        sparseS(i, j) = S(i, j);
      if (C(i, j) != 0.0)
        sparseC(i, j) = C(i, j);
    }
  }

  vnl_generalized_eigensystem gev(C, S);

  std::cout << "V = " << gev.V << '\n' << "D = " << gev.D << '\n' << std::endl;
}

static void
test_sparse_matrix()
{
  std::cout << "Starting test 1\n";
  doTest1();
  std::cout << "Starting test 2\n";
  doTest2();
  std::cout << "Starting test 3\n";
  doTest3();
  std::cout << "Starting test 4\n";
  doTest4();
  std::cout << "Starting test 5\n";
  doTest5();
  std::cout << "Starting test 6\n";
  doTest6();
}

TESTMAIN(test_sparse_matrix);
