#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <testlib/testlib_test.h>
//:
// \file
#include <vcl_compiler.h>
#include <vcl_cassert.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_math.h>

#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_estimator_sptr.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_spline.h>
#include <rgrl/rgrl_trans_spline.h>
#include <rgrl/rgrl_est_spline.h>
#include <rgrl/rgrl_est_quadratic.h>
#include <rgrl/rgrl_trans_quadratic.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <rgrl/rgrl_trans_rigid.h>
#include <rgrl/rgrl_est_similarity2d.h>
#include <rgrl/rgrl_est_rigid.h>
#include <rgrl/rgrl_trans_reduced_quad.h>
#include <rgrl/rgrl_est_reduced_quad2d.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_est_homography2d.h>
#include <rgrl/rgrl_est_homo2d_lm.h>
#include <rgrl/rgrl_est_homo2d_proj.h>
#include <rgrl/rgrl_est_homo2d_proj_rad.h>
#include <rgrl/rgrl_trans_homography2d.h>
#include <rgrl/rgrl_trans_homo2d_proj_rad.h>
#include <rgrl/rgrl_trans_rad_dis_homo2d.h>
#include <rgrl/rgrl_est_dis_homo2d_lm.h>
#include <rgrl/rgrl_cast.h>

#include "test_util.h"

namespace {
  // seed the random number generator for better test repeatability
  static  vnl_random random(468002);

  vnl_vector<double>
  random_3d_vector( )
  {
    vnl_vector<double> v(3);
    v[0] = random.drand32(-100,100);
    v[1] = random.drand32(-100,100);
    v[2] = random.drand32(-100,100);
    return v;
  }

  vnl_vector<double>
  random_3d_normal_error( )
  {
    vnl_vector<double> v(3);
    v[0] = random.normal64();
    v[1] = random.normal64();
    v[2] = random.normal64();
    return v;
  }

  vnl_vector<double>
  random_2d_vector( )
  {
    vnl_vector<double> v(2);
    v[0] = random.drand32(-100,100);
    v[1] = random.drand32(-100,100);
    return v;
  }

  vnl_vector<double>
  random_2d_normal_error( )
  {
    vnl_vector<double> v(2);
    v[0] = random.normal64();
    v[1] = random.normal64();
    return v;
  }

  vnl_vector<double>
  random_1d_vector( )
  {
    vnl_vector<double> v(1);
    v[0] = random.drand32(-100,100);
    return v;
  }

  vnl_vector<double>
  random_1d_normal_error( )
  {
    vnl_vector<double> v(1);
    v[0] = random.normal64();
    return v;
  }

  vnl_vector<double>
  inhomo( vnl_vector<double> p )
  {
    assert( p.size()  );
    const unsigned last = p.size()-1;
    vnl_vector<double> q( p.size() -1 );
    for ( unsigned int i=0; i<p.size()-1; ++i )
      q[i] = p[i]/p[last];
    return q;
  }

  // This computes the weighted least squares one dimension at a time.
  void
  weighted_least_squares( std::vector< vnl_vector<double> > const& from,
                          std::vector< vnl_vector<double> > const& to,
                          std::vector< double > const& wgt,
                          vnl_matrix< double >& A,
                          vnl_vector< double >& t,
                          vnl_matrix< double >& covar )
  {
    unsigned const m = from[0].size(); // number of dimensions
    unsigned const n = from.size(); // number of correspondences

    A.assert_size( m, m );
    t.assert_size( m );
    covar.assert_size( m*(m+1), m*(m+1) );

    // Compute centres
    vnl_vector<double> from_centre( m, 0.0 );
    vnl_vector<double> to_centre( m, 0.0 );
    double sum_wgt = 0.0;
    for ( unsigned i=0; i < n; ++i ) {
      from_centre += from[i];
      to_centre += to[i];
      sum_wgt += wgt[i];
    }
    from_centre /= sum_wgt;
    to_centre /= sum_wgt;

    // Compute our solution
    covar.fill( 0.0 );
    for ( unsigned dim=0; dim < m; ++dim ) {
      vnl_matrix<double> X( n, m+1 );
      vnl_vector<double> y( n );
      vnl_matrix<double> W( n, n, 0.0 );
      for ( unsigned i=0; i < n; ++i ) {
        for ( unsigned j=0; j < m; ++j ) {
          X(i,j) = from[i][j] - from_centre[j];
        }
        X(i,m) = 1.0;
        y(i) = to[i][dim] - to_centre[dim];
        W(i,i) = wgt[i];
      }
      vnl_svd<double> svd( X.transpose() * W * X );
      vnl_matrix<double> c = svd.inverse();
      vnl_vector<double> p = c * X.transpose() * W * y;
      for ( unsigned i=0; i < m; ++i ) {
        A(dim,i) = p[i];
      }
      t[dim] = p[m];
      covar.update( c, dim*(m+1), dim*(m+1) );
    }
    t += to_centre - A*from_centre;
  }

