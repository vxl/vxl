//:
// \file
// \author Lee, Ying-Lin (Bess)
// \date   Sept 2003

#include "rgrl_est_spline.h"
#include "rgrl_spline.h"
#include "rgrl_match_set.h"
#include "rgrl_trans_spline.h"
#include "rgrl_cast.h"
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/algo/vnl_conjugate_gradient.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_lbfgs.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vcl_cassert.h>

namespace{
  // for Levenberg Marquardt
  struct spline_least_squares_func : public vnl_least_squares_function
  {
    spline_least_squares_func( rgrl_spline_sptr spline,
                               vcl_vector< vnl_vector< double > > const& pts,
                               vnl_diag_matrix<double> const& wgt,    // ( num of residuals ) x ( num of residuals )
                               vnl_vector<double> const& displacement, // ( num of residuals ) x 1
                               vcl_vector<unsigned> const& free_control_pt_index )
        : vnl_least_squares_function( free_control_pt_index.size(), pts.size(), use_gradient ),
                                //number of unknowns, number of residuals, has gradient function or not
                                spline_( spline ),
                                pts_( pts ), wgt_( wgt ), displacement_( displacement ),
                                free_control_pt_index_( free_control_pt_index )
    {
      assert( pts.size() == wgt.rows() );
      assert( displacement.size() == wgt.rows() );
    }

    // x is the parameters
    void f( vnl_vector< double > const& x, vnl_vector< double > & fx )
    {
      // x is the dof-reduced parameters. Convert it back to control points
      assert( x.size() == free_control_pt_index_.size() );
      vnl_vector< double > c( spline_->num_of_control_points(), 0.0 );
      for ( unsigned i=0; i< free_control_pt_index_.size(); ++i )
        c[ free_control_pt_index_[i] ] = x[ i ];

      spline_->set_control_points( c );
      // check the number of residuals
      assert( fx.size() == pts_.size() );
      for ( unsigned i = 0; i < pts_.size(); ++i ) {
        fx[ i ] = ( displacement_[ i ] - spline_->f_x( pts_[ i ] ) )
          * vcl_sqrt( wgt_[ i ] );
      }
    }

    // x is the parameters
    void gradf( vnl_vector< double > const& x, vnl_matrix< double > & jacobian )
    {
      assert( x.size() == free_control_pt_index_.size() );
      vnl_vector< double > c( spline_->num_of_control_points(), 0.0 );
      for ( unsigned i=0; i< free_control_pt_index_.size(); ++i )
        c[ free_control_pt_index_[i] ] = x[ i ];

      spline_->set_control_points( c );
      vnl_vector< double > gr;
      for ( unsigned i = 0; i < pts_.size(); ++i ) {
        spline_->basis_response( pts_[i], gr );
        for ( unsigned j = 0; j < x.size(); ++j )
          jacobian[ i ][ j ] = - gr[ free_control_pt_index_[j] ]  * vcl_sqrt( wgt_[ i ] );
      }
    }

   private:
    rgrl_spline_sptr spline_;
    vcl_vector< vnl_vector< double > >  pts_;
    vnl_diag_matrix< double > wgt_;
    vnl_vector< double > displacement_;
    vcl_vector< unsigned > free_control_pt_index_;
  };

  // for Conjugate Gradient and other optimizers
  struct spline_cost_function : public vnl_cost_function
  {
    spline_cost_function( rgrl_spline_sptr spline,
                          vcl_vector< vnl_vector< double > >  pts,
                          vnl_diag_matrix<double> wgt,    // ( num of residuals ) x ( num of residuals )
                          vnl_vector<double> displacement ) // ( num of residuals ) x 1
      : vnl_cost_function( spline->num_of_control_points() ),  //number of unknowns
                           spline_( spline ),
                           pts_( pts ), wgt_( wgt ), displacement_( displacement )
    {
      assert( pts.size() == wgt.rows() );
      assert( displacement.size() == wgt.rows() );
    }

