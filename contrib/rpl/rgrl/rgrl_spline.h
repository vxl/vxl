#ifndef rgrl_spline_h_
#define rgrl_spline_h_

//: \file
//  \author Lee, Ying-Lin (Bess)
//  \date   Sep 2003
//  \brief A class for dealing a uniform cubic B-spline up to 4D (a hypersurface in 4D).

#include "rgrl_spline_sptr.h"
#include "rgrl_object.h"

#include <vcl_iosfwd.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

//: A tensor-product B-spline in 4D can be written in
//  \f[
//      \mathbf{Q}_{ijk}(u,v,w) = \sum_{r=0}^{3} \sum_{s=0}^{3} \sum_{t=0}^{3}V_{i+r,j+s,k+t} b_r(u) b_s(v) b_t(w)
//  \f]
//  where
// \verbatim
// i=\ceil{x}-1,  i=\ceil{y}-1, i=\ceil{z}-1,
// \endverbatim
//  and \f$u=x-i\f$,\f$v=y-j\f$\f$w=z-k\f$ are local parameters ranging in $[0,1)$
//  and
//  \f[
//    b_{3}(u) = \frac{1}{6}u^3
//  \f]
//  \f[
//    b_{2}(u) = \frac{1}{6}(-3u^3+3u^2+3u+1)
//  \f]
//  \f[
//    b_{1}(u) = \frac{1}{6}(3u^3-6u^2+4)
//  \f]
//  \f[
//    b_{0}(u) = \frac{1}{6}(1-u)^3
//  \f]
//  or can be expressed in
//  \f[
//      \mathbf{Q}_(x,y,z)=
//      \sum_{i=-1}^{m+1} \sum_{j=-1}^{n+1}  \sum_{k=-1}^{l+1}V_{ijk} B_i(x) B_j(y) B_k(z)
//  \f]
// The control vertex \f$V_{0,0,0}\f$ is located at \f$(0,0,0)\f$.

class rgrl_spline
  : public rgrl_object
{
 public:
  //: Constructor
  rgrl_spline( ) : rgrl_object() { }
  rgrl_spline( vnl_vector< unsigned > const& m );
  rgrl_spline( vnl_vector< unsigned > const& m, vnl_vector< double > const& c );

//    rgrl_spline( vnl_vector<double> const& delta,
//                 vnl_vector<double> const& x0,
//                 vnl_vector<double> const& x1 );
//    rgrl_spline( vnl_vector<int> const& m,
//                    vnl_vector<double> const& delta,
//                    vnl_vector<double> const& p0);
//    rgrl_spline( vnl_vector<double> const& c,
//                 vnl_vector<double> const& delta,
//                 vnl_vector<double> const& x0,
//                 vnl_vector<double> const& x1 );
//    rgrl_spline( vnl_vector<double> const& c,
//                    vnl_vector<int> const& m,
//                    vnl_vector<double> const& delta,
//                    vnl_vector<double> const& p0 );

  //: Destructor.
  ~rgrl_spline(){}

  //: Set controls points
  void set_control_points( vnl_vector<double> const& c ) ;
  vnl_vector< double > const& get_control_points() const { return c_; }
  vnl_vector< double > & get_control_points() { return c_; }

  //: The spline value
  //
  //  \f[ f(\overlin{u},y,z) = \sum_{i=-1}^{m[0]_+1}
  //  \sum_{j=-1}^{m[1]_+1} \sum_{k=-1}^{m_[2]+1} V_{ijk} * B_i(x) *
  //  B_j(y) * B_k(z)\f]
  double f_x( vnl_vector<double> const& x ) const;
  vnl_vector< double > jacobian( vnl_vector< double > const& x ) const;

  //: The basis responses at \f$(x,y,z)\f$.
  //
  //  It returns a 1 by \f$(m_[0]+3)*(m_[1]+3)*(m_[2]+3)\f$ vector which contains
  //  [$B_{-1}(x)*B_{-1}(y)*B_{-1}(z)$,...,
  //  $B_{m_[0]+1}(x)B_{m_[1]+1}(y)B_{m_[2]+1}(z)$],
  //  in the same order as in c_
  void basis_response( vnl_vector<double> const& point, vnl_vector<double>& br ) const;

  void thin_plate_regularization( vnl_matrix<double>& regularization ) const;

  unsigned num_of_control_points( ) const { return c_.size(); }

  // for tester to access its private members
  friend class test_rgrl_spline;

  // for output
  friend vcl_ostream& operator<< (vcl_ostream& os, rgrl_spline const& spline );

  // for input
  friend vcl_istream& operator>> (vcl_istream& is, rgrl_spline& spline );


  //: Generate a refined B-spline that produces the same surface.
  rgrl_spline_sptr refinement( vnl_vector< unsigned > const& new_m ) const;

//    //: True if the i-th control point has support on pt
//    bool is_support( vnl_vector< double > const& pt, unsigned i );

  // Defines type-related functions
  rgrl_type_macro( rgrl_spline, rgrl_object );

 private:

  double element_1d_thin_plate( unsigned i, unsigned j ) const;
  double element_2d_thin_plate( unsigned i, unsigned j ) const;
  double element_3d_thin_plate( unsigned i, unsigned j ) const;

  typedef double (*func_type)( int i, double u );
  void basis_response_helper( vnl_vector<double> const& point, vnl_vector<double>& br, func_type func ) const;

  // The control vertices indices are from -1 to \f$m_[i]+1\f$.
  // The total number of control vertices in each dimension is \f$m_[i]+3\f$
  vnl_vector< unsigned > m_;

  // The control points in the order of
  // $c_{-1,-1,-1}$,..., $c_{m_[0]+1,-1,-1}$
  // $c_{-1,0, -1}$,..., $c_{m_[0]+1,0,-1}$,..., $c_{m_[0]+1,m_[1]+1,-1}$
  // ... $c_{m_[0]+1, m_[1]+1, m_[2]+1}$
  // Gehua:
  // In general The control point (i, j, k) is converted to
  // (i+1) + (m_[0]+3)(j+1) + (m_[0]+3)(m_[1]+3)(k+1)
  // i+1 is to shift the starting point from -1 to 0
  vnl_vector<double> c_;


//    // to specify $\delta x$, $\delta y$, and $\delta z$ for uniform spline
//    vnl_vector<double> delta_;
//    vnl_vector<double> p0_;
};

#endif
