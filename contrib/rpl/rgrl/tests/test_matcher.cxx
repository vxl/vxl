#include <testlib/testlib_test.h>

#include <vcl_algorithm.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vul/vul_sprintf.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_2.h>

#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_matcher_k_nearest_pick_one.h>
#include <rgrl/rgrl_matcher_k_nearest_adv.h>
#include <rgrl/rgrl_matcher_k_nearest_boundary.h>
#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_set_bins_2d.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_scale.h>
#include <rgrl/rgrl_matcher_sptr.h>

#include "test_util.h"

namespace {

  void test_matcher_k_nearest_boundary() {

    // create the transformation
    vnl_matrix<double> A( 2, 2 );
    vnl_vector<double> t( 2 );
    vnl_double_2 diff1(12.0, 0.0), diff2(0.0, 12.0);
    vnl_matrix<double> covar( 6, 6 );

    A(0,0) = 1.0;  A(0,1) = 0.0;
    A(1,0) = 0.0;  A(1,1) = 1.0;

    t[0] =  3.0;
    t[1] = -4.0;

    covar.set_identity();

    rgrl_transformation_sptr trans = new rgrl_trans_affine(A, t, covar);

    rgrl_mask_sptr roi = new rgrl_mask_box(vnl_double_2(0, -50), vnl_double_2(50, 50));
    rgrl_estimator_sptr est_p = new rgrl_est_affine;
    rgrl_view_sptr view = new rgrl_view( roi, roi, roi->bounding_box(), roi->bounding_box(), est_p, trans, 0 );
    rgrl_scale_sptr scale = new rgrl_scale();

    // Create a vector of rgrl_features of the appropriate type.
    //
    vcl_vector<rgrl_feature_sptr> from_pts, to_pts;
    
    from_pts.push_back(new rgrl_feature_trace_pt( vnl_double_2(10.0, 20.0), 
                                                  vnl_double_2(0.0, 1.0),
                                                  5, 5));
    from_pts.push_back(new rgrl_feature_trace_pt( vnl_double_2(35.0, -20.0), 
                                                  vnl_double_2(1.0, 0.0), 
                                                  5, 5)); 
    to_pts.push_back(new rgrl_feature_trace_pt( vnl_double_2( 25.0, 16.0), 
                                                vnl_double_2(0.0, 1.0),
                                                5, 5));
    to_pts.push_back(new rgrl_feature_trace_pt( vnl_double_2(38.0, -12.0), 
                                                vnl_double_2(-1.0, 0.0), 
                                                5, 5)); 

    // Create feature sets from the vector of features.
    //
    rgrl_feature_set_sptr from_set, to_set;
    from_set = new rgrl_feature_set_bins_2d( from_pts );
    to_set = new rgrl_feature_set_bins_2d( to_pts );

    rgrl_matcher_sptr matcher = new rgrl_matcher_k_nearest_boundary(1);
    rgrl_match_set_sptr match_set =  matcher->compute_matches( *from_set, *to_set,
                                                               *view, *trans, *scale);
    testlib_test_begin( "Correct number of matches" );
    testlib_test_perform( match_set->from_size() == 4 );
    
    vcl_vector<vnl_vector<double> > from_bd_locs;
    vcl_vector<vnl_vector<double> > to_bd_locs;
    vcl_vector<vnl_vector<double> > xformed_bd_locs;

    typedef rgrl_match_set::from_iterator FIter;
    typedef FIter::to_iterator TIter;

    for( FIter fi = match_set->from_begin(); fi != match_set->from_end(); ++fi ) {
      for( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
        from_bd_locs.push_back( fi.from_feature()->location() );
        to_bd_locs.push_back( ti.to_feature()->location() );
        xformed_bd_locs.push_back(A*fi.from_feature()->location() +t);
      }
    }

    testlib_test_begin( "Correct boundary points" );
    testlib_test_perform( close( from_bd_locs[0], vnl_double_2(5, 20)) &&
                          close( from_bd_locs[1], vnl_double_2(15, 20)) );

    testlib_test_begin( "Correct boundary matches, same side" );
    testlib_test_perform( close( to_bd_locs[0], xformed_bd_locs[0]+diff1 ) && 
                          close( to_bd_locs[1], xformed_bd_locs[1]+diff1 ));
   
    testlib_test_begin( "Correct boundary matches, opposite side" );
    testlib_test_perform( close( to_bd_locs[2], xformed_bd_locs[2]+diff2 ) && 
                          close( to_bd_locs[3], xformed_bd_locs[3]+diff2 ));
   
  }
  
  struct dist_id {
    double dist_;
    unsigned id_;
    
