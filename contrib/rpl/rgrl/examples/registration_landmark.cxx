// BeginLatex
//
// The registration problem is to align the retinal images using the
// pre-computed landmarks. A landmark feature is defined as a
// branching or cross-over point of a vascular-like structure. It is
// characterized by a number of outgoing direction vectors. To make it
// more general, any feature with a center location and a set of
// outgoing direction vectors can be a "landmark", such as a
// corner. The implementation of a landmark is
// \code{rgrl\_feature\_landmark}.
//
// This example demonstrates a simplified implementation of the
// hierarchical pairwise registration algorithm in
// \cite{can:pami-pair-02}. Characteristics of the algorithm include
// the use of landmarks, multiple matches enhanced by signature
// similarity, and random sampling \cite{fisch:cacm81} for
// initialization, which is the focus of this section.
//
// The simplified hierarchical pairwise registration algorithm consists
// of the following steps:
// \begin{enumerate}
// \item Manually guess the initial translation (need not be very accurate).
// \item Based on the initial translation, generate 2 matches for each
// landmark in the moving image. A distance threshold maybe applied to
// further reduce the number of matches.
// \item Run random sampling on the candidate match set to obtain the
// best affine transformation. Since the estimate is computed using only
// 3 matches, refinement of the transformation is necessary.
// \item Refine the affine transformation using Iteratively-Reweighted
// Least-Squares (IRLS). IRLS is handled by the registration engine,
// and it takes the original feature sets. The transformation model for the
// engine is quadratic, which is the final model of the application.
// \end{enumerate}
//
// EndLatex


#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector_fixed.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view.h>

#include <rrel/rrel_lms_obj.h>
#include <rrel/rrel_tukey_obj.h>

#include <rgrl/rgrl_feature_based_registration.h>
#include <rgrl/rgrl_initializer_ran_sam.h>
#include <rgrl/rgrl_feature_landmark.h>
#include <rgrl/rgrl_feature_set_location_masked.h>
#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_match_set.h>

#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_est_quadratic.h>
#include <rgrl/rgrl_trans_translation.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_trans_quadratic.h>

#include <rgrl/rgrl_scale.h>
#include <rgrl/rgrl_scale_est_all_weights.h>
#include <rgrl/rgrl_scale_est_closest.h>
#include <rgrl/rgrl_weighter_m_est.h>
#include <rgrl/rgrl_convergence_on_weighted_error.h>

#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_data_manager.h>

#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_event.h>
#include <rgrl/rgrl_command.h>

#include "test_util.h"

typedef vcl_vector< rgrl_feature_sptr >  feature_vector;
typedef vnl_vector_fixed<double,2>       vector_2d;
typedef vcl_vector< vnl_vector<double> > vec_vec_type;

void
read_feature_file( const char* filename,
                   feature_vector& landmarks )
{
  vcl_ifstream istr( filename );

  if ( !istr ) {
    vcl_cerr<<"ERROR: Cannot open "<<filename<<vcl_endl;
    return;
  }

  vector_2d location;
  vector_2d direction;
  int num_landmarks;
  int num_directions;
  double temp1, temp2; //to store information not needed in this example

  istr >> num_landmarks;
  for ( int li = 0; li < num_landmarks; ++li ) {
    istr >> location[0] >> location[1] >> temp1 >> temp2 >> num_directions;
    vec_vec_type directions;
    for ( int di = 0; di < num_directions; ++di ) {
      istr >> direction[0] >> direction[1];
      directions.push_back( direction.as_ref() );
    }
    landmarks.push_back( new rgrl_feature_landmark(location, directions) );
  }

  istr.close();
  vcl_cout<<"There are "<<landmarks.size()<<" landmarks"<<vcl_endl;
}

// using command/observer pattern
class command_iteration_update: public rgrl_command
{
 public:
  void execute(rgrl_object* caller, const rgrl_event & event )
  {
    execute( (const rgrl_object*) caller, event );
  }