    // x is the parameters
    double f( vnl_vector< double > const& x )
    {
      double fx = 0;
      spline_->set_control_points( x );
      for ( unsigned i = 0; i < pts_.size(); ++i ) {
        fx += vnl_math_sqr( displacement_[ i ] - spline_->f_x( pts_[ i ] ) )
          * wgt_[ i ] ;
      }
      return fx;
    }

    void gradf (vnl_vector< double > const &x, vnl_vector< double > &gradient )
    {
      gradient.fill( 0.0 );
      vnl_vector< double > gr;
      for ( unsigned i=0; i<pts_.size(); ++i ) {
        spline_->basis_response( pts_[i], gr );
        for ( unsigned j=0; j<spline_->num_of_control_points(); ++j ) {
          gradient[ j ] -= 2 * ( displacement_[i] - spline_->f_x( pts_[i] ) ) * wgt_[i]* gr[j];
        }
      }
    }

   private:
    rgrl_spline_sptr spline_;
    vcl_vector< vnl_vector< double > > pts_;
    vnl_diag_matrix< double > wgt_;
    vnl_vector< double > displacement_;
  };
} // namespace

rgrl_est_spline::
rgrl_est_spline( unsigned dof,
                 rgrl_mask_box const& roi, vnl_vector<double> const& delta,
                 vnl_vector< unsigned > const& m,
                 bool use_thin_plate, double lambda )
    : rgrl_estimator( dof ),
      roi_(roi), delta_(delta),
      m_( m ),
      use_thin_plate_( use_thin_plate ),
      lambda_(lambda),
      optimize_method_( RGRL_LEVENBERG_MARQUARDT ),
      global_xform_( 0 )
{
  unsigned num_control = 1;
  for ( unsigned i=0; i<m.size(); ++i )
    num_control *= m[i] + 3;

  assert( num_control == dof );
}

rgrl_est_spline::
rgrl_est_spline( unsigned dof,
                 rgrl_transformation_sptr global_xform,
                 rgrl_mask_box const& roi, vnl_vector<double> const& delta,
                 vnl_vector< unsigned > const& m,
                 bool use_thin_plate, double lambda )
  : rgrl_estimator( dof ),
    roi_(roi), delta_(delta),
    m_( m ),
    use_thin_plate_( use_thin_plate ),
    lambda_(lambda),
    optimize_method_( RGRL_LEVENBERG_MARQUARDT ),
    global_xform_( global_xform )
{
  unsigned num_control = 1;
  for ( unsigned i=0; i<m.size(); ++i )
    num_control *= m[i] + 3;

  vcl_cerr << "rgrl_est_spline.cxx : number of control points: " << num_control << ", dof=" << dof << vcl_endl;
  assert( num_control == dof );
}

void
rgrl_est_spline::
point_in_knots( vnl_vector< double > const& point, vnl_vector< double > & spline_pt ) const
{
  spline_pt.set_size( point.size() );
  for ( unsigned i = 0; i < point.size(); ++i ) {
    spline_pt[ i ] = point[ i ] / delta_[ i ];
  }
}

