//:
// \file
// An executable of pseudo-matching using a set of edge features. The
// input file contains data of different resolutions, and the
// transformation is estimated at each resolution, and passed to the
// next resolution level for initialization.

#include <fstream>
#include <iostream>
#include <algorithm>
#include <list>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_double_2.h>

#include <rrel/rrel_muset_obj.h>
#include <rrel/rrel_m_est_obj.h>
#include <rrel/rrel_tukey_obj.h>
#include <rrel/rrel_cauchy_obj.h>

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
#include <rgrl/rgrl_debug_util.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_convergence_on_weighted_error.h>
#include <rgrl/rgrl_feature_based_registration.h>
#include <vul/vul_arg.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/algo/vil_gauss_reduce.h>
#include <testlib/testlib_test.h>

void testlib_enter_stealth_mode(); // defined in core/testlib/testlib_main.cxx

typedef vxl_byte pixel_type;

// adding observer to view change
class command_iteration_update: public rgrl_command
{
 public:
  void execute(rgrl_object* caller, const rgrl_event & event ) override
  {
    execute( (const rgrl_object*) caller, event );
  }

  void execute(const rgrl_object* caller, const rgrl_event & /*event*/ ) override
  {
    const auto* reg_engine =
      dynamic_cast<const rgrl_feature_based_registration*>(caller);

    // set precision
    std::cout.precision( 10 );

    // get stage
    int stage = reg_engine->current_stage();
    std::cout <<"Current stage = " << stage << std::endl;

    // re-scale transformation to *physical* coordinate
    rgrl_transformation_sptr trans = reg_engine->current_transformation()->scale_by( double(1<<stage) );

    // Now output the xform
    if ( trans->is_type( rgrl_trans_quadratic::type_id() ) ) {
      rgrl_trans_quadratic* xform = rgrl_cast<rgrl_trans_quadratic*>(trans);
      std::cout<<"Xform Q =\n"<<xform->Q()
              <<"      A =\n"<<xform->A()
              <<"      t =\n"<<xform->t() << std::endl;
    } else if ( trans->is_type( rgrl_trans_affine::type_id() ) ) {
      rgrl_trans_affine* xform = rgrl_cast<rgrl_trans_affine*>(trans);
      std::cout<<"Xfrom A =\n"<<xform->A()
              <<"      t =\n"<<xform->t() << std::endl;
    } else {
      std::cerr << "Fatal error: unexpected transform.\n";
      std::exit(3);
    }
  }
};


rgrl_feature_sptr
scale_location_by( const rgrl_feature_sptr& before_sptr, const double& scale )
{
  rgrl_feature_sptr after;
  if ( before_sptr->is_type( rgrl_feature_face_region::type_id() ) ) {
    const rgrl_feature_face_region* before = rgrl_cast< rgrl_feature_face_region* >(before_sptr);
    after = new rgrl_feature_face_region( before->location()*scale, before->normal(),
                                          before->thickness()*scale, before->radius()*scale );
  } else {
    std::cerr << "Warning: cannot recognize this type of feature. The result will be unpredictable.\n";
  }

  return after;
}


struct scaled_feature_node {
  //: scale at this level
  double sigma_;

  //: mask_box
  //  should be enough to set it as image size
  rgrl_mask_box roi_;

  //: a vector containing all features at this level
  std::vector< rgrl_feature_sptr >  features_;

  //: ctor
  scaled_feature_node() : sigma_(0.0), roi_(2)  {   }

  //: size operator
  unsigned size() const { return features_.size(); }

  //: push back function
  void push_back( const rgrl_feature_sptr& sptr ) { features_.push_back(sptr); }
};

