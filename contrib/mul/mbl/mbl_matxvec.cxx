// This is mul/mbl/mbl_matxvec.cxx
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
#include <vcl_cassert.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_iostream.h>


// Some of the code in this file has been converted to use VXL,
// the rest has been #if-ed out. Only convert the other functions
// as they are needed, and after chacking that a suitable function doesn't
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
      vcl_cout<<s<<" has NaN at element "<<i<<","<<j;
      vcl_cout<<" in a "<<M.nrows()<<" x "<<M.ncols()<<" matrix."<<vcl_endl;
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
    vcl_cerr << "NC_VecXMat : M is a "<<M.nrows()<<"-row matrix."<<vcl_endl;
    vcl_cerr << "it may only be 1-row." <<vcl_endl;
    vcl_abort();
  }

  R.resize(nrows,ncols);

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

  R.resize(nr);

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

  int nc = M.cols();
  int nr = M.rows();

  int nR = R.size();
#ifndef NDEBUG
  if (nR>nr)
  {
    vcl_cerr<<"ERROR: mbl_matxvec_prod_mv_2() R too long."<<vcl_endl;
    vcl_abort();
  }
#endif

  double *Rdata = R.data_block();

  int t = V.size();
#ifndef NDEBUG
  if (t>nc)
  {
    vcl_cerr<<"ERROR: mbl_matxvec_prod_mv_2() V too long. V has "<<t<<" elements. M has "<<nc<<" columns."<<vcl_endl;
    vcl_abort();
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
    vcl_cerr<<"ERROR: mbl_matxvec_prod_mv_2() - vnl_matrix<double> is 0 x 0"<<vcl_endl;
    vcl_cerr<<"V has dimension "<<V.size()<<vcl_endl;
    vcl_abort();
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
                const vcl_vector<int>& index)
// R = MV but only use sub-set of all rows of M
{
  int nc = M.ncols();
  int nr = M.nrows();
  if (index.lo()!=1)
  {
    vcl_cerr<<"TC_MatXVec(M,V,R,index) : index array must begin at 1"<<vcl_endl;
    vcl_abort();
  }
  int n = index.size();


  if (R.size()!=n) R.resize(n);
  double *Rdata = R.dataPtr();

  int t = V.size();
  if (t>nc)
  {
    vcl_cerr<<"TC_MatXVec() R too long."<<vcl_endl;
    vcl_abort();
  }

  if (t==0)
  {
    int r = n+1;
    while (--r) Rdata[r]=0.0; // Zero R
    return;
  }

  if ((nr<1) || (nc<1))
  {
    vcl_cerr<<"TC_MatXVec - vnl_matrix<double> is 0 x 0"<<vcl_endl;
    vcl_cerr<<"V has dimension "<<V.size()<<vcl_endl;
    vcl_abort();
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


void mbl_matxvec_prod_vm(const vnl_vector<double>& V,
                         const vnl_matrix<double>& M,
                         vnl_vector<double>& R)
// R = (V'M)' = M'V
{
  R.fill(0.0);
  mbl_matxvec_add_prod_vm(V,M,R);
}


//=======================================================================


void mbl_matxvec_add_prod_vm(const vnl_vector<double>& V,
                             const vnl_matrix<double>& M,
                             vnl_vector<double>& R)
// R += M'V
{
  unsigned int nc = M.cols();
  unsigned int nr = M.rows();

#ifndef NDEBUG
  if (nr!=V.size())
  {
    vcl_cerr<<"ERROR: mbl_matxvec_add_prod_vm - V wrong length"<<vcl_endl;
    vcl_cerr<<"Expected "<<nr<<" got "<<V.size()<<vcl_endl;
    vcl_abort();
  }
#endif //!NDEBUG

  unsigned int t = R.size();
  assert(t<=nc); // R too long
  if (t==0) return;

#ifndef NDEBUG
  if ((nr<1) || (nc<1))
  {
    vcl_cerr<<"ERROR: mbl_matxvec_add_prod_vm - vnl_matrix<double> is 0 x 0"<<vcl_endl;
    vcl_cerr<<"V has dimension "<<V.size()<<vcl_endl;
    vcl_abort();
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
      vcl_cerr<<"TC_ProductABt : B.ncols != A.ncols"<<vcl_endl;
      vcl_abort() ;
   }
#endif //!NDEBUG

   if ( (ABt.nrows()!=nr1) || (ABt.ncols()!= nr2) )
    ABt.resize( nr1, nr2 ) ;

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
      vcl_cerr<<"TC_ProductAtB : B.nrows != A.nrows"<<vcl_endl;
      vcl_abort() ;
   }

   if ( (AtB.nrows()!=nc1) || (AtB.ncols()!= nc2) )
    AtB.resize( nc1, nc2 ) ;

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


//: Computes MD where D is diagonal with elememts d(i)
void TC_ProductMD(vnl_matrix<double>& MD,
                  const vnl_matrix<double>& M,
                  const vnl_vector<double>& d)
{
  int nr = M.nrows();
  int nc = M.ncols();
  if (d.size()!=nc)
  {
    vcl_cerr<<"TC_ProductMD() d doesnt match M"<<vcl_endl;
    vcl_cerr<<"d is "<<d.size()<<" element vector."<<vcl_endl;
    vcl_cerr<<"M is "<<nr<<" x "<<nc<<vcl_endl;
    vcl_abort();
  }

  if ((MD.nrows()!=nr) || (MD.ncols()!=nc)) MD.resize(nr,nc);

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

//: Computes DM where D is diagonal with elememts d(i)
void TC_ProductDM(vnl_matrix<double>& DM,
                  const vnl_matrix<double>& M,
                  const vnl_vector<double>& d)
{
  int nr = M.nrows();
  int nc = M.ncols();
  if (d.size()!=nr)
  {
    vcl_cerr<<"TC_ProductDM() d doesnt match M"<<vcl_endl;
    vcl_cerr<<"d is "<<d.size()<<" element vector."<<vcl_endl;
    vcl_cerr<<"M is "<<nr<<" x "<<nc<<vcl_endl;
    vcl_abort();
  }

  if ((DM.nrows()!=nr) || (DM.ncols()!=nc)) DM.resize(nr,nc);

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
