// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1998 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
#ifdef __GNUC__
#pragma implementation "vnl_sparse_symmetric_eigensystem.h"
#endif
//
// Class: vnl_sparse_symmetric_eigensystem
// Author: Rupert W. Curwen, GE CR&D.
// Created: 20 Oct 98
// Modifications:
//
//-----------------------------------------------------------------------------

#include "vnl_sparse_symmetric_eigensystem.h"

// Declare fortran function, and what a magnificent function it is too!
extern "C" int dnlaso_(int (*op)(const int* n,
				 const int* m,
				 const double* p,
				 double* q),
		       int (*iovect)(const int* n,
				     const int* m,
				     double* q,
				     const int* j,
				     const int* k),
		       const int* n,
		       const int* nval, 
		       const int* nfig,
		       int* nperm, 
		       const int* nmval,
		       double* val,
		       const int* nmvec,
		       double* vec,
		       const int* nblock,
		       const int* maxop, 
		       const int* maxj, 
		       double* work,
		       int* ind,
		       int* ierr);

static vnl_sparse_symmetric_eigensystem* current_system = NULL;

//------------------------------------------------------------
// -- Callback for multiplying our matrix by a number of vectors.  The
// input is p, which is an NxM matrix.  This function returns q = A p,
// where A is the current sparse matrix.
#ifdef VCL_SUNPRO_CC
extern "C"
#else
static
#endif
int sse_op_callback(const int* n,
		    const int* m,
		    const double* p,
		    double* q)
{
  assert(current_system);

  return current_system->CalculateProduct(*n,*m,p,q);
}

//------------------------------------------------------------
// -- Callback for saving the Lanczos vectors as required by dnlaso.
// If k=0, save the m columns of q as the (j-m+1)th through jth
// vectors.  If k=1 then return the (j-m+1)th through jth vectors in
// q.
#ifdef VCL_SUNPRO_CC
extern "C"
#else
static
#endif
int sse_iovect_callback(const int* n,
			const int* m,
			double* q,
			const int* j,
			const int* k)
{
  assert(current_system);

  if (*k==0)
    return current_system->SaveVectors(*n,*m,q,*j-*m);
  else if (*k==1)
    return current_system->RestoreVectors(*n,*m,q,*j-*m);

  return 0;
}

vnl_sparse_symmetric_eigensystem::vnl_sparse_symmetric_eigensystem()
  : vectors(NULL), values(NULL), nvalues(0)
{
}