  void
  test_est_affine_pt_to_pt()
  {
    rgrl_trans_affine null3d_trans( vnl_matrix<double>( 3, 3, 0.0 ),
                                    vnl_vector<double>( 3, 0.0 ),
                                    vnl_matrix<double>( 12, 12, 0.0 ) );

    std::cout << "Estimate affine transformation with point-to-point correspondences\n";
    // Test zero error case
    {
      vnl_matrix<double> A( 3, 3 );
      vnl_vector<double> t( 3 );

      A(0,0) = 2.0;  A(0,1) = 4.0;  A(0,2) = 1.0;
      A(1,0) = 1.0;  A(1,1) =-1.0;  A(1,2) = 5.0;
      A(2,0) = 3.0;  A(2,1) = 2.0;  A(2,2) = 2.0;

      t[0] =  3.0;
      t[1] = -4.0;
      t[2] =  1.0;

      std::vector< rgrl_feature_sptr > from_pts;
      std::vector< rgrl_feature_sptr > to_pts;

      {
        vnl_vector<double> v = vec3d( 2.0, 3.0, 5.0 );
        from_pts.   push_back( pf( v ) );
        to_pts.     push_back( pf( A*v + t ) );
      }

      {
        vnl_vector<double> v = vec3d( 1.0, 1.0, 2.0 );
        from_pts.   push_back( pf( v ) );
        to_pts.     push_back( pf( A*v + t ) );
      }

      {
        vnl_vector<double> v = vec3d( 1.0, 2.0, 1.0 );
        from_pts.   push_back( pf( v ) );
        to_pts.     push_back( pf( A*v + t ) );
      }

      {
        rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
        for ( unsigned i=0; i < from_pts.size(); ++i ) {
          ms->add_feature_and_match( from_pts[i], nullptr, to_pts[i] );
        }

        rgrl_estimator_sptr est = new rgrl_est_affine();
        TEST("Estimator type is correct", bool( (est->transformation_type()==rgrl_trans_affine::type_id())!=0 ), true );
        rgrl_transformation_sptr trans = est->estimate( ms, null3d_trans );
        TEST("Underconstrained (not enough correspondences)", !trans, true);
        if ( trans ) {
          rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
          std::cout << "Estimated (shouldn't have):\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()<<'\n';
        }
      }

      {
        rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
        ms->add_feature_and_match( from_pts[0], nullptr, to_pts[0] );
        for ( unsigned i=0; i < from_pts.size(); ++i ) {
          ms->add_feature_and_match( from_pts[i], nullptr, to_pts[i] );
        }

        rgrl_transformation_sptr trans = rgrl_est_affine().estimate( ms, null3d_trans );
        TEST("Underconstrained (samples not independent)", !trans, true);
        if ( trans ) {
          rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
          std::cout << "Estimated (shouldn't have):\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()<<'\n';
        }
      }


      {
        vnl_vector<double> v = vec3d( 2.0, 1.0, 1.0 );
        from_pts.   push_back( pf( v ) );
        to_pts.     push_back( pf( A*v + t ) );
      }

      {
        rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
        for ( unsigned i=0; i < from_pts.size(); ++i ) {
          ms->add_feature_and_match( from_pts[i], nullptr, to_pts[i] );
        }

        rgrl_transformation_sptr trans = rgrl_est_affine().estimate( ms, null3d_trans );
        TEST("Minimal set of correspondences", !trans, false);
        if ( trans ) {
          TEST("Result is affine (is_type())", trans->is_type(rgrl_trans_affine::type_id()) , true);
          rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
          TEST("Result is affine (dynamic_cast)", !aff_trans, false);
          std::cout << "Estimated:\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()
                   << "\n\nTrue:\nA=\n"<<A<<"\nt="<<t<<std::endl;
          TEST("A is close", close( aff_trans->A(), A), true);
          TEST("t is close", close( aff_trans->t(), t), true);
        }
      }

      {
        vnl_vector<double> v = vec3d( 1.0, 2.0, 1.0 );
        from_pts.   push_back( pf( v ) );
        to_pts.     push_back( pf( A*v + t ) );
      }

      {
        vnl_vector<double> v = vec3d( 8.0, 4.0, 3.0 );
        from_pts.   push_back( pf( v ) );
        to_pts.     push_back( pf( A*v + t ) );
      }

      {
        vnl_vector<double> v = vec3d( 1.0, 5.0, 9.0 );
        from_pts.   push_back( pf( v ) );
        to_pts.     push_back( pf( A*v + t ) );
      }

      {
        rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id());
        for ( unsigned i=0; i < from_pts.size(); ++i ) {
          ms->add_feature_and_match( from_pts[i], nullptr, to_pts[i] );
        }

        rgrl_transformation_sptr trans = rgrl_est_affine().estimate( ms, null3d_trans );
        TEST("Many correspondences (zero error)", !trans, false);
        if ( trans ) {
          TEST("Result is affine (is_type())", trans->is_type(rgrl_trans_affine::type_id()) , true);
          rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
          TEST("Result is affine (dynamic_cast)", !aff_trans, false);
          std::cout << "Estimated:\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()
                   << "\n\nTrue:\nA=\n"<<A<<"\nt="<<t<<std::endl;
          TEST("A is close", close( aff_trans->A(), A), true);
          TEST("t is close", close( aff_trans->t(), t), true);
        }
      }

      {
        rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id());
        for ( unsigned i=0; i < from_pts.size(); ++i ) {
          ms->add_feature_and_match( from_pts[i], nullptr, to_pts[i] );
        }

        // add two zero weighted outliers
        ms->add_feature_and_match( pf( vec3d( 4.0, 1.0, 8.0 ) ), nullptr,
                                   pf( vec3d( 4.0, 1.0, 8.0 ) ),
                                   0.0 );
        ms->add_feature_and_match( pf( vec3d( 2.0, 5.0, 8.0 ) ), nullptr,
                                   pf( vec3d( 2.0, 7.0, 4.0 ) ),
                                   0.0 );

        rgrl_transformation_sptr trans = rgrl_est_affine().estimate( ms, null3d_trans );
        TEST("With zero wgted outliers (zero error)", !trans, false);
        if ( trans ) {
          TEST("Result is affine (is_type())", trans->is_type(rgrl_trans_affine::type_id()) , true);
          rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
          TEST("Result is affine (dynamic_cast)", !aff_trans, false);
          std::cout << "Estimated:\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()
                   << "\n\nTrue:\nA=\n"<<A<<"\nt="<<t<<std::endl;
          TEST("A is close", close( aff_trans->A(), A), true);
          TEST("t is close", close( aff_trans->t(), t), true);
        }
      }
    }

    // Weighted least squares
    {
      std::vector< vnl_vector<double> > from;
      std::vector< vnl_vector<double> > to;
      std::vector< double > wgt;

      from.   push_back( vec3d( 0.0, 0.0, 0.0 ) );
      to.     push_back( vec3d( 0.0, 0.0, 0.0 ) );
      wgt.    push_back( 5.0 );

      from.   push_back( vec3d( 1.0, 4.0, 3.0 ) );
      to.     push_back( vec3d( 7.0, 2.0, 6.0 ) );
      wgt.    push_back( 2.0 );

      from.   push_back( vec3d( 5.0, 2.0, 4.0 ) );
      to.     push_back( vec3d( 3.0, 6.0, 1.0 ) );
      wgt.    push_back( 0.5 );

      from.   push_back( vec3d( 3.0, 5.0, 3.0 ) );
      to.     push_back( vec3d( 9.0, 4.0, 6.0 ) );
      wgt.    push_back( 3.0 );

      from.   push_back( vec3d( 1.0, 4.0, 2.0 ) );
      to.     push_back( vec3d( 9.0, 3.0, 5.0 ) );
      wgt.    push_back( 1.0 );

      from.   push_back( vec3d( 1.0, 5.0, 2.0 ) );
      to.     push_back( vec3d( 2.0, 5.0, 3.0 ) );
      wgt.    push_back( 2.0 );

      from.   push_back( vec3d( 6.0, 2.0, 4.0 ) );
      to.     push_back( vec3d( 8.0, 6.0, 9.0 ) );
      wgt.    push_back( 1.0 );

      // Compute the rgrl solution
      //
      rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id());
      for ( unsigned i=0; i < from.size(); ++i ) {
        ms->add_feature_and_match( pf(from[i]), nullptr, pf(to[i]), wgt[i] );
      }
      rgrl_transformation_sptr trans = rgrl_est_affine().estimate( ms, null3d_trans );
      TEST("Weighted least squares", !trans, false);

      // compute an independent solution
      //
      vnl_matrix<double> A( 3, 3 );
      vnl_vector<double> t( 3 );
      vnl_matrix<double> covar( 12, 12 );
      weighted_least_squares( from, to, wgt, A, t, covar );

      if ( trans ) {
        TEST("Result is affine (is_type())", trans->is_type(rgrl_trans_affine::type_id()), true );
        rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
        TEST("Result is affine (dynamic_cast)", !aff_trans, false);
        std::cout << "Estimated:\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()<<"\ncovar="<<aff_trans->covar()
                 << "\n\nTrue:\nA=\n"<<A<<"\nt="<<t<<"\ncovar="<<covar<<std::endl;
        TEST("A is close", close( aff_trans->A(), A, 1e-4), true);
        TEST("t is close", close( aff_trans->t(), t, 1e-4), true);
        TEST("covar is close", close_det( aff_trans->covar(), covar), true );
      }
    }
    std::cout << "--------------------------------------------------------------------------------\n";
  }


  void
  add_point_on_line( std::vector< vnl_vector<double> >& from,
                     std::vector< vnl_vector<double> >& to,
                     std::vector< vnl_vector<double> >& to_tang,
                     vnl_matrix<double> const& A,
                     vnl_vector<double> const& trans,
                     vnl_vector<double> const& v,
                     vnl_vector<double> const& t )
  {
    from.push_back( v );
    to.push_back( A * v + trans + t );
    vnl_vector<double> tp = t;
    tp.normalize();
    to_tang.push_back( tp );
  }


  void
  add_point_on_line( std::vector< vnl_vector<double> >& from,
                     std::vector< vnl_vector<double> >& to,
                     std::vector< vnl_vector<double> >& to_tang,
                     std::vector< double >            & wgt,
                     vnl_vector<double> const& f,
                     vnl_vector<double> const& t,
                     vnl_vector<double> const& tan,
                     double                    w )
  {
    from.push_back( f );
    to.push_back( t );
    vnl_vector<double> tp = tan;
    tp.normalize();
    to_tang.push_back( tp );
    wgt.push_back( w );
  }


  // This computes the weighted least squares one dimension at a
  // time. It adds a single constraint per point based on the normal
  // error.
  void
  weighted_least_squares_to_line( std::vector< vnl_vector<double> > const& from,
                                  std::vector< vnl_vector<double> > const& to,
                                  std::vector< vnl_vector<double> > const& tang,
                                  std::vector< double > const& wgt,
                                  vnl_matrix< double >& A,
                                  vnl_vector< double >& t,
                                  vnl_matrix< double >& covar )
  {
    unsigned const m = from[0].size(); // number of dimensions
    unsigned const n = from.size(); // number of correspondences

    assert ( m == 2 ); // otherwise is more than one constraint per correspondence
    A.assert_size( m, m );
    t.assert_size( m );
    covar.assert_size( m*(m+1), m*(m+1) );

    vnl_matrix<double> X( n, m*(m+1) );
    vnl_vector<double> y( n );
    vnl_matrix<double> W( n, n, 0.0 );

    // Compute centres
    vnl_vector<double> from_centre( m, 0.0 );
    vnl_vector<double> to_centre( m, 0.0 );
    double sum_wgt = 0.0;
    for ( unsigned i=0; i < n; ++i ) {
      from_centre += from[i];
      to_centre += to[i];
      sum_wgt += wgt[i];
    }
    from_centre /= sum_wgt;
    to_centre /= sum_wgt;

    for ( unsigned r=0; r < n; ++r ) {
      vnl_vector<double> norm( 2 );
      norm[0] = -tang[r][1];
      norm[1] =  tang[r][0];
      assert ( std::abs( norm.two_norm() - 1 ) < 1e-6 );

      // build LHS [ [ x y 1 ] * nx  [ x y 1 ] * ny ]
      for ( unsigned dim=0; dim < m; ++dim ) {
        for ( unsigned i=0; i < m; ++i ) {
          X( r, dim*(m+1)+i ) = (from[r][i]-from_centre[i]) * norm[dim];
        }
        X( r, dim*(m+1)+m ) = norm[dim];
      }

      // build RHS [ u v ] . [ nx ny ]
      y[r] = dot_product( to[r]-to_centre, norm );

      W(r,r) = wgt[r];
    }

    vnl_svd<double> svd( X.transpose() * W * X );
    vnl_matrix<double> c = svd.inverse();
    vnl_vector<double> p = c * X.transpose() * W * y;
    for ( unsigned dim=0; dim < m; ++dim ) {
      for ( unsigned i=0; i < m; ++i ) {
        A(dim,i) = p[dim*(m+1)+i];
      }
      t[dim] = p[dim*(m+1)+m];
    }
    t += to_centre - A * from_centre;
    covar = c;
  }


  void
  test_est_affine_pt_to_line()
  {
    std::cout << "Estimate affine transformation with point-to-line correspondences\n";

    rgrl_trans_affine null2d_trans( vnl_matrix<double>( 2, 2, 0.0 ),
                                    vnl_vector<double>( 2, 0.0 ),
                                    vnl_matrix<double>( 6, 6, 0.0 ) );


    // Test zero error case: errors are only in the tangential direction
    {
      vnl_matrix<double> A( 2, 2 );
      vnl_vector<double> t( 2 );

      A(0,0) = 2.0;  A(0,1) = 4.0;
      A(1,0) = 1.0;  A(1,1) =-1.0;

      t[0] =  3.0;
      t[1] = -4.0;

      std::vector< vnl_vector<double> > from;
      std::vector< vnl_vector<double> > to;
      std::vector< vnl_vector<double> > to_tang;

      add_point_on_line( from, to, to_tang, A, t,
                         vec2d( 2.0, 3.0 ), vec2d( 1.0, 2.0 ) );

      add_point_on_line( from, to, to_tang, A, t,
                         vec2d( 1.0, 4.0 ), vec2d( 5.0, 1.0 ) );

      add_point_on_line( from, to, to_tang, A, t,
                         vec2d( 8.0, 0.0 ), vec2d( 2.0, 4.0 ) );

      add_point_on_line( from, to, to_tang, A, t,
                         vec2d( 3.0, 2.0 ), vec2d( 1.0, 0.0 ) );

      add_point_on_line( from, to, to_tang, A, t,
                         vec2d( 5.0, 1.0 ), vec2d( 2.0, 1.0 ) );

      {
        rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id(), rgrl_feature_trace_pt::type_id() );
        for ( unsigned i=0; i < from.size(); ++i ) {
          ms->add_feature_and_match( pf( from[i] ), nullptr, tf( to[i], to_tang[i] ) );
        }

        rgrl_transformation_sptr trans = rgrl_est_affine().estimate( ms, null2d_trans );
        TEST("Underconstrained (not enough correspondences)", !trans, true);
        if ( trans ) {
          rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
          std::cout << "Estimated (shouldn't have):\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()<<'\n';
        }
      }

      {
        rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id(), rgrl_feature_trace_pt::type_id() );
        ms->add_feature_and_match( pf( from[0] ), nullptr, tf( to[0], to_tang[0] ) );
        for ( unsigned i=0; i < from.size(); ++i ) {
          ms->add_feature_and_match( pf( from[i] ), nullptr, tf( to[i], to_tang[i] ) );
        }

        rgrl_transformation_sptr trans = rgrl_est_affine().estimate( ms, null2d_trans );
        TEST("Underconstrained (samples not independent)", !trans, true);
        if ( trans ) {
          rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
          std::cout << "Estimated (shouldn't have):\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()<<'\n';
        }
      }


      add_point_on_line( from, to, to_tang, A, t,
                         vec2d( 8.0, 2.0 ), vec2d( -1.0, 1.0 ) );

      {
        rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id(), rgrl_feature_trace_pt::type_id() );
        for ( unsigned i=0; i < from.size(); ++i ) {
          ms->add_feature_and_match( pf( from[i] ), nullptr, tf( to[i], to_tang[i] ) );
        }

        rgrl_transformation_sptr trans = rgrl_est_affine().estimate( ms, null2d_trans );
        TEST("Minimal set of correspondences", !trans, false);
        if ( trans ) {
          TEST("Result is affine (is_type())", trans->is_type(rgrl_trans_affine::type_id()), true );
          rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
          TEST("Result is affine (dynamic_cast)", !aff_trans, false);
          std::cout << "Estimated:\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()
                   << "\n\nTrue:\nA=\n"<<A<<"\nt="<<t<<std::endl;
          TEST("A is close", close( aff_trans->A(), A), true);
          TEST("t is close", close( aff_trans->t(), t), true);
        }
      }

      add_point_on_line( from, to, to_tang, A, t,
                         vec2d( 9.0, 1.0 ), vec2d( -2.0, 1.0 ) );

      add_point_on_line( from, to, to_tang, A, t,
                         vec2d( 5.0, 4.0 ), vec2d(  3.0,-1.0 ) );

      add_point_on_line( from, to, to_tang, A, t,
                         vec2d( 1.0, 9.0 ), vec2d(  3.0,-1.0 ) );

      {
        rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id(), rgrl_feature_trace_pt::type_id() );
        for ( unsigned i=0; i < from.size(); ++i ) {
          ms->add_feature_and_match( pf( from[i] ), nullptr, tf( to[i], to_tang[i] ) );
        }

        rgrl_transformation_sptr trans = rgrl_est_affine().estimate( ms, null2d_trans );
        TEST("Many correspondences (zero normal error)", !trans, false);
        if ( trans ) {
          TEST("Result is affine (is_type())", trans->is_type(rgrl_trans_affine::type_id()) , true);
          rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
          TEST("Result is affine (dynamic_cast)", !aff_trans, false);
          std::cout << "Estimated:\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()
                   << "\n\nTrue:\nA=\n"<<A<<"\nt="<<t<<std::endl;
          TEST("A is close", close( aff_trans->A(), A), true);
          TEST("t is close", close( aff_trans->t(), t), true);
        }
      }

      {
        rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id(), rgrl_feature_trace_pt::type_id() );
        for ( unsigned i=0; i < from.size(); ++i ) {
          ms->add_feature_and_match( pf( from[i] ), nullptr, tf( to[i], to_tang[i] ) );
        }

        // add two zero weighted outliers
        ms->add_feature_and_match( pf( vec2d( 4.0, 1.0 ) ), nullptr,
                                   tf( vec2d( 4.0, 1.0 ), vec2d( 5.0, 1.0 ) ),
                                   0.0 );
        ms->add_feature_and_match( pf( vec2d( 2.0, 5.0 ) ), nullptr,
                                   tf( vec2d( 2.0, 7.0 ), vec2d( 1.0, 1.0 ) ),
                                   0.0 );

        rgrl_transformation_sptr trans = rgrl_est_affine().estimate( ms, null2d_trans );
        TEST("With zero wgted outliers (zero normal error)", !trans, false);
        if ( trans ) {
          TEST("Result is affine (is_type())", trans->is_type(rgrl_trans_affine::type_id()), true );
          rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
          TEST("Result is affine (dynamic_cast)", !aff_trans, false);
          std::cout << "Estimated:\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()
                   << "\n\nTrue:\nA=\n"<<A<<"\nt="<<t<<std::endl;
          TEST("A is close", close( aff_trans->A(), A), true);
          TEST("t is close", close( aff_trans->t(), t), true);
        }
      }
    }

    // Weighted least squares
    {
      std::vector< vnl_vector<double> > from;
      std::vector< vnl_vector<double> > to;
      std::vector< vnl_vector<double> > to_tang;
      std::vector< double >             wgt;

      add_point_on_line( from, to, to_tang, wgt,
                         vec2d( 0.0, 0.0 ),
                         vec2d( 3.0, 6.0 ), vec2d( 3.0, 2.0 ), 2.8 );

      add_point_on_line( from, to, to_tang, wgt,
                         vec2d( 3.0, 4.0 ),
                         vec2d( 1.0, 4.0 ), vec2d( 4.0,-1.0 ), 8.9 );

      add_point_on_line( from, to, to_tang, wgt,
                         vec2d( 3.0, 9.0 ),
                         vec2d( 8.0, 0.0 ), vec2d( 6.0, 4.0 ), 0.2 );

      add_point_on_line( from, to, to_tang, wgt,
                         vec2d( 3.0, 7.0 ),
                         vec2d( 4.0, 1.0 ), vec2d( 2.0, 0.0 ), 8.7 );

      add_point_on_line( from, to, to_tang, wgt,
                         vec2d( 6.0, 4.0 ),
                         vec2d( 5.0, 1.0 ), vec2d( 1.0, 6.0 ), 7.2 );

      add_point_on_line( from, to, to_tang, wgt,
                         vec2d( 4.0, 8.0 ),
                         vec2d( 1.0, 2.0 ), vec2d( 9.0, 8.0 ), 5.2 );

      add_point_on_line( from, to, to_tang, wgt,
                         vec2d( 1.0, 5.0 ),
                         vec2d( 3.0, 6.0 ), vec2d( 3.0, 7.0 ), 1.4 );

      add_point_on_line( from, to, to_tang, wgt,
                         vec2d( 5.0, 2.0 ),
                         vec2d( 9.0, 2.0 ), vec2d( 5.0, 9.0 ), 4.5 );

      add_point_on_line( from, to, to_tang, wgt,
                         vec2d( 2.0, 7.0 ),
                         vec2d( 3.0, 8.0 ), vec2d( 6.0, 5.0 ), 7.6 );

      add_point_on_line( from, to, to_tang, wgt,
                         vec2d( 1.0, 4.0 ),
                         vec2d( 5.0, 2.0 ), vec2d( 7.0, 3.0 ), 2.6 );

      add_point_on_line( from, to, to_tang, wgt,
                         vec2d( 5.0, 4.0 ),
                         vec2d( 1.0, 4.0 ), vec2d( 7.0, 2.0 ), 8.4 );

      // Compute the rgrl solution
      //
      rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id(), rgrl_feature_trace_pt::type_id() );
      for ( unsigned i=0; i < from.size(); ++i ) {
        ms->add_feature_and_match( pf( from[i] ), nullptr, tf( to[i], to_tang[i] ), wgt[i] );
      }
      rgrl_transformation_sptr trans = rgrl_est_affine().estimate( ms, null2d_trans );
      TEST("Weighted least squares", !trans, false);

      // compute an independent solution
      //
      vnl_matrix<double> A( 2, 2 );
      vnl_vector<double> t( 2 );
      vnl_matrix<double> covar( 6, 6 );
      weighted_least_squares_to_line( from, to, to_tang, wgt, A, t, covar );

      if ( trans ) {
        TEST("Result is affine (is_type())", trans->is_type(rgrl_trans_affine::type_id()), true );
        rgrl_trans_affine* aff_trans = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
        TEST("Result is affine (dynamic_cast)", !aff_trans, false);
        std::cout << "Estimated:\nA=\n"<<aff_trans->A()<<"\nt="<<aff_trans->t()<<"\ncovar="<<aff_trans->covar()
                 << "\n\nTrue:\nA=\n"<<A<<"\nt="<<t<<"\ncovar="<<covar<<std::endl;
        TEST("A is close", close( aff_trans->A(), A), true);
        TEST("t is close", close( aff_trans->t(), t), true);
        TEST("covar is close", close_det( aff_trans->covar(), covar), true );
      }
    }
    std::cout << "--------------------------------------------------------------------------------\n";
  }


