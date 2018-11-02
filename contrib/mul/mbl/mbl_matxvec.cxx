// This is mul/mbl/mbl_matxvec.cxx
#include <iostream>
#include <cstdlib>
#include "mbl_matxvec.h"
//:
// \file
// \brief Various specialised versions of simple linear algebra operators.
// \author Tim Cootes
// \date 3-Oct-97
// Most of the specialisations are to do with size priorities. If the sizes
// do not match for normal linear algebra operations, these functions will
// for example, only use the first n elements of an >n input vector
//
// \verbatim
// Modifications
// TFC    Revised version 3-Oct-97
// TFC    Added TC_MatXVec2
// NPC    Added NC_VecXMat
// IMS    started conversion to VXL 18 April 2001
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


// Some of the code in this file has been converted to use VXL,
// the rest has been #if-ed out. Only convert the other functions
// as they are needed, and after checking that a suitable function doesn't
// already exist in VXL.


#if 0 // long part commented out
// For testing
static bool HasNaN(const char* s,
                   const vnl_matrix<double>& M)
{
  for (int i=1;i<=M.nrows();++i)
  for (int j=1;j<=M.ncols();++j)
  {
    if (isnan(M(i,j)))
    {
      std::cout<<s<<" has NaN at element "<<i<<','<<j
              <<" in a "<<M.nrows()<<" x "<<M.ncols()<<" matrix.\n";
      return true;
    }
  }
  return false;
}
//=======================================================================

  //: Compute R = V*M
  //  R is resized to the number of rows of V * cols of M
void NC_VecXMat(const vnl_vector<double>& V,
                const vnl_matrix<double>& M,
                vnl_matrix<double>& R)
{
  int nrows = V.size();
  int ncols = M.ncols();
  if (M.nrows()>1)
  {
    std::cerr << "NC_VecXMat : M is a "<<M.nrows()<<"-row matrix.\n"
             << "it may only be 1-row.\n";
    std::abort();
  }

  R.set_size(nrows,ncols);

  const double* v_data = V.dataPtr();
  const double** M_data = M.dataPtr();
  double** R_data = R.dataPtr();

  for (int i=1;i<=nrows;i++)
  {
    const double *m_data = M_data[i];
    double *r_data = R_data[i];
    double vval = v_data[i];
    for (int j=1;j<=ncols;j++)
    {
      r_data[j] = vval * m_data[j];
    }
  }

  for (int i=1;i<=nrows;i++)
    for (int j=1;j<=ncols;j++)
      R(i,j) = V(i) * M(1,j);
}
#endif  //0 - long part commented out

//=======================================================================

  //: Compute R = M*V
  //  Only use first V.size() columns of M
  //  R is resized to the number of rows of M
void mbl_matxvec_prod_mv(const vnl_matrix<double>& M,
                         const vnl_vector<double>& V,
                         vnl_vector<double>& R)
{
  int nr = M.rows();

  R.set_size(nr);

  mbl_matxvec_prod_mv_2(M,V,R);
}

//=======================================================================


  //: Compute R = M*V
  //  Only use first V.size() columns of M
  //  Only use first R.size() rows of M
  //  R is not resized - its size determines how many rows to use
void mbl_matxvec_prod_mv_2(const vnl_matrix<double>& M,
                           const vnl_vector<double>& V,
                           vnl_vector<double>& R)
{
  if (R.size()==0) return;


  int nR = R.size();
#ifndef NDEBUG
  int nc = M.cols();
  int nr = M.rows();
  if (nR>nr)
  {
    std::cerr<<"ERROR: mbl_matxvec_prod_mv_2() R too long.\n";
    std::abort();
  }
#endif

  double *Rdata = R.data_block();

  int t = V.size();
#ifndef NDEBUG
  if (t>nc)
  {
    std::cerr<<"ERROR: mbl_matxvec_prod_mv_2() V too long. V has "<<t<<" elements. M has "<<nc<<" columns.\n";
    std::abort();
  }
#endif

  if (t==0)
  {
    int r = nR;
    while (r--) Rdata[r]=0.0; // Zero R
    return;
  }

#ifndef NDEBUG
  if ((nr<1) || (nc<1))
  {
    std::cerr<<"ERROR: mbl_matxvec_prod_mv_2() - vnl_matrix<double> is 0 x 0\n"
            <<"V has dimension "<<V.size()<<std::endl;
    std::abort();
  }
#endif

  const double * const* Mdata = M.data_array();
  const double *Mdata2;
  const double *Vdata = V.data_block();

  int r = nR;
  while (r--) // Runs from nr-1 .. 0
  {
    double T=0.0;
    Mdata2 = Mdata[r];
    int c=t;
    while (c--) // Runs from t-1 .. 0
      T+=Mdata2[c] * Vdata[c];

    Rdata[r] = T;
  }
}


