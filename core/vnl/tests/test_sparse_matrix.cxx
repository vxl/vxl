/*
<begin copyright notice>
---------------------------------------------------------------------------

                  Copyright (c) 1998 TargetJr Consortium
              GE Corporate Research and Development (GE CRD)
                            1 Research Circle
                           Niskayuna, NY 12309
                           All Rights Reserved
             Reproduction rights limited as described below.
                              
     Permission to use, copy, modify, distribute, and sell this software
     and its documentation for any purpose is hereby granted without fee,
     provided that (i) the above copyright notice and this permission
     notice appear in all copies of the software and related documentation,
     (ii) the name TargetJr Consortium (represented by GE CRD), may not be
     used in any advertising or publicity relating to the software without
     the specific, prior written permission of GE CRD, and (iii) any
     modifications are clearly marked and summarized in a change history
     log.
      
     THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
     EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
     WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
     IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
     INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
     DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
     WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
     ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
     USE OR PERFORMANCE OF THIS SOFTWARE.

---------------------------------------------------------------------------
<end copyright notice>
*/

#include <iostream.h>
#include <vnl/vnl_sparse_matrix.h>
#include <vnl/algo/vnl_sparse_symmetric_eigensystem.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_test.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matops.h>
#include <iomanip.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

// Test the sparse matrix operations.

int doTest1()
{
  const unsigned int n = 20;

  unsigned int i,j;
  vnl_sparse_matrix<double> m1(n,n);
  for (i=0; i<n; i++)
    {
      m1(i,i) = 2.0;
      m1(i,(i+3)%n) = 1.0;
    }

  cout << "m1:\n";
  for (i=0; i<n; i++)
    {
      for (j=0; j<n; j++)
	cout << m1(i,j) << " ";
      cout << endl;
    }

  vnl_sparse_matrix<double> m2(n,n);
  for (i=0; i<n; i++)
    {
      m2(i,i) = 2.0;
      m2(i,(i+n-3)%n) = 1.0;
    }

  cout << "m2:\n";
  for (i=0; i<n; i++)
    {
      for (j=0; j<n; j++)
	cout << m2(i,j) << " ";
      cout << endl;
    }

  vnl_sparse_matrix<double> prod;
  m1.mult(m2,prod);

  cout << "prod:\n";
  for (i=0; i<n; i++)
    {
      for (j=0; j<n; j++)
	cout << prod(i,j) << " ";
      cout << endl;
    }

  vnl_sparse_matrix<double> sum;
  m1.add(m2,sum);

  cout << "sum:\n";
  for (i=0; i<n; i++)
    {
      for (j=0; j<n; j++)
	cout << sum(i,j) << " ";
      cout << endl;
    }
  
  vnl_sparse_matrix<double> diff;
  m1.subtract(m2,diff);

  cout << "diff:\n";
  for (i=0; i<n; i++)
    {
      for (j=0; j<n; j++)
	cout << diff(i,j) << " ";
      cout << endl;
    }
  
  return 0;
}

int doTest2()
{
  clock_t t = clock();
  for (unsigned int n = 1000; n<4000; n+=1000)
    {
      unsigned int i;
      vnl_sparse_matrix<double> m1(n,n);
      for (i=0; i<n; i++)
	{
	  m1(i,i) = 2.0;
	  m1(i,(i+3)%n) = 1.0;
	}

      vnl_sparse_matrix<double> m2(n,n);
      for (i=0; i<n; i++)
	{
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

  unsigned int i,j;
  vnl_sparse_matrix<double> ms(n,n);
  vnl_matrix<double> md(n,n);
  md = 0.0;
  for (i=0; i<n; i++)
    {
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
  for (i=0; i<n; i++)
    {
      for (j=0; j<n; j++)
	cout << ms(i,j) << " ";
      cout << endl;
    }
  cout << "md:" << endl << md << endl;

  const unsigned int nvals = 2;
  vnl_symmetric_eigensystem ed(md);
  vnl_sparse_symmetric_eigensystem es;
  es.CalculateNPairs(ms,nvals,true,20);

  // Report 'em.
  for (i=0; i<nvals; i++)
    {
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

  unsigned int i;
  vnl_sparse_matrix<double> ms(n,n);
  vnl_matrix<double> md(n,n);
  md = 0.0;
  for (i=0; i<n; i++)
    {
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
  vnl_symmetric_eigensystem ed(md);
  vnl_sparse_symmetric_eigensystem es;
  es.CalculateNPairs(ms,nvals);

  // Report 'em.
  for (i=0; i<nvals; i++)
    {
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
int testvnl_sparse_matrix()
{
  int r = doTest1();
  r = r + doTest2();
  r = r + doTest3();
  r = r + doTest4();
  return r;
}

TESTMAIN(testvnl_sparse_matrix);
