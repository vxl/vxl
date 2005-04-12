// An executable of pseudo-matching using a set of edge features. The
// input file contains data of different resolutions, but all features
// are used simultaneously in the physical co-ordinate space. In other
// words, the executable is a single-resolution estimation. To perform
// estimation in true multi-resolution framework, please run
// match_2d_pseudo.

#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_sstream.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h> // for exit()

#include <vnl/vnl_double_2.h>
#include <vnl/algo/vnl_svd.h>

#include <rrel/rrel_muset_obj.h>
#include <rrel/rrel_m_est_obj.h>
#include <rrel/rrel_tukey_obj.h>

#include <rgrl/rgrl_feature_trace_region.h>
#include <rgrl/rgrl_feature_face_region.h>
#include <rgrl/rgrl_feature_set_location.h>

#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_trans_quadratic.h>
#include <rgrl/rgrl_est_quadratic.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_set_of.h>
#include <rgrl/rgrl_data_manager.h>
#include <rgrl/rgrl_data_manager_sptr.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_matcher_pseudo.h>
#include <rgrl/rgrl_matcher_sptr.h>
#include <rgrl/rgrl_evaluator_ssd.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_initializer_prior.h>
#include <rgrl/rgrl_weighter_m_est.h>
#include <rgrl/rgrl_scale_est_all_weights.h>

#include <rgrl/rgrl_scale_est_closest.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_convergence_on_weighted_error.h>
#include <rgrl/rgrl_feature_based_registration.h>
#include <vul/vul_arg.h>
#include <vbl/vbl_bounding_box.h>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>

#include <testlib/testlib_test.h>
void testlib_enter_stealth_mode(); // defined in core/testlib/testlib_main.cxx

typedef vxl_byte pixel_type;

// adding observer to view change
class command_iteration_update: public rgrl_command
{
 public:
  void execute(rgrl_object* caller, const rgrl_event & event )
  {
    execute( (const rgrl_object*) caller, event );
  }

  void execute(const rgrl_object* caller, const rgrl_event & /*event*/ )
  {
    const rgrl_feature_based_registration* reg_engine =
      dynamic_cast<const rgrl_feature_based_registration*>(caller);
    vcl_cout <<"Current stage = " << reg_engine->current_stage() << vcl_endl;
    rgrl_transformation_sptr trans = reg_engine->current_transformation();

    if ( trans->is_type( rgrl_trans_affine::type_id() ) ) {
      rgrl_trans_affine* final = rgrl_cast<rgrl_trans_affine*>(trans);
      vcl_cout << final->A() << final->t() << vcl_endl << vcl_endl;
    }
    else {
      rgrl_trans_quadratic* final = rgrl_cast<rgrl_trans_quadratic*>(trans);
      vcl_cout << final->Q() << final->A() << final->t() << vcl_endl << vcl_endl;
    }
  }
};


void
make_mask_box_hold( rgrl_mask_box& box,
               vnl_vector<double> const& p )
{
  vnl_vector<double> x0 = box.x0();
  vnl_vector<double> x1 = box.x1();

  for ( unsigned i=0; i < p.size(); ++i ) {
    if ( x0[i] > p[i] )   x0[i] = p[i];
    if ( x1[i] < p[i] )   x1[i] = p[i];
  }

  box.set_x0( x0 );
  box.set_x1( x1 );
}

