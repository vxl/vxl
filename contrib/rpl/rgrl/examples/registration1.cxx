// BeginLatex
//
// This is a simple example to introduce the main elements for
// feature-based image registration. To fully understand this example,
// prior knowledge in object-oriented programming and memory
// management using smart pointer (\verb+vxl\vbl\vbl_smart_ptr+) is
// required.
//
// Most components in the registration library are not templated over
// the dimension of the image, since the dimension can be easily
// derived from the feature set. The following header files provide
// declarations for basic types of these components.
//
// EndLatex


#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector_fixed.h>
#include <testlib/testlib_test.h>
#include "test_util.h"

// BeginCodeSnippet
#include <rgrl/rgrl_feature_based_registration.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_initializer_prior.h>
#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_trans_translation.h>
#include <rgrl/rgrl_est_translation.h>
#include <rgrl/rgrl_convergence_on_median_error.h>
// EndCodeSnippet

#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_weighter_unit.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_data_manager.h>
#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_event.h>
#include <rgrl/rgrl_command.h>

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
    rgrl_trans_translation* xform = rgrl_cast<rgrl_trans_translation*>(trans);
    vcl_cout<<"Xform T = "<<xform->t()<<vcl_endl;
  }
};

int
main( int argc, char* argv[] )
{
  if ( argc < 2 ) {
    vcl_cerr << "Missing Parameters " << vcl_endl
             << "Usage: " << argv[0]
             << " ImageFeatureFile\n";
    return 1;
  }

  // BeginLatex
  //
  // In this over-simplified example, both the moving image and the
  // fixed image are chosen to be the same image. The initial
  // transformation is a translation perturbed from the
  // \emph{identity} transform. The purpose of the registration is to
  // align the two feature sets starting with the initial
  // transform. The final transform should be very close to the
  // \emph{identity} transform, since we're registering two identical
  // images.
  //
  // First, we load the features from the external file(s). The
  // features are the results of a preprocessor for feature
  // extraction.  Figure~\ref{fb-diagram} shows two examples of
  // retinal images. The features that are involved in registration
  // are the center locations of the vessels. The program reads in the
  // features according to the feature file format, and stores the
  // relevant information into the feature set. In this example, the
  // feature set is \code{rgrl\_feature\_set\_location}, which is a
  // sub-class of \code{rgrl\_feature\_set}. As implied by the class
  // name, \code{rgrl\_feature\_set\_location} stores a collection of
  // discrete features for which proximity is determined only by the
  // location. We're only interested in point-to-point registration
  // for now, and discard the directions from the feature files.
  //
  // EndLatex

  const char* filename = argv[1];
  vcl_ifstream istr( filename );
  if ( !istr ) {
    vcl_cerr<<"ERROR: Cannot open "<<filename<<vcl_endl;
    return 1;
  }

  prepare_testing();

  typedef vcl_vector< rgrl_feature_sptr >         feature_vector;
  typedef vnl_vector_fixed<double,2>              vector_2d;

  feature_vector  moving_feature_points;
  feature_vector  fixed_feature_points;
  vector_2d location;
  vector_2d direction;

  // BeginCodeSnippet
  const unsigned int dimension = 2;
  bool done = false;
  while ( !done && istr ) {
    if ( !(istr >> location[0] >> location[1] >> direction[0] >> direction[1]) )
      done = true;
    else moving_feature_points.push_back( new rgrl_feature_point(location) );
  }

  fixed_feature_points = moving_feature_points;

  rgrl_feature_set_sptr moving_feature_set;
  rgrl_feature_set_sptr fixed_feature_set;
  moving_feature_set = new rgrl_feature_set_location<dimension>(moving_feature_points);
  fixed_feature_set =  new rgrl_feature_set_location<dimension>(fixed_feature_points);
  // EndCodeSnippet

  istr.close();

  // BeginLatex
  //
  // A translation model is used for the registration. Here we
  // instantiate an estimator for the translation transformation
  //
  // EndLatex

  // BeginCodeSnippet
  rgrl_estimator_sptr estimator = new rgrl_est_translation();
  // EndCodeSnippet

  // BeginLatex
  //
  // The {\bf initial transformation} contains shift of 15 pixels in
  // both $x$ and $y$ directions from the \emph{identity} transform.
  //
  // EndLatex

  // BeginCodeSnippet
  rgrl_transformation_sptr init_transform;
  vector_2d init_parameters( 15, 15);
  init_transform = new rgrl_trans_translation(init_parameters);
  // EndCodeSnippet

  // BeginLatex
  //
  // We now prepare the {\bf initializer} for the registration.  The
  // initializer has to have the knowledge of the region of interest
  // for both the moving and fixed images, the transformation model
  // (defined by the estimator), and the initial transformation.
  //
  // EndLatex

  // BeginCodeSnippet
  vector_2d x0(0,0);          //upper left corner
  vector_2d x1(1023,1023);    //bottom right corner
  rgrl_mask_box moving_image_roi(x0, x1), fixed_image_roi(x0, x1);
  rgrl_initializer_sptr initializer =
    new rgrl_initializer_prior(moving_image_roi,
                               fixed_image_roi,
                               estimator,
                               init_transform);
  // EndCodeSnippet

  // BeginLatex
  //
  // The {\bf matcher} generates the feature correspondences.
  // \code{rgrl\_matcher\_k\_nearest} returns $k$ nearest matches for
  // each feature. By specifying $k=1$, it becomes the "Iterative
  // Closest Point" method \cite{besl:pami92,chen:ivc92,zhang:ijcv94}.
  //
  // EndLatex

  // BeginCodeSnippet
  unsigned int k = 1;
  rgrl_matcher_sptr cp_matcher = new rgrl_matcher_k_nearest( k );
  // EndCodeSnippet

  // BeginLatex
  //
  // The list of matches returned by the matcher may contain outliers
  // (wrong matches) due to occlusion or noise in the images. Common
  // methods to handle outliers include thresholding based on some
  // distance metric, and robust weighting, which is implemented in
  // rgrl. In this example, we use simple weighting scheme by
  // assigning a unit weight to all matches.  This results in
  // Least-Squares estimation.
  //
  // EndLatex

  // BeginCodeSnippet
  rgrl_weighter_sptr wgter = new rgrl_weighter_unit;
  // EndCodeSnippet

  // BeginLatex
  //
  // The {\bf convergence tester} determines if the transformation has
  // converged and is considered good enough. The convergence test can
  // be based on the median or weighted error of the matches. In this
  // example, since we assign a unit weight to the matches, it is more
  // robust to use median error for the convergence test. The upper
  // bound of the error (tolerance) for the result to be considered
  // good enough is set to 1.5 pixels for this example. However, this
  // value has no effect on the termination of the registration
  // process. The registration process terminates only if the
  // transformation has converged, the estimation has been oscillating
  // or the number of iterations has exceeded the limit.
  //
  // EndLatex

  // BeginCodeSnippet
  double tolerance = 1.5;
  rgrl_convergence_tester_sptr conv_test =
    new rgrl_convergence_on_median_error( tolerance );
  // EndCodeSnippet

  // BeginLatex
  //
  // We now have all the components and data ready for the
  // registration process. A registration process may contain several
  // stages, such as a number of resolutions. Each stage has its own
  // data sets, matcher, weighter, scale estimators (if needed), and
  // transformation model(s). Only the initializer and the convergence
  // tester are global to all stages.
  //
  // EndLatex

  // BeginCodeSnippet
  rgrl_data_manager_sptr data = new rgrl_data_manager();
  data->add_data( moving_feature_set, // data from moving image
                  fixed_feature_set,  // data from fixed image
                  cp_matcher );       // matcher for this data

  rgrl_feature_based_registration reg( data, conv_test );


  // EndCodeSnippet

  // BeginLatex
  //
  // In the case where the estimation never converged, it is prudent
  // to establish a limit on the number of iterations, where one
  // iteration consists of matching and transformation estimation (see
  // Figure~\ref{fb-diagram}).  The default is set to 25. The number
  // can be changed using method \code{set\_max\_icp\_iter()}.
  //
  // EndLatex

  // BeginCodeSnippet
  reg.set_max_icp_iter(10);
  reg.set_expected_min_geometric_scale(0.001);
  // EndCodeSnippet

  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());
  reg.set_debug_flag(1);

  // BeginLatex
  //
  // The registration process is triggered by an invocation of the method
  // \code{run()}.
  //
  // EndLatex

  // BeginCodeSnippet
  reg.run( initializer );
  // EndCodeSnippet

  // BeginLatex
  //
  // If the registration succeeds with a transformation within the
  // specified accuracy, we down-cast the transformation to the proper
  // type (\code{rgrl\_trans\_translation}) in order to access the
  // transformation parameters. The final alignment error is
  // accessible via the \code{final\_status()} method.
  //
  // EndLatex

  // BeginCodeSnippet
  if ( reg.has_final_transformation() ) {
    vcl_cout<<"Final xform: "<<vcl_endl;
    rgrl_transformation_sptr trans = reg.final_transformation();
    rgrl_trans_translation* a_xform = rgrl_cast<rgrl_trans_translation*>(trans);
    vcl_cout<<"t = "<<a_xform->t()<<vcl_endl
            <<"Final alignment error = "<<reg.final_status()->error()<<vcl_endl;
  }
  // EndCodeSnippet

  // BeginLatex
  //
  // Let's execute this example with the feature file
  // \verb+examples/IMG0002.pgm.txt+
  //
  // EndLatex

  // Perform testing
  //
  test_macro( "Registration for introduction" , reg.final_status()->error(), 1e-2 );
}