#if 0 // no similarity3d yet...
  vnl_matrix<double>
  rot3d_x( double angle )
  {
    vnl_matrix<double> m( 3, 3 );
    m(0,0) = 1;   m(1,0) =  0;               m(2,0) = 0;
    m(1,0) = 0;   m(1,1) =  std::cos(angle);  m(1,2) = -std::sin(angle);
    m(2,0) = 0;   m(2,1) =  std::sin(angle);  m(2,2) =  std::cos(angle);
    return m;
  }

  vnl_matrix<double>
  rot3d_y( double angle )
  {
    vnl_matrix<double> m( 3, 3 );
    m(0,0) =  std::cos(angle);    m(0,1) = 0;   m(0,2) =  std::sin(angle);
    m(1,0) =  0;                 m(1,1) = 1;   m(1,2) = 0;
    m(2,0) = -std::sin(angle);    m(2,1) = 0;   m(2,2) =  std::cos(angle);
    return m;
  }


  vnl_matrix<double>
  rot3d_z( double angle )
  {
    vnl_matrix<double> m( 3, 3 );
    m(0,0) =  std::cos(angle);  m(0,1) = -std::sin(angle);   m(0,2) = 0;
    m(1,0) =  std::sin(angle);  m(1,1) =  std::cos(angle);   m(1,2) = 0;
    m(2,0) =  0;               m(2,1) = 0;                 m(2,2) = 1;
    return m;
  }

  vnl_matrix<double>
  rot3d( double x_ang, double y_ang, double z_ang )
  {
    return rot3d_z( z_ang ) * rot3d_y( y_ang ) * rot3d_x( x_ang );
  }

  void
  test_similarity_pt_to_pt()
  {
    rgrl_trans_similarity identity( vnl_matrix<double>( 3, 3, vnl_matrix_identity ),
                                    vnl_vector<double>( 3, 0.0 ),
                                    vnl_matrix<double>( 7, 7, 0.0 ) );

    {
      vnl_matrix<double> smallR = rot3d( 0.1, 0.05, 0.15 );
      vnl_vector<double> smallT( 3 );
      smallT[0] = 1;
      smallT[1] = 5;
      smallT[2] = 3;

      rgrl_trans_similarity sim( smallR, smallT, vnl_matrix<double>( 7, 7, 0.0 ) );

      std::vector< vnl_vector<double> > from;
      from.push_back( vec3d(  0.0,  0.0,  0.0 ) );
      from.push_back( vec3d( 10.0, 50.0, 90.0 ) );
      from.push_back( vec3d( 23.0, 74.0, 19.0 ) );
      from.push_back( vec3d( 81.0, 10.0, 20.0 ) );
      from.push_back( vec3d( 75.0, 18.0, 23.0 ) );
      from.push_back( vec3d( 35.0, 19.0, 65.0 ) );

      rgrl_match_set ms( rgrl_feature_point::type_id() );
      for ( unsigned int i=0; i < from.size(); ++i ) {
        vnl_vector<double> t = smallR * from[i] + smallT;
        ms.add_feature_and_match( pf( from[i] ), 0, pf( t ) );
      }

      {
        ms.remap_from_features( sim );

        rgrl_estimator_sptr est = new rgrl_est_similarity3d();
        TEST("Similarity estimator type is correct", est->transformation_type(), rgrl_trans_similarity::type_id() );
        rgrl_transformation_sptr trans = est->estimate( ms, sim );
        TEST("Similarity estimation with zero error and exact initial", !trans, false);
        if ( trans ) {
          TEST("Result is similarity (is_type())", trans->is_type(rgrl_trans_similarity::type_id()), true );
          rgrl_trans_similarity* sim_trans = dynamic_cast<rgrl_trans_similarity*>(trans.as_pointer());
          TEST("Result is similarity (dynamic_cast)", !sim_trans, false);
          std::cout << "Estimated:\nA=\n"<<sim_trans->A()<<"\nt="<<sim_trans->t()
                   << "\n\nTrue:\nA=\n"<<smallR<<"\nt="<<smallT<<std::endl;
          TEST("A is close", close( sim_trans->A(), smallR ), true);
          TEST("t is close", close( sim_trans->t(), smallT ), true);
        }
      }

      {
        ms.remap_from_features( identity );
        rgrl_transformation_sptr trans = rgrl_est_similarity3d().estimate( ms, identity );
        TEST("Similarity estimation with zero error and close initial", !trans, false);
        if ( trans ) {
          TEST("Result is similarity (is_type())", trans->is_type(rgrl_trans_similarity::type_id()), true );
          rgrl_trans_similarity* sim_trans = dynamic_cast<rgrl_trans_similarity*>(trans.as_pointer());
          TEST("Result is similarity (dynamic_cast)", !sim_trans, false);
          // should probably check the residual error, not the parameters.
          std::cout << "Estimated:\nA=\n"<<sim_trans->A()<<"\nt="<<sim_trans->t()
                   << "\n\nTrue:\nA=\n"<<smallR<<"\nt="<<smallT<<std::endl;
          TEST("A is close", close( sim_trans->A(), smallR, 0.1 ), true);
          TEST("t is close", close( sim_trans->t(), smallT, 1.0 ), true);

          std::cout << "---------\n"
                   << "True:\nR=\n" << smallR << "\nt=\n"<<smallT
                   << "\n\nEstimated:\nR=\n"<< sim_trans->A() << "\nt=\n" << sim_trans->t()
                   << "\n\nMappings: From, To real, To est\n";
          for ( unsigned i=0; i < from.size(); ++i ) {
            std::cout << from[i] << "    " << (smallR*from[i] + smallT) << "    "
                     << (sim_trans->A()*from[i] + sim_trans->t()) << std::endl;
          }
          std::cout << "---------\n\n";
        }
      }


      {
        rgrl_transformation_sptr curr = new rgrl_trans_similarity( identity );
        rgrl_transformation_sptr trans;

        vnl_matrix<double> d(3,12,0.0);

        for ( unsigned int i=0; i < 100 && curr; ++i ) {
          ms.remap_from_features( *curr );
          trans = rgrl_est_similarity3d().estimate( ms, *curr );
          curr = trans;
        }
        TEST("Similarity estimation converges", !trans, false);

        if ( trans ) {
          TEST("Result is similarity (is_type())", trans->is_type(rgrl_trans_similarity::type_id()), true );
          rgrl_trans_similarity* sim_trans = dynamic_cast<rgrl_trans_similarity*>(trans.as_pointer());
          TEST("Result is similarity (dynamic_cast)", !sim_trans, false);
          std::cout << "Estimated:\nA=\n"<<sim_trans->A()<<"\nt="<<sim_trans->t()
                   << "\n\nTrue:\nA=\n"<<smallR<<"\nt="<<smallT<<std::endl;
          TEST("A is close", close( sim_trans->A(), smallR ), true);
          TEST("t is close", close( sim_trans->t(), smallT ), true);

          std::cout << "---------\n"
                   << "True:\nR=\n" << smallR << "\nt=\n"<<smallT
                   << "\n\nEstimated:\nR=\n"<< sim_trans->A() << "\nt=\n" << sim_trans->t()
                   << "\n\nMappings: From, To real, To est\n";
          for ( unsigned i=0; i < from.size(); ++i ) {
            std::cout << from[i] << "    " << (smallR*from[i] + smallT) << "    "
                     << (sim_trans->A()*from[i] + sim_trans->t()) << std::endl;
          }
          std::cout << "---------\n\n";
        }
      }
    }
    std::cout << "--------------------------------------------------------------------------------\n";
  }
