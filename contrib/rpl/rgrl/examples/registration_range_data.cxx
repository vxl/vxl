// BeginLatex
//
// The focus of this example is to demonstrate registration of 3D
// range data using rgrl_feature_face_pt as the feature type.
//
// Read in the files of the format
//      total_number_of_features
//      x y z nx ny nz
//      x y z nx ny nz
//      ...
//
// If the feature file only contains (x, y, z), run
// estimate_normals_3d first to generate the normals.
//
// Be aware that this program dumps out files for transformations and
// matches in the callback(.) function.
//
//  EndLatex

#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>

#include <rrel/rrel_muset_obj.h>
#include <rrel/rrel_tukey_obj.h>

#include <rgrl/rgrl_feature_based_registration.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_initializer_prior.h>
#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_est_affine.h>

#include <rgrl/rgrl_feature_face_pt.h>
#include <rgrl/rgrl_scale_est_all_weights.h>
#include <rgrl/rgrl_scale_est_closest.h>
#include <rgrl/rgrl_weighter_m_est.h>
#include <rgrl/rgrl_convergence_on_weighted_error.h>
#include <rgrl/rgrl_match_set.h>

#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_data_manager.h>

#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_event.h>
#include <rgrl/rgrl_command.h>

#include <testlib/testlib_test.h>
void testlib_enter_stealth_mode(); // defined in core/testlib/testlib_main.cxx

typedef vcl_vector< rgrl_feature_sptr >  feature_vector;
typedef vnl_vector_fixed<double,3>       vector_3d;

void
read_feature_file( const char* filename,
                   feature_vector& features, int sample_spacing )
{
  vcl_ifstream istr( filename );

  if ( !istr ) {
    vcl_cerr<<"ERROR: Cannot open "<<filename<<vcl_endl;
    return;
  }

  vector_3d location;
  vector_3d normal;

  int total;
  istr >> total;
  for (int i = 0; i<total; i+=sample_spacing) {
    istr >> location[0] >> location[1] >> location[2]
         >>normal[0]>>normal[1]>>normal[2];
    features.push_back( new rgrl_feature_face_pt(location, normal) );
  }

  istr.close();

  vcl_cout<<"There are "<<features.size()<<" features"<<vcl_endl;
}

// using command/observer pattern
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
    rgrl_transformation_sptr trans = reg_engine->current_transformation();
    rgrl_trans_affine* a_xform = rgrl_cast<rgrl_trans_affine*>(trans);
    vcl_cout<<"xform: A = "<<a_xform->A()<<"t = "<<a_xform->t()<<vcl_endl;

#if 0 // commented out
    static unsigned count = 0;
    ++count;

    vcl_ostringstream s;
    s << "xform-dump-"<<count;
    vcl_ofstream xform_out( s.str().c_str() );

    xform_out<<"xform: A = "<<a_xform->A()<<"t = "<<a_xform->t()<<vcl_endl;
    xform_out.close();

    // Output the matches
    //
    typedef rgrl_match_set::from_iterator  from_iter;
    typedef from_iter::to_iterator         to_iter;

    vcl_ostringstream ss,ss2;
    ss << "matches-dump-"<<count;
    vcl_ofstream fout( ss.str().c_str() );
    ss2 << "sub-matches-dump-"<<count;
    vcl_ofstream fout2( ss2.str().c_str() );

    for ( unsigned ms=0; ms < match_sets.size(); ++ms ) {
      rgrl_match_set_sptr match_set = match_sets[ms];
      //  for each from image feature being matched
      for ( from_iter fitr = match_set->from_begin();
            fitr != match_set->from_end(); ++fitr ){
        if ( fitr.size() == 0 )  continue;

        rgrl_feature_sptr from_feature = fitr.from_feature();
        fout<<from_feature->location()<<' ';
        rgrl_feature_sptr mapped_from = fitr.mapped_from_feature();
        fout<<' '<<mapped_from->location();
        for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
          //  for each match with a "to" image feature
          rgrl_feature_sptr to_feature = titr.to_feature();
          fout<<' '<<to_feature->location();
          double error = titr.to_feature()->geometric_error( *mapped_from );
          fout<<' '<<error<<vcl_endl;
          if (error > 1) fout2<<mapped_from->location()<<vcl_endl;
        }
      }
    }
    fout.close();
    fout2.close();
#endif // 0
  }
};

