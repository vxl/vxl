// This is oxl/mvl/FMatrixPlanar.cxx
//:
// \file

#include <iostream>
#include <cmath>
#include "FMatrixPlanar.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_3.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

#include <mvl/FMatrix.h>

//--------------------------------------------------------------
//
//: Default constructor.

FMatrixPlanar::FMatrixPlanar()
{
  rank2_flag_ = true;
}

//--------------------------------------------------------------
//
//: Constructor.

FMatrixPlanar::FMatrixPlanar(const double* f_matrix)
{
  rank2_flag_ = true;
  set(f_matrix);
}

//--------------------------------------------------------------
//
//: Constructor.

FMatrixPlanar::FMatrixPlanar(const vnl_double_3x3& f_matrix)
{
  rank2_flag_ = true;
  set(f_matrix.data_block());
}


//--------------------------------------------------------------
//
//: Destructor.

FMatrixPlanar::~FMatrixPlanar() = default;

//--------------------------------------------------------------
//
//: Set the fundamental matrix using the two-dimensional array f_matrix.
// Only returns true if f_matrix contained a planar matrix, not an
// approximation to one. Otherwise returns false and the matrix is not set.
// Patch on FMatrixSkew::set (const vnl_double_3x3& f_matrix ).

bool FMatrixPlanar::set (const double* f_matrix )
{
  vnl_double_3x3 temp(f_matrix);
  return set(temp);
}


//--------------------------------------------------------------
//
//: Set the fundamental matrix using the vnl_double_3x3 f_matrix.
// Only returns true if f_matrix contained a planar matrix, not an
// approximation to one. The test is against a Rank 2 constraint for
// both ${\tt F}$ and the symmetric part ({\tt F}+{\tt F}^\top).
// Otherwise returns false and the matrix is not set.

bool
FMatrixPlanar::set (vnl_double_3x3 const& f_matrix )
{
  int row_index, col_index;

#ifdef PARANOID

  // CRUDE test for planar form with tolerance 0
  // test F and F+F' are Rank 2
  // HACK: has been altered to have some tolerances
  bool planar = true;
  vnl_svd<double> svd(f_matrix.as_ref(),1e-8);
  if (svd.rank()!=2)
  {
    planar = false;
    std::cerr << "WARNING in FMatrixPlanar::set\n"
             << "F matrix not rank 2: svd = " << svd.W() << std::endl;
  }
  else
  {
    vnl_svd<double> svd((f_matrix + f_matrix.transpose()).as_ref(),1e-8);
    if (svd.rank()!=2)
    {
      planar = false;
      std::cerr << "WARNING in FMatrixPlanar::set\n"
               << "Symmetric part matrix not rank 2: svd = " << svd.W() << '\n';
    }
  }

  if (!planar)
  {
    std::cerr << "WARNING: F matrix not planar so cannot allocate to FMatrixPlanar\n" ;
    return FALSE;
  }

#endif

  for (row_index = 0; row_index < 3; row_index++)
    for (col_index = 0; col_index < 3; col_index++)
    {
      f_matrix_. put (row_index, col_index,f_matrix.get(row_index,col_index));
      ft_matrix_. put (col_index, row_index,f_matrix.get(row_index,col_index));
    }

  // set rank flag true

  this->set_rank2_flag(true);

  return true;
}


//----------------------------------------------------------------
//
//: Initialises the FMatrixPlanar using a general fundamental matrix F.
// Does so by finding the nearest planar fundamental matrix to F.
// This should be used prior to FMPlanarComputeNonLinear to give
// an initial value for the non-linear minimisation.
// This function is required as trying to set FMatrixPlanar using a
// general fundamental matrix
// will fail as it does not satisfy the extra constraint of
// \f$\det ({\tt F} + {\tt F}^\top) = 0\f$.

void FMatrixPlanar::init(const FMatrix& F)
{
  // this converts to 6 parameter form of [e2]x[ls]x[e1]x - see A Zisserman
  // HACK this is not the most efficient/accurate way to convert to this form
  // as it goes via the Armstrong implementation of the
  // Lingrand Veiville formula (ECCV96).
  // This should be redone at some point.

  vgl_homg_point_2d<double> e1,e2;
  F.get_epipoles(e1,e2);

  vnl_symmetric_eigensystem<double> symm_eig((F.get_matrix()+F.get_matrix().transpose()).as_ref()); // size 3x3

  double eig0 = symm_eig.D(0,0);
  double eig1 = symm_eig.D(2,2);

  vnl_double_3 v0 = symm_eig.get_eigenvector(0);
  vnl_double_3 v1 = symm_eig.get_eigenvector(2);

  vnl_double_3 f1, f2;

  if (eig0 > 0 && eig1 < 0) {
    f1 = std::sqrt(eig0)*v0 + std::sqrt(-eig1)*v1;
    f2 = std::sqrt(eig0)*v0 - std::sqrt(-eig1)*v1;
  }
  else if (eig0 < 0 && eig1 > 0) {
    f1 = std::sqrt(eig1)*v1 + std::sqrt(-eig0)*v0;
    f2 = std::sqrt(eig1)*v1 - std::sqrt(-eig0)*v0;
  }
  else {
    std::cerr << "ERROR in FMatrix::init\n"
             << "EXITING...\n";
    assert(false);
  }

#define dot_n(a,b) (a.x()*b(0)/a.w()+a.y()*b(1)/a.w()+b(2))
  vnl_double_3 ls;
  if (std::fabs(dot_n(e1,f1))+
      std::fabs(dot_n(e2,f1)) >
      std::fabs(dot_n(e1,f2))+
      std::fabs(dot_n(e2,f2)) )
    ls = f1;
  else
    ls = f2;
#undef dot_n

  ls.normalize();

  double ls_thi = std::acos(ls[2]);
  if (ls_thi < 0) ls_thi += vnl_math::pi;

  double ls_theta;
  if (ls[1] >= 0)
    ls_theta =  std::acos(ls[0]/std::sin(ls_thi));
  else
    ls_theta = -std::acos(ls[0]/std::sin(ls_thi));

  double ls1 = std::cos(ls_theta)*std::sin(ls_thi);
  double ls2 = std::sin(ls_theta)*std::sin(ls_thi);
  double ls3 = std::cos(ls_thi);

  double list1[9] = {0,-1.0,e1.y()/e1.w(),
                     1,0,-e1.x()/e1.w(),
                     -e1.y()/e1.w(),e1.x()/e1.w(),0};
  double list2[9] = {0,-ls3,ls2,ls3,0,-ls1,-ls2,ls1,0};
  double list3[9] = {0,-1.0,e2.y()/e2.w(),
                     1,0,-e2.x()/e2.w(),
                     -e2.y()/e2.w(),e2.x()/e2.w(),0};

  vnl_double_3x3 mat1(list1),mat2(list2),mat3(list3);

  vnl_double_3x3 fmat = mat1*mat2*mat3;

  fmat /= fmat.fro_norm();

  // store the corrected fmatrix
  set(fmat);
}