//------------------------------------------------------------
// -- Here is where the fortran converted code gets called.  The
// sparse matrix M is assumed to be symmetric.  The n smallest
// eigenvalues and their corresponding eigenvectors are calculated if
// smallest is true (the default).  Otherwise the n largest eigenpairs
// are found.  The accuracy of the eigenvalues is to nfigures decimal
// digits.  Returns 0 if successful, non-zero otherwise.
int vnl_sparse_symmetric_eigensystem::CalculateNPairs(vnl_sparse_matrix<double>& M, 
						int n, 
						bool smallest,
						int nfigures)
{
  mat = &M;

  // Clear current vectors.
  if (vectors)
    {
      delete [] vectors;
      delete [] values;
    }
  nvalues = 0;

  current_system = this;

  int dim = mat->columns();
  int nvals = (smallest)?-n:n;
  int nperm = 0;
  int nmval = n;
  int nmvec = dim;
  double* temp_vals = new double[n*4];
  double* temp_vecs = new double[n*dim];

  int nblock = 10;         // nblock = 2
  if (nblock > dim/6) nblock = dim/6;
  int maxop = dim*10;      // dim*20;
  int maxj = maxop*nblock; // 2*n+1;
  int t1 = 6*nblock+1;
  if (maxj < t1) maxj = t1;
  if (maxj<40) maxj=40;

  // Calculate size of workspace needed.  These expressions come from
  // the LASO documentation.
  int work_size = dim*nblock;
  int t2 = maxj*(2*nblock+3) + 2*n + 6 + (2*nblock+2)*(nblock+1);
  if (work_size < t2) work_size = t2;
  work_size += 2*dim*nblock + maxj*(nblock + n + 2) + 2*nblock*nblock + 3*n;
  double* work = new double[work_size+10];

  // Set starting vectors to zero.
  int i;
  for (i=0; i<dim*nblock; i++) work[i] = 0.0;

  int* ind = new int[n];

  int ierr = 0;

  dnlaso_(sse_op_callback, sse_iovect_callback,
	  &dim, &nvals, &nfigures, &nperm, 
	  &nmval, temp_vals,
	  &nmvec, temp_vecs,
	  &nblock,
	  &maxop,
	  &maxj, 
	  work,
	  ind,
	  &ierr);
  if (ierr > 0)
    {
      if (ierr & 0x1)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem: N < 6*NBLOCK" 
	       << endl;
	}
      if (ierr & 0x2)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem: NFIG < 0" 
	       << endl;
	}
      if (ierr & 0x4)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem: NMVEC < N" 
	       << endl;
	}
      if (ierr & 0x8)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem: NPERM < 0" 
	       << endl;
	}
      if (ierr & 0x10)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem: MAXJ < 6*NBLOCK" 
	       << endl;
	}
      if (ierr & 0x20)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem: NVAL < max(1,NPERM)" 
	       << endl;
	}
      if (ierr & 0x40)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem: NVAL > NMVAL" 
	       << endl;
	}
      if (ierr & 0x80)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem: NVAL > MAXOP" 
	       << endl;
	}
      if (ierr & 0x100)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem: NVAL > MAXJ/2" 
	       << endl;
	}
      if (ierr & 0x200)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem: NBLOCK < 1" 
	       << endl;
	}
    }
  else if (ierr < 0)
    {
      if (ierr == -1)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem:" << endl
	       << "  poor initial vectors chosen" << endl;
	}
      else if (ierr == -2)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem:" << endl
	       << "  reached maximum operations " << maxop 
	       << " without finding all eigenvalues," << endl
	       << "  found " << nperm << " eigenvalues" << endl;
	}
      else if (ierr == -8)
	{
	  cerr << "Error: vnl_sparse_symmetric_eigensystem:" << endl
	       << "  disastrous loss of orthogonality - internal error" << endl;
	}
    }

  // Copy the eigenvalues and vectors.
  nvalues = n;
  vectors = new vnl_vector<double>[n];
  values = new double[n];
  for (i=0; i<n; i++)
    {
      values[i] = temp_vals[i];
      // cout << "value " << temp_vals[i] 
      //   << " accuracy " << temp_vals[i+n*2] << endl;
      vnl_vector<double> vec(dim,0.0);
      for (int j=0; j<dim; j++)
	vec[j] = temp_vecs[j + dim*i];
      vectors[i] = vec;
    }

  // Delete temporary space.
  for (vcl_vector<double*>::iterator iter = temp_store.begin(); 
       iter != temp_store.end();
       ++iter)
    delete [] *iter;
  temp_store.clear();
  
  delete [] temp_vals;
  delete [] temp_vecs;
  delete [] work;
  delete [] ind;

  return ierr;
}

//------------------------------------------------------------
// -- Callback from solver to calculate the product A p.
int vnl_sparse_symmetric_eigensystem::CalculateProduct(int n, int m, 
						 const double* p, double* q)
{
  // Call the special multiply method on the matrix.
  mat->mult(n,m,p,q);

  return 0;
}

//------------------------------------------------------------
// -- Callback to store vectors for dnlaso.
int vnl_sparse_symmetric_eigensystem::SaveVectors(int n, int m,
					    const double* q, 
					    int base)
{
  // Store the contents of q.  Basically this is a fifo.  When a write
  // with base=0 is called, we start another fifo.
  if (base == 0)
    {
      for (vcl_vector<double*>::iterator iter = temp_store.begin(); 
	   iter != temp_store.end();
	   ++iter)
	delete [] *iter;
      temp_store.clear();
    }

  double* temp = new double[n*m];
  memcpy(temp,q,n*m*sizeof(double));
  //  cout << "Save vectors " << base << " " << temp << endl;

  temp_store.push_back(temp);

  return 0;
}

//------------------------------------------------------------
// -- Callback to restore vectors for dnlaso.
int vnl_sparse_symmetric_eigensystem::RestoreVectors(int n, int m, 
					       double* q,
					       int base)
{
  // Store the contents of q.  Basically this is a fifo.  When a read
  // with base=0 is called, we start another fifo.
  static int read_idx = 0;
  if (base == 0)
    read_idx = 0;

  double* temp = temp_store[read_idx];
  memcpy(q,temp,n*m*sizeof(double));
  //  cout << "Restore vectors " << base << " " << temp << endl;

  read_idx++;
  return 0;
}

//------------------------------------------------------------
// -- Return a calculated eigenvector.
vnl_vector<double> vnl_sparse_symmetric_eigensystem::get_eigenvector(int i) const
{
  assert(i<nvalues);
  return vectors[i];
}

double vnl_sparse_symmetric_eigensystem::get_eigenvalue(int i) const
{
  assert(i<nvalues);
  return values[i];
}