// The example read in a text file containing features , each with an
// associated normal direction. The file may contain features of
// different resolutions. However, all features are expressed in the
// physical co-ordinate and are treated as features from the same
// resolution. The contents of the text file is:
//
// face
// [ # of features]
// [ smoothing scale of the image ]
// [ loc_x loc_y norm_x norm_y ]
// [ ...]
// [ # of features]
// [ another smoothing scale of the image ]
// [ loc_x loc_y norm_x norm_y ]
// [ ...]
//
void
read_feature_file( char const* filename,
                   vcl_vector< rgrl_feature_sptr >& features,
                   rgrl_mask_box &roi,
                   unsigned spacing )
{
  // when spacing==1, it means every other point
  spacing++;

  vcl_ifstream istr( filename );
  if ( !istr ) {
    vcl_cerr << "Cannot open file to read: " << filename << vcl_endl;
    vcl_exit(3);
  }

  const double min_sigma=1.3; // don't want to take the lowest sigma=1
  const double max_sigma=6;   // stop after the 5th resolution
  vcl_string type_str;
  vcl_getline( istr, type_str );
  vbl_bounding_box<double, 2> box;

  if ( type_str=="face" ) { // for face features
    vcl_string s;
    double sigma;
    int num_of_features;
    // obtain features of various scale until end of file
    while ( !istr.eof() ) {
      // init
      sigma = -1;
      num_of_features = -1;
      istr >> num_of_features >> sigma;

      // sanity check
      if ( num_of_features<=0 || sigma < 0 )
        continue;

      // check min & max conditions
      if ( sigma > max_sigma )
        break;
      if ( sigma < min_sigma ) {
        // skip the rest of the line
        vcl_getline( istr, s );
        // have to skip the records
        for ( int i=0; i<num_of_features; ++i )
          vcl_getline( istr, s );
        continue;
      }

      // define thickness & radius, using magic number
      const double thickness = sigma * 11.0;   // thickness is diameter
      const double radius    = sigma * 3.0;
      // read in these features
      vnl_double_2 loc, normal;
      rgrl_feature_sptr fea_sptr;
      for ( int i=0; i<num_of_features; ++i ){
        // read in data no matter if it is used or not
        istr >> loc >> normal;
        if ( i%spacing == 0 ){
          fea_sptr = new rgrl_feature_face_region( loc.as_ref(), normal.as_ref(), thickness, radius );
          features.push_back( fea_sptr );
          // update the mask box
          box.update( loc[0], loc[1] );
        }
      }
    }

    // end of reading
    istr.close();
  } else {
    // the point, or trace feature is to be implemented.
    vcl_cerr << "this feature type(" << type_str << ") is not yet implemented.\n";
  }

  // update mask box
  vnl_vector<double> p(2, 0.0);
  p[0] = box.xmin();
  p[1] = box.ymin();
  roi.set_x0( p );
  p[0] = box.xmax();
  p[1] = box.ymax();
  roi.set_x1( p );
}

// Read the initial 2D affine transform. The contents of the file is
//
// a(0,0) a(0,1)
// a(1,0) a(1,1)
// t_x t_y
//
void
read_affine_trans_2d( const char* trans_file, vnl_matrix< double > & A, vnl_vector< double > & T )
{
  vcl_cerr << "reading transformation file " << trans_file << '\n';
  A.set_size( 2, 2 );
  T.set_size( 2 );

  vcl_ifstream ifs( trans_file );
  if ( !ifs ) {
    vcl_cerr << "Cannot open file to read: " << trans_file << vcl_endl;
    vcl_exit(2);
  }
  vcl_string str;
  for ( unsigned i=0; i<A.rows(); ++i ) {
    for ( unsigned j=0; j<A.columns(); ++j ) {
      ifs >> A( i, j ) ;
    }
  }
  vcl_cerr << A << '\n';
  for ( unsigned i=0; i<T.size(); ++i ) {
    ifs >> T( i );
  }
  vcl_cerr << T << '\n';

  ifs.close();
}

