//:
// \file
// \author Tomasz Malisiewicz
// \date   March 2004

#include "rgrl_est_rigid.h"

#include <vcl_cassert.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_transpose.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include "rgrl_trans_rigid.h"
#include "rgrl_match_set.h"

rgrl_est_rigid::
rgrl_est_rigid( unsigned int dimension )
{
  // only allow 2d and 3d estimation
  assert(dimension == 2 || dimension == 3);

  unsigned int param_dof = (dimension == 3)?6 : 3;

  // Pass the two variable to the parent class, where they're stored
  //
  rgrl_estimator::set_param_dof( param_dof );
}

rgrl_transformation_sptr 
rgrl_est_rigid::
estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
          rgrl_transformation const& cur_trans ) const
{

  // so we want to have some sort of state, but we dont really want to un-const-ize this method, 
  // so we implement a quick hack by making by using const_cast to change the value of stats 
  vcl_vector<vcl_vector<double> >* pp = 
    const_cast<vcl_vector<vcl_vector<double> >* >(&stats);

  // reset the stats before this new run
  pp->clear();
  vcl_vector<double> cur_stat;

  rgrl_transformation_sptr current_trans = new rgrl_trans_rigid(3);
  *current_trans = cur_trans;

  // Iterators to go over the matches
  //
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  // The dimensionality of the space we are working in. Find it by
  // looking at the dimension of one of the data points.
  //
  unsigned int m = 0;
  {
    unsigned ms = 0;
    while( ms < matches.size() &&
           matches[ms]->from_begin() == matches[ms]->from_end() ) {
      ++ms;
    }
    if( ms == matches.size() ) {
      DebugMacro( 0, "No data!\n" );
      return 0; // no data!
    }
    m = matches[ms]->from_begin().from_feature()->location().size();
  }

  // currently only 3D estimation is implemented
  assert(m==3);

  int numiterations = 1;
  do
  {

    cur_stat.clear();

  // This problem can be written as Xp=y. Then, the Weighted Least Squares solution
  // is p = inv(X^t W X)  X^t W y.
  //
  // We use all the constraints from all the match sets to develop a
  // single linear system for the rigid transformation.
  //
  vnl_matrix<double> XtWX( 6, 6 );
  vnl_vector<double> XtWy( 6 );
  XtWX.fill( 0.0 );
  XtWy.fill( 0.0 );

  unsigned count=0;
  
  vnl_vector<double> from_pt( m );
  vnl_vector<double> to_pt( m );
  for( int ms=0; ms < matches.size(); ++ms ) 
  {
    rgrl_match_set const& match_set = *matches[ms];
    for( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) 
    {
      for( TIter ti = fi.begin(); ti != fi.end(); ++ti ) 
      {
        from_pt = fi.from_feature()->location();
        to_pt = ti.to_feature()->location();
        vnl_matrix<double> const& B = ti.to_feature()->error_projector();

        double const wgt = ti.cumulative_weight();

        assert( from_pt.size() == m );
        assert( to_pt.size() == m );
        assert( B.cols() == m && B.rows() == m );
        ++count;

        // form XX matrix and RR vector
        vnl_matrix<double> XX(3,6,0.0);
        vnl_vector<double> RR(3,0.0);

        vnl_vector<double> g(current_trans->map_location(from_pt));
        vnl_vector<double> f(to_pt);

        XX(0,0)=0;      XX(0,1)=g[2];   XX(0,2)=-g[1];  XX(0,3)=1; 
        XX(1,0)=-g[2];  XX(1,1)=0;      XX(1,2)=g[0];   XX(1,4)=1;
        XX(2,0)=g[1];   XX(2,1)=-g[0];  XX(2,2)=0;      XX(2,5)=1;

        RR=f-g;

        XtWX+=XX.transpose()*B*XX*wgt;
        XtWy+=XX.transpose()*B*RR*wgt;

        //There are some differences in here from Amitha's implementation; however the math is equivalent since
        //the error projector is idempotent(B^2=B) and symmetric(B^T=B) (is this always true?)

      }  
    }
  }


  // ----------------------------
  // Compute the solution

  vnl_svd<double> svd( XtWX );

  // Due to floating point inaccuracies, some zero singular values may
  // look non-zero, so we correct for that.
  svd.zero_out_relative();

  // Exit if not full rank
  if( (unsigned)svd.rank() < m ) {
    DebugMacro(1, "rank ("<<svd.rank()<<") < "<<m<<"; no solution." );
    DebugMacro_abv(1, "(used " << count << " correspondences)\n" );
    return 0; // no solution
  }

  // Compute the solution into XtWy
  //
  vnl_matrix<double> covar = svd.inverse();
  XtWy.pre_multiply( covar );

  // Copy the solution into the result variables, and construct a
  // transformation object.

  // Translation component
  vnl_vector<double> trans( m );
  trans[0]=XtWy[3];
  trans[1]=XtWy[4];
  trans[2]=XtWy[5];

  //vcl_cerr<<"Estimated parameter vector in raw form is \n"<<XtWy<<vcl_endl;

  // Matrix component
  vnl_matrix<double> R( m, m );
  R(0,0)=1.0;      R(0,1)=-XtWy[2]; R(0,2)=XtWy[1];
  R(1,0)=XtWy[2];  R(1,1)=1.0;      R(1,2)=-XtWy[0];
  R(2,0)=-XtWy[1]; R(2,1)=XtWy[0];  R(2,2)=1.0;
  
  // Force the estimated "rotation" matrix to be a rotation matrix.
  //

  cur_stat.push_back(vnl_determinant(R));
  DebugMacro_abv(2, "about to orthonormalize with determinant "<<vnl_determinant(R)<<vcl_endl;)
  vnl_svd<double> svdR( R );

  // Set singular values to unity
  double scale = 1;
  svdR.W(0) = scale;
  svdR.W(1) = scale;
  svdR.W(2) = scale;
  R = svdR.recompose();

  // The new estimate is incremental over the old one. 
  rgrl_trans_rigid* old_sim = dynamic_cast<rgrl_trans_rigid* >( current_trans.as_pointer() );
  double fro_norm2 = trans.magnitude();
  trans += R * old_sim->t();

  vnl_matrix<double> I(3,3,vnl_matrix_identity);
  I -= R;

  double fro_norm1 = I.frobenius_norm();

  //vcl_cerr<<"fro_norm of deltaT,deltatrans is "<<fro_norm1<<","<<fro_norm2<<vcl_endl;

  cur_stat.push_back(fro_norm1);
  cur_stat.push_back(fro_norm2);

  // The new rotation is the old rotation followed by our
  // estimated incremental rotation.
  //
  R *= old_sim->R();

  current_trans = new rgrl_trans_rigid(R,trans,covar);

  // push back statistics before quitting
  pp->push_back(cur_stat);

  // this is the termination criterion: it is based on the fro_norm of the current iteration's
  // estimate of the rotation and the translation.  Iterations stop when new rotatations and new translations
  // become negligible
  //if (fro_norm1<1e-4 && fro_norm2<1e-4)
  if (fro_norm1<1e-2 && fro_norm2<1e-1)
  {
    //std::cerr<<"----------- done rgrl_est_rigid w/ "<<numiterations<<" iterations----"<<std::endl;
    break;
  }
  //else
  //{
    //std::cerr<<"forbenius norms are: "<<fro_norm1<<" and "<<fro_norm2<<std::endl;
  //}

  numiterations++;

  } while(1);

  // at this point we should calculate the covariance matrix
  vnl_matrix<double> covar(3,3,vnl_matrix_identity);

  // TJM: turn this off for now since it is fairly computationally intensive
  //determine_covariance(matches,current_trans);

  return current_trans;

  //return new rgrl_trans_rigid(R,t,covar);
}

