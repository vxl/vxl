//:
// \file
// \author Tim Cootes
// \date 25-Apr-2001
// \brief Various specialised versions of matrix product operations

#include "mbl_matrix_products.h"
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_iostream.h>

//=======================================================================
//: Compute product AB = A * B
//=======================================================================
void mbl_matrix_product(vnl_matrix<double>& AB, const vnl_matrix<double>& A,
                        const vnl_matrix<double>& B)
{
   unsigned int nr1 = A.rows();
   unsigned int nc1 = A.cols();
   unsigned int nr2 = B.rows();
   unsigned int nc2 = B.cols();

   if ( nr2 != nc1 )
   {
      vcl_cerr<<"Product : B.rows != A.cols\n";
      vcl_abort() ;
   }

   if ( (AB.rows()!=nr1) || (AB.cols()!= nc2) )
    AB.set_size( nr1, nc2 ) ;

  const double *const * AData = A.data_array();
  const double *const * BData = B.data_array();
  double ** RData = AB.data_array();

  // Zero the elements of AB
  AB.fill(0);

  for (unsigned int r=0; r < nr1; ++r)
  {
    const double* A_row = AData[r];
    double* R_row = RData[r]-1;
    for (unsigned int c=0; c < nc1 ; ++c )
    {
      double a = A_row[c];
      if (a==0.0) continue;

      const double* B_row = BData[c]-1;
      int i = nc2+1;
      while (--i)
      {
        R_row[i] += a * B_row[i];
      }
    }
  }
}

//=======================================================================
//: Compute product ABt = A * B.transpose()
//=======================================================================
void mbl_matrix_product_a_bt(vnl_matrix<double>& ABt,
                             const vnl_matrix<double>& A,
                             const vnl_matrix<double>& B)
{
  int nc1 = A.columns();
  int nc2 = B.columns();
#ifndef NDEBUG
  if ( nc2 != nc1 )
  {
    vcl_cerr<<"mbl_matrix_product_a_bt : B.columns != A.columns\n";
    vcl_abort();
  }
#endif //!NDEBUG

  mbl_matrix_product_a_bt(ABt,A,B,nc1);
}

//=======================================================================
//: Compute product ABt = A * B.transpose(), using only nc cols of A and B
//=======================================================================
void mbl_matrix_product_a_bt(vnl_matrix<double>& ABt,
                             const vnl_matrix<double>& A,
                             const vnl_matrix<double>& B,
                             int nc)
{
  unsigned int nr1 = A.rows();
  unsigned int nr2 = B.rows();

  assert(A.columns()>=(unsigned int)nc);
  assert(B.columns()>=(unsigned int)nc);

  if ( (ABt.rows()!=nr1) || (ABt.columns()!= nr2) )
    ABt.set_size( nr1, nr2 ) ;

  double const *const * A_data = A.data_array();
  double const *const * B_data = B.data_array();
  double ** R_data = ABt.data_array();

  for (unsigned int r=0;r<nr1;++r)
  {
    const double* A_row = A_data[r];
    double* R_row = R_data[r];
    for (unsigned int c=0;c<nr2;++c)
    {
      const double* B_row = B_data[c];
      R_row[c] = vnl_c_vector<double>::dot_product(A_row,B_row,nc);
    }
  }
}

//=======================================================================
//: Compute product AtB = A.transpose() * B
//=======================================================================
void mbl_matrix_product_at_b(vnl_matrix<double>& AtB,
                             const vnl_matrix<double>& A,
                             const vnl_matrix<double>& B)
{
  mbl_matrix_product_at_b(AtB,A,B,A.columns());
}

//=======================================================================
//: Compute product AtB = A.transpose() * B, using nc_a cols of A
//=======================================================================
void mbl_matrix_product_at_b(vnl_matrix<double>& AtB,
                             const vnl_matrix<double>& A,
                             const vnl_matrix<double>& B,
                             int nc_a)
{
  assert(nc_a >= 0 && A.columns()>=(unsigned int)nc_a);
  unsigned int nr1 = A.rows();
  unsigned int nr2 = B.rows();
  unsigned int nc2 = B.columns();

  if ( nr2 != nr1 )
  {
    vcl_cerr<<"TC_ProductAtB : B.rows != A.rows\n";
    vcl_abort();
  }

  if ( (AtB.rows()!=(unsigned int)nc_a) || (AtB.columns()!= nc2) )
    AtB.set_size( nc_a, nc2 ) ;

  double const *const * A_data = A.data_array();
  double const *const * B_data = B.data_array();
  double ** R_data = AtB.data_array()-1;

  AtB.fill(0);

  for (unsigned int r1 = 0; r1<nr1; ++r1)
  {
    const double* A_row = A_data[r1]-1;
    const double* B_row = B_data[r1]-1;
    double a;
    int c1 =  nc_a+1;
    while (--c1)
    {
      double *R_row = R_data[c1]-1;
      a = A_row[c1];
      int c2 = nc2+1;
      while (--c2)
      {
         R_row[c2] +=a*B_row[c2];
      }
    }
  }
}

//: Returns ADB = A * D * B
//  where D is diagonal with elements d
void mbl_matrix_product_adb(vnl_matrix<double>& ADB,
                            const vnl_matrix<double>& A,
                            const vnl_vector<double>& d,
                            const vnl_matrix<double>& B)
{
  unsigned int nr1 = A.rows();
  unsigned int nc1 = A.cols();
  unsigned int nr2 = B.rows();
  unsigned int nc2 = B.cols();

  assert ( nr2 == nc1 ); //Product : B.nrows != A.ncols

  assert ( nr2 == d.size() ); // Product : d.nelems != A.ncols

  if ( (ADB.rows()!=nr1) || (ADB.cols()!= nc2) )
    ADB.set_size( nr1, nc2 ) ;

  const double * const* AData = A.data_array();
  const double * const* BData = B.data_array();
  const double *  d_data = d.data_block();
  double ** ADBdata = ADB.data_array();

  ADB.fill(0);

  for (unsigned int r=0; r < nr1; ++r)
  {
    const double* A_row = AData[r];
    double* ADB_row = ADBdata[r]-1;
    for (unsigned int c=0; c < nc1 ; ++c )
    {
      double ad = A_row[c] * d_data[c];
      if (ad==0.0) continue;

      const double* B_row = BData[c]-1;
      int i = nc2+1;
      while (--i)
      {
        ADB_row[i] += ad * B_row[i];
      }
    }
  }
}