#endif //0 // no similarity yet


  void
  test_est_spline()
  {
    for ( unsigned dim=1; dim<=3; ++dim ) {
      std::cout << "Estimate " << dim << "-D spline transformation\n";
      // create n n-D splines
      vnl_vector< unsigned > m( dim, 1 );
      rgrl_spline_sptr spline = new rgrl_spline( m );
      unsigned dof = spline->num_of_control_points();
      vnl_vector<double> c( dof );
      std::vector<rgrl_spline_sptr> splines;
      for ( unsigned j=0; j<dim; ++j ) {
        for ( unsigned i=0; i<dof; ++i )
          c[ i ] = random.drand32( 0, 5 );
        spline->set_control_points( c );
        splines.push_back( spline );
        std::cout << "true control points: " << c << '\n';
      }

      // roi
      vnl_vector<double> x0( dim, 0 ), x1( dim );
      for ( unsigned i=0; i<dim; ++i )
        x1[ i ] = m[ i ];
      rgrl_mask_box roi( x0, x1 );

      // the coordinate reference
      vnl_vector<double> delta( dim, 1 );
      vnl_vector<double> p( dim, 0 );
      // Create the transformation of splines
      rgrl_trans_spline cur_trans( splines, p, delta );
      unsigned num_data_pts = c.size();

      // generating random from_points
      std::vector< vnl_vector<double> > pts;
      vnl_vector<double> from_pt( dim );
      for ( unsigned i=0; i<num_data_pts; ++i ) {
        for ( unsigned j=0; j<dim; ++j ) {
          from_pt[j] = random.drand32(0, m[j]);
        }
        pts.push_back(from_pt);
      }

      // Computing to_points
      std::vector< vnl_vector<double> > to_pts;
      rgrl_match_set_sptr match_set = new rgrl_match_set(rgrl_feature_point::type_id());
      vnl_vector<double> to_pt( dim );
      for ( unsigned i=0; i<num_data_pts; ++i ) {
        for ( unsigned j=0; j<dim; ++j ) {
          double sv = splines[ j ]->f_x( pts[i] );
          to_pt[j] = pts[i][j] + sv;
        }
        to_pts.push_back(to_pt);
        rgrl_feature_sptr from_feature = new rgrl_feature_point( pts[i] );
        rgrl_feature_sptr to_feature = new rgrl_feature_point( to_pt );
        match_set->add_feature_and_match( from_feature, nullptr, to_feature, 1.0 );
      }
      rgrl_set_of<rgrl_match_set_sptr> set;
      set.push_back(match_set);
      {
        rgrl_est_spline est_spline( dof, roi, delta, m );
        est_spline.set_optimize_method( RGRL_LEVENBERG_MARQUARDT );
        // cur_trans is not used in estimate() currently
        rgrl_transformation_sptr trans_sptr = est_spline.estimate( set, cur_trans );
        vnl_vector<double> map_to;
        double err_sum = 0;
        bool test_pass = true;
        for ( unsigned i=0; i<num_data_pts; ++i ) {
          trans_sptr->map_location( pts[i], map_to );
          double error = (map_to - to_pts[i]).two_norm();
          if ( error > 1e-4 ) {
            std::cout << " point (" << pts[i] << ") is transformed to (" << map_to << ")\n"
                     << " true mapping points is (" << to_pts[i] << ')' << std::endl;
            test_pass = false;
          }
          err_sum += error;
        }
        TEST("Test random points",  test_pass, true);
        std::cout << " sum of error : " << err_sum << '\n';
        vnl_vector<double> from_pt( dim );
        for ( unsigned i=0; i<dim; ++i )
          from_pt[ i ] = random.drand32( 0, m[i] );
        trans_sptr->map_location( from_pt, map_to );
        vnl_vector<double> true_to( dim );
        test_pass = true;
        for ( unsigned i=0; i<dim; ++i )
          true_to[ i ] = from_pt[ i ] + splines[i]->f_x( from_pt );
        double error = ( map_to - true_to ).two_norm() ;
        if ( error > 1e-4 ) {
          test_pass = false;
          std::cout << " point (" << from_pt << ") is transformed to (" << map_to << ")\n"
                   << " true mapping points is (" << true_to << ")\n";
        }
        TEST("Test random point", test_pass, true);

        // Use the estimated result as cur_xform to estimate refined splines.

        std::cout << "\nTest the refinement estimation.\n";
        rgrl_spline tmp_spline( m*2 );
        std::cout << "number of control points: " << tmp_spline.num_of_control_points() << '\n';
        rgrl_est_spline est_spline2( tmp_spline.num_of_control_points(), roi, delta/2, m*2 );
        rgrl_transformation_sptr trans_sptr2 = est_spline2.estimate( set, *trans_sptr );
        err_sum = 0;
        test_pass = true;
        for ( unsigned i=0; i<num_data_pts; ++i ) {
          trans_sptr2->map_location( pts[i], map_to );
          double error = (map_to - to_pts[i]).two_norm();
          if ( error > 1e-4 ) {
            std::cout << " point (" << pts[i] << ") is transformed to (" << map_to << ")\n"
                     << " true mapping points is (" << to_pts[i] << ')' << std::endl;
            test_pass = false;
          }
          err_sum += error;
        }
        TEST("Test estimated mapping points",  test_pass, true);
        std::cout << " sum of error : " << err_sum << '\n';
        for ( unsigned i=0; i<dim; ++i )
          from_pt[ i ] = random.drand32( 0.1, m[i]-0.1 );
        trans_sptr2->map_location( from_pt, map_to );
        for ( unsigned i=0; i<dim; ++i )
          true_to[ i ] = from_pt[ i ] + splines[i]->f_x( from_pt );
        error = (map_to - true_to).two_norm();
        test_pass = true;
        if ( error > 5e-1 ) {
          test_pass = false;
          std::cout << " point (" << from_pt << ") is transformed to (" << map_to << ")\n"
                   << " true mapping points is (" << true_to << ")\n";
        }
        TEST("Test a random point inside the range",  test_pass, true);
      }
    }
    std::cout << "--------------------------------------------------------------------------------\n";
  }

  void
  test_est_spline_reduce_dof()
  {
    unsigned dim = 3;
    std::cout << "Estimate " << dim << "-D spline transformation with dof reduced\n";
    // create n n-D splines
    // The m of roi
    vnl_vector< unsigned > m1( dim, 1 );
    // The m of the whole region
    vnl_vector< unsigned > m2( dim, 2 );
    std::vector<rgrl_spline_sptr> splines;

    rgrl_spline tmp( m2 );
    unsigned dof2 = tmp.num_of_control_points();
    vnl_vector<double> c( dof2, 0.0 );

    for ( unsigned k=m1[2]; k<m2[2]+3; ++k ) {
      for ( unsigned j=m1[1]; j<m2[1]+3; ++j ) {
        for ( unsigned i=m1[0]; i<m2[0]+3; ++i ){
          unsigned n = k*(m2[0]-m1[0]+3)*(m2[1]-m1[1]+3) + j*(m2[0]-m1[0]+3) + i;
          c[ n ] = random.drand32( 0, 5 );
        }
      }
    }
    std::cout << "true control points: " << c << '\n';
    for ( unsigned m=0; m<dim; ++m ) {
      rgrl_spline_sptr spline2 = new rgrl_spline( m2 );
      spline2->set_control_points( c );
      splines.push_back( spline2 );
    }

      // roi
    vnl_vector<double> x0( dim, 0.0 ), x1( dim );
    for ( unsigned i=0; i<dim; ++i ) {
      x1[ i ] = m2[ i ];
      x0[ i ] = m1[ i ];
    }
    rgrl_mask_box roi( x0, x1 );

    // the coordinate reference
    vnl_vector<double> delta( dim, 1 );
    vnl_vector<double> p( dim, 0 );
    // Create the transformation of splines
    rgrl_trans_spline cur_trans( splines, p, delta );
    unsigned num_data_pts = c.size();

    std::vector< vnl_vector<double> > pts;
    vnl_vector<double> from_pt( dim );
    // generating points in roi
    for ( unsigned i=0; i<num_data_pts; ++i ) {
      for ( unsigned j=0; j<dim; ++j ) {
        from_pt[j] = random.drand32(m1[j], m2[j]);
      }
      pts.push_back(from_pt);
    }
    // generating random from_points outside roi (shouldn't affect the result)
    for ( unsigned i=0; i<num_data_pts; ++i ) {
      for ( unsigned j=0; j<dim; ++j ) {
        from_pt[j] = random.drand32(0, m1[j]);
      }
      pts.push_back(from_pt);
    }

    // Computing to_points
    std::vector< vnl_vector<double> > to_pts;
    rgrl_match_set_sptr match_set = new rgrl_match_set(rgrl_feature_point::type_id());
    vnl_vector<double> to_pt( dim );
    for ( unsigned i=0; i<num_data_pts; ++i ) {
      for ( unsigned j=0; j<dim; ++j ) {
        double sv = splines[ j ]->f_x( pts[i] );
        to_pt[j] = pts[i][j] + sv;
      }
      to_pts.push_back(to_pt);
      rgrl_feature_sptr from_feature = new rgrl_feature_point( pts[i] );
      rgrl_feature_sptr to_feature = new rgrl_feature_point( to_pt );
      match_set->add_feature_and_match( from_feature, nullptr, to_feature, 1.0 );
    }
    rgrl_set_of<rgrl_match_set_sptr> set;
    set.push_back(match_set);

    {
      rgrl_est_spline est_spline( dof2, roi, delta, m2 );
      est_spline.set_optimize_method( RGRL_LEVENBERG_MARQUARDT );
      // cur_trans is not used in estimate() currently
      rgrl_transformation_sptr trans_sptr = est_spline.estimate( set, cur_trans );
      vnl_vector<double> map_to;
      double err_sum = 0;
      bool test_pass = true;
      for ( unsigned i=0; i<num_data_pts; ++i ) {
        trans_sptr->map_location( pts[i], map_to );
        double error = (map_to - to_pts[i]).two_norm();
        if ( error > 1e-4 ) {
          test_pass = false;
          std::cout << " point (" << pts[i] << ") is transformed to (" << map_to << ")\n"
                   << " true mapping points is (" << to_pts[i] << ')' << std::endl;
        }
        err_sum += error;
      }
        TEST("Test estimated mapping points",  test_pass, true);
      std::cout << " sum of error : " << err_sum << '\n';
      vnl_vector<double> from_pt( dim );
      for ( unsigned i=0; i<dim; ++i )
        from_pt[ i ] = random.drand32( m1[i], m2[i] );
      trans_sptr->map_location( from_pt, map_to );
      vnl_vector<double> true_to( dim );
      for ( unsigned i=0; i<dim; ++i )
        true_to[ i ] = from_pt[ i ] + splines[i]->f_x( from_pt );
      test_pass = ((map_to - true_to).two_norm()<1e-5);
      if ( !test_pass ) {
        std::cout << " point (" << from_pt << ") is transformed to (" << map_to << ")\n"
                 << " true mapping points is (" << true_to << ")\n";
      }
      TEST("Test random point", test_pass, true);

      std::cout << "\nTest the refinement estimation.\n";
      rgrl_spline tmp_spline( m2*2 );
      std::cout << "number of control points: " << tmp_spline.num_of_control_points() << '\n';
      rgrl_est_spline est_spline2( tmp_spline.num_of_control_points(), roi, delta/2, m2*2 );
      rgrl_transformation_sptr trans_sptr2 = est_spline2.estimate( set, *trans_sptr );
      err_sum = 0;
      test_pass = true;
      for ( unsigned i=0; i<num_data_pts; ++i ) {
        trans_sptr2->map_location( pts[i], map_to );
        double error = (map_to - to_pts[i]).two_norm();
        if ( error > 1e-4 ) {
          test_pass = false;
          std::cout << " point (" << pts[i] << ") is transformed to (" << map_to << ")\n"
                   << " true mapping points is (" << to_pts[i] << ')' << std::endl;
        }
        err_sum += error;
      }
      TEST("Test estimated mapping points",  test_pass, true);
      std::cout << " sum of error : " << err_sum << '\n';
      for ( unsigned i=0; i<dim; ++i )
        from_pt[ i ] = random.drand32( m1[i], m2[i] );
      trans_sptr2->map_location( from_pt, map_to );
      for ( unsigned i=0; i<dim; ++i )
        true_to[ i ] = from_pt[ i ] + splines[i]->f_x( from_pt );
      test_pass = (map_to - true_to).two_norm() < 0.01;
      if ( !test_pass ) {
        std::cout << " point (" << from_pt << ") is transformed to (" << map_to << ")\n"
                 << " true mapping points is (" << true_to << ")\n";
      }
      TEST("Test random point", test_pass, true);
    }
    std::cout << "--------------------------------------------------------------------------------\n";
  }

  void
  test_est_quadratic()
  {
    vnl_matrix<double> Q( 2, 3, 0.0 );
    vnl_matrix<double> A( 2, 2 );
    vnl_vector<double> t( 2 );
    vnl_matrix<double> covar( 12, 12 );

    Q(0,0) = 0.005; Q(0,1) = 0.008; Q(0,2) = 0.0004;
    Q(1,0) = 0.015;  Q(1,1) = 0.009; Q(1,2) = 0.0013;


    A(0,0) = 1.0;  A(0,1) = 0.0;
    A(1,0) = 0.0;  A(1,1) = 1.0;

    t[0] =  3.0;
    t[1] = -4.0;

    covar.set_identity();

    rgrl_estimator_sptr est = new rgrl_est_quadratic(2);
    rgrl_trans_quadratic quadratic_trans(Q, A, t, covar);

    std::vector< rgrl_feature_sptr > from_pts;
    std::vector< rgrl_feature_sptr > to_pts;

    for ( unsigned i=0; i < 9; ++i ) {
      vnl_vector<double> v = random_2d_vector();
      from_pts.   push_back( pf( v ) );
      to_pts.     push_back( pf( quadratic_trans.map_location(v)
                                 + random_2d_normal_error()/1000.0 ) );
      std::cout << "Datum " << i << ":   " << from_pts.back()->location() << "    --->   " << to_pts.back()->location() << '\n';
    }

    {
      rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
      for ( unsigned i=0; i < from_pts.size(); ++i ) {
        ms->add_feature_and_match( from_pts[i], nullptr, to_pts[i] );
      }

      rgrl_trans_quadratic dummy_trans(2);
      rgrl_transformation_sptr trans = est->estimate( ms, dummy_trans );
      TEST("Estimate Quadratic", !trans, false);
      if ( trans ) {
        TEST("Result is quadratic (is_type())", trans->is_type(rgrl_trans_quadratic::type_id()), true );
        rgrl_trans_quadratic* q_trans = dynamic_cast<rgrl_trans_quadratic*>(trans.as_pointer());
        TEST("Result is quadratic (dynamic_cast)", !q_trans, false );
        std::cout << "Estimated:\nQ=\n"<<q_trans->Q()<<"\nA=\n"<<q_trans->A()<<"\nt="<<q_trans->t()
                 << "\n\nTrue:\nQ=\n"<<Q<<"\nA=\n"<<A<<"\nt="<<t<<std::endl;
        TEST("Q is close", close( q_trans->Q(), Q, 0.01), true);
        TEST("A is close", close( q_trans->A(), A, 0.01), true);
        TEST("t is close", close( q_trans->t(), t, 0.01), true);
      }
    }
    std::cout << "--------------------------------------------------------------------------------\n";
  }

  void
  test_est_similarity2d()
  {
    vnl_matrix<double> A( 2, 2 );
    vnl_vector<double> t( 2 );
    vnl_matrix<double> covar( 4, 4 );

    A(0,0) = 0.5;  A(0,1) = -2.0;
    A(1,0) = 2.0;  A(1,1) = 0.5;

    t[0] =  3.0;
    t[1] = -4.0;

    covar.set_identity();

    rgrl_estimator_sptr est = new rgrl_est_similarity2d(2);
    rgrl_trans_similarity similarity_trans(A, t, covar);

    std::vector< rgrl_feature_sptr > from_pts;
    std::vector< rgrl_feature_sptr > to_pts;

    for ( unsigned i=0; i < 9; ++i ) {
      vnl_vector<double> v = random_2d_vector();
      from_pts.   push_back( pf( v ) );
      to_pts.     push_back( pf( similarity_trans.map_location(v)
                                 + random_2d_normal_error()/1000.0 ) );
      std::cout << "Datum " << i << ":   " << from_pts.back()->location() << "    --->   " << to_pts.back()->location() << '\n';
    }

    {
      rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
      for ( unsigned i=0; i < from_pts.size(); ++i ) {
        ms->add_feature_and_match( from_pts[i], nullptr, to_pts[i] );
      }

      rgrl_trans_similarity dummy_trans(2);
      rgrl_transformation_sptr trans = est->estimate( ms, dummy_trans );
      TEST("Estimate Similarity", !trans, false);
      if ( trans ) {
        TEST("Result is similarity (is_type())", trans->is_type(rgrl_trans_similarity::type_id()), true );
        rgrl_trans_similarity* s_trans = dynamic_cast<rgrl_trans_similarity*>(trans.as_pointer());
        TEST("Result is similarity (dynamic_cast)", !s_trans, false );
        std::cout << "Estimated:\nA=\n"<<s_trans->A()<<"\nt="<<s_trans->t()
                 << "\n\nTrue:\nA=\n"<<A<<"\nt="<<t<<std::endl;
        TEST("A is close", close( s_trans->A(), A, 0.01), true);
        TEST("t is close", close( s_trans->t(), t, 0.01), true);
      }
    }
    std::cout << "--------------------------------------------------------------------------------\n";
  }

  void
  test_est_reduced_quad2d()
  {
    vnl_matrix<double> Q( 2, 3, 0.0 );
    vnl_matrix<double> A( 2, 2 );
    vnl_vector<double> t( 2 );
    vnl_matrix<double> covar( 6, 6 );

    Q(0,0) = 0.005; Q(0,1) = 0.005; Q(0,2) = 0;
    Q(1,0) = 0.015;  Q(1,1) = 0.015; Q(1,2) = 0;


    A(0,0) = 0.5;  A(0,1) = -2.0;
    A(1,0) = 2.0;  A(1,1) = 0.5;

    t[0] =  3.0;
    t[1] = -4.0;

    covar.set_identity();

    rgrl_estimator_sptr est = new rgrl_est_reduced_quad2d(2);
    rgrl_trans_reduced_quad quadratic_trans(Q, A, t, covar);

    std::vector< rgrl_feature_sptr > from_pts;
    std::vector< rgrl_feature_sptr > to_pts;

    { //pt 1
      vnl_vector<double> v = vec2d( 2.0, 0.0);
      from_pts.   push_back( pf( v ) );
      to_pts.     push_back( pf( quadratic_trans.map_location(v)
                                 + random_2d_normal_error()/1000.0 ) );
    }
    { //pt 2
      vnl_vector<double> v = vec2d( -2.0, 0.0);
      from_pts.   push_back( pf( v ) );
      to_pts.     push_back( pf( quadratic_trans.map_location(v)
                                 + random_2d_normal_error()/1000.0 )  );
    }
    { //pt 3
      vnl_vector<double> v = vec2d( 0.0, 4.0);
      from_pts.   push_back( pf( v ) );
      to_pts.     push_back( pf( quadratic_trans.map_location(v)
                                 + random_2d_normal_error()/1000.0 )  );
    }
    { //pt 4
      vnl_vector<double> v = vec2d( 0.0, -4.0);
      from_pts.   push_back( pf( v ) );
      to_pts.     push_back( pf( quadratic_trans.map_location(v)
                                 + random_2d_normal_error()/1000.0 )  );
    }
    { //pt 5
      vnl_vector<double> v = vec2d( 10.0, 10.0);
      from_pts.   push_back( pf( v ) );
      to_pts.     push_back( pf( quadratic_trans.map_location(v)
                                 + random_2d_normal_error()/1000.0 )  );
    }
    { //pt 6
      vnl_vector<double> v = vec2d( -10.0, -10.0);
      from_pts.   push_back( pf( v ) );
      to_pts.     push_back( pf( quadratic_trans.map_location(v)
                                 + random_2d_normal_error()/1000.0 )  );
    }

    {
      rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
      for ( unsigned i=0; i < from_pts.size(); ++i ) {
        ms->add_feature_and_match( from_pts[i], nullptr, to_pts[i] );
      }

      rgrl_trans_quadratic dummy_trans(2);
      rgrl_transformation_sptr trans = est->estimate( ms, dummy_trans );
      TEST("Estimate Reduced Quadratic", !trans, false);
      if ( trans ) {
        TEST("Result is quadratic (is_type())", trans->is_type(rgrl_trans_reduced_quad::type_id()), true );
        rgrl_trans_reduced_quad* q_trans = dynamic_cast<rgrl_trans_reduced_quad*>(trans.as_pointer());
        TEST("Result is quadratic (dynamic_cast)", !q_trans, false);
        std::cout << "Estimated:\nQ=\n"<<q_trans->Q()<<"\nA=\n"<<q_trans->A()<<"\nt="<<q_trans->t()
                 << "\n\nTrue:\nQ=\n"<<Q<<"\nA=\n"<<A<<"\nt="<<t<<std::endl;
        TEST("Q is close", close( q_trans->Q(), Q, 0.01), true);
        TEST("A is close", close( q_trans->A(), A, 0.01), true);
        TEST("t is close", close( q_trans->t(), t, 0.01), true);
      }
    }
  }


  //: A test for the rigid transformation
  void
  test_est_rigid()
  {
    std::cerr<<"----Testing rigid xform estimator----\n";
    vnl_matrix<double> A( 3, 3 );
    vnl_vector<double> t( 3 );
    vnl_matrix<double> covar( 6, 6 );

    // rotations around {x,y,z} axes have angles {phi,alpha,theta} { aaa, bbb,c,}
    double theta=1.2, alpha=0, phi=0, ttxx=0, ttyy=0, ttzz=0;
    double cos_a = std::cos(alpha), sin_a = std::sin(alpha),
           cos_t = std::cos(theta), sin_t = std::sin(theta),
           cos_p = std::cos(phi),   sin_p = std::sin(phi);

    A(0,0) = cos_a*cos_t ;                   A(0,1) =-cos_a*sin_t;                    A(0,2) = sin_a;
    A(1,0) = cos_t*sin_a*sin_p+cos_p*sin_t;  A(1,1) =-sin_a*sin_p*sin_t+cos_p*cos_t;  A(1,2) =-cos_a*sin_p;
    A(2,0) =-cos_p*cos_t*sin_a+sin_p*sin_t;  A(2,1) = cos_p*sin_a*sin_t+cos_t*sin_p;  A(2,2) = cos_a*cos_p;

    t[0] =  ttxx;
    t[1] =  ttyy;
    t[2] =  ttzz;

    std::cerr<<"Goal rotation is\n---\n"<<A<<"\n----\n\n"
            <<"Goal translation is\n---\n"<<t<<"\n----\n\n";

    covar.set_identity();

    // create estimator object with dimension=3
    rgrl_estimator_sptr est = new rgrl_est_rigid(3);
    rgrl_trans_rigid rigid_trans(A, t, covar);

    std::vector< rgrl_feature_sptr > from_pts;
    std::vector< rgrl_feature_sptr > to_pts;

    vnl_vector<double> temp(3);
    temp[0]=0;
    temp[1]=0;
    temp[2]=0;
    from_pts.push_back( pf( temp ) );
    to_pts.push_back  ( pf( rigid_trans.map_location(temp) ) );

    temp[0]=0;
    temp[1]=1;
    temp[2]=0;
    from_pts.push_back( pf( temp ) );
    to_pts.push_back  ( pf( rigid_trans.map_location(temp) ) );

    temp[0]=0;
    temp[1]=2;
    temp[2]=0;
    from_pts.push_back( pf( temp ) );
    to_pts.push_back  ( pf( rigid_trans.map_location(temp) ) );

    temp[0]=0;
    temp[1]=3;
    temp[2]=0;
    from_pts.push_back( pf( temp ) );
    to_pts.push_back  ( pf( rigid_trans.map_location(temp) ) );

    temp[0]=0;
    temp[1]=4;
    temp[2]=0;
    from_pts.push_back( pf( temp ) );
    to_pts.push_back  ( pf( rigid_trans.map_location(temp) ) );


#if 0
    for ( unsigned i=0; i < 9; ++i ) {
      vnl_vector<double> v = random_3d_vector();
      from_pts.   push_back( pf( v ) );
      to_pts.     push_back( pf( rigid_trans.map_location(v)
                                 + random_3d_normal_error()/1000.0 ) );
      std::cout << "Datum " << i << ":   " << from_pts.back()->location() << "    --->   " << to_pts.back()->location() << '\n';
    }
#endif

    {
      rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
      for ( unsigned i=0; i < from_pts.size(); ++i ) {
        ms->add_feature_and_match( from_pts[i], nullptr, to_pts[i] );
    }

      rgrl_trans_rigid dummy_trans(3);
      rgrl_transformation_sptr trans = est->estimate( ms, dummy_trans );
      TEST("Estimate Rigid", !trans, false);

      if ( trans )
      {
        TEST("Result is rigid (is_type())", trans->is_type(rgrl_trans_rigid::type_id()), true );
        rgrl_trans_rigid* s_trans = dynamic_cast<rgrl_trans_rigid*>(trans.as_pointer());
        TEST("Result is rigid (dynamic_cast)", !s_trans, false);
        std::cout << "Estimated:\nR=\n"<<s_trans->R()<<"\nt="<<s_trans->t()
                 << "\n\nTrue:\nR=\n"<<A<<"\nt="<<t<<std::endl;
        TEST("R is close", close( s_trans->R(), A), true);
        TEST("t is close", close( s_trans->t(), t), true);
      }
    }
  }

  void test_homography2d()
  {
    vnl_double_3x3 H(0.0), est_H(0.0);
    vnl_matrix<double> cofact;
    std::vector <int> indices;
    std::vector <vnl_double_3 > p,q;
    std::vector <vnl_double_2 > d2_p,d2_q;
    vnl_vector<double> param(9,0.0);
    vnl_vector<double> true_param(9,0.0), est_param(9,0.0);
    const double pi = vnl_math::pi;
    const double tol = 1e-8;
    vnl_double_3 t(0,0,1);

    p.push_back(t);

    //first 4 points are collinear.
    t(0) = 2; t(1) = 5;
    p.push_back(t);

    t(0) = 4; t(1) = 9;
    p.push_back(t);

    t(0) = -1; t(1) = -1;
    p.push_back(t);

    t(0) = -3; t(1) = -5;
    p.push_back(t);

    t(0) = -9; t(1) = .5;
    p.push_back(t);

    t(0) = 5; t(1) = -5.678;
    p.push_back(t);

    t(0) = 5/3; t(1) = -5.678/3;
    p.push_back(t);

    t(0) = 0.4/0.1; t(1) = 0.894/0.1; ;
    p.push_back(t);

    t(0) = 5; t(1) = -5.678; t(2) = 3;
    p.push_back(t);

    t(0) = 0.4; t(1) = 0.894; t(2) = 0.1;
    p.push_back(t);

    t(0) = 500; t(1) = -100; t(2) = 100;
    p.push_back(t);

    t(0) = -20; t(1) = -20; t(2) = 1;
    p.push_back(t);

    t(0) = 4; t(1) = 0.02; t(2) = 1.5;
    p.push_back(t);

    t(0) = 2.345; t(1) = -10; t(2) = 1;
    p.push_back(t);

    t(0) = 8.9e-4; t(1) = -3.1e-4; t(2) = -1e-4;
    p.push_back(t);

    t(0) = -10; t(1) = 40; t(2) = 1;
    p.push_back(t);

    unsigned int n = p.size();
    q.resize(n);
    d2_q.resize(n);
    d2_p.resize(n);

    // Test projective transform
    H(0,2) = -4;
    H(1,2) = 2;
    H(2,2) = 1;
    H(0,0) = 2*std::cos(pi/3);
    H(0,1) = -5; H(1,1) = -1.5;
    H(1,0) = -H(0,1);
    H(2,0) = 0.05; H(2,1) = -.2;

    for (int i=0;i<3;i++)
      for (int j=0;j<3;j++)
        true_param[i*3+j] = H(i,j);
    true_param /= true_param.two_norm();
    std::cout<<"Original H = "<<true_param<<std::endl;

    {
      // generate the corresponding points
      for (unsigned int i=0;i<n;++i) {
        q[i] = H *p[i];
        d2_p[i][0] = p[i][0]/p[i][2];
        d2_p[i][1] = p[i][1]/p[i][2];
        d2_q[i][0] = q[i][0]/q[i][2];
        d2_q[i][1] = q[i][1]/q[i][2];
      }
      rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id());

      for (unsigned i=0; i < n; ++i) {
        ms->add_feature_and_match( new rgrl_feature_point(d2_p[i].as_ref()), nullptr,
                                   new rgrl_feature_point(d2_q[i].as_ref()) );
      }
      rgrl_estimator_sptr estimator = new rgrl_est_homography2d();
      rgrl_transformation_sptr dummy_trans = new rgrl_trans_homography2d();
      rgrl_transformation_sptr est = estimator->estimate( ms, *dummy_trans);
      rgrl_trans_homography2d* homo_est = rgrl_cast<rgrl_trans_homography2d*>(est.as_pointer());
      est_H = homo_est->H();
      for (int i=0;i<3;i++)
        for (int j=0;j<3;j++)
          est_param[i*3+j] = est_H(i,j);
      est_param /= est_param.two_norm();
      if ( est_param[0] < 0 ) est_param *= -1;

      std::cout<<"Estimated H = "<<est_param<<std::endl;
      TEST("Estimation of Projective xform", (est_param-true_param).two_norm() <tol ||
                                             (est_param+true_param).two_norm() <tol, true);

      // Test inverse_mapping
      bool initialize_next = false;
      vnl_vector<double> to_delta;
      vnl_vector<double> from(2,5);
      vnl_vector<double> from_next_est;
      homo_est->inv_map( d2_q[3].as_ref(), initialize_next, to_delta, from, from_next_est);
      std::cout<<"from = "<<from<<std::endl;
      TEST("Test incremental inverse mapping", (from-d2_p[3]).two_norm() <0.1, true);
      homo_est->inv_map( d2_q[3].as_ref(), from);
      TEST("Test inverse mapping", (from-d2_p[3]).two_norm() <tol, true);
    }
  }

  void test_homography2d_lm(rgrl_estimator_sptr estimator)
  {
    vnl_double_3x3 H(0.0), est_H(0.0);
    vnl_matrix<double> cofact;
    std::vector <int> indices;
    std::vector <vnl_double_3 > p,q;
    std::vector <vnl_double_2 > d2_p,d2_q;
    vnl_vector<double> param(9,0.0);
    vnl_vector<double> true_param(9,0.0), est_param(9,0.0);
    const double pi = vnl_math::pi;
    const double tol = 1e-8;
    vnl_double_3 t(0,0,1);

    p.push_back(t);

    //first 4 points are collinear.
    t(0) = 2; t(1) = 5;
    p.push_back(t);

    t(0) = 4; t(1) = 9;
    p.push_back(t);

    t(0) = -1; t(1) = -1;
    p.push_back(t);

    t(0) = -3; t(1) = -5;
    p.push_back(t);

    t(0) = -9; t(1) = .5;
    p.push_back(t);

    t(0) = 5; t(1) = -5.678;
    p.push_back(t);

    t(0) = 5/3; t(1) = -5.678/3;
    p.push_back(t);

    t(0) = 0.4/0.1; t(1) = 0.894/0.1; ;
    p.push_back(t);

    t(0) = 5; t(1) = -5.678; t(2) = 3;
    p.push_back(t);

    t(0) = 0.4; t(1) = 0.894; t(2) = 0.1;
    p.push_back(t);

    t(0) = 500; t(1) = -100; t(2) = 100;
    p.push_back(t);

    t(0) = -20; t(1) = -20; t(2) = 1;
    p.push_back(t);

    t(0) = 4; t(1) = 0.02; t(2) = 1.5;
    p.push_back(t);

    t(0) = 2.345; t(1) = -10; t(2) = 1;
    p.push_back(t);

    t(0) = 8.9e-4; t(1) = -3.1e-4; t(2) = -1e-4;
    p.push_back(t);

    t(0) = -10; t(1) = 40; t(2) = 1;
    p.push_back(t);

    unsigned int n = p.size();
    q.resize(n);
    d2_q.resize(n);
    d2_p.resize(n);

    // Test projective transform
    H(0,2) = -4;
    H(1,2) = 2;
    H(2,2) = 1;
    H(0,0) = 2*std::cos(pi/3);
    H(0,1) = -5; H(1,1) = -1.5;
    H(1,0) = -H(0,1);
    H(2,0) = 0.5; H(2,1) = -2;
    H /= H.array_two_norm();

    std::cout<<"Original H =\n"<< H <<std::endl;

    {
      // generate the corresponding points
      for (unsigned int i=0;i<n;++i) {
        q[i] = H *p[i];
        d2_p[i][0] = p[i][0]/p[i][2];
        d2_p[i][1] = p[i][1]/p[i][2];
        d2_q[i][0] = q[i][0]/q[i][2];
        d2_q[i][1] = q[i][1]/q[i][2];
      }
      rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id());

      for (unsigned i=0; i < n; ++i) {
        ms->add_feature_and_match( new rgrl_feature_point(d2_p[i].as_ref()), nullptr,
                                   new rgrl_feature_point(d2_q[i].as_ref()) );
      }

      {
        vnl_double_3x3 perturbed_H( H );
        perturbed_H( 0, 0 ) += 1;
        rgrl_transformation_sptr init_trans = new rgrl_trans_homography2d( perturbed_H.as_ref() );
        rgrl_transformation_sptr est = estimator->estimate( ms, *init_trans);
        rgrl_trans_homography2d* homo_est = rgrl_cast<rgrl_trans_homography2d*>(est.as_pointer());
        est_H = homo_est->H();
        est_H /= est_H.array_two_norm();
        if ( est_H(0,0) < 0 ) est_H *= -1;

        std::cout<<"Estimated H =\n"<<est_H<<std::endl;
        TEST_NEAR("[Fixed error in (0,0)] Estimation of Projective xform", (est_H-H).array_two_norm(), 0.0, tol);

        // test on transfer error
        vnl_vector<double> pt(2);
        vnl_double_2x2 trans_error = est->transfer_error_covar( inhomo(p[2].as_ref()) );
        std::cout << "transfer error at this point:" << trans_error << std::endl;
      }
      // error STD = 0.5
      {
        vnl_double_3x3 perturbed_H( H );
        constexpr double err_std = 0.5;
        for ( unsigned i=0; i<3; ++i )
          for ( unsigned j=0; j<3; ++j )
            perturbed_H( 0, 0 ) += random.drand32()*err_std;
        rgrl_transformation_sptr init_trans = new rgrl_trans_homography2d( perturbed_H.as_ref() );
        rgrl_transformation_sptr est = estimator->estimate( ms, *init_trans);
        rgrl_trans_homography2d* homo_est = rgrl_cast<rgrl_trans_homography2d*>(est.as_pointer());
        est_H = homo_est->H();
        est_H /= est_H.array_two_norm();
        if ( est_H(0,0) < 0 ) est_H *= -1;

        std::cout<<"Estimated H =\n"<<est_H<<std::endl;
        TEST_NEAR("[Random error with scale 0.5] Estimation of Projective xform",
                  (est_H-H).array_two_norm(), 0.0, tol);
      }
      // error STD = 1
      {
        vnl_double_3x3 perturbed_H( H );
        constexpr double err_std = 1;
        for ( unsigned i=0; i<3; ++i )
          for ( unsigned j=0; j<3; ++j )
            perturbed_H( 0, 0 ) += random.drand32()*err_std;
        rgrl_transformation_sptr init_trans = new rgrl_trans_homography2d( perturbed_H.as_ref() );
        rgrl_transformation_sptr est = estimator->estimate( ms, *init_trans);
        rgrl_trans_homography2d* homo_est = rgrl_cast<rgrl_trans_homography2d*>(est.as_pointer());
        est_H = homo_est->H();
        est_H /= est_H.array_two_norm();
        if ( est_H(0,0) < 0 ) est_H *= -1;

        std::cout<<"Estimated H =\n"<<est_H<<std::endl;
        TEST_NEAR("[Random error with scale 1] Estimation of Projective xform",
                  (est_H-H).array_two_norm(), 0.0, tol);
      }
      // error STD = 50
      {
        vnl_double_3x3 perturbed_H( H );
        constexpr double err_std = 50;
        for ( unsigned i=0; i<3; ++i )
          for ( unsigned j=0; j<3; ++j )
            perturbed_H( 0, 0 ) += random.drand32()*err_std;
        rgrl_transformation_sptr init_trans = new rgrl_trans_homography2d( perturbed_H.as_ref() );
        rgrl_transformation_sptr est = estimator->estimate( ms, *init_trans);
        rgrl_trans_homography2d* homo_est = rgrl_cast<rgrl_trans_homography2d*>(est.as_pointer());
        est_H = homo_est->H();
        est_H /= est_H.array_two_norm();
        if ( est_H(0,0) < 0 ) est_H *= -1;

        std::cout<<"Estimated H =\n"<<est_H<<std::endl;
        TEST_NEAR("[Random error with scale 50] Estimation of Projective xform",
                  (est_H-H).array_two_norm(), 0.0, tol);
      }
    }
  }

  inline
  void
  inhomo_map( vnl_double_2& mapped, vnl_double_3x3 const& H, vnl_double_2 const& p )
  {
    static vnl_double_3 homo(0.0,0.0,1.0);
    homo[0]=p[0];
    homo[1]=p[1];
    vnl_double_3 homo_mapped = H*homo;

    // division
    mapped[0] = homo_mapped[0]/homo_mapped[2];
    mapped[1] = homo_mapped[1]/homo_mapped[2];
  }

  void
  test_homography2d_points_on_circle(rgrl_estimator_sptr estimator)
  {
    vnl_double_3x3 H;
    H.set_identity();
    const double tol = 1e-6;

    // Test projective transform
    H(0,2) = -4;
    H(1,2) = 2;
    H(2,2) = 1;
    H(0,0) = 2*std::cos(vnl_math::pi/3);
    H(0,1) = -5; H(1,1) = -1.5;
    H(1,0) = -H(0,1);
    H(2,0) = 0.5; H(2,1) = -3;
    H /= H.array_two_norm();

    std::cout<<"Original H =\n"<< H <<std::endl;

    std::vector< vnl_double_2 > p, q;


    vnl_double_2 pt, mapped;
    constexpr double c = 10;
    bool xform_is_good=true;

    for ( unsigned int num=20; num<200; num+=16 )
    {
      std::cout << "num of points = " << num << "   ";

      //reset points
      p.clear();
      q.clear();
      // create points on a circle
      for ( unsigned int i=0; i<num; ++i ) {
        const double angle = vnl_math::twopi*double(i)/double(num);
        pt[0] = c*std::cos(angle);
        pt[1] = c*std::sin(angle);
        p.push_back(pt);
        // Map point
        inhomo_map( mapped, H, pt );
        q.push_back(mapped);
      }

      // create match set
      rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id());

      for (unsigned int i=0; i < num; ++i) {
        ms->add_feature_and_match( new rgrl_feature_point(p[i].as_ref()), nullptr,
                                   new rgrl_feature_point(q[i].as_ref()) );
      }

      // error STD = 1
      {
        vnl_double_3x3 perturbed_H( H ), est_H;
        const double err_std = 1e-3;
        for ( unsigned i=0; i<3; ++i )
          for ( unsigned j=0; j<3; ++j )
            perturbed_H( i, j ) += random.drand32()*err_std;
        rgrl_transformation_sptr init_trans = new rgrl_trans_homography2d( perturbed_H.as_ref() );

        //estimate
        rgrl_transformation_sptr est = estimator->estimate( ms, *init_trans);
        rgrl_trans_homography2d* homo_est = rgrl_cast<rgrl_trans_homography2d*>(est.as_pointer());
        est_H = homo_est->H();
        est_H /= est_H.array_two_norm();
        if ( est_H(0,0) < 0 ) est_H *= -1;
        if ( (est_H-H).array_two_norm() > tol) {
          std::cout<<"Estimated H = "<<est_H<<std::endl;
          xform_is_good = false;
        }
      }
    }
    TEST("Estimation of Projective xform", xform_is_good, true);
  }

  void
  test_homo2d_rad_points_on_circle(rgrl_estimator_sptr estimator,
                                   vnl_double_2 const& camera_centre)
  {
    vnl_double_3x3 H;
    const double tol = 1e-6;

    // Test projective transform
    H(0,2) = -4;
    H(1,2) = 2;
    H(2,2) = 1;
    H(0,0) = 2*std::cos(vnl_math::pi/3);
    H(0,1) = -5; H(1,1) = -1.5;
    H(1,0) = -H(0,1);
    H(2,0) = 0.5; H(2,1) = -3;
     H /= H.array_two_norm();

    std::vector<double> radk( 1, 1e-4 );

    std::cout<<"Original H =\n"<< H
            << "with k: " << radk[0]
            << " at camera centre " << camera_centre
            <<std::endl;

    std::vector< vnl_double_2 > p, q;


    vnl_double_2 pt, mapped, centre_mapped;
    constexpr double c = 20;
    bool xform_is_good=true;

    for ( unsigned int num=12; num<100; num+=16 )
    {
      std::cout << "num of points = " << num << "   ";

      //reset points
      p.clear();
      q.clear();
      // create points on a circle
      for ( unsigned int i=0; i<num; ++i ) {
        const double angle = vnl_math::twopi*double(i)/double(num);
        pt[0] = c*std::cos(angle);
        pt[1] = c*std::sin(angle);
        p.push_back(pt);
        // Map point
        inhomo_map( mapped, H, pt );
        centre_mapped = mapped - camera_centre;
        mapped += radk[0]*centre_mapped.squared_magnitude()*centre_mapped;
        q.push_back(mapped);
      }

      // create match set
      rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id());

      for (unsigned int i=0; i < num; ++i) {
        ms->add_feature_and_match( new rgrl_feature_point(p[i].as_ref()), nullptr,
                                   new rgrl_feature_point(q[i].as_ref()) );
      }

      // error STD = 1
      {
        vnl_double_3x3 perturbed_H( H ), est_H;
        const double err_std = 2e-3;
        for ( unsigned i=0; i<3; ++i )
          for ( unsigned j=0; j<3; ++j )
            perturbed_H( i, j ) += random.drand32()*err_std;
        rgrl_transformation_sptr init_trans = new rgrl_trans_homography2d( perturbed_H.as_ref() );

        //estimate
        rgrl_transformation_sptr est = estimator->estimate( ms, *init_trans);
        rgrl_trans_homo2d_proj_rad* homo_est
          = rgrl_cast<rgrl_trans_homo2d_proj_rad*>(est.as_pointer());
        est_H = homo_est->H();
        est_H /= est_H.array_two_norm();
        if ( est_H(0,0) < 0 ) est_H *= -1;

        if ( (est_H-H).array_two_norm() > tol) {
          std::cout<<"Incorrect estimated H = "<<est_H<<std::endl;
          xform_is_good = false;
        }
        std::vector<double> radk_est = homo_est->radial_params();
        if ( radk_est.size() != radk.size() || std::abs( radk_est[0] - radk[0]) > tol ) {
          std::cout << "Incorrect estimated k1 = ";
          std::copy( radk_est.begin(), radk_est.end(), std::ostream_iterator<double>( std::cout, " " ) );
          std::cout << std::endl;

          xform_is_good = false;
        }
      }
    }
    TEST("Estimation of Projective xform with rad distortion", xform_is_good, true);
  }

  void
  test_homography2d_points_on_circle_w_noise( rgrl_estimator_sptr estimator,
                                              double noise_level,
                                              double tol_xform,
                                              double tol_trans_error )
  {
    vnl_double_3x3 H;
    // Test projective transform
    H.set_identity();
    H(0,0) = 1.5;
    H(1,1) = 1.5;
    H /= H.array_two_norm();

    std::cout<<"Original H =\n"<< H <<std::endl;

    std::vector< vnl_double_2 > p, q;

    vnl_double_2 pt, mapped;
    constexpr double c = 20;
    bool cov_is_good = true;
    bool homography_is_good = true;
    for ( unsigned int num=12; num<200; num+=16 )
    {
      std::cout << "using " << num << " out of 200 correspondences.\n";
      //reset points
      p.clear();
      q.clear();

      // create points on a circle
      for ( unsigned int i=0; i<num; ++i )
      {
        const double angle = vnl_math::twopi*double(i)/double(num);
        pt[0] = c*std::cos(angle);
        pt[1] = c*std::sin(angle);
        p.push_back(pt);

        // Map point
        inhomo_map( mapped, H, pt );

        // add noise
        mapped[0] += noise_level * random.normal();
        mapped[1] += noise_level * random.normal();
        q.push_back(mapped);
      }

      // create match set
      rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id());

      for (unsigned int i=0; i < num; ++i) {
        ms->add_feature_and_match( new rgrl_feature_point(p[i].as_ref()), nullptr,
                                   new rgrl_feature_point(q[i].as_ref()) );
      }

      // error STD = 3
      {
        vnl_double_3x3 perturbed_H( H ), est_H;
        vnl_matrix<double> iid_cov(2,2,vnl_matrix_identity);
        const double err_std = 5e-4;
        for ( unsigned i=0; i<3; ++i )
          for ( unsigned j=0; j<3; ++j )
            perturbed_H( i, j ) += random.drand32()*err_std;
        perturbed_H( 2, 2 ) = H(2,2);  // no change to the bottom-left element.
        rgrl_transformation_sptr init_trans = new rgrl_trans_homography2d( perturbed_H.as_ref() );

        // estimate
        rgrl_transformation_sptr est = estimator->estimate( ms, *init_trans);
        if (!est) {
          TEST("rgrl_estimator::estimate() returns zero pointer!!", 0, 1);
          homography_is_good = false;
          break;
        }
        rgrl_trans_homography2d* homo_est = rgrl_cast<rgrl_trans_homography2d*>(est.as_pointer());
        est_H = homo_est->H();
        est_H /= est_H.array_two_norm();
        if ( est_H(0,0) < 0 ) est_H *= -1;

        // check homography transform
        homography_is_good = homography_is_good && (est_H-H).array_two_norm() < tol_xform;
        if ( !homography_is_good ) {
          std::cout << "transformation is incorrect!\n" << est_H << '\n' << H << std::endl;
        }

        // test on transfer error on 4 canonical directions
        vnl_matrix<double> trans_error;
        double ratio;

        trans_error = est->transfer_error_covar( p[0].as_ref() );
        ratio = trans_error(0,0);  // this constant shall be the same
        std::cout << "transfer error(1):\n" << trans_error << std::endl;
        if ((ratio*iid_cov-trans_error).array_two_norm() > tol_trans_error) {
          cov_is_good = false;
          std::cout << "*** correct=\n"<< ratio*iid_cov << std::endl;
        }

        trans_error = est->transfer_error_covar( p[num/4].as_ref() );
        std::cout << "transfer error(1):\n" << trans_error << std::endl;
        if ((ratio*iid_cov-trans_error).array_two_norm() > tol_trans_error) {
          cov_is_good = false;
          std::cout << "*** correct=\n"<< ratio*iid_cov << std::endl;
        }

        trans_error = est->transfer_error_covar( p[num/2].as_ref() );
        std::cout << "transfer error(1):\n" << trans_error << std::endl;
        if ((ratio*iid_cov-trans_error).array_two_norm() > tol_trans_error) {
          cov_is_good = false;
          std::cout << "*** correct=\n"<< ratio*iid_cov << std::endl;
        }

        trans_error = est->transfer_error_covar( p[num*3/4].as_ref() );
        std::cout << "transfer error(1):\n" << trans_error << std::endl;
        if ((ratio*iid_cov-trans_error).array_two_norm() > tol_trans_error) {
          cov_is_good = false;
          std::cout << "*** correct=\n"<< ratio*iid_cov << ", error= "<< trans_error<< std::endl;
        }
      }
    }

    TEST("[i.i.d error in coordinates] Estimation of Projective xform", homography_is_good, true );
    TEST("transfer error shall be i.i.d", cov_is_good, true );
  }
} // end anonymous namespace