// The example read in a text file containing features , each with an
// associated normal direction. The file contains features of
// different resolutions. However, all features are expressed in the
// physical co-ordinate. The contents of the text file is:
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
                   std::list<scaled_feature_node>& feature_list,
                   unsigned spacing )
{
  // when spacing==1, it means every other point
  spacing++;

  std::ifstream istr( filename );
  if ( !istr ) {
    std::cerr << "Cannot open file to read: " << filename << std::endl;
    std::exit(3);
  }

  std::string type_str;
  std::getline( istr, type_str );

  if ( type_str=="face" ) { // for face features
    std::string s;
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

      // push it into list
      feature_list.emplace_back( );
      scaled_feature_node& this_node = feature_list.back();
      this_node.sigma_ = sigma;

      // define thickness & radius, using magic number
      const double thickness = sigma * 10.0;   // thickness is diameter
      const double radius    = sigma * 2.0;
      // read in these features
      vnl_double_2 loc, normal;
      rgrl_feature_sptr fea_sptr;
      for ( int i=0; i<num_of_features; ++i ) {
        // read in data no matter if it is used or not
        istr >> loc >> normal;
        if ( i%spacing == 0 ) {
          fea_sptr = new rgrl_feature_face_region( loc.as_ref(), normal.as_ref(), thickness, radius );
          this_node.push_back( fea_sptr );
        }
      }
    }

    // end of reading
    istr.close();
  } else {
    // the point, or trace feature is to be implemented.
    std::cerr << "this feature type(" << type_str << ") is not yet implemented.\n";
  }
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
  std::cerr << "reading transformation file " << trans_file << '\n';
  A.set_size( 2, 2 );
  T.set_size( 2 );

  std::ifstream ifs( trans_file );
  if ( !ifs ) {
    std::cerr << "Cannot open file to read: " << trans_file << std::endl;
    std::exit(2);
  }
  std::string str;
  for ( unsigned i=0; i<A.rows(); ++i ) {
    for ( unsigned j=0; j<A.columns(); ++j ) {
      ifs >> A( i, j ) ;
    }
  }
  std::cerr << A << '\n';
  for ( unsigned i=0; i<T.size(); ++i ) {
    ifs >> T( i );
  }
  std::cerr << T << '\n';

  ifs.close();
}