int
main( int argc, char* argv[] )
{
  vul_arg< unsigned > spacing( 0, "spacing for fewer features" );
  vul_arg< const char* > feature_file( 0, "the feature file" );
  vul_arg< const char* > from_files( 0, "from image file" );
  vul_arg< const char* > to_files( 0, "to image file" );
  vul_arg< const char* > output_xform( "-o", "output xformation file", 0 );
  vul_arg< const char* > mask_file( "-mask", "mask file", 0 );
  vul_arg< const char* > trans_file( "-init", "the initialization" );
  vul_arg< const char* > model( "-model", "Final model (affine, quadratic)", "quadratic" );

  vul_arg_parse( argc, argv );
  vcl_cout << feature_file() << vcl_endl << from_files() << vcl_endl << to_files() << vcl_endl;

  // Don't allow Visual Studio to open critical error dialog boxes
  testlib_enter_stealth_mode();

  // Load images
  //
  vcl_cout << "reading from images..." << vcl_endl;
  vil_image_view< pixel_type > from_image, to_image;
  {
    from_image = vil_load( from_files() );
    if ( !from_image ) {
      vcl_cerr << "reading from images failed\n";
      return 1;
    }
    vcl_cout << "from image size: " << from_image.ni() << ' ' << from_image.nj() << ' ' << from_image.nplanes() << '\n'

             << "reading to images..." << vcl_endl;
    to_image = vil_load( to_files() );
    if ( !to_image ) {
      vcl_cerr << "reading to images failed\n";
      return 1;
    }
    vcl_cout << "to image size: " << to_image.ni() << ' ' << to_image.nj() << ' ' << to_image.nplanes() << vcl_endl;
  }

  // load the mask image. If not supplied, assume valid in image dimension
  //
  vil_image_view<vxl_byte> mask_image;
  if ( mask_file.set() ) {
    mask_image = vil_load( mask_file() );
    if ( !mask_image.ni() || !mask_image.nj() ) {
      vcl_cerr << "Error: Cannot loading mask file: " << mask_file() << vcl_endl;
      return 2;
    } else {
      vcl_cout << "mask image size: " << mask_image.ni() << ' ' << mask_image.nj() << vcl_endl;
    }
  } else {
    // no mask is supplied.
    vcl_cout << "Warning: NO mask is supplied. Assume valid range is the image dimension\n" << vcl_endl;
    mask_image.set_size( from_image.ni(), from_image.nj() );
    mask_image.fill( vxl_byte(1) );
  }

  // 1. Load data into rgrl_feature_sets and determine the "global"
  // ROI for each data set.
  //
  rgrl_feature_set_sptr from_set, to_set;
  rgrl_mask_box from_roi(2), to_roi(2);
  {
    // "From" feature set
    vcl_vector<rgrl_feature_sptr> from_pts, to_pts( 1 );
    read_feature_file( feature_file(), from_pts, from_roi, spacing() );
    vcl_cout << "Num from_pts = " << from_pts.size() << vcl_endl;

    // "To" feature set. We leave to_pts to be generated in pseudo matcher
    //
    to_roi = from_roi;  // assuming the same roi for both "from" and "to".
    if ( from_pts[0]->is_type( rgrl_feature_trace_region::type_id() ) )
      to_pts[0] = new rgrl_feature_trace_region
        ( vnl_vector< double > ( 2, 0.0 ), vnl_vector< double > (2, 0.0), 0.0, 0.0 );
    else if ( from_pts[0]->is_type( rgrl_feature_face_region::type_id() ) )
      to_pts[0] = new rgrl_feature_face_region
        ( vnl_vector< double > ( 2, 0.0 ), vnl_vector< double > (2, 0.0), 0.0, 0.0 );
    else {
      vcl_cerr << " Wrong type of feature points!\n";
      vcl_exit(1);
    }

    // Feature sets of "from" and "to"
    //
    from_set = new rgrl_feature_set_location<2>( from_pts );
    to_set = new rgrl_feature_set_location<2>( to_pts );

    if ( !from_set || !to_set) {
      vcl_cerr << "Couldn't read data\n";
      return 1;
    }
  }

  // 2. Create the initializer
  //
  // We will initialize to an affine transform and set the initial
  // region to the global ROI.
  //
  // Estimator for initial transform estimate type.
  rgrl_estimator_sptr est_p;
  rgrl_initializer_sptr initializer;
  {
    if ( model() == vcl_string("quadratic") )
      est_p = new rgrl_est_quadratic( 2 );
    else if ( model() == vcl_string("affine") )
      est_p = new rgrl_est_affine( 2 );
    else {
      vcl_cerr<<"Unknown model "<<model()<<vcl_endl;
      vcl_exit(1);
    }

    vnl_matrix<double> A( 2, 2, vnl_matrix_identity );
    vnl_vector<double> t( 2, 0.0 );
    if ( trans_file.set() )
      read_affine_trans_2d( trans_file(), A, t );
    vcl_cout << "A =\n" << A << ", T = " << t << vcl_endl;
    rgrl_transformation_sptr init_trans = new rgrl_trans_affine( A, t, vnl_matrix<double>( 6, 6, 0.0 ) );

    unsigned resolution = 0;
    rgrl_scale_sptr prior_scale = new rgrl_scale(); //for pseudo matcher
    double geometric_scale = 6.0;
    prior_scale->set_geometric_scale( geometric_scale );

    initializer = new rgrl_initializer_prior( from_roi, to_roi, est_p, init_trans, resolution, prior_scale );
  }

  // 3. Create the matcher
  //
  // Pseudo matching
  rgrl_matcher_sptr matcher;
  {
    rgrl_evaluator_sptr evaluator = new rgrl_evaluator_ssd();
    matcher = new rgrl_matcher_pseudo< pixel_type> ( from_image, to_image, evaluator,
                                                     new rgrl_mask_2d_image(mask_image),
                                                     new rgrl_mask_2d_image(mask_image) );
    matcher->set_debug_flag( 0 );
  }

  // 4. robust weighting
  //
  rgrl_weighter_sptr wgter;
  {
    vcl_auto_ptr< rrel_m_est_obj > m_est_obj( new rrel_tukey_obj(4) );
    wgter = new rgrl_weighter_m_est( m_est_obj, true, true) ;
  }

  // 5. Scale estimator
  //
  // Estimate scales based on weighted match.
  //
  rgrl_scale_estimator_unwgted_sptr unwgted_scale_est;
  rgrl_scale_estimator_wgted_sptr wgted_scale_est;
  {
    // muse and unwgted_scale_est are not used
    vcl_auto_ptr<rrel_objective> obj( new rrel_muset_obj( 1 ) );
    unwgted_scale_est = new rgrl_scale_est_closest( obj );
    wgted_scale_est = new rgrl_scale_est_all_weights( );
  }

  // 6. Convergence: how to determine when we are done
  //
  rgrl_convergence_tester_sptr conv_test = new rgrl_convergence_on_weighted_error( 1.5 );

  // 7. Create the registration object.
  //
  // This is putting all the components together.

  // Set up the data manager
  rgrl_data_manager_sptr data_sptr = new rgrl_data_manager();
  data_sptr->add_data( from_set,   // data from moving image
                       to_set,     // data from fixed image
                       matcher,    // matching mechanism
                       wgter,      // robust weighting
                       unwgted_scale_est,  // unweighted scale estimator
                       wgted_scale_est  );
  rgrl_feature_based_registration reg( data_sptr, conv_test );
  // enforce lower bound of geometric scale
  reg.set_expected_min_geometric_scale( 0.25 );
  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());
  // Get an estimate
  //
  vcl_cerr << "Start registering...\n\n";
  reg.run( initializer );

  // Output the xform
  //
  rgrl_transformation_sptr final_trans = reg.final_transformation();
  rgrl_scale_sptr final_scale = reg.final_scale();

  vcl_cout << "Final objective = " << reg.final_status()->objective_value() << vcl_endl;

  if ( output_xform.set() ) {
    vcl_ofstream ofs( output_xform() );

    if ( final_trans->is_type( rgrl_trans_affine::type_id() ) ) {
      rgrl_trans_affine* final = rgrl_cast<rgrl_trans_affine*>(final_trans);
      ofs << final->A() << final->t() << vcl_endl << vcl_endl;
    }
    else {
      rgrl_trans_quadratic* final = rgrl_cast<rgrl_trans_quadratic*>(final_trans);
      ofs << final->Q() << final->A() << final->t() << vcl_endl << vcl_endl;
    }

    ofs << final_scale->geometric_scale() << '\n' << 0 ;
    ofs.close();
  }
  else { // output_xform not set, so dump to the std output
    vcl_cout << "Final transform:" << vcl_endl;

    if ( final_trans->is_type( rgrl_trans_affine::type_id() ) ) {
      rgrl_trans_affine* final = rgrl_cast<rgrl_trans_affine*>(final_trans);
      vcl_cout << final->A() << final->t() << vcl_endl << vcl_endl;
    }
    else {
      rgrl_trans_quadratic* final = rgrl_cast<rgrl_trans_quadratic*>(final_trans);
      vcl_cout << final->Q() << final->A() << final->t() << vcl_endl << vcl_endl;
    }

    vcl_cout << final_scale->geometric_scale() << "\n\n";
  }

  // Compute errors
  {
    rgrl_scale_sptr scale;   //  dummy scale ... unused
    rgrl_match_set const& ms = *reg.final_match_set();
    bool use_signature_only = true;
    scale = wgted_scale_est->estimate_weighted( ms, scale, use_signature_only );
    vcl_vector< double > d;
    double sum = 0.0;
    unsigned num = 0;
    typedef rgrl_match_set::const_from_iterator from_iter;
    typedef from_iter::to_iterator              to_iter;
    for ( from_iter fitr = ms.from_begin(); fitr != ms.from_end(); ++fitr ) {
      for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
        rgrl_feature_sptr m = fitr.from_feature()->transform( *final_trans );
        double error = titr.to_feature()->geometric_error( *m );
        d.push_back( error );
        sum += error;
        ++num;
      }
    }

    double e;
    vcl_nth_element( d.begin(), d.begin()+d.size()/2, d.end() );
    e = d[d.size()/2];

    vcl_cout << "CP match set from size = " << ms.from_size() << vcl_endl
             << "Final median of alignment error: " << e << vcl_endl
             << "Final average of alignment error: " << sum / num << vcl_endl;
  }

  // Perform testing
  //
  testlib_test_start( "Registration using pseudo-matching" );
  testlib_test_assert_near("", reg.final_status()->objective_value(), 0.1, 1e-2 );
  return testlib_test_summary();
}