void
test_rad_dis_homo2d_lm()
{
  vnl_double_3x3 H;  H.set_identity();
  vnl_double_3x3 initH(H);
  const double k1_from = -1e-6;
  constexpr double k1_to = 0;
  vnl_vector<double> centre(2,0.0);
  rgrl_transformation_sptr true_xform = new rgrl_trans_rad_dis_homo2d( H.as_ref(), k1_from, k1_to, centre, centre );

  // add errors
  initH(1,1) = 0.98;
  initH(0,2) = -0.005;
  initH(1,2) = 0.1;
  rgrl_transformation_sptr init_xform = new rgrl_trans_homography2d( initH.as_ref() );

  rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
  vnl_vector<double> pt_loc( 2 );
  for ( int i=-10; i<=10; ++i )
    for ( int j=-10; j<=10; ++j ) {
      pt_loc[0] = double(i)*50;
      pt_loc[1] = double(j)*40;
      rgrl_feature_sptr from_sptr = new rgrl_feature_point( pt_loc );
      rgrl_feature_sptr to_sptr = from_sptr->transform( *true_xform );

      // add feature
      ms->add_feature_and_match( from_sptr, nullptr, to_sptr );
    }

  // remap features
  ms->remap_from_features( *init_xform );

  // set up estimator
  rgrl_estimator_sptr est_sptr = new rgrl_est_dis_homo2d_lm(centre, centre);

  rgrl_transformation_sptr result_sptr = est_sptr->estimate( ms, *init_xform );
  TEST("rgrl_est_dis_homo2d_lm::estimate(): valid transformation", bool(result_sptr), true );
}