//=======================================================================


#if 0 // long part commented out

void TC_MatXVec(const vnl_matrix<double>& M,
                const vnl_vector<double>& V,
                vnl_vector<double>& R,
                const std::vector<int>& index)
// R = MV but only use sub-set of all rows of M
{
  int nc = M.ncols();
  int nr = M.nrows();
  if (index.lo()!=1)
  {
    std::cerr<<"TC_MatXVec(M,V,R,index) : index array must begin at 1\n";
    std::abort();
  }
  int n = index.size();


  if (R.size()!=n) R.set_size(n);
  double *Rdata = R.dataPtr();

  int t = V.size();
  if (t>nc)
  {
    std::cerr<<"TC_MatXVec() R too long.\n";
    std::abort();
  }

  if (t==0)
  {
    int r = n+1;
    while (--r) Rdata[r]=0.0; // Zero R
    return;
  }

  if ((nr<1) || (nc<1))
  {
    std::cerr<<"TC_MatXVec - vnl_matrix<double> is 0 x 0\n"
            <<"V has dimension "<<V.size()<<std::endl;
    std::abort();
  }

  const double ** Mdata = M.dataPtr();
  const double *Mdata2;
  const double *Vdata = V.dataPtr();
  const int * i_data = index.dataPtr();

  int i = n+1;
  while (--i) // Runs from i .. 1
  {
    int r = i_data[i];
    double T=0.0;
    Mdata2 = Mdata[r];
    int c=t+1;
    while (--c) // Runs from t .. 1
      T+=Mdata2[c] * Vdata[c];

    Rdata[i] = T;
  }
}
#endif  //0 - long part commented out


//=======================================================================
// Fast Compute R = V' * M = ( M.transpose() * V ).transpose()
void mbl_matxvec_prod_vm(const vnl_vector<double>& V,
                         const vnl_matrix<double>& M,
                         vnl_vector<double>& R)
{
  R.fill(0.0);
  mbl_matxvec_add_prod_vm(V,M,R);
}


//=======================================================================
// Fast Compute R += V' * M = ( M.transpose() * V ).transpose()
void mbl_matxvec_add_prod_vm(const vnl_vector<double>& V,
                             const vnl_matrix<double>& M,
                             vnl_vector<double>& R)
{
  unsigned int nr = M.rows();

#ifndef NDEBUG
  unsigned int nc = M.cols();
  if (nr!=V.size())
  {
    std::cerr<<"ERROR: mbl_matxvec_add_prod_vm - V wrong length\n"
            <<"Expected "<<nr<<" got "<<V.size()<<std::endl;
    std::abort();
  }
#endif //!NDEBUG

  unsigned int t = R.size();
  if (t==0) return;

#ifndef NDEBUG
  assert(t<=nc); // R too long
  if ((nr<1) || (nc<1))
  {
    std::cerr<<"ERROR: mbl_matxvec_add_prod_vm - vnl_matrix<double> is 0 x 0\n"
            <<"V has dimension "<<V.size()<<std::endl;
    std::abort();
  }
#endif //!NDEBUG

   const double *const * Mdata = M.data_array();
   const double * Vdata = V.data_block();
   double * Rdata = R.data_block();

  int r = nr;
  while (r--) // Runs from nr-1..0
  {
    double v = Vdata[r];
    const double *Mdata2 = Mdata[r];
    int c=t;
    while (c--) // Runs from t-1..0
      Rdata[c]+=Mdata2[c] * v;
  }
}


//=======================================================================


#if 0 // long part commented out
void TC_ProductABt(vnl_matrix<double>& ABt,
                   const vnl_matrix<double>& A,
                   const vnl_matrix<double>& B)