    dist_id()
    : dist_(-1), id_(0)
    {}
    
    dist_id( double d, unsigned id )
    : dist_(d), id_(id)
    { }
    
    bool operator< (dist_id const& rhs ) const 
    { return this->dist_ < rhs.dist_; }

    bool operator== (dist_id const& rhs ) const 
    { return this->dist_ == rhs.dist_; }
  };

  void test_matcher_k_nearest() {

    // the points layout
    //
    //    t1(0,0)                                           t2(15,0)
    //                 m1(4, 4)                                             m2(25,4)
    //
    //
    //    t3(0, 10)                                         t4(15, 10)
    //                 m3(4, 16)                                                   m4(30, 30)
    //
    
    // create the transformation
    vnl_matrix<double> A( 2, 2 );
    vnl_vector<double> t( 2 );
    vnl_double_2 diff1(12.0, 0.0), diff2(0.0, 12.0);
    vnl_matrix<double> covar( 6, 6 );

    A(0,0) = 1.0;  A(0,1) = 0.0;
    A(1,0) = 0.0;  A(1,1) = 1.0;

    t[0] =   0.0;
    t[1] =   0.0;

    covar.set_identity();

    rgrl_transformation_sptr trans = new rgrl_trans_affine(A, t, covar);

    rgrl_mask_sptr roi = new rgrl_mask_box(vnl_double_2(-1, -1), vnl_double_2(40, 40));
    rgrl_estimator_sptr est_p = new rgrl_est_affine;
    rgrl_view_sptr view = new rgrl_view( roi, roi, roi->bounding_box(), roi->bounding_box(), est_p, trans, 0 );
    rgrl_scale_sptr scale = new rgrl_scale();

    // Create a vector of rgrl_features of the appropriate type.
    //
    vcl_vector<rgrl_feature_sptr> from_pts, to_pts;

    from_pts.push_back( new rgrl_feature_point( vnl_double_2(4,4) ) );
    from_pts.push_back( new rgrl_feature_point( vnl_double_2(25,4) ) );
    from_pts.push_back( new rgrl_feature_point( vnl_double_2(4,16) ) );
    from_pts.push_back( new rgrl_feature_point( vnl_double_2(30,30) ) );

    to_pts.push_back( new rgrl_feature_point( vnl_double_2(0,0) ) );
    to_pts.push_back( new rgrl_feature_point( vnl_double_2(15,0) ) );
    to_pts.push_back( new rgrl_feature_point( vnl_double_2(0,10) ) );
    to_pts.push_back( new rgrl_feature_point( vnl_double_2(15,10) ) );


    // Create feature sets from the vector of features.
    //
    rgrl_feature_set_sptr from_set, to_set;
    from_set = new rgrl_feature_set_bins_2d( from_pts );
    to_set = new rgrl_feature_set_bins_2d( to_pts );

    // iterate through different matcher and different k value
    for( unsigned id=0; id<2; ++id )
      for( unsigned k=1; k<4; ++k ) {
        
        rgrl_matcher_sptr matcher;
        vcl_string str;
        switch( id ) {
          case 0:     matcher = new rgrl_matcher_k_nearest(k);
                      str = "rgrl_matcher_k_nearest";
                      break;
          case 1:     matcher = new rgrl_matcher_k_nearest_adv(k);
                      str = "rgrl_matcher_k_nearest_adv";
                      break;
        }
        str += vul_sprintf( "(%1d)", k );
        
        // get the match_set
        rgrl_match_set_sptr match_set =  matcher->compute_matches( *from_set, *to_set,
                                                               *view, *trans, *scale);
    
        TEST( (str + ": size of match set").c_str(), match_set->from_size(), 4 );
    
    
        // check nearest neighbors
        //
        typedef rgrl_match_set::from_iterator FIter;
        typedef FIter::to_iterator TIter;
        bool neighbor_size_is_k = true;
        bool nearest_correct = true;
        vcl_vector<dist_id> dist(4);
        
        for( FIter fi = match_set->from_begin(); fi != match_set->from_end(); ++fi ) {
          
          // check neighbor size first
          neighbor_size_is_k = neighbor_size_is_k && (fi.size() == k);

          // build neighbor distance vector
          for( unsigned i=0; i<to_pts.size(); ++i )
            dist[i] = dist_id( (to_pts[i]->location() - fi.mapped_from_feature()->location()).squared_magnitude(), i );
          vcl_sort( dist.begin(), dist.end() );
          
          unsigned j=0;
          for( TIter ti = fi.begin(); ti != fi.end(); ++ti,++j ) {
            
            nearest_correct =  nearest_correct && ( ti.to_feature() == to_pts[ dist[j].id_ ] );
            
          }
        }
        TEST( (str+": all matches have k neighbors").c_str(), neighbor_size_is_k, true );
        TEST( (str+": nearest neighbors are correct").c_str(), nearest_correct, true );
    }
  }

