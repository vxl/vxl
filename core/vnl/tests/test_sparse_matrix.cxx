#include <vcl/vcl_cstdlib.h>
#include <vcl/vcl_ctime.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_iomanip.h>

#include <vnl/vnl_sparse_matrix.h>
#include <vnl/vnl_test.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matops.h>
#include <vnl/algo/vnl_sparse_symmetric_eigensystem.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

// Test the sparse matrix operations.

int doTest1()
{
  const unsigned int n = 20;

  vnl_sparse_matrix<double> m1(n,n);
  for (unsigned i=0; i<n; i++) {
    m1(i,i) = 2.0;
    m1(i,(i+3)%n) = 1.0;
  }

  cout << "m1:\n";
  for (unsigned i=0; i<n; i++) {
    for (unsigned j=0; j<n; j++)
      cout << m1(i,j) << " ";
    cout << endl;
  }
  
  vnl_sparse_matrix<double> m2(n,n);
  for (unsigned i=0; i<n; i++) {
    m2(i,i) = 2.0;
    m2(i,(i+n-3)%n) = 1.0;
  }

  cout << "m2:\n";
  for (unsigned i=0; i<n; i++) {
    for (unsigned j=0; j<n; j++)
      cout << m2(i,j) << " ";
    cout << endl;
  }

  vnl_sparse_matrix<double> prod;
  m1.mult(m2,prod);

  cout << "prod:\n";
  for (unsigned i=0; i<n; i++) {
    for (unsigned j=0; j<n; j++)
      cout << prod(i,j) << " ";
    cout << endl;
  }

  vnl_sparse_matrix<double> sum;
  m1.add(m2,sum);

  cout << "sum:\n";
  for (unsigned i=0; i<n; i++) {
    for (unsigned j=0; j<n; j++)
      cout << sum(i,j) << " ";
    cout << endl;
  }
  
  vnl_sparse_matrix<double> diff;
  m1.subtract(m2,diff);

  cout << "diff:\n";
  for (unsigned i=0; i<n; i++) {
    for (unsigned j=0; j<n; j++)
      cout << diff(i,j) << " ";
    cout << endl;
  }
  
  return 0;
}

int doTest2()
{
  clock_t t = clock();
  for (unsigned int n = 1000; n<4000; n+=1000) {
    vnl_sparse_matrix<double> m1(n,n);
    for (unsigned i=0; i<n; i++) {
      m1(i,i) = 2.0;
      m1(i,(i+3)%n) = 1.0;
    }
    
    vnl_sparse_matrix<double> m2(n,n);
    for (unsigned i=0; i<n; i++) {
      m2(i,i) = 2.0;
      m2(i,(i+n-3)%n) = 1.0;
    }
    
    vnl_sparse_matrix<double> prod;
    m1.mult(m2,prod);
    
    clock_t tn = clock();
    cout << n << " " << tn - t << endl;
    t = tn;
  }
  
  return 0;
}

int doTest3()
{
  const unsigned int n = 20;

  vnl_sparse_matrix<double> ms(n,n);
  vnl_matrix<double> md(n,n);
  md = 0.0;
  for (unsigned i=0; i<n; i++) {
    md(i,i) = i+1.0;
    md(i,(i+3)%n) = 1.0;
    md(i,(i+n-3)%n) = 1.0;
    ms(i,i) = i+1.0;
    ms(i,(i+3)%n) = 1.0;
    ms(i,(i+n-3)%n) = 1.0;
    //       md(i,i) = 1.0*(i+1)*(i+1);
    //       ms(i,i) = 1.0*(i+1)*(i+1);
  }

  cout << "ms:" << endl;
  for (unsigned i=0; i<n; i++) {
    for (unsigned j=0; j<n; j++)
      cout << ms(i,j) << " ";
    cout << endl;
  }
  cout << "md:" << endl << md << endl;
  
  const unsigned int nvals = 2;
  vnl_symmetric_eigensystem<double> ed(md);
  vnl_sparse_symmetric_eigensystem es;
  es.CalculateNPairs(ms,nvals,true,20);
  
  // Report 'em.
  for (unsigned i=0; i<nvals; i++) {
    cout << "Dense : " << ed.D(i,i) << " -> " 
	 << ed.get_eigenvector(i) << endl;
    cout << "Sparse: " << es.get_eigenvalue(i) << " -> " 
	 << es.get_eigenvector(i) << endl;
  }

  return 0;
}

int doTest4()
{
  const unsigned int n = 20;

  vnl_sparse_matrix<double> ms(n,n);
  vnl_matrix<double> md(n,n);
  md = 0.0;
  for (unsigned i=0; i<n; i++) {
    md(i,i) = i+1.0;
    md(i,(i+3)%n) = 1.0;
    md(i,(i+n-3)%n) = 1.0;
    ms(i,i) = i+1.0;
    ms(i,(i+3)%n) = 1.0;
    ms(i,(i+n-3)%n) = 1.0;
    //       md(i,i) = 1.0*(i+1)*(i+1);
    //       ms(i,i) = 1.0*(i+1)*(i+1);
  }
  
  const unsigned int nvals = 3;
  vnl_symmetric_eigensystem<double> ed(md);
  vnl_sparse_symmetric_eigensystem es;
  es.CalculateNPairs(ms,nvals);

  // Report 'em.
  for (unsigned i=0; i<nvals; i++) {
    double dense = ed.D(i,i);
    double sparse = es.get_eigenvalue(i);
    cout << "Dense : " << dense << endl;
    cout << "Sparse: " << sparse << endl;
    double err = fabs(dense - sparse);
    cout << "Error: " << err << endl;
    Assert("vnl_sparse_symmetric_eigensystem eigenvalue error", err < 1e-10);
  }
  
  return 0;
}


extern "C"
int test_sparse_matrix()
{
  int r = doTest1();
  r = r + doTest2();
  r = r + doTest3();
  r = r + doTest4();
  return r;
}

TESTMAIN(test_sparse_matrix);
