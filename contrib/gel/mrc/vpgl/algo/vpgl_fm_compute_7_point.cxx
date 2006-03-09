// This is gel/mrc/vpgl/algo/vpgl_fm_compute_7_point.cxx
#ifndef _vpgl_fm_compute_7_point_cxx_
#define _vpgl_fm_compute_7_point_cxx_
//:
// \file

#include <vnl/vnl_fwd.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_fwd.h>
#include <vgl/algo/vgl_norm_trans_2d.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

#include "vpgl_fm_compute_7_point.h"


//-------------------------------------------
bool 
vpgl_fm_compute_7_point::compute( 
  const vcl_vector< vgl_homg_point_2d<double> >& pr,
  const vcl_vector< vgl_homg_point_2d<double> >& pl,
  vcl_vector< vpgl_fundamental_matrix<double>* >& fm )
{
  // Check that there are at least 7 points.
  if( pr.size() < 7 || pl.size() < 7 ){
    vcl_cerr << "vpgl_fm_compute_7_point: Need at least 7 point pairs.\n"
    << "Number in each set: " << pr.size() << ", " << pl.size() << vcl_endl;
    return false;
  }

  // Check that the correspondence lists are the same size.
  if( pr.size() != pl.size() ){
    vcl_cerr << "vpgl_fm_compute_7_point: Need correspondence lists of same size.\n";
    return false;
  }

  // Condition if necessary.
  vcl_vector< vgl_homg_point_2d<double> > pr_norm, pl_norm;
  vgl_norm_trans_2d<double> prnt, plnt;
  if( precondition_ ){
    prnt.compute_from_points(pr);
    plnt.compute_from_points(pl);
    for( unsigned i = 0; i < pl.size(); i++ ){
      pr_norm.push_back( prnt*pr[i] );
      pl_norm.push_back( plnt*pl[i] );
    }
  }
  else{
    for( unsigned i = 0; i < pl.size(); i++ ){
      pr_norm.push_back( pr[i] );
      pl_norm.push_back( pl[i] );
    }
  }

  // Construct the design matrix from the point correspondences.
  vnl_matrix<double> design_matrix(pr_norm.size(),9);
  for( unsigned r = 0; r < pr_norm.size(); r++ ){
    design_matrix(r,0) = pr_norm[r].x()*pl_norm[r].x();
    design_matrix(r,1) = pr_norm[r].y()*pl_norm[r].x();
    design_matrix(r,2) = pr_norm[r].w()*pl_norm[r].x();
    design_matrix(r,3) = pr_norm[r].x()*pl_norm[r].y();
    design_matrix(r,4) = pr_norm[r].y()*pl_norm[r].y();
    design_matrix(r,5) = pr_norm[r].w()*pl_norm[r].y();
    design_matrix(r,6) = pr_norm[r].x()*pl_norm[r].w();
    design_matrix(r,7) = pr_norm[r].y()*pl_norm[r].w();
    design_matrix(r,8) = pr_norm[r].w()*pl_norm[r].w();
  }

  design_matrix.normalize_rows();
  vnl_svd<double> design_svd( design_matrix );
  vnl_matrix<double> W = design_svd.nullspace();

  // Take the first and second nullvectors from the nullspace
  // Since rank 2 these should be the only associated with non-zero
  // root (pr_normobably need conditioning first to be actually rank 2)
  
  vnl_double_3x3 F1(W.get_column(0).data_block());
  vnl_double_3x3 F2(W.get_column(1).data_block());

  // Using the fact that Det(alpha*F1 +(1 - alpha)*F2) == 0
  // find the real roots of the cubic equation that satisfy
  vcl_vector<double> a = get_coeffs(F1, F2);
  vcl_vector<double> roots = solve_cubic(a);
  for (unsigned int i = 0; i < roots.size(); i++) {
    vpgl_fundamental_matrix<double> F_temp( F1.as_ref()*roots[0] + F2*(1 - roots[i]) );
    if( precondition_ )
      F_temp.set_matrix( plnt.get_matrix().transpose()*F_temp.get_matrix()*prnt.get_matrix() );
    fm.push_back( new vpgl_fundamental_matrix<double>(F_temp) );
  }
  return true;
};