// Returns A * B.transpose()
{
   int nr1 = A.nrows();
   int nc1 = A.ncols();
   int nr2 = B.nrows();
   int nc2 = B.ncols();

#ifndef NDEBUG
   if ( nc2 != nc1 )
   {
      std::cerr<<"TC_ProductABt : B.ncols != A.ncols\n";
      std::abort() ;
   }
#endif //!NDEBUG

   if ( (ABt.nrows()!=nr1) || (ABt.ncols()!= nr2) )
    ABt.set_size( nr1, nr2 ) ;

  const double ** A_data = A.dataPtr();
  const double ** B_data = B.dataPtr();
  double ** R_data = ABt.dataPtr();

  int r = nr1 + 1;
  while (--r)
  {
    const double* A_row = A_data[r];
    double* R_row = R_data[r];
    int c = nr2 + 1;
    while (--c)
    {
      const double* B_row = B_data[c];
      double sum = 0.0 ;
      int i = nc1 + 1;
      while (--i) { sum += A_row[i] * B_row[i]; }

      R_row[c] = sum ;
    }
  }
}


//=======================================================================


void TC_ProductAtB(vnl_matrix<double>& AtB,
                   const vnl_matrix<double>& A,
                   const vnl_matrix<double>& B)
// Returns A.transpose() * B
{
   int nr1 = A.nrows();
   int nc1 = A.ncols();
   int nr2 = B.nrows();
   int nc2 = B.ncols();

   if ( nr2 != nr1 )
   {
      std::cerr<<"TC_ProductAtB : B.nrows != A.nrows\n";
      std::abort() ;
   }

   if ( (AtB.nrows()!=nc1) || (AtB.ncols()!= nc2) )
    AtB.set_size( nc1, nc2 ) ;

  const double ** A_data = A.dataPtr();
  const double ** B_data = B.dataPtr();
  double ** R_data = AtB.dataPtr();

  // Zero the elements of R
  for (int r=1;r<=nc1;r++)
  {
    double *R_row = R_data[r];
    int c=nc2;
    while (c) { R_row[c] = 0.0; --c; }
  }

  for (int r1 = 1; r1<=nr1; r1++)
  {
    const double* A_row = A_data[r1];
    const double* B_row = B_data[r1];
    double a;
    int c1 =  nc1;
    while (c1)
    {
      double *R_row = R_data[c1];
      a = A_row[c1];
      int c2 = nc2;
      while (c2)
      {
        R_row[c2] +=a*B_row[c2];
        c2--;
      }
      c1--;
    }
  }
}


//=======================================================================


//: Computes MD where D is diagonal with elements d(i)
void TC_ProductMD(vnl_matrix<double>& MD,
                  const vnl_matrix<double>& M,
                  const vnl_vector<double>& d)
{
  int nr = M.nrows();
  int nc = M.ncols();
  if (d.size()!=nc)
  {
    std::cerr<<"TC_ProductMD() d doesnt match M\n"
            <<"d is "<<d.size()<<" element vector.\n"
            <<"M is "<<nr<<" x "<<nc<<std::endl;
    std::abort();
  }

  if ((MD.nrows()!=nr) || (MD.ncols()!=nc)) MD.set_size(nr,nc);

  double **MD_data = MD.dataPtr();
  const double **M_data = M.dataPtr();
  const double *d_data = d.dataPtr();

  for (int r=1;r<=nr;++r)
  {
    const double* M_row = M_data[r];
    double *MR_row = MD_data[r];
    int c=nc;
    while (c) { MR_row[c] = M_row[c]*d_data[c]; --c; }
  }
}

//=======================================================================

//: Computes DM where D is diagonal with elements d(i)
void TC_ProductDM(vnl_matrix<double>& DM,
                  const vnl_matrix<double>& M,
                  const vnl_vector<double>& d)
{
  int nr = M.nrows();
  int nc = M.ncols();
  if (d.size()!=nr)
  {
    std::cerr<<"TC_ProductDM() d doesnt match M\n"
            <<"d is "<<d.size()<<" element vector.\n"
            <<"M is "<<nr<<" x "<<nc<<std::endl;
    std::abort();
  }

  if ((DM.nrows()!=nr) || (DM.ncols()!=nc)) DM.set_size(nr,nc);

  double **DM_data = DM.dataPtr();
  const double **M_data = M.dataPtr();
  const double *d_data = d.dataPtr();

  for (int r=1;r<=nr;++r)
  {
    const double* M_row = M_data[r];
    double *DM_row = DM_data[r];
    double di = d_data[r];
    int c=nc;
    while (c) { DM_row[c] = di * M_row[c]; --c; }
  }
}
#endif  //0 - long part commented out