  void test_matcher_k_nearest_pick_one() {

    // the points layout, the 1st and 2nd are coordinate and 3rd is the scale
    //
    //    t1(0,0,100)                                           t3(15,0,20)
    //                 m1(4, 4, 1.0) 
    //
    //
    //    t2(0, 10, 0.02)                                         t4(15, 10,5)
    //
    
    // create the transformation
    vnl_matrix<double> A( 2, 2 );
    vnl_vector<double> t( 2 );
    vnl_double_2 diff1(12.0, 0.0), diff2(0.0, 12.0);
    vnl_matrix<double> covar( 6, 6 );

    A(0,0) = 1.0;  A(0,1) = 0.0;
    A(1,0) = 0.0;  A(1,1) = 1.0;

    t[0] =   0.0;
    t[1] =   0.0;

    covar.set_identity();

    rgrl_transformation_sptr trans = new rgrl_trans_affine(A, t, covar);

    rgrl_mask_sptr roi = new rgrl_mask_box(vnl_double_2(-1, -1), vnl_double_2(40, 40));
    rgrl_estimator_sptr est_p = new rgrl_est_affine;
    rgrl_view_sptr view = new rgrl_view( roi, roi, roi->bounding_box(), roi->bounding_box(), est_p, trans, 0 );
    rgrl_scale_sptr scale = new rgrl_scale();

    // Create a vector of rgrl_features of the appropriate type.
    //
    vcl_vector<rgrl_feature_sptr> from_pts, to_pts(4);

    from_pts.push_back( new rgrl_feature_point( vnl_double_2(4,4) ) );
    from_pts[0]->set_scale(1.0);

    to_pts[0] = new rgrl_feature_point( vnl_double_2(0,0) );
    to_pts[0]->set_scale(100);
    to_pts[1] = new rgrl_feature_point( vnl_double_2(0,10) );
    to_pts[1]->set_scale(0.02);
    to_pts[2] = new rgrl_feature_point( vnl_double_2(15,0) );
    to_pts[2]->set_scale(20);
    to_pts[3] = new rgrl_feature_point( vnl_double_2(15,10) );
    to_pts[3]->set_scale(5);


    // Create feature sets from the vector of features.
    //
    rgrl_feature_set_sptr from_set, to_set;
    from_set = new rgrl_feature_set_bins_2d( from_pts );
    to_set = new rgrl_feature_set_bins_2d( to_pts );

    // iterate through different matcher and different k value
    for( unsigned k=1; k<4; ++k ) {
      
      rgrl_matcher_sptr matcher = new rgrl_matcher_k_nearest_pick_one(k);
      vcl_string str = "rgrl_matcher_k_nearest_pick_one";
      str += vul_sprintf( "(%1d)", k );
      
      // get the match_set
      rgrl_match_set_sptr match_set =  matcher->compute_matches( *from_set, *to_set,
                                                             *view, *trans, *scale);
  
      TEST( (str + ": size of match set").c_str(), match_set->from_size(), 1 );
  
  
      // check nearest neighbors
      //
      typedef rgrl_match_set::from_iterator FIter;
      typedef FIter::to_iterator TIter;
      bool neighbor_size_is_k = true;
      bool nearest_correct = true;
      vcl_vector<dist_id> dist(4);
      
      for( FIter fi = match_set->from_begin(); fi != match_set->from_end(); ++fi ) {
        
        // check neighbor size first
        neighbor_size_is_k = neighbor_size_is_k && (fi.size() == 1);

        // build neighbor distance vector
        for( unsigned i=0; i<to_pts.size(); ++i )
          dist[i] = dist_id( (to_pts[i]->location() - fi.mapped_from_feature()->location()).squared_magnitude(), i );
        vcl_sort( dist.begin(), dist.end() );
        
        nearest_correct =  nearest_correct && ( fi.begin().to_feature() == to_pts[ k-1 ] );
      }
      TEST( (str+": all matches have k neighbors").c_str(), neighbor_size_is_k, true );
      TEST( (str+": nearest neighbors are correct").c_str(), nearest_correct, true );
    }
  }
}

MAIN( test_matcher )
{
  START( "various matcher" );

  test_matcher_k_nearest_boundary();

  test_matcher_k_nearest();
  
  test_matcher_k_nearest_pick_one();
  
  SUMMARY();
}