//------------------------------------------------
vcl_vector<double> 
vpgl_fm_compute_7_point::get_coeffs( vnl_double_3x3 const& F1, 
    vnl_double_3x3 const& F2 )
{
  // All the following computed with Mapl_norme for oxl/mvl/FMatrixCompute7Point. 
  double a=F1(0,0), j=F2(0,0), aa=a-j,
         b=F1(0,1), k=F2(0,1), bb=b-k,
         c=F1(0,2), l=F2(0,2), cc=c-l,
         d=F1(1,0), m=F2(1,0), dd=d-m,
         e=F1(1,1), n=F2(1,1), ee=e-n,
         f=F1(1,2), o=F2(1,2), ff=f-o,
         g=F1(2,0), p=F2(2,0), gg=g-p,
         h=F1(2,1), q=F2(2,1), hh=h-q,
         i=F1(2,2), r=F2(2,2), ii=i-r;

  double a1=ee*ii-ff*hh, b1=ee*r+ii*n-ff*q-hh*o, c1=r*n-o*q;
  double d1=bb*ii-cc*hh, e1=bb*r+ii*k-cc*q-hh*l, f1=r*k-l*q;
  double g1=bb*ff-cc*ee, h1=bb*o+ff*k-cc*n-ee*l, i1=o*k-l*n;

  vcl_vector<double> v;
  v.push_back(aa*a1-dd*d1+gg*g1);
  v.push_back(aa*b1+a1*j-dd*e1-d1*m+gg*h1+g1*p);
  v.push_back(aa*c1+b1*j-dd*f1-e1*m+gg*i1+h1*p);
  v.push_back(c1*j-f1*m+i1*p);

  return v;
};


//------------------------------------------------
vcl_vector<double> 
vpgl_fm_compute_7_point::solve_quadratic( vcl_vector<double> v )
{
  double a = v[1], b = v[2], c = v[3];
  double s = (b > 0.0) ? 1.0 : -1.0;
  double d = b * b - 4 * a * c;

  // round off error
  if ( d > -1e-5 && d < 0)
    d = 0.0;

  if (d < 0.0) // doesn't work for compl_normex roots
    return vcl_vector<double>(); // empty list

  double q = -0.5 * ( b + s * vcl_sqrt(d));
  vcl_vector<double> l; l.push_back(q/a); l.push_back(c/q);
  return l;
};
 

//------------------------------------------------
vcl_vector<double> 
vpgl_fm_compute_7_point::solve_cubic( vcl_vector<double> v )
{
  double a = v[0], b = v[1], c = v[2], d = v[3];
  
  // firstly check to see if we have appr_normoximately a quadratic 
  double len = a*a + b*b + c*c + d*d;
  if (vcl_abs(a*a/len) < 1e-6 )
    return solve_quadratic(v);

  b /= a; c /= a; d /= a; b /= 3;
  // With the substitution x = y-b, the equation becomes y^3-3qy+2r = 0:
  double q = b*b - c/3;
  double r = b*(b*b-c/2) + d/2;
  // At this point, a, c and d are no longer needed (c and d will be reused).

  if (q == 0) {
    vcl_vector<double> v; 
    double cbrt = (r<0) ? vcl_exp(vcl_log(-2*r)/3.0) : -vcl_exp(vcl_log(2*r)/3.0);
    v.push_back(cbrt - b); 
    return v;
  }

  // With the Vieta substitution y = z+q/z this becomes z^6+2rz^3+q^3 = 0
  // which is essentially a quadratic equation:

  d = r*r - q*q*q;
  if ( d >= 0.0 ){
    // Compute a cube root
    double z;
    if( -r + vcl_sqrt(d) >= 0 ) z = vcl_exp(vcl_log(-r + vcl_sqrt(d))/3.0);
    else z = -vcl_exp(vcl_log(-r + vcl_sqrt(d))/3.0);

    // The case z=0 is excluded since this is q==0 which is handled above
    vcl_vector<double> v; v.push_back(z + q/z - b); return v;
  }

  // And finally the "irreducible case" (with 3 solutions):
  c = vcl_sqrt(q);
  double theta = vcl_acos( r/q/c ) / 3;
  vcl_vector<double> l;
  l.push_back(-2.0*c*vcl_cos(theta)                    - b);
  l.push_back(-2.0*c*vcl_cos(theta + 2*vnl_math::pi/3) - b);
  l.push_back(-2.0*c*vcl_cos(theta - 2*vnl_math::pi/3) - b);
  return l;
};


#endif //_vpgl_fm_compute_7_point_cxx_