  void execute(const rgrl_object* caller, const rgrl_event & event )
  {
    const rgrl_feature_based_registration* reg_engine =
      dynamic_cast<const rgrl_feature_based_registration*>(caller);
    rgrl_transformation_sptr trans = reg_engine->current_transformation();

    if ( trans->is_type( rgrl_trans_affine::type_id() ) ) {
      rgrl_trans_affine* affine_xform = rgrl_cast<rgrl_trans_affine*>(trans);
      vcl_cout<<"Initial xform: A =\n"<<affine_xform->A()<<"t = "<<affine_xform->t()<<vcl_endl;
    }
    else if ( trans->is_type( rgrl_trans_quadratic::type_id() ) ) {
      rgrl_trans_quadratic* q_xform = rgrl_cast<rgrl_trans_quadratic*>(trans);
      vcl_cout<<"xform: Q\n"<<q_xform->Q()<<"A = "<<q_xform->A()<<
        "t = "<<q_xform->t()<<vcl_endl;
    }
    else vcl_cout<<"Unknown type"<<vcl_endl;
  }
};

int
main( int argc, char* argv[] )
{
  if ( argc < 4 ) {
    vcl_cerr << "Missing Parameters\n"
             << "Usage: " << argv[0]
             << " FixedImageLandmarkFile MovingImageLandmarkFile MaskImage\n";
    return 1;
  }

  prepare_testing();

  // First, read in the features from the external files.
  //
  feature_vector moving_landmark_set;
  feature_vector fixed_landmark_set;
  const char* fixed_file_name = argv[1];
  const char* moving_file_name = argv[2];

  read_feature_file( moving_file_name, moving_landmark_set );
  read_feature_file( fixed_file_name, fixed_landmark_set );

  // Prepare the feature sets. The mask is defined by an image with
  // non-zero intensity value for valid pixel positions.
  //
  const char* make_file_name = argv[3];
  vil_image_view<vxl_byte> mask_image = vil_load(make_file_name);
  rgrl_mask_sptr mask = new rgrl_mask_2d_image( mask_image );

  // A masked feature set, \code{rgrl\_feature\_set\_location\_masked},
  // differs from its super-class, \code{rgrl\_feature\_set\_location}, by
  // checking if the requested features are in the valid region.
  //
  const unsigned int dimension = 2;
  rgrl_feature_set_sptr moving_feature_set =
    new rgrl_feature_set_location_masked<dimension>(moving_landmark_set,
                                                    mask);
  rgrl_feature_set_sptr fixed_feature_set =
    new rgrl_feature_set_location_masked<dimension>(fixed_landmark_set,
                                                    mask);
  rgrl_mask_box moving_image_region = moving_feature_set->bounding_box();
  rgrl_mask_box fixed_image_region  = fixed_feature_set->bounding_box();

  // BeginLatex
  //
  // Landmarks are first imported from files. With the initial
  // translation estimate (\code{init\_translation}), we generate 2
  // closest matches, in terms of the Euclidean distance, for each
  // landmark in the moving image. This task is handled by the
  // k-nearest matcher, which is one of the major components in the
  // registration engine. In this example, the matcher operates
  // outside the engine. The \code{pruned\_match\_set}, returned by
  // the matcher, contains matches for random sampling.
  //
  // EndLatex


  double shift_x = -286;
  double shift_y = -42;
  vector_2d shift( shift_x, shift_y);
  rgrl_transformation_sptr init_translation = new rgrl_trans_translation( shift );
  rgrl_scale_sptr dummy_scale = new rgrl_scale();

  // BeginCodeSnippet
  int k = 2;
  rgrl_matcher_sptr cp_matcher = new rgrl_matcher_k_nearest( k );
  rgrl_match_set_sptr pruned_match_set =
    cp_matcher->compute_matches(*moving_feature_set,
                                *fixed_feature_set,
                                *init_translation,
                                moving_image_region,
                                *dummy_scale);
  // EndCodeSnippet

  vcl_cout<<"pruned match set = "<<pruned_match_set->from_size()<<vcl_endl;

  // BeginLatex
  //
  // Random sampling \cite{stewart:pami95} is performed by
  // \code{rgrl\_initializer\_ran\_sam} . It is crucial to provide the
  // transformation estimator with the dimension of the image. This
  // allows the initializer to compute the minimum number of matches
  // to constrain a transformation. Another necessary component for
  // random sampling is the unweighted scale estimator
  // (\code{unwgted\_scale\_est}), which is needed to measure the
  // error for each fit. All elements are passed into the initializer
  // via a function call \code{set\_data}. The current implementation
  // only handles a pre-computed match set.
  //
  // EndLatex

  // BeginCodeSnippet
  int dim = 2;

  rgrl_estimator_sptr affine_estimator = new rgrl_est_affine(dim);

  vcl_auto_ptr<rrel_objective> obj_fun( new rrel_lms_obj(1) );
  rgrl_scale_estimator_unwgted_sptr unwgted_scale_est =
    new rgrl_scale_est_closest( obj_fun );

  rgrl_initializer_ran_sam* ran_sam = new rgrl_initializer_ran_sam();
  ran_sam->set_data(pruned_match_set,
                    unwgted_scale_est,
                    moving_image_region,
                    affine_estimator,
                    0 );
  rgrl_initializer_sptr initializer = ran_sam;
  // EndCodeSnippet

  // BeginLatex
  //
  // The estimation of the best affine transformation is triggered
  // when the registration engine asks for an initial
  // transformation. Only one initial estimate is provided by random
  // sampling initializer, since the estimation can only be triggered
  // once.
  //
  // EndLatex

  // To allow the similarity weight to be part of the final match
  // weight, we have to be careful when defining
  // \code{rgrl\_weighter\_m\_est}. Make sure
  // \code{signature\_precomputed} is allowed.
  //
  vcl_auto_ptr<rrel_m_est_obj>  m_est_obj( new rrel_tukey_obj(4) );
  bool use_signature_error = false;
  bool signature_precomputed = true;
  rgrl_weighter_sptr wgter = new rgrl_weighter_m_est(m_est_obj,
                                                     use_signature_error,
                                                     signature_precomputed);

  // weighted scale estimators
  //
  rgrl_scale_estimator_wgted_sptr wgted_scale_est =
    new rgrl_scale_est_all_weights();

  // Transformation estimator for quadratic
  //
  rgrl_estimator_sptr quad_estimator = new rgrl_est_quadratic();


  // Convergence test
  //
  double tolerance = 1.5;
  rgrl_convergence_tester_sptr conv_test =
    new rgrl_convergence_on_weighted_error( tolerance );

  // Set up the data storage
  //
  rgrl_data_manager_sptr data = new rgrl_data_manager();
  data->add_data( moving_feature_set,   // data from moving image
                  fixed_feature_set,    // data from fixed image
                  cp_matcher,           // matcher for this data
                  wgter,                // weighter
                  unwgted_scale_est,    // unweighted scale estimator
                  wgted_scale_est );    // weighted scale estimator

  // The quadratic estimtaor has to be added to the data manager,
  // since it differs from the one provided by the initializer.
  data->add_estimator( quad_estimator );

  // Set up the registration engine
  //
  rgrl_feature_based_registration reg( data, conv_test );

  // Add observer for debugging
  //
  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());

  // Run ...
  //
  // BeginLatex
  //
  // We skip the part on the setting of the registration engine for
  // robust estimation of the final quadratic
  // transformation. Different from previous examples, the
  // initializer, instead of the initial estimate, initiates the
  // registration process.
  //
  // EndLatex

  // BeginCodeSnippet
  reg.run( initializer );
  // EndCodeSnippet

  // Output Results
  //
  if ( reg.has_final_transformation() ) {
    vcl_cout<<"Final xform:\n";
    rgrl_transformation_sptr trans = reg.final_transformation();
    rgrl_trans_quadratic* q_xform = rgrl_cast<rgrl_trans_quadratic*>(trans);
    vcl_cout<<"Q =\n"<<q_xform->Q()<<"A = "<<q_xform->A()<<"t = "<<q_xform->t()<<vcl_endl
            <<"Final alignment error = "<<reg.final_status()->error()<<vcl_endl;
  }

  // BeginLatex
  //
  // Since the initial translation prior to random sampling was
  // specified manually, this example only works with specific feature
  // files, \verb+rgrl/examples/IMG0004.pgm.landmarks.txt+ for the
  // fixed image, and \verb+rgrl/examples/IMG0002.pgm.landmarks.txt+
  // for the moving image. The mask image is
  // \verb+rgrl/examples/mask.png+. Running the example with the given
  // input files, the final result is
  //
  // \begin{verbatim}
  // Final xform:
  // Q =
  // 0.000101208 0.000103199 9.30317e-06
  // 1.73496e-05 2.33522e-05 -5.01135e-06
  // A = 0.858602 -0.0974608
  // -0.0434711 0.972142
  // t = -208.821 -10.0152
  // Final alignment error = 1.00458
  // \end{verbatim}
  //
  // EndLatex

  // Perform testing
  //
  test_macro( "Registration with landmarks only",
              reg.final_status()->error(), 1.1 );
}