//: Determine the covariance matrix of this rigid xform given the matches
void rgrl_est_rigid::determine_covariance( rgrl_set_of<rgrl_match_set_sptr> const& matches, rgrl_transformation_sptr current_trans) const
{

  // first, we have to extract the angles from our linearized rotation matrix
  double alpha,theta,phi;

  rgrl_trans_rigid* tttt = dynamic_cast<rgrl_trans_rigid*>(current_trans.ptr());

  assert(tttt);
  tttt->determine_angles(phi,alpha,theta);
  //determine_angles(tttt->R(),phi,alpha,theta);
  vnl_vector<double> t = tttt->t();


  vnl_matrix<double> Rphi(3,3,0.0);
  vnl_matrix<double> Ralpha(3,3,0.0);
  vnl_matrix<double> Rtheta(3,3,0.0);
  vnl_matrix<double> Rphid(3,3,0.0);
  vnl_matrix<double> Ralphad(3,3,0.0);
  vnl_matrix<double> Rthetad(3,3,0.0);

  vnl_matrix<double> Rphidd(3,3,0.0);
  vnl_matrix<double> Ralphadd(3,3,0.0);
  vnl_matrix<double> Rthetadd(3,3,0.0);


  Rtheta(0,0) = cos(theta);
  Rtheta(0,1) = -sin(theta);
  Rtheta(1,0) = sin(theta);
  Rtheta(1,1) = cos(theta);
  Rtheta(2,2) = 1;

  Ralpha(0,0) = cos(alpha);
  Ralpha(0,2) = sin(alpha);
  Ralpha(1,1) = 1;
  Ralpha(2,0) = -sin(alpha);
  Ralpha(2,2) = cos(alpha);

  Rphi(0,0) = 1;
  Rphi(1,1) = cos(phi);
  Rphi(1,2) = -sin(phi);
  Rphi(2,1) = sin(phi);
  Rphi(2,2) = cos(phi);

  // derivative matrices now
  Rthetad(0,0) = -sin(theta);
  Rthetad(0,1) = -cos(theta);
  Rthetad(1,0) = cos(theta);
  Rthetad(1,1) = -sin(theta);

  Ralphad(0,0) = -sin(alpha);
  Ralphad(0,2) = cos(alpha);
  Ralphad(2,0) = -cos(alpha);  
  Ralphad(2,2) = -sin(alpha);

  Rphid(1,1) = -sin(phi);
  Rphid(1,2) = -cos(phi);
  Rphid(2,1) = cos(phi);
  Rphid(2,2) = -sin(phi);

  //second derivative matrices
  Rthetadd(0,0) = -cos(theta);
  Rthetadd(0,1) = sin(theta);
  Rthetadd(1,0) = -sin(theta);
  Rthetadd(1,1) = -cos(theta);

  Ralphadd(0,0) = -cos(alpha);
  Ralphadd(0,2) = -sin(alpha);
  Ralphadd(2,0) = sin(alpha); 
  Ralphadd(2,2) = -cos(alpha);

  Rphidd(1,1) = -cos(phi);
  Rphidd(1,2) = sin(phi);
  Rphidd(2,1) = -sin(phi);
  Rphidd(2,2) = -cos(phi);


  // now the entire rotation matrices
  vnl_matrix<double> R = Rphi * Ralpha * Rtheta;
  vnl_matrix<double> dRdphi  = Rphid * Ralpha  * Rtheta;
  vnl_matrix<double> dRdalpha = Rphi  * Ralphad * Rtheta;
  vnl_matrix<double> dRdtheta = Rphi  * Ralpha  * Rthetad;
  
  vnl_matrix<double> d2Rdphidtheta   = Rphid * Ralpha  * Rthetad;
  vnl_matrix<double> d2Rdphidalpha   = Rphid * Ralphad * Rtheta;
  vnl_matrix<double> d2Rdthetadalpha = Rphi  * Ralphad * Rthetad;


  vnl_matrix<double> d2Rdphi2   = Rphidd * Ralpha   * Rtheta;
  vnl_matrix<double> d2Rdalpha2 = Rphi   * Ralphadd * Rtheta;
  vnl_matrix<double> d2Rdtheta2 = Rphi   * Ralpha   * Rthetadd;


  // we now calculate the hessian, then invert it to get our approximate covariance

  vnl_matrix<double> Htt(3,3,0.0);
  vnl_matrix<double> Hoo(3,3,0.0);
  vnl_matrix<double> Hot(3,3,0.0);

  const unsigned m = 3;

  vnl_vector<double> from_pt( m );
  vnl_vector<double> to_pt( m );

    typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;


  for( int ms=0; ms < matches.size(); ++ms ) 
  {
    rgrl_match_set const& match_set = *matches[ms];
    for( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) 
    {
      for( TIter ti = fi.begin(); ti != fi.end(); ++ti ) 
      {
        from_pt = fi.from_feature()->location();
        to_pt = ti.to_feature()->location();
        vnl_matrix<double> const& B = ti.to_feature()->error_projector();
        double const wgt = ti.cumulative_weight();

        Htt = Htt + wgt*2*B; 
        Hot.set_row(0,Hot.get_row(0) + wgt*2*(B*dRdtheta*from_pt));
        Hot.set_row(1,Hot.get_row(1) + wgt*2*(B*dRdalpha*from_pt));
        Hot.set_row(2,Hot.get_row(2) + wgt*2*(B*dRdphi*from_pt));


        Hoo(0,0) = Hoo(0,0) + wgt*2* ( 
          inner_product ((t - to_pt), B * d2Rdtheta2 * from_pt) +
          inner_product (from_pt , ( dRdtheta.transpose() * B * dRdtheta + R.transpose()*B*d2Rdtheta2) * from_pt ));           

        Hoo(1,1) = Hoo(1,1) + wgt*2* ( 
          inner_product ((t - to_pt), B * d2Rdalpha2 * from_pt) +
          inner_product (from_pt , ( dRdalpha.transpose() * B * dRdalpha + R.transpose()*B*d2Rdalpha2) * from_pt ));           

        Hoo(2,2) = Hoo(2,2) + wgt*2* ( 
          inner_product ((t - to_pt), B * d2Rdphi2 * from_pt) +
          inner_product (from_pt , ( dRdphi.transpose() * B * dRdphi+ R.transpose()*B*d2Rdphi2) * from_pt ));           

        Hoo(0,1) = Hoo(0,1) + wgt*2* ( 
          inner_product ((t - to_pt), B * d2Rdthetadalpha * from_pt) +
          inner_product (from_pt , ( dRdtheta.transpose() * B * dRdalpha + R.transpose()*B*d2Rdthetadalpha) * from_pt ));           


        Hoo(0,2) = Hoo(0,2) + wgt*2* ( 
          inner_product ((t - to_pt), B * d2Rdphidtheta * from_pt) +
          inner_product (from_pt , ( dRdtheta.transpose() * B * dRdphi + R.transpose()*B*d2Rdphidtheta) * from_pt ));           

        Hoo(1,2) = Hoo(1,2) + wgt*2* ( 
          inner_product ((t - to_pt), B * d2Rdphidalpha * from_pt) +
          inner_product (from_pt , ( dRdalpha.transpose() * B * dRdphi + R.transpose()*B*d2Rdphidalpha) * from_pt ));           


      }

    }
  }

  Hoo(1,0) = Hoo(0,1);
  Hoo(2,0) = Hoo(0,2);
  Hoo(2,1) = Hoo(1,2);

  vnl_matrix<double> hessian(6,6,0.0);

  //std::cout<<"hoo is \n"<<Hoo<<std::endl;
  //std::cout<<"hot is \n"<<Hot<<std::endl;
  //std::cout<<"htt is \n"<<Htt<<std::endl;
  hessian.update(Hoo,0,0);
  hessian.update(Htt,3,3);
  hessian.update(Hot,0,3);
  hessian.update(Hot.transpose(),3,0);
  

  //vnl_matrix<double> covar = vnl_matrix_inverse<double>(hessian);

  vnl_svd<double> svd(hessian);
  //svd.zero_out_absolute(10e-8);
  vnl_matrix<double> covar = svd.inverse();

  rgrl_trans_rigid* rigid = dynamic_cast<rgrl_trans_rigid*>(current_trans.ptr());

  current_trans = new rgrl_trans_rigid(rigid->R(),rigid->t(),covar);
}


rgrl_transformation_sptr
rgrl_est_rigid::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  // use base class implementation
  return rgrl_estimator::estimate( matches, cur_transform );
}

const vcl_type_info&
rgrl_est_rigid::
transformation_type() const
{
  return rgrl_trans_rigid::type_id();
}
