#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_sstream.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_3.h>

#include <vnl/algo/vnl_svd.h>

#include <rrel/rrel_muset_obj.h>

#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_est_affine.h>
//#include <rgrl/rgrl_est_auto_affine.h>
#include <rgrl/rgrl_set_of.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_view.h>
//#include <rgrl/rgrl_view_gen_null.h>
#include <rgrl/rgrl_view_gen_dbicp.h>
#include <rgrl/rgrl_initializer_prior.h>
#include <rgrl/rgrl_weighter_unit.h>
#include <rgrl/rgrl_scale_est_closest.h>
#include <rgrl/rgrl_dbicp_registration.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_convergence_on_median_error.h>

#include <rgrl/rgrl_match_set.h>

// for the 3d vessels
double global_scales[3] = { 550.0/768.0, 550.0/768.0, 5.0 };

static inline double sqr(double x) { return x*x; }

void
make_roi_hold( rgrl_mask_box& roi,
               vnl_vector<double> const& p )
{
  vnl_vector<double> x0 = roi.x0();
  vnl_vector<double> x1 = roi.x1();

  for( unsigned i=0; i < p.size(); ++i ) {
    if( x0[i] > p[i] )   x0[i] = p[i];
    if( x1[i] < p[i] )   x1[i] = p[i];
  }

  roi.set_x0( x0 );
  roi.set_x1( x1 );
}


void
average_using_len( unsigned int n,
                   vcl_vector< vnl_vector<double> > const& in,
                   vcl_vector< vnl_vector<double> >& out,
                   vcl_vector< vnl_vector<double> >& out_tang )
{
  vcl_vector< vnl_vector<double> > segment;

  out.clear();
  for( unsigned i=0; i < in.size(); ++i ) {
    unsigned start, end;
    if( i > n/2 ) {
      start = i-n/2;
    } else {
      start = 0;
    }
    if( start + n <= in.size() ) {
      end = start+n;
    } else {
      end = in.size();
      if( end >= n ) {
        start = end-n;
      }
    }
    segment.clear();
    vnl_matrix<double> X( (end-start)*3, 6, 0.0 );
    vnl_vector<double> y( (end-start)*3, 0.0 );
    for( unsigned j=start, k=0; j < end; ++j, ++k ) {
      for( unsigned c=0; c < 3; ++c ) {
        X(k*3+c,c) = 1.0;
        X(k*3+c,c+3) = k;
        y[k*3+c] = in[j][c];
      }
    }
    vnl_svd<double> svd( X.transpose() * X );
    svd.zero_out_relative();
    if( svd.rank() == 6 ) {
      vnl_vector<double> param = svd.inverse() * X.transpose() * y;
      vnl_double_3 a( param[0], param[1], param[2] );
      vnl_double_3 t( param[3], param[4], param[5] );
      out.push_back( a + t * double(i-start) );
      t.normalize();
      out_tang.push_back( t );
    } else {
      vcl_cerr << "No line fit at " << i << ": " << in[i] << "\n";
      out.push_back( in[i] );
      if( i > 0 ) {
        out_tang.push_back( out_tang[i-1] );
      } else {
        out_tang.push_back( vnl_double_3(1.0,0.0,0.0) );
      }
    }
  }

  assert( out.size() == in.size() );
}
                

void
read_trace3d_file( char const* filename,
                   vcl_vector< vnl_vector<double> >& points,
                   vcl_vector< vnl_vector<double> >& tangents,
                   vcl_vector< vcl_vector< vnl_vector<double> > >& seg_points,
                   rgrl_mask_box& roi )
{
  vcl_ifstream istr( filename );

  if( !istr ) {
    return;
  }

  vcl_string s;
  vcl_getline( istr, s );

  if( s != "# 3D trace file" ) {
    vcl_cerr << filename << " is not a 3D trace file\n";
    return;
  }

  vcl_vector< vnl_vector<double> > pts;
  vcl_vector< vnl_vector<double> > avg;
  vcl_vector< vnl_vector<double> > tang;

  points.clear();
  seg_points.clear();
  while( istr >> s ) {
    if( s == "#count=" ) {
      seg_points.push_back( vcl_vector< vnl_vector<double> >() );
      // new segment
      unsigned int count;
      vnl_vector<double> loc(3);
      unsigned int seg, axon;
      istr >> count;
      pts.clear();
      avg.clear();
      tang.clear();
      for( unsigned i=0; i < count; ++i ) {
        istr >> loc[0] >> loc[1] >> loc[2] >> seg >> axon;
        loc[0] *= global_scales[0];
        loc[1] *= global_scales[1];
        loc[2] *= global_scales[2];
        pts.push_back( loc );
      }
      average_using_len( 20, pts, avg, tang );
      for( unsigned i=0; i < count; ++i ) {
        points.push_back( avg[i] );
        seg_points.back().push_back( avg[i] );
        tangents.push_back( tang[i] );
        make_roi_hold( roi, avg[i] );
      }
    }
  }

  vcl_cout << "Read " << points.size() << " points\n";
}