int
main( int argc, char* argv[] )
{
  if ( argc < 3 ) {
    vcl_cerr << "Missing Parameters\n"
             << "Usage: " << argv[0]
             << " FixedImageFeatureFile MovingImageFeatureFile\n";
    return 1;
  }

  // Don't allow Visual Studio to open critical error dialog boxes
  testlib_enter_stealth_mode();

  // BeginLatex
  //
  // In the main body of the program, we first read in the features
  // from the external files.
  //
  // EndLatex

  feature_vector moving_feature_points;
  feature_vector fixed_feature_points;
  const char* fixed_file_name = argv[1];
  const char* moving_file_name = argv[2];

  read_feature_file( moving_file_name, moving_feature_points, 50 );
  read_feature_file( fixed_file_name, fixed_feature_points, 1 );

  const unsigned int dimension = 3;
  rgrl_feature_set_sptr moving_feature_set;
  rgrl_feature_set_sptr fixed_feature_set;
  bool use_bins = true;
  moving_feature_set =
    new rgrl_feature_set_location<dimension>(moving_feature_points, !use_bins);
  fixed_feature_set =
    new rgrl_feature_set_location<dimension>(fixed_feature_points, !use_bins);

  // Transformation estimator
  //
  rgrl_estimator_sptr estimator = new rgrl_est_affine();

  //set the initial transformation to identity
  //
  // The correct xform from the Stanford range data repository
  // (http://graphics.stanford.edu/data/3Dscanrep/) is about 27 degree
  // around the y-axis, and
  // translation=[-0.0520211 -0.000383981 -0.0109223]
  //
  rgrl_transformation_sptr init_trans;
  vnl_matrix<double> A(3,3,vnl_matrix_identity);
  double angle = 35*vnl_math::pi/180; //35 degree rotation around y-axis
  //double angle = 27*vnl_math::pi/180; //35 degree rotation around y-axis
  A(0,0) = vcl_cos(angle); A(0,2) = vcl_sin(angle);
  A(2,0) = -vcl_sin(angle); A(2,2) = vcl_cos(angle);
  //vector_3d t(-0.06, -0.01, -0.02);
  //vector_3d t(-0.0520211, -0.000383981, -0.0109223);
  vector_3d t(-0.065, -0.015, -0.02);

  init_trans = new rgrl_trans_affine(A, t);

  //Initializer
  vector_3d x0(-1,-1,-1);           //upper left corner
  vector_3d x1(1,1,1);              //bottom right corner
  rgrl_mask_sptr moving_image_region = new rgrl_mask_box(x0, x1);
  rgrl_mask_sptr fixed_image_region = moving_image_region;  // assume two are identical
  rgrl_initializer_sptr initializer =
    new rgrl_initializer_prior( moving_image_region,
                                fixed_image_region,
                                estimator,
                                init_trans);

  //Matcher
  //
  unsigned int k = 1;
  rgrl_matcher_sptr cp_matcher = new rgrl_matcher_k_nearest( k );

  //Weighter
  //
  vcl_auto_ptr<rrel_m_est_obj>  m_est_obj( new rrel_tukey_obj(4) );
  rgrl_weighter_sptr wgter = new rgrl_weighter_m_est(m_est_obj, false, false);

  //Scale estimator
  //
  int max_set_size = 1000;  //maximum expected number of features
  vcl_auto_ptr<rrel_objective> muset_obj( new rrel_muset_obj( max_set_size , false) );

  rgrl_scale_estimator_unwgted_sptr unwgted_scale_est;
  rgrl_scale_estimator_wgted_sptr wgted_scale_est;

  unwgted_scale_est = new rgrl_scale_est_closest( muset_obj );
  wgted_scale_est = new rgrl_scale_est_all_weights();

  //convergence tester
  //
  double tolerance = 0.1;
  rgrl_convergence_tester_sptr conv_test =
    new rgrl_convergence_on_weighted_error( tolerance );

  // BeginCodeSnippet
  rgrl_data_manager_sptr data = new rgrl_data_manager();
  data->add_data( moving_feature_set,   // data from moving image
                  fixed_feature_set,    // data from fixed image
                  cp_matcher,           // matcher for this data
                  wgter,                // weighter
                  unwgted_scale_est,    // unweighted scale estimator
                  wgted_scale_est );    // weighted scale estimator

  rgrl_feature_based_registration reg( data, conv_test );
  reg.set_expected_min_geometric_scale( 0.01/1000 );
  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());

  reg.run( initializer );
  // EndCodeSnippet

  // Output Results
  if ( reg.has_final_transformation() ) {
    vcl_cout<<"Final xform:"<<vcl_endl;
    rgrl_transformation_sptr final_trans = reg.final_transformation();
    rgrl_trans_affine* a_xform = rgrl_cast<rgrl_trans_affine*>(final_trans);

    vcl_cout<<"A =\n"<<a_xform->A()<<"t = "<<a_xform->t()<<vcl_endl
            <<"Final alignment error = "<<reg.final_status()->error()<<vcl_endl;
  }

  // BeginLatex
  //
  // Let's execute this example with the feature files
  // \verb+examples/bun000.txt.out+ as the fixed set,
  // \verb+examples/bun045.txt.out+ as the moving set.
  //
  // EndLatex

  // Final xform: A =
  // 0.849755 -0.0318519 0.535087
  // -0.0309556 0.791855 0.117775
  // -0.561513 -0.0457463 0.834776
  // t = -0.0487823 0.00170826 -0.0100543
  // Final alignment error = 8.18704e-005
  //

  // Perform testing
  //
  testlib_test_start( "Registration of range data" );
  testlib_test_assert_near("", reg.final_status()->error(), 0.0, 1e-4 );
  return testlib_test_summary();
}
