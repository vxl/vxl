// BeginLatex
//
// Multi-resolution approach is widely used to improve the domain of
// convergence, since there are fewer local minima present at the
// lower resolution. Different models can also be used at different
// resolutions for better stability
// \cite{bergen:eccv92,feldmar:cviu97,sawhney:pami99}. Registration is
// first performed at a coarse scale (low resolution), and the result
// is used to initialize the process at the next finer scale.
//
// Multi-resolution is handled naturally by the registration engine,
// as illustrated in Figure~\ref{fig:multi-resol}. A resolution is
// represented as a "stage". Components for each stage are stored in
// the data manager, same as previous examples, and the data manager
// handles data of multiple stages. The higher the index of the stage,
// the lower the resolution is. Stage 0 represents the original
// resolution.
//
// \begin{figure}[t]
// \center{\includegraphics[width=5in]{multi_resol}}
// \caption{Multi-resolution registration. The diagram depicts the
// replationship among the registration engine, the initializer and
// the data manager. The initializer specifies the stage at which the
// registration begins. At each stage/resolution, the engine iterates
// among matching, robust estimation and convergence testing as
// illustrated in Figure~\ref{fb-diagram}. The registration process
// terminates when the stage is below 0, which is the highest resolution.}
// \label{fig:multi-resol}
// \end{figure}
//
// In this example, we register the retinal images using 2
// resolutions: the original resolution and the down-sampled. The
// image of the lower resolution is generated from the original using
// a down-sample factor of 2. We also assign different transformation
// models to different resolutions. An affine transformation model (6
// parameters) is for the lower resolution and a quadratic model (12
// parameters) is for the original. Starting at the lower resolution,
// the registration process is initialized with an \emph{identity}
// transform.  In this section, we'll only discuss steps that are
// relevant to multi-resolution registration.
//
// EndLatex


#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector_fixed.h>

#include <rrel/rrel_muset_obj.h>
#include <rrel/rrel_tukey_obj.h>

#include <rgrl/rgrl_feature_based_registration.h>
#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_initializer_prior.h>
#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_est_quadratic.h>
#include <rgrl/rgrl_trans_quadratic.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_trans_affine.h>

#include <rgrl/rgrl_weighter_m_est.h>
#include <rgrl/rgrl_convergence_on_weighted_error.h>
#include <rgrl/rgrl_scale_est_all_weights.h>
#include <rgrl/rgrl_scale_est_closest.h>

#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_data_manager.h>

#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_event.h>
#include <rgrl/rgrl_command.h>

#include "test_util.h"

typedef vcl_vector< rgrl_feature_sptr >  feature_vector;
typedef vnl_vector_fixed<double,2>       vector_2d;

void
read_feature_file( const char*     filename,
                   feature_vector& trace_points )
{
  vcl_ifstream istr( filename );

  if ( !istr ) {
    vcl_cerr<<"ERROR: Cannot open "<<filename<<vcl_endl;
    return;
  }

  vector_2d location;
  vector_2d direction;
  bool done = false;
  while ( !done && istr ) {
    if ( !(istr >> location[0] >> location[1] >> direction[0] >> direction[1]) )
      done = true;
    else trace_points.push_back( new rgrl_feature_trace_pt(location, direction) );
  }
  istr.close();
  vcl_cout<<"There are "<<trace_points.size()<<" features"<<vcl_endl;
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
      rgrl_trans_affine* a_xform = rgrl_cast<rgrl_trans_affine*>(trans);
      vcl_cout<<"xform: A =\n"<<a_xform->A()<< "t = "<<a_xform->t()<<vcl_endl;
    }
    else if ( trans->is_type( rgrl_trans_quadratic::type_id() ) ){
      rgrl_trans_quadratic* q_xform = rgrl_cast<rgrl_trans_quadratic*>(trans);
      vcl_cout<<"xform: Q =\n"<<q_xform->Q()<<"A = "<<q_xform->A()
              <<"t = "<<q_xform->t()<<vcl_endl;
    }
  }
};