int
main( int argc, char* argv[] )
{
  vul_arg< unsigned > spacing( nullptr, "spacing for fewer features" );
  vul_arg< const char* > feature_file( nullptr, "the feature file" );
  vul_arg< const char* > from_files( nullptr, "from image file" );
  vul_arg< const char* > to_files( nullptr, "to image file" );
  vul_arg< const char* > output_xform( "-o", "output xformation file", nullptr );
  vul_arg< const char* > mask_file( "-mask", "mask file", nullptr );
  vul_arg< const char* > trans_file( "-init", "the initialization" );
  vul_arg< const char* > model( "-model", "Final model (affine, quadratic) in the highest resolution", "quadratic" );

  vul_arg_parse( argc, argv );
  std::cout << feature_file() << std::endl << from_files() << std::endl << to_files() << std::endl;

  // Don't allow Visual Studio to open critical error dialog boxes
  testlib_enter_stealth_mode();

  // Load images
  std::cout << "reading from images..." << std::endl;
  vil_image_view< pixel_type > from_image, to_image;
  {
    from_image = vil_load( from_files() );
    if ( !from_image ) {
      std::cerr << "reading from images failed\n";
      return 1;
    }
    std::cout << "from image size: " << from_image.ni() << ' ' << from_image.nj()
             << ' ' << from_image.nplanes() << std::endl
             << "reading to images..." << std::endl;
    to_image = vil_load( to_files() );
    if ( !to_image ) {
      std::cerr << "reading to images failed\n";
      return 1;
    }
    std::cout << "to image size: " << to_image.ni() << ' ' << to_image.nj() << ' ' << to_image.nplanes() << std::endl;
  }

  // load the mask image. If not supplied, assume valid in image dimension
  //
  vil_image_view<vxl_byte> mask_image;
  if ( mask_file.set() ) {
    mask_image = vil_load( mask_file() );
    if ( !mask_image.ni() || !mask_image.nj() ) {
      std::cerr << "Error: Cannot loading mask file: " << mask_file() << std::endl;
      return 2;
    } else {
      std::cout << "mask image size: " << mask_image.ni() << ' ' << mask_image.nj() << std::endl;
    }

    // if mask pixel value is 255, it could cause problem when
    // downsample image, where it smooth the mask boundary and cause
    // mask larger than it should be the way to avoid it is to
    // truncate the value to very small number, likely to 1.  But also
    // due rounding error, 1 may resulting a smaller mask than the
    // correct one...  By pure guessing, I choose 2.
    vil_math_truncate_range( mask_image, vxl_byte(0), vxl_byte(2) );
  } else {
    // no mask is supplied.
    std::cout << "Warning: NO mask is supplied. Assume valid range is the image dimension\n" << std::endl;
    mask_image.set_size( from_image.ni(), from_image.nj() );
    mask_image.fill( vxl_byte(1) );
  }

  // 1. Load data into rgrl_feature_sets and
  //    the "global" is set to image size
  //
  std::list< scaled_feature_node > feature_sets;
  read_feature_file( feature_file(), feature_sets, spacing() );

  // 1.5 Determine the number of "stages" to use
  //     in multi-resolution registration.
  //     And downsample images
  vil_image_view<pixel_type> dest, prev_from, prev_to, prev_mask;
  vil_image_view<pixel_type> work_space( from_image.ni(), from_image.nj() );
  std::vector< vil_image_view<pixel_type> > from_images, to_images, mask_images;
  from_images.reserve(6);
  to_images.reserve(6);
  mask_images.reserve(6);
  from_images.push_back( from_image );
  to_images.push_back( to_image );
  mask_images.push_back( mask_image );
  prev_from=from_image;
  prev_to=to_image;
  prev_mask=mask_image;

  unsigned int min_dim = std::min( from_image.ni(), from_image.nj() );
  unsigned int num_stages=0;
  std::list<scaled_feature_node>::const_iterator it;
  for (it=feature_sets.begin();
      min_dim>128 && it!=feature_sets.end();
      num_stages++) {
    // downsample image
    vil_gauss_reduce( prev_from, dest, work_space );
    from_images.push_back( dest );
    prev_from = dest;
    dest.clear();  // clear the link underneath, to prevent misoperation

    // to image
    vil_gauss_reduce( prev_to, dest, work_space );
    to_images.push_back( dest );
    prev_to = dest;
    dest.clear( );  // clear the link underneath, to prevent misoperation

    // to image
    vil_gauss_reduce( prev_mask, dest, work_space );
    mask_images.push_back( dest );
    prev_mask = dest;
    dest.clear( );  // clear the link underneath, to prevent misoperation

    // reduce dimension
    min_dim >>= 1;

    // two scales per resolution
    ++it;
    if ( it==feature_sets.end() ) break;
    ++it;
  }

  // 2. robust weighting
  //
  rgrl_weighter_sptr wgter;
  {
    std::unique_ptr< rrel_m_est_obj > m_est_obj( new rrel_cauchy_obj(4) );
    wgter = new rgrl_weighter_m_est( std::move(m_est_obj), true, true) ;
  }

  // 3. Scale estimator
  //
  // Estimate scales based on weighted match.
  //
  rgrl_scale_estimator_unwgted_sptr unwgted_scale_est;
  rgrl_scale_estimator_wgted_sptr wgted_scale_est;
  {
    // muse and unwgted_scale_est are not used
    std::unique_ptr<rrel_objective> obj( new rrel_muset_obj( 1 ) );
    unwgted_scale_est = new rgrl_scale_est_closest( std::move(obj) );
    wgted_scale_est = new rgrl_scale_est_all_weights( );
    wgted_scale_est->set_debug_flag( 1 );
  }

  // 4. Setup evaluator used in pseudo matching, currently used
  //    normalized SSD
  rgrl_evaluator_sptr evaluator = new rgrl_evaluator_ssd();
  rgrl_estimator_sptr affine_sptr = new rgrl_est_affine();
  rgrl_estimator_sptr quadratic_sptr = new rgrl_est_quadratic();

  // 5. create multi-resolution data manager
  //
  // Set up the data manager
  rgrl_data_manager_sptr data_sptr = new rgrl_data_manager( true );  //activate multi-resolution
  it=feature_sets.begin(); // init
  for ( unsigned int i=0; i<=num_stages; ++i ) {
    // 5.0 create mask
    //
    rgrl_mask_sptr mask_sptr = new rgrl_mask_2d_image(mask_images[i]);

    // 5.1 create matcher at this resolution
    //
    // Pseudo matching
    rgrl_matcher_sptr matcher;
    matcher = new rgrl_matcher_pseudo< pixel_type>
              ( from_images[i], to_images[i], evaluator, mask_sptr, mask_sptr);
    matcher->set_debug_flag( 0 );

    // 5.2 create from features
    //
    // It is a problem since there are two scales within one resolution.
    // Brute force approach:
    // combine two scales
    // thus, need to copy two vectors into one
    std::vector< rgrl_feature_sptr > from_features;
    std::list< scaled_feature_node >::const_iterator next_iteration_it;
    // make next_iteration_it pointed to next resolution
    next_iteration_it = it;
    ++next_iteration_it;
    if( next_iteration_it !=  feature_sets.end() )  ++next_iteration_it;   // do not go beyond the end of list.

    double scaling_factor = 1.0 / double(1<<i);
    from_features.reserve( 3* it->size()  );

    for ( unsigned int j=0; j<2&&it!=feature_sets.end(); ++j,++it) {
      // Now copying features with corresponding coordinate scale
      for (const auto & feature : it->features_) {
        from_features.push_back( scale_location_by( feature, scaling_factor ) );
      }
    }

    rgrl_feature_set_sptr from_set = new rgrl_feature_set_location<2>( from_features );
    // it & it -1 are the two set of features corresponding to one resolution
    // skip to next
    it = next_iteration_it;

    // 5.3 create useless "to" features set
    std::vector< rgrl_feature_sptr > to_features( 1 );
    if ( from_features[0]->is_type( rgrl_feature_trace_region::type_id() ) )
      to_features[0] = new rgrl_feature_trace_region
        ( vnl_vector< double > ( 2, 0.0 ), vnl_vector< double > (2, 0.0), 0.0, 0.0 );
    else if ( from_features[0]->is_type( rgrl_feature_face_region::type_id() ) )
      to_features[0] = new rgrl_feature_face_region
        ( vnl_vector< double > ( 2, 0.0 ), vnl_vector< double > (2, 0.0), 0.0, 0.0 );
    else {
      std::cerr << " Wrong type of feature points!\n";
      std::exit(1);
    }
    rgrl_feature_set_sptr to_set = new rgrl_feature_set_location<2>( to_features );

    // 5.5 add data
    //
    data_sptr->add_data( i,          // stage
                         from_set,   // data from moving image
                         to_set,     // data from fixed image
                         matcher,    // matching mechanism
                         wgter,      // robust weighting
                         unwgted_scale_est,  // unweighted scale estimator
                         wgted_scale_est  );
    // 5.6 add estimator
    //
    // use quadratic transformation only at the finest stage
    if ( (i==0 || i==1 ) && model() == std::string("quadratic") )
      data_sptr->add_estimator( i, quadratic_sptr );
    else {
      data_sptr->add_estimator( i, affine_sptr );
    }

    // 5.7 set dimension increase
    //
    data_sptr->set_dimension_increase_for_next_stage( i, 2 );
  }

  // 6. Create the initializer
  //
  // We will initialize to an affine transform with an affine
  // transform estimator, and set the initial region to the global
  // ROI.
  //
  // Estimator for initial transform estimate type.

  rgrl_initializer_sptr initializer;
  {
    vnl_matrix<double> A( 2, 2, vnl_matrix_identity );
    vnl_vector<double> t( 2, 0.0 );
    std::cerr << "A =\n" << A << ", T = " << t << std::endl;
    if ( trans_file.set() )
      read_affine_trans_2d( trans_file(), A, t );
    std::cout << "A =\n" << A << ", T = " << t << std::endl;

    rgrl_transformation_sptr init_trans = new rgrl_trans_affine( A, t, vnl_matrix<double>( 6, 6, 0.0 ) );

    // now scale the transform for multi-resolution
    unsigned denom = 1<<num_stages;
    double scaling_factor = 1.0 / double(denom);
    init_trans = init_trans->scale_by( scaling_factor );

    // pseudo matcher needs prior scale
    rgrl_scale_sptr prior_scale = new rgrl_scale();
    double geometric_scale = 4.0;
    prior_scale->set_geometric_scale( geometric_scale );

    // set mask_box to image size
    auto* box_ptr = new rgrl_mask_box(2);
    box_ptr->set_x0( vnl_double_2(0, 0).as_ref() );
    const unsigned ni = from_images[num_stages].ni();
    const unsigned nj = from_images[num_stages].nj();
    box_ptr->set_x1( vnl_double_2( double(ni-1), double(nj-1) ).as_ref() );

    // set smart pointer
    rgrl_mask_sptr roi = box_ptr;
    box_ptr = nullptr;

    initializer = new rgrl_initializer_prior( roi, roi, affine_sptr, init_trans, num_stages, prior_scale );
  }

  // 7. Convergence: how to determine when we are done
  //
  rgrl_convergence_tester_sptr conv_test = new rgrl_convergence_on_weighted_error( 1.5 );

  // 8. Setup engine
  //
  rgrl_feature_based_registration reg( data_sptr, conv_test );
  // enforce lower bound of geometric scale
  reg.set_expected_min_geometric_scale( 0.25 );
  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());
  // before running reg engine, release unnecessary space
  feature_sets.clear();

  // Get an estimate
  //
  std::cerr << "Start registering...\n\n";
  reg.run( initializer );

  // Output the xform
  //
  rgrl_transformation_sptr final_trans = reg.final_transformation();
  rgrl_scale_sptr final_scale = reg.final_scale();

  std::cout << "Final objective = " << reg.final_status()->objective_value() << std::endl;

  if ( output_xform.set() ) {
    std::ofstream ofs( output_xform() );

    if ( final_trans->is_type( rgrl_trans_affine::type_id() ) ) {
      rgrl_trans_affine* final = rgrl_cast<rgrl_trans_affine*>(final_trans);
      ofs << final->A() << final->t() << std::endl << std::endl;
    }
    else {
      rgrl_trans_quadratic* final = rgrl_cast<rgrl_trans_quadratic*>(final_trans);
      ofs << final->Q() << final->A() << final->t() << std::endl << std::endl;
    }

    ofs << final_scale->geometric_scale() << '\n' << 0 ;
    ofs.close();
  }
  else { // output_xform not set, so dump to the std output
    std::cout << "Final transform:" << std::endl;

    if ( final_trans->is_type( rgrl_trans_affine::type_id() ) ) {
      rgrl_trans_affine* final = rgrl_cast<rgrl_trans_affine*>(final_trans);
      std::cout << final->A() << final->t() << std::endl << std::endl;
    }
    else {
      rgrl_trans_quadratic* final = rgrl_cast<rgrl_trans_quadratic*>(final_trans);
      std::cout << final->Q() << final->A() << final->t() << std::endl << std::endl;
    }

    std::cout << final_scale->geometric_scale() << "\n\n";
  }

  // Compute errors
  {
    rgrl_scale_sptr scale;   //  dummy scale ... unused
    rgrl_match_set const& ms = *reg.final_match_set();
    bool use_signature_only = true;
    scale = wgted_scale_est->estimate_weighted( ms, scale, use_signature_only );

    std::vector< double > d;
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
    std::nth_element( d.begin(), d.begin()+d.size()/2, d.end() );
    e = d[d.size()/2];

    std::cout << "CP match set from size = " << ms.from_size() << std::endl
             << "Final median of alignment error: " << e << std::endl
             << "Final average of alignment error: " << sum / num << std::endl;
  }

  // Perform testing
  //
  testlib_test_start( "Registration using pseudo-matching" );
  testlib_test_assert_near("", reg.final_status()->objective_value(), 0.25, 1e-2 );
  return testlib_test_summary();
}
