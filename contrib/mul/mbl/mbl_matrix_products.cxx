//:
// \file
// \author Tim Cootes
// \date 25-Apr-2001
// \brief Various specialised versions of matrix product operations

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <mbl/mbl_matxvec.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_iostream.h>

//=======================================================================
//: Compute product AB = A * B
//=======================================================================
void mbl_matrix_product(vnl_matrix<double>& AB, const vnl_matrix<double>& A,
                        const vnl_matrix<double>& B)
{
   int nr1 = A.rows();
   int nc1 = A.cols();
   int nr2 = B.rows();
   int nc2 = B.cols();

   if( nr2 != nc1 )
   {
      vcl_cerr<<"Product : B.rows != A.cols"<<vcl_endl;
      vcl_abort() ;
   }

   if ( (AB.rows()!=nr1) || (AB.cols()!= nc2) )
    AB.resize( nr1, nc2 ) ;

  const double *const * AData = A.data_array();
  const double *const * BData = B.data_array();
  double ** RData = AB.data_array();

  // Zero the elements of AB
  AB.fill(0);

  for(int r=0; r < nr1; ++r)
  {
    const double* A_row = AData[r];
    double* R_row = RData[r]-1;
    for(int c=0; c < nc1 ; ++c )
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
  int nr1 = A.rows();
  int nc1 = A.columns();
  int nr2 = B.rows();
  int nc2 = B.columns();

#ifndef NDEBUG
  if( nc2 != nc1 )
  {
    vcl_cerr<<"mbl_matrix_product_a_bt : B.columns != A.columns"<<vcl_endl;
    vcl_abort();
  }
#endif //!NDEBUG

  if ( (ABt.rows()!=nr1) || (ABt.columns()!= nr2) )
    ABt.resize( nr1, nr2 ) ;

  double const *const * A_data = A.data_array();
  double const *const * B_data = B.data_array();
  double ** R_data = ABt.data_array();

  for (int r=0;r<nr1;++r)
  {
    const double* A_row = A_data[r];
    double* R_row = R_data[r];
    for (int c=0;c<nr2;++c)
    {
      const double* B_row = B_data[c];
      R_row[c] = vnl_c_vector<double>::dot_product(A_row,B_row,nc1);
    }
  }
}

//=======================================================================
//: Compute product ABt = A * B.transpose()
//=======================================================================
void mbl_matrix_product_at_b(vnl_matrix<double>& AtB,
                   const vnl_matrix<double>& A,
                   const vnl_matrix<double>& B)
{
  int nr1 = A.rows();
  int nc1 = A.columns();
  int nr2 = B.rows();
  int nc2 = B.columns();

  if( nr2 != nr1 )
  {
    vcl_cerr<<"TC_ProductAtB : B.rows != A.rows"<<vcl_endl;
    vcl_abort();
  }

  if ( (AtB.rows()!=nc1) || (AtB.columns()!= nc2) )
    AtB.resize( nc1, nc2 ) ;

  double const *const * A_data = A.data_array();
  double const *const * B_data = B.data_array();
  double ** R_data = AtB.data_array();

  AtB.fill(0);

  for (int r1 = 0; r1<nr1; ++r1)
  {
    const double* A_row = A_data[r1];
    const double* B_row = B_data[r1];
    double a;
    int c1 =  nc1;
    while (c1)
    {
      c1--;
      double *R_row = R_data[c1];
      a = A_row[c1];
      int c2 = nc2;
      while (c2)
      {
        c2--;
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
  int nr1 = A.rows();
  int nc1 = A.cols();
  int nr2 = B.rows();
  int nc2 = B.cols();

  assert ( nr2 == nc1 ); //Product : B.nrows != A.ncols

  assert ( nr2 == d.size() ); // Product : d.nelems != A.ncols

  if ( (ADB.rows()!=nr1) || (ADB.cols()!= nc2) )
    ADB.resize( nr1, nc2 ) ;

  const double * const* AData = A.data_array();
  const double * const* BData = B.data_array();
  const double *  d_data = d.data_block();
  double ** ADBdata = ADB.data_array();

  ADB.fill(0);

  for(int r=0; r < nr1; ++r)
  {
    const double* A_row = AData[r];
    double* ADB_row = ADBdata[r];
    for(int c=0; c < nc1 ; ++c )
    {
      double ad = A_row[c] * d_data[c];
      if (ad==0.0) continue;

      const double* B_row = BData[c];
      int i = nc2;
      while (i)
      {
        --i;
        ADB_row[i] += ad * B_row[i];
      }
    }
  }
}