void
write_out( char const* filename,
           vcl_vector< vcl_vector< vnl_vector<double> > > const& seg_points,
           rgrl_trans_affine const& aff )
{
  vcl_ofstream fout( filename );

  if( !fout ) {
    vcl_cerr << "Couldn't write output to " << filename << "\n";
    return;
  }

  for( unsigned int j=0; j < seg_points.size(); ++j ) {
    vcl_vector< vnl_vector<double> > const& pts = seg_points[j];
    for( unsigned int i=0; i < pts.size(); ++i ) {
      vnl_vector<double> t = aff.A() * pts[i] + aff.t();
      fout << t << "\n";
    }
    fout << "\n\n";
  }

  vcl_cout << "Output written to " << filename << "\n";
}


extern void (*dbicp_callback)(rgrl_transformation_sptr trans,
                              rgrl_view_sptr view,
                              rgrl_set_of<rgrl_match_set_sptr> const& matches);

vcl_vector< vcl_vector< vnl_vector<double> > >* global_from_pts;
void callback( rgrl_transformation_sptr trans,
               rgrl_view_sptr view,
               rgrl_set_of<rgrl_match_set_sptr> const& matches )
{
  static unsigned count = 0;
  ++count;
  rgrl_trans_affine* final_aff = rgrl_cast<rgrl_trans_affine*>(trans);
  {
    vcl_ostringstream s;
    s << "out2-dump-"<<count;

    vcl_cout << "\n\nAffine for " << count << "\n";
    for( unsigned i=0; i < 3; ++i ) {
      for( unsigned j=0; j < 3; ++j ) {
        vcl_cout << final_aff->A()(i,j) << "  ";
      }
      vcl_cout << final_aff->t()(i) << "\n";
    }
    vcl_cout << "\n\n";
    vcl_cout << "Covar:\n" << final_aff->covar() << "\n\n";
    
    write_out( s.str().c_str(), *global_from_pts, *final_aff );
  }
  {
    vcl_ostringstream s;
    s << "out2-view-"<<count;

    rgrl_mask_box const& r = view->region();
    vcl_ofstream fout( s.str().c_str() );

    vnl_vector<double> x0 = final_aff->map_location( r.x0() );
    vnl_vector<double> x1 = final_aff->map_location( r.x1() );

    fout << "  " << x0[0] << "  " << x0[1] << "  " << x0[2] << "\n";
    fout << "  " << x1[0] << "  " << x0[1] << "  " << x0[2] << "\n";
    fout << "  " << x1[0] << "  " << x1[1] << "  " << x0[2] << "\n";
    fout << "  " << x0[0] << "  " << x1[1] << "  " << x0[2] << "\n";
    fout << "  " << x0[0] << "  " << x0[1] << "  " << x0[2] << "\n";
    fout << "\n\n";

    fout << "  " << x0[0] << "  " << x0[1] << "  " << x0[2] << "\n";
    fout << "  " << x0[0] << "  " << x1[1] << "  " << x0[2] << "\n";
    fout << "  " << x0[0] << "  " << x1[1] << "  " << x1[2] << "\n";
    fout << "  " << x0[0] << "  " << x0[1] << "  " << x1[2] << "\n";
    fout << "  " << x0[0] << "  " << x0[1] << "  " << x0[2] << "\n";
    fout << "\n\n";

    fout << "  " << x0[0] << "  " << x0[1] << "  " << x0[2] << "\n";
    fout << "  " << x1[0] << "  " << x0[1] << "  " << x0[2] << "\n";
    fout << "  " << x1[0] << "  " << x0[1] << "  " << x1[2] << "\n";
    fout << "  " << x0[0] << "  " << x0[1] << "  " << x1[2] << "\n";
    fout << "  " << x0[0] << "  " << x0[1] << "  " << x0[2] << "\n";
    fout << "\n\n";

    fout << "  " << x0[0] << "  " << x0[1] << "  " << x1[2] << "\n";
    fout << "  " << x1[0] << "  " << x0[1] << "  " << x1[2] << "\n";
    fout << "  " << x1[0] << "  " << x1[1] << "  " << x1[2] << "\n";
    fout << "  " << x0[0] << "  " << x1[1] << "  " << x1[2] << "\n";
    fout << "  " << x0[0] << "  " << x0[1] << "  " << x1[2] << "\n";
    fout << "\n\n";

    fout << "  " << x1[0] << "  " << x0[1] << "  " << x0[2] << "\n";
    fout << "  " << x1[0] << "  " << x1[1] << "  " << x0[2] << "\n";
    fout << "  " << x1[0] << "  " << x1[1] << "  " << x1[2] << "\n";
    fout << "  " << x1[0] << "  " << x0[1] << "  " << x1[2] << "\n";
    fout << "  " << x1[0] << "  " << x0[1] << "  " << x0[2] << "\n";
    fout << "\n\n";

    fout << "  " << x0[0] << "  " << x1[1] << "  " << x0[2] << "\n";
    fout << "  " << x1[0] << "  " << x1[1] << "  " << x0[2] << "\n";
    fout << "  " << x1[0] << "  " << x1[1] << "  " << x1[2] << "\n";
    fout << "  " << x0[0] << "  " << x1[1] << "  " << x1[2] << "\n";
    fout << "  " << x0[0] << "  " << x1[1] << "  " << x0[2] << "\n";
    fout << "\n\n";
  }

  {
    vcl_ostringstream s;
    s << "out2-map-"<<count;

    vcl_ofstream fout( s.str().c_str() );

    typedef rgrl_match_set::const_from_iterator FIter;
    typedef FIter::to_iterator TIter;

    for( unsigned ms=0; ms < matches.size(); ++ms ) {
      rgrl_match_set const& match_set = *matches[ms];
      for( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
        fout << fi.from_feature()->location() << "\n"
             << fi.mapped_from_feature()->location() << "\n\n\n";
      }
    }
  }

  {
    vcl_ostringstream s;
    s << "out2-matches-"<<count;

    vcl_ofstream fout( s.str().c_str() );

    typedef rgrl_match_set::const_from_iterator FIter;
    typedef FIter::to_iterator TIter;

    for( unsigned ms=0; ms < matches.size(); ++ms ) {
      rgrl_match_set const& match_set = *matches[ms];
      for( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
        for( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
          fout << fi.mapped_from_feature()->location() << "\n"
               << ti.to_feature()->location() << "\n\n\n";
        }
      }
    }
  }
 
}