int
main( int argc, char* argv[] )
{
  if ( argc < 5 ) {
    vcl_cerr << "Missing Parameters " << vcl_endl
             << "Usage: " << argv[0]
          << " FixedImageFeatureFileHighRes FixedImageFeatureFileLowRes MovingImageFeatureFileHighRes MovingImageFeatureFileLowRes";
    return 1;
  }

  prepare_testing();

  //read in the feature files
  //
  feature_vector moving_fps_high_res;
  feature_vector moving_fps_low_res;
  feature_vector fixed_fps_high_res;
  feature_vector fixed_fps_low_res;

  const char* fixed_file_name_high_res = argv[1];
  const char* fixed_file_name_low_res = argv[2];
  const char* moving_file_name_high_res = argv[3];
  const char* moving_file_name_low_res = argv[4];

  read_feature_file( moving_file_name_high_res, moving_fps_high_res );
  read_feature_file( moving_file_name_low_res, moving_fps_low_res );
  read_feature_file( fixed_file_name_high_res, fixed_fps_high_res );
  read_feature_file( fixed_file_name_low_res, fixed_fps_low_res );

  // Prepare feature sets
  //
  rgrl_feature_set_sptr moving_feature_set_high_res = new rgrl_feature_set_location<2>(moving_fps_high_res);
  rgrl_feature_set_sptr moving_feature_set_low_res = new rgrl_feature_set_location<2>(moving_fps_low_res);
  rgrl_feature_set_sptr fixed_feature_set_high_res = new rgrl_feature_set_location<2>(fixed_fps_high_res);
  rgrl_feature_set_sptr fixed_feature_set_low_res = new rgrl_feature_set_location<2>(fixed_fps_low_res);
  rgrl_mask_box image_roi = moving_feature_set_low_res->bounding_box();

  // Set the initial transformation to be identity
  //
  int dim = 2;
  rgrl_transformation_sptr initial_transformation = new rgrl_trans_affine(dim);

  // BeginLatex
  //
  // We add data to each stage the same way as before. However, a few
  // extra operations on the data manager are required for
  // multi-resolution registration. First, it is necessary to specify
  // the transition going from the lower to the higher resolution
  // using \code{set\_dimension\_increase\_for\_next\_stage}
  // method. This method sets the parameter that allows the internal
  // data to be scaled properly when moving to the next resolution
  // level if feature locations are in image coordinate\footnote{In
  // ITK, such a parameter is not necessary, since the estimation is
  // performed in physical coordinate.}. In this example, since the
  // lower resolution image is half the size for each dimension of the
  // original, the \code{dimension\_increase} is set to 2 for the
  // lower resolution. Second, the estimator for each stage is
  // specified explicitly, since different models are desirable at
  // different resolutions. If the estimators are not set, the one
  // provided by the initialization is the default for the lowest
  // resolution, which in turn passes the estimator to the next stage.
  //
  // EndLatex

  // BeginCodeSnippet
  bool multi_resol = true;
  rgrl_data_manager_sptr data = new rgrl_data_manager( multi_resol );

  unsigned resolution = 1;                   //lower resolution
  double dimension_increase = 2;
  data->add_data( resolution,
                  moving_feature_set_low_res,
                  fixed_feature_set_low_res );
  rgrl_estimator_sptr affine_model = new rgrl_est_affine();
  data->add_estimator( resolution, affine_model);
  data->set_dimension_increase_for_next_stage( resolution, dimension_increase);

  resolution = 0;              //original resolution
  data->add_data( resolution,
                  moving_feature_set_high_res,
                  fixed_feature_set_high_res );
  rgrl_estimator_sptr quadratic_model = new rgrl_est_quadratic();
  data->add_estimator( resolution, quadratic_model);

  rgrl_feature_based_registration reg( data );

  //EndCodeSnippet

  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());

  // BeginLatex
  //
  // To run the registration engine, the starting resolution has to be
  // specified. If an \code{rgrl\_initializer} handles initialization,
  // the initializer has to have the knowledge of the resolution level
  // at which the registration begins. Again, the higher the number is
  // for the resolution level, the lower the image resolution. The
  // highest resolution is always for $0^{th}$ stage\footnote{In ITK,
  // it is the opposite. The highest index represents the finest
  // resolution.}.
  //
  //EndLatex

  int starting_resolution = 1;
  reg.run( image_roi, affine_model, initial_transformation, 0,
           starting_resolution );

  // Output Results
  //
  if ( reg.has_final_transformation() ) {
    vcl_cout<<"Final xform: "<<vcl_endl;
    rgrl_transformation_sptr trans = reg.final_transformation();
    rgrl_trans_quadratic* q_xform = rgrl_cast<rgrl_trans_quadratic*>(trans);
    vcl_cout<<"Q =\n"<<q_xform->Q()<<"A = "<<q_xform->A()
            <<"t = "<<q_xform->t()<<vcl_endl
            <<"Final alignment error = "<<reg.final_status()->error()<<vcl_endl;
  }

  // BeginLatex
  //
  // Let's execute this example with the feature files
  // \verb+rgrl/examples/IMG0002.pgm.txt+ and
  // \verb+rgrl/examples/IMG0003.pgm.txt+. The corresponding low
  // resolution files are \verb+rgrl/examples/IMG0002_low.pgm.txt+ and
  // \verb+rgrl/examples/IMG0003_low.pgm.txt+, respectively. The first
  // transformation below is the result at the lower resolution. The
  // second transformation is the final estimate using a quadratic
  // model.
  //
  // \begin{verbatim}
  //
  // xform: A =
  // 0.999887 -0.00609213
  // 0.00332669 0.998528
  // t = -10.1254 -0.63757
  //
  //
  // Final xform:
  // Q =
  // 1.69245e-05 1.00902e-05 2.49065e-06
  // 2.81146e-07 3.95073e-06 8.30134e-06
  // A = 0.979931 -0.018122
  // -0.00138591 0.989724
  // t = -13.0714 2.17319
  // Final alignment error = 0.430113
  //
  // \end{verbatim}
  //
  // EndLatex

  // Perform testing
  //
  test_macro( "Registration with multi-resolution", reg.final_status()->error(), 1);
}