static void test_estimator()
{
  test_est_affine_pt_to_pt();
  test_est_affine_pt_to_line();
#if 0 // no similarity_3d yet
  test_similarity_pt_to_pt();
#endif // 0

  std::cout << "\n=== test_rad_dis_homo2d_lm ===\n";
  test_rad_dis_homo2d_lm();
  std::cout << "\n=== test_est_spline ===\n";
  test_est_spline();
  std::cout << "\n=== test_est_spline_reduce_dof ===\n";
  test_est_spline_reduce_dof();
  std::cout << "\n=== test_est_quadratic ===\n";
  test_est_quadratic();
  std::cout << "\n=== test_est_similarity2d ===\n";
  test_est_similarity2d();
  std::cout << "\n=== test_est_reduced_quad2d ===\n";
  test_est_reduced_quad2d();
  std::cout << "\n=== test_est_rigid ===\n";
  test_est_rigid();
  std::cout << "\n=== test_homography2d ===\n";
  test_homography2d();

  // old homography estimator
  {
    std::cout << "\n=== (old) test_homography2d_lm ===\n";
    rgrl_estimator_sptr estimator = new rgrl_est_homo2d_lm();
    estimator->set_debug_flag(5);
    test_homography2d_lm( estimator );

    std::cout << "\n=== test_homography2d_points_on_circle ===\n";
    test_homography2d_points_on_circle( estimator );
    std::cout << "\n=== test_homography2d_points_on_circle_w_noise ===\n";
    test_homography2d_points_on_circle_w_noise(estimator, 0.0, 1e-9, 1e-9);
    std::cout << "\n=== test_homography2d_points_on_circle_w_noise ===\n";
    test_homography2d_points_on_circle_w_noise(estimator, 0.1, 0.1, 0.005);
  }

  // new homography estimator
  {
    std::cout << "\n=== (new) test_homography2d_lm ===\n";
    rgrl_estimator_sptr estimator = new rgrl_est_homo2d_proj();
    //estimator->set_debug_flag(5);
    test_homography2d_lm( estimator);

    std::cout << "\n=== test_homography2d_points_on_circle ===\n";
    test_homography2d_points_on_circle( estimator );
    std::cout << "\n=== test_homography2d_points_on_circle_w_noise ===\n";
    test_homography2d_points_on_circle_w_noise(estimator, 0.0, 1e-9, 1e-9);
    std::cout << "\n=== test_homography2d_points_on_circle_w_noise ===\n";
    test_homography2d_points_on_circle_w_noise(estimator, 0.1, 0.1, 0.005);
  }

  // new homography+radial distortion estimator
  {
    std::cout << "\n=== (new) test_homo2d_rad_points_on_circle ===\n";
    vnl_double_2 camera_centre( 0.0, 0.0 );
    std::cout << "using camera centre " << camera_centre << std::endl;

    rgrl_est_homo2d_proj_rad* homo2d_rad_est
      = new rgrl_est_homo2d_proj_rad( 1, camera_centre );
    homo2d_rad_est->set_rel_thres( 1e-6 );
    homo2d_rad_est->set_max_num_iter( 5000 );
    //estimator->set_debug_flag(5);

    rgrl_estimator_sptr estimator = homo2d_rad_est;
    test_homo2d_rad_points_on_circle( estimator, camera_centre );
  }
  // new homography+radial distortion estimator
  {
    std::cout << "\n=== (new) test_homo2d_rad_points_on_circle ===\n";
    vnl_double_2 camera_centre( 5.0, 10.0 );
    std::cout << "using camera centre " << camera_centre << std::endl;

    rgrl_est_homo2d_proj_rad* homo2d_rad_est
      = new rgrl_est_homo2d_proj_rad( 1, camera_centre );
    homo2d_rad_est->set_rel_thres( 1e-6 );
    homo2d_rad_est->set_max_num_iter( 5000 );
    //estimator->set_debug_flag(5);

    rgrl_estimator_sptr estimator = homo2d_rad_est;
    test_homo2d_rad_points_on_circle( estimator, camera_centre );
  }
}

TESTMAIN(test_estimator);