int
main( int argc, char* argv[] )
{
  if( argc < 4 ) {
    vcl_cerr << "Need two input 3d trace file names and the expected affine file\n";
    return 1;
  }


  // 1. Load data into rgrl_feature_sets and determine the "global"
  // ROI for each data set.
  //
  rgrl_mask_box from_roi(3), to_roi(3);
  vcl_vector< vcl_vector< vnl_vector<double> > > from_seg, to_seg;
  vcl_vector< vnl_vector<double> > from_loc, to_loc;
  vcl_vector< vnl_vector<double> > from_tang, to_tang;

  // Read the data from file
  //
  read_trace3d_file( argv[1], from_loc, from_tang, from_seg, from_roi );
  read_trace3d_file( argv[2], to_loc, to_tang, to_seg, to_roi );

  // Create a vector of rgrl_features of the appropriate type.
  //
  vcl_vector<rgrl_feature_sptr> from_pts, to_pts;
  for( unsigned int i=0; i < from_loc.size(); ++i ) {
    from_pts.push_back( new rgrl_feature_point( from_loc[i] ) );
  }


  for( unsigned int i=0; i < to_loc.size(); ++i ) {
    to_pts.push_back( new rgrl_feature_trace_pt( to_loc[i], to_tang[i] ) );
    //to_pts.push_back( new rgrl_feature_point( to_loc[i] ) );
  }
  global_from_pts = &from_seg;
  dbicp_callback = callback;

  // Create feature sets from the vector of features.
  //
  rgrl_feature_set_sptr from_set, to_set;
  from_set = new rgrl_feature_set_location<3>( from_pts );
  to_set = new rgrl_feature_set_location<3>( to_pts );

  if( !from_set || !to_set) {
    vcl_cerr << "Couldn't read data\n";
    return 1;
  }

  vcl_cout << "Num from_pts = " << from_pts.size() << "\n";
  vcl_cout << "Num to_pts = " << to_pts.size() << "\n";

  // 2. Create the initializer
  //
  // We will initialize to an affine transform with an affine
  // transform estimator, and set the initial region to the global
  // ROI.
  //
  rgrl_initializer_sptr initializer;
  rgrl_transformation_sptr init_trans;
  {
//     // Omar's estimate for set1
//     double A_data[] = { -1.00047,    -0.0622815,  -0.00886743,
//                         -0.00928127, 1.00363,     -0.00216731,
//                         0.000878027, -0.00269161, -0.942521    };

    vnl_matrix<double> A( 3, 3, vnl_matrix_identity );
    vnl_vector<double> t( 3, 0.0 );

    init_trans = new rgrl_trans_affine( A, t, vnl_matrix<double>( 12, 12, 0.0 ) );
    write_out( "out2-from.txt", from_seg, *(rgrl_cast<rgrl_trans_affine*>(init_trans)) );
    write_out( "out2-to.txt", to_seg, *(rgrl_cast<rgrl_trans_affine*>(init_trans)) );

    vcl_ifstream fin( argv[3] );
    for( unsigned i=0; i < A.rows(); ++i ) {
      for( unsigned j=0; j < A.cols(); ++j ) {
        fin >> A(i,j);
      }
      fin >> t[i];
    }
    if( !fin ) {
      vcl_cerr << "Could't read affine from " << argv[3] << "\n";
      return 1;
    }
    vcl_cout << "\n\nTarget A=\n"<<A<<"\nTarget t=\n"<<t<<"\n\n\n";

//     double B_data[] = { 1, 0.1, 0,
//                         0, 1, 0.1,
//                         0, 0, 1 };
//     vnl_matrix<double> B( B_data, 3, 3 );
//     vnl_double_3 move( 200, 140, 150 );

//     A = B * A;
//     t = B * t - B * move.as_ref() + move;

    init_trans = new rgrl_trans_affine( A, t, vnl_matrix<double>( 12, 12, 0.0 ) );
    write_out( "out2-targ.txt", from_seg, *(rgrl_cast<rgrl_trans_affine*>(init_trans)) );

    // Remove affine components of A
//     for( unsigned i=0; i < A.rows(); ++i ) {
//       for( unsigned j=0; j < A.cols(); ++j ) {
//         if( i != j ) {
//           A(i,j) = 0.0;
//         } else {
//           // preserve sign because that gives us the flipping
//           A(i,j) = A(i,j) / vcl_abs(A(i,j));
//         }
//       }
//     }

//     vcl_cout << "\n\nStarting  A=\n"<<A<<"\nStarting t=\n"<<t<<"\n\n\n";

//     // initial transform estimate
//     //
//     init_trans = new rgrl_trans_affine( A, t, vnl_matrix<double>( 12, 12, 0.0 ) );

    // Estimator for initial transform estimate type.
    //
//    rgrl_estimator_sptr est_p = new rgrl_est_affine;
    rgrl_estimator_sptr est_p = new rgrl_est_affine;

    // View of initial estimator and initial region.

    // for 3d vessels 1->2
//     rgrl_roi start( vnl_double_3( 150, 100, 60 ),
//                     vnl_double_3( 200, 160, 85 ) );
    rgrl_mask_box start( vnl_double_3( 170, 100, 60 ),
                         vnl_double_3( 230, 160, 95 ) );
//     rgrl_roi start = from_roi;

    rgrl_view_sptr view = new rgrl_view( from_roi, to_roi, start, from_roi, est_p, init_trans, 0 );

    initializer = new rgrl_initializer_prior( view );

    write_out( "out2-init.txt", from_seg, *(rgrl_cast<rgrl_trans_affine*>(init_trans)) );
  }

  // 3. Create the view generator
  //
  // We will choose to only have one "view" and one resolution, so
  // this will be a straigtforward global estimation.
  //
  rgrl_view_generator_sptr view_gen = new rgrl_view_gen_dbicp;


//   // 4. Create a data set corresponding to the data required by the
//   //    view generator.
//   //
//   // There is only one resolution, so we just add our features for that resolution.
//   rgrl_data_set_sptr data_set;
//   {
//     data_set = new rgrl_data_set;
//     data_set->add_feature_set( 0, "from locations", from_set );
//     data_set->add_feature_set( 0, "to trace points", to_set );
//   }

  // 5. Create the registration object.
  //
  // This is putting all the components together.

  // closest point matching, only if close enough
  //
  rgrl_matcher_sptr cp_matcher = new rgrl_matcher_k_nearest( 1, 50 );

  // no weighting
  //
  rgrl_weighter_sptr wgter = new rgrl_weighter_unit;

  // estimate scales based on closest match. Since we will only have
  // one-to-one matches (because of the choice in matcher above), the
  // closest match is _the_ match.
  //
  rgrl_scale_estimator_unwgted_sptr unwgted_scale_est;
  rgrl_scale_estimator_wgted_sptr wgted_scale_est;
  {
    vcl_auto_ptr<rrel_objective> obj( new rrel_muset_obj( 5000 ) );
    rgrl_scale_est_closest* est = new rgrl_scale_est_closest( obj );
    unwgted_scale_est = est;
    wgted_scale_est = 0; // don't want a weighted scale estimator
    //wgted_scale_est = 
  }

  // how to determine when we are done
  //
  rgrl_convergence_tester_sptr conv_test = new rgrl_convergence_on_median_error( 1e-3 );

  // Set up the initialization problem
  //
  rgrl_dbicp_registration reg( initializer, view_gen, conv_test );

  // Add the data needed by the view generator.
  //
  reg.add_data( 0,          // stage
                from_set,   // from data
                to_set,     // to data
                cp_matcher, // how to create correspondences for this data
                unwgted_scale_est,  // unweighted scale estimator
                wgted_scale_est,    // weighted scale estimator
                wgter );    // weighter

  // Get an estimate
  //
  reg.run();

  rgrl_transformation_sptr final_trans = reg.final_transformation();
  rgrl_trans_affine* final_aff = rgrl_cast<rgrl_trans_affine*>(final_trans);
  
  write_out( "out2-est.txt", from_seg, *final_aff );

  vcl_cout << "Final objective = " << reg.final_status()->objective_value() << "\n";
  vcl_cout << "Final A=\n" << final_aff->A() << "\nFinal t=\n"<<final_aff->t()<<"\n\n";

  if( reg.final_matches().size() > 0 ) {
    vcl_ofstream fout( "out2-matches.txt" );
    rgrl_match_set const& ms = *reg.final_matches()[0];
    typedef rgrl_match_set::const_from_iterator from_iter;
    typedef from_iter::to_iterator              to_iter;
    for( from_iter fitr = ms.from_begin(); fitr != ms.from_end(); ++fitr ) {
      for( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
        rgrl_feature_sptr m = fitr.from_feature()->transform( *final_trans );
        fout << titr.to_feature()->location() << "\n" << m->location() <<"\n\n";
      }
      fout << "\n";
    }

    vcl_cout << "Final match set from size = " << ms.from_size() << "\n";
  }

  // Compute errors based on closest point
  {
    rgrl_matcher_sptr matcher = new rgrl_matcher_k_nearest( 1 );
    rgrl_scale scale;   //  dummy scale ... unused
    rgrl_match_set_sptr cp_matches = matcher->compute_matches( *from_set,
                                                               *to_set,
                                                               *reg.final_view(),
                                                               *reg.final_transformation(),
							       scale );
    rgrl_match_set const& ms = *reg.final_matches()[0];

    vcl_vector<double> d[3];
    typedef rgrl_match_set::const_from_iterator from_iter;
    typedef from_iter::to_iterator              to_iter;
    for( from_iter fitr = ms.from_begin(); fitr != ms.from_end(); ++fitr ) {
      for( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
        rgrl_feature_sptr m = fitr.from_feature()->transform( *final_trans );
        d[0].push_back( vcl_abs(titr.to_feature()->location()[0] - m->location()[0]) );
        d[1].push_back( vcl_abs(titr.to_feature()->location()[1] - m->location()[1]) );
        d[2].push_back( vcl_abs(titr.to_feature()->location()[2] - m->location()[2]) );
      }
    }

    double e[3];
    vcl_nth_element( d[0].begin(), d[0].begin()+d[0].size()/2, d[0].end() );
    vcl_nth_element( d[1].begin(), d[1].begin()+d[1].size()/2, d[1].end() );
    vcl_nth_element( d[2].begin(), d[2].begin()+d[2].size()/2, d[2].end() );
    e[0] = d[0][d[0].size()/2];
    e[1] = d[1][d[1].size()/2];
    e[2] = d[2][d[2].size()/2];

    vcl_cout << "CP match set from size = " << ms.from_size() << "\n";

    vcl_cout << "median: ";
    vcl_cout << "dx="<<e[0] << "    dy="<<e[1]<<"   dz="<<e[2]<<"\n";
  }

  return 0;
}