rgrl_transformation_sptr
rgrl_est_spline::
estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
          rgrl_transformation const& cur_transform ) const
{
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  unsigned dim = delta_.size();

  // Find the number of correspondences
//  vnl_vector<int> num( dim, 0 );
  unsigned num_match = 0;
  for ( unsigned ms=0; ms < matches.size(); ++ms ) {
    rgrl_match_set const& match_set = *matches[ms];
    for ( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
      vnl_vector<double> const& from_pt = fi.from_feature()->location();
      // If the point is not inside the region of interest, skip it.
      if ( roi_.inside( from_pt ) ) {
        num_match += fi.size();
      }
    }
  }

  vcl_vector< rgrl_spline_sptr > splines( dim );
  if ( cur_transform.is_type( rgrl_trans_spline::type_id() ) ) {
    rgrl_trans_spline const& cur_trans_spline = dynamic_cast< rgrl_trans_spline const& >(cur_transform);
    vcl_cerr << "delta_: " << delta_ << vcl_endl
             << "current transformation's delta_: " << cur_trans_spline.get_delta() << vcl_endl;
    if ( ( delta_ - cur_trans_spline.get_delta()/2 ).two_norm() < 1e-5 ) {
      for ( unsigned i=0; i<dim; ++i ) {
        splines[ i ] = cur_trans_spline.get_spline( i )->refinement( m_ );
      }
      DebugMacro(1, "###spline is initialized by refinement\n" );
    }
     else if ( ( delta_ - cur_trans_spline.get_delta() ).two_norm() < 1e-5 ) {
       DebugMacro(1, "###spline is initialized by copying\n" );
      for ( unsigned i=0; i<dim; ++i )
        splines[ i ] = new rgrl_spline( *(cur_trans_spline.get_spline( i )) );
    }
    else {
      DebugMacro(1, "create spline with m_=" << m_ << "\n");
      for ( unsigned i=0; i<dim; ++i )
        splines[ i ] = new rgrl_spline( m_ );
    }
  }
  else {
    DebugMacro(1, "create spline with m_=" << m_ << "\n" );
    for ( unsigned i=0; i<dim; ++i )
      splines[ i ] = new rgrl_spline( m_ );
  }

  unsigned int num_control = splines[0]->num_of_control_points();

  vnl_diag_matrix<double> wgt( num_match, 0 );
  vnl_matrix<double> displacement( num_match, dim );
  vnl_matrix<double> g( num_match, num_control );

  // from points in the spline coordinates
  vnl_vector< double > tmp( dim, 0.0 );
  vcl_vector< vnl_vector< double > > from_pts_in_knots( num_match, tmp );

  // The index of control points that have constraints.
  // Used for reducing the degree of freedom
  vcl_vector< unsigned > free_control_pt_index;
  // calculate weight, displacement for each match
  {
    unsigned i=0;
    vcl_vector< double > score_constraint( num_control, 0.0 );
    vcl_vector< bool > control_point_constraint( num_control, false );
    for ( unsigned ms=0; ms < matches.size(); ++ms ) {
      rgrl_match_set const& match_set = *matches[ms];
      DebugMacro_abv(2, "rgrl_est_spline.cxx: from_pt \t to_pt \t displacement\n");
      for ( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
        vnl_vector<double> const& from_pt = fi.from_feature()->location();
        // If the point is not inside the region of interest, skip it.
        if ( roi_.inside( from_pt ) && fi.size() != 0 ) {
          // convert it into spline's coordinates
          point_in_knots( from_pt, from_pts_in_knots[i] );

          vnl_vector< double > gr;

          splines[0]->basis_response( from_pts_in_knots[i], gr );

          // see on which control points the point gives constraints
          for ( unsigned j=0; j<num_control; ++j ) {
            if ( !control_point_constraint[ j ] ) { // && gr[j]>1e-4
              score_constraint[ j ] += gr[j];
              if ( score_constraint[j] > 1e-3 )
                control_point_constraint[ j ] = true;
            }
          }

          for ( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
            if ( !global_xform_ ) {
              displacement.set_row( i, ti.to_feature()->location() - from_pt );

              DebugMacro_abv(2, from_pt << " \t " << ti.to_feature()->location() << " \t " << displacement.get_row( i ) << "\n" );
            }
            else {
              displacement.set_row( i, ti.to_feature()->location() - global_xform_->map_location( from_pt ) );

              DebugMacro_abv(2, global_xform_->map_location( from_pt ) << " \t " << ti.to_feature()->location()
                                                                       << " \t " << displacement.get_row( i ) << "\n" );
            }
            g.set_row( i, gr );
            wgt[i] = ti.cumulative_weight();
            ++i;
          }
        }
      }
    }

    // recording which control points have constraints.
    for ( unsigned i=0; i<num_control; ++i ) {
      if ( control_point_constraint[ i ] )
        free_control_pt_index.push_back( i );
    }
  }

  vcl_cerr << "\nafter reduce dof, dof=" << free_control_pt_index.size() << vcl_endl;
  DebugMacro( 1,  "\nafter reduce dof, dof=" << free_control_pt_index.size()<< vcl_endl );

  vul_timer timer;
  // Levenberg Marquardt
  if ( optimize_method_ == RGRL_LEVENBERG_MARQUARDT )
  {
    DebugMacro( 1, "Levenberg marquardt :\n" );
    vnl_matrix< double > covar;

    if (this->debug_flag() > 1) {
      vcl_cout << "rgrl_est_spline.cxx: displacement \t weight\n";
      for ( unsigned i=0; i<displacement.rows(); ++i )
        vcl_cout << i << "    " << displacement.get_row( i ) << " \t " << wgt[i] << '\n';
    }

    for ( unsigned i = 0; i < dim ; ++i ) {
      spline_least_squares_func f( splines[i], from_pts_in_knots, wgt, displacement.get_column( i ), free_control_pt_index );
      vnl_levenberg_marquardt minimizer( f );
      // initialization of c is important
      vnl_vector< double > x( free_control_pt_index.size(), 0 );
      vnl_vector< double > & c = splines[i]->get_control_points();
      for ( unsigned j=0; j<x.size(); ++j )
        x[ j ] = c[ free_control_pt_index[ j ] ];
      if (this->debug_flag() > 1 ) timer.mark();
      minimizer.minimize( x );
      minimizer.diagnose_outcome(vcl_cout);
      if (this->debug_flag() > 1 ) {
        timer.print( vcl_cout );
        vcl_cout << "computing covariance\n";
        timer.mark();
        if ( i==0 )
          covar = minimizer.get_JtJ();
        timer.print( vcl_cout );
        vcl_cout << "covariance " << covar.rows() << 'x' << covar.columns() << vcl_endl;
      }

      // Convert x back to control points
//      vnl_vector< double > c( splines[i]->num_of_control_points(), 0.0 );
      for ( unsigned j=0; j<x.size(); ++j )
        c[ free_control_pt_index[ j ] ] = x[ j ];
      splines[i]->set_control_points( c );
      DebugMacro( 1, "control points:\n" << c << vcl_endl );
    }
    return new rgrl_trans_spline( splines, vnl_vector<double>(dim,0.0), delta_, global_xform_ );
  }
  else if ( optimize_method_ == RGRL_CONJUGATE_GRADIENT ) {
    DebugMacro( 1, "Conjugate Gradient\n" );
    for ( unsigned i = 0; i < dim ; ++i ) {
      spline_cost_function f( splines[i], from_pts_in_knots, wgt, displacement.get_column( i ) );
      vnl_conjugate_gradient minimizer( f );

      // initialization of c is important
      vnl_vector< double > c( splines[i]->num_of_control_points(), 0 );
      minimizer.minimize( c );
      splines[i]->set_control_points( c );
      DebugMacro( 1, "control points:\n" << c << "\n" );
      }
    if ( this->debug_flag() > 1) timer.print( vcl_cout );
    return new rgrl_trans_spline( splines, vnl_vector<double>(dim,0.0), delta_, global_xform_ );
  }
  else if ( optimize_method_ == RGRL_AMOEBA ) {
    DebugMacro( 1, "Nelder-Meade downhill simplex (AMOEBA)\n" );
    for ( unsigned i = 0; i < dim ; ++i ) {
      spline_cost_function f( splines[i], from_pts_in_knots, wgt, displacement.get_column( i ) );
      vnl_amoeba minimizer( f );

      // initialization of c is important
      vnl_vector< double > c( splines[i]->num_of_control_points(), 0 );
      minimizer.minimize( c );
      splines[i]->set_control_points( c );
      DebugMacro( 1, "control points:\n" << c << "\n" );
    }
    if (this->debug_flag() > 1) timer.print( vcl_cout );
    return new rgrl_trans_spline( splines, vnl_vector<double>(dim,0.0), delta_, global_xform_ );
  }
  else if ( optimize_method_ == RGRL_POWELL ) {
    DebugMacro( 1, "Powell's direction-set\n " );
    for ( unsigned i = 0; i < dim ; ++i ) {
      spline_cost_function f( splines[i], from_pts_in_knots, wgt, displacement.get_column( i ) );
      vnl_powell minimizer( &f );

      // initialization of c is important
      vnl_vector< double > c( splines[i]->num_of_control_points(), 0 );
      minimizer.minimize( c );
      splines[i]->set_control_points( c );
      DebugMacro( 1, "control points:\n" << c << "\n" );
    }
    if (this->debug_flag() > 1) timer.print( vcl_cout );
    return new rgrl_trans_spline( splines, vnl_vector<double>(dim,0.0), delta_, global_xform_ );
  }
  else if ( optimize_method_ == RGRL_LBFGS ) {
    DebugMacro( 1, "LBFGS\n" );
    for ( unsigned i = 0; i < dim ; ++i ) {
      spline_cost_function f( splines[i], from_pts_in_knots, wgt, displacement.get_column( i ) );
      vnl_lbfgs minimizer( f );

      // initialization of c is important
      vnl_vector< double > c( splines[i]->num_of_control_points(), 0 );
      minimizer.minimize( c );
      splines[i]->set_control_points( c );
      DebugMacro( 1, "control points:\n" << c << "\n" );
    }
    if (this->debug_flag()> 1) timer.print( vcl_cout );
    return new rgrl_trans_spline( splines, vnl_vector<double>(dim,0.0), delta_, global_xform_ );
  }
  else {   //    // No approximation
    // Without thin-plate constraints:
    // Cost = (Z - G*C)^T * W * (Z - G*C)
    // => C = (G^T * W * G)^{-1} * G^T * W * Z
    // With thin-plate constraints:
    // Cost = (Z - G*C)^T * W * (Z - G*C) + C^T * K * C,
    // where K is the symmetric thin-plate regularization
    // => C = (G^T * W * G + \lambda * K)^{-1} * G^T * W * Z
    //DBG( vcl_cout << "No approximation\n" );
    DebugMacro( 1, "No approximation\n" );

    vnl_matrix<double> X0;
    vnl_matrix<double> covar;
    // get the covariance
    if ( use_thin_plate_ ) {
      vnl_matrix<double> X1 = g.transpose() * wgt.asMatrix();
      vnl_matrix<double> X3;
      // covariance = sigma^2 * (G^T * W * G + \lambda * K)^{-1} * G^T * W^2 * G * (G^T *  W * G + \lambda * K)^{-T}
      {
        vnl_matrix<double> X2 = X1 * g;
        vnl_matrix<double> k;
        splines[0]->thin_plate_regularization(k);
        vnl_svd<double> svd( X2 + lambda_ * k );
        X3 = svd.inverse();
      }
      X0 = X3 * X1;
      covar = X0 * X1.transpose() * X3.transpose();
    }
    else {
      vnl_matrix<double> X1 = g.transpose() * wgt.asMatrix();
      vnl_matrix<double> X3;
      // covariance = sigma^2 * (G^T * W * G)^{-1} * G^T * W^2 * G * (G^T *  W * G)^{-T}
      {
        vnl_matrix<double> X2 = X1 * g;
        vnl_svd<double> svd( X2 );
        X3 = svd.inverse();
      }
      X0 = X3 * X1;
      covar = X0 * X1.transpose() * X3.transpose();
    }

    // Calculate the parameters of splines
    for ( unsigned j = 0; j < dim ; ++j ) {
      vnl_vector<double> c = displacement.get_column( j ).pre_multiply( X0 );
      splines[j]->set_control_points(c);
    }
    if (this->debug_flag()>1) timer.print( vcl_cout );
    return new rgrl_trans_spline( splines, vnl_vector<double>(dim,0.0), delta_, covar, global_xform_ );
  }
}

rgrl_transformation_sptr
rgrl_est_spline::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  // use base class implementation
  return rgrl_estimator::estimate( matches, cur_transform );
}

const vcl_type_info&
rgrl_est_spline::
transformation_type() const
{
  return rgrl_trans_spline::type_id();
}
