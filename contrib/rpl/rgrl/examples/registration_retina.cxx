// BeginLatex
//
// Another level of flexibility is achieved by allowing a view
// generation, as illustrated in Figure~\ref{fg:view_based}. A
// \emph{view} is a snapshot of the status of the registration
// process. It is a combination of a stage (resolution), a
// transformation model and estimate, and an image region over which
// the model currently applies. The view-based framework unifies a
// number of variations of general correspondence-based registration.
//
// Through the manipulation of the \emph{view}, the \emph{view
// generator} may define complex behavior during registration. We have
// implemented a dual-bootstrap view generator~\cite{stewart:tmi03},
// which applies the uncertainty in the transformation estimates to
// automatically control region growth and model selection throughout
// the registration. Other variations of correspondence-based
// registration can be built into the view-based framework in the
// future by defining new view generators.
//
// \begin{figure}[tb]
// \center{\includegraphics[width=5in]{view_based_new}}
// \caption{Architecture of the inner loop of single-stage, view-based
// registration} \label{fg:view_based} \end{figure}
//
// With the same set of images and features as in
// section~\ref{sec:init:inv}, we now apply invariant indexing for
// initialization and DBICP (Dual-Bootstrap Iterative Closest Point)
// for refinement. Details of the algorithm can be found in
// \cite{stewart:tmi03}. We will not repeat the discussion on
// initialization, but only concentrate on DBICP.
//
// EndLatex

// A input landmark file contains a landmark entry in the format:
//
// x y       
// dx1 dy1 w1 
// dx2 dy2 w2
// dx3 dy3 w3
//
// The first row is the location. The 2nd to the 4th each contains a
// vessel direction with width. 
//


#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view.h>

#include <rrel/rrel_muset_obj.h>
#include <rrel/rrel_tukey_obj.h>

// BeginLatex
//
// The following header files provide declarations for classes needed
// to perform view-based
// registration. \code{rgrl\_view\_based\_registration} is the
// registration engine which incorporates view generation.
//
// EndLatex

// BeginCodeSnippet
#include <rgrl/rgrl_view_based_registration.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_view_gen_dbicp.h>
// EndCodeSnippet

#include <rgrl/rgrl_feature_set_location_masked.h>
#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_trans_quadratic.h>
#include <rgrl/rgrl_est_quadratic.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <rgrl/rgrl_est_similarity2d.h>
#include <rgrl/rgrl_trans_reduced_quad.h>
#include <rgrl/rgrl_est_reduced_quad2d.h>
#include <rgrl/rgrl_initializer_inv_indexing.h>
#include <rgrl/rgrl_invariant_single_landmark.h>

#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_scale_est_all_weights.h>
#include <rgrl/rgrl_scale_est_closest.h>
#include <rgrl/rgrl_weighter_m_est.h>
#include <rgrl/rgrl_convergence_on_weighted_error.h>

#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_data_manager.h>

#include "test_util.h"

typedef vcl_vector< rgrl_feature_sptr >  feature_vector;
typedef vnl_vector_fixed<double,2>       vector_2d;
typedef vcl_vector< rgrl_invariant_sptr> landmark_invaraint_vector;

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
    const rgrl_view_based_registration* reg_engine =
      dynamic_cast<const rgrl_view_based_registration*>(caller);
    rgrl_transformation_sptr trans = reg_engine->current_view()->xform_estimate();

    if ( trans->is_type( rgrl_trans_similarity::type_id() ) ) {
      rgrl_trans_similarity* sim_xform = rgrl_cast<rgrl_trans_similarity*>(trans);
      vcl_cout<<"Initial xform: A = \n"<<sim_xform->A()<<"t = "<<sim_xform->t()<<vcl_endl;}
    else if ( trans->is_type( rgrl_trans_reduced_quad::type_id() ) ) {
      rgrl_trans_reduced_quad* rq_xform = rgrl_cast<rgrl_trans_reduced_quad*>(trans);
      vcl_cout<<"Initial xform: Q = \n"<<rq_xform->Q()<<"A = "<<rq_xform->A()<<
        "t = "<<rq_xform->t()<<vcl_endl;
    }
    else if ( trans->is_type( rgrl_trans_quadratic::type_id() ) ) {
      rgrl_trans_quadratic* q_xform = rgrl_cast<rgrl_trans_quadratic*>(trans);
      vcl_cout<<"Initial xform: Q = \n"<<q_xform->Q()<<"A = "<<q_xform->A()<<
        "t = "<<q_xform->t()<<vcl_endl;
    }
    else vcl_cout<<"Unknown type"<<vcl_endl; 
  }
};

void 
read_feature_file( const char* filename,
                   feature_vector& trace_points )
{
  vcl_ifstream istr( filename );

  if( !istr ) {
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

void
read_landmark_file( const char* filename,
                    landmark_invaraint_vector&  landmark_inv )
{
  vcl_ifstream istr( filename );

  if( !istr ) {
    vcl_cerr<<"ERROR: Cannot open "<<filename<<vcl_endl;
    return;
  }

  double angular_std = 5.5*vnl_math::pi/180;
  double width_ratio_std = 4.5*vnl_math::pi/180;

  vector_2d location;
  vector_2d direction1;
  vector_2d direction2;
  vector_2d direction3;
  double width1, width2, width3;
  
  bool done = false;
  while ( !done && istr ) {
    if ( !(istr >> location[0] >> location[1] ) )
      done = true;
    else {
      // read in the directions and widths
      istr >> direction1[0] >> direction1[1]>>width1;
      istr >> direction2[0] >> direction2[1]>>width2;
      istr >> direction3[0] >> direction3[1]>>width3;
      // Make sure all widths are at least 1
      width1 = vnl_math_max( 1.0, width1);
      width2 = vnl_math_max( 1.0, width2);
      width3 = vnl_math_max( 1.0, width3);

      rgrl_invariant_single_landmark* single =  
        new rgrl_invariant_single_landmark( location, direction1, 
                                            direction2, direction3,
                                            width1, width2, width3,
                                            angular_std, width_ratio_std );
      landmark_inv.push_back( single );

      // If the single constellation was ambiguous create a copy with
      // the indices shifted to releive the ambiguity
      if ( single->is_ambiguous() ) {
        rgrl_invariant_single_landmark* copy = 
          new rgrl_invariant_single_landmark( *single, angular_std, width_ratio_std );
        landmark_inv.push_back( copy );
      } 
    }
  }
}            
      
int 
main( int argc, char* argv[] )
{
  if( argc < 5 ) {
    vcl_cerr << "Missing Parameters " << vcl_endl;
    vcl_cerr << "Usage: " << argv[0];
    vcl_cerr << " FixedImageTraceFile FixedImageLandmarkFile MovingImageTraceFile MovingImageLandmarkFile MaskImage";
    return 1;
  }

  prepare_testing();

  // Prepare feature sets
  //
  feature_vector moving_set;
  feature_vector fixed_set;
  landmark_invaraint_vector moving_landmark_set;
  landmark_invaraint_vector fixed_landmark_set;

  const char* fixed_trace_file_name = argv[1];
  const char* fixed_landmark_file_name = argv[2];
  const char* moving_trace_file_name = argv[3];
  const char* moving_landmark_file_name = argv[4];

  read_feature_file( moving_trace_file_name, moving_set );
  read_feature_file( fixed_trace_file_name, fixed_set );
  read_landmark_file( moving_landmark_file_name, moving_landmark_set );
  read_landmark_file( fixed_landmark_file_name, fixed_landmark_set );


  const unsigned int dimension = 2;
  rgrl_feature_set_sptr moving_feature_set;
  rgrl_feature_set_sptr fixed_feature_set;

  const char* make_file_name = argv[5];
  vil_image_view<vxl_byte> mask_image;
  mask_image = vil_load(make_file_name);
  rgrl_mask_sptr mask = new rgrl_mask_2d_image( mask_image );

  moving_feature_set = 
    new rgrl_feature_set_location_masked<dimension>(moving_set,
                                                    mask);
  fixed_feature_set = 
    new rgrl_feature_set_location_masked<dimension>(fixed_set,
                                                    mask);
  rgrl_mask_box moving_image_region = moving_feature_set->bounding_box();
  rgrl_mask_box fixed_image_region  = fixed_feature_set->bounding_box(); 

  // Create the initializer
  //
  rgrl_initializer_inv_indexing* inv_initializer = 
    new rgrl_initializer_inv_indexing( moving_image_region,
                                       fixed_image_region );
  double angular_std = 5.5*vnl_math::pi/180;
  double nn_radius = angular_std * vcl_sqrt(11.0704);//95% chi-sqr uncertainty bound 
  inv_initializer->add_data( fixed_landmark_set,
                             moving_landmark_set,
                             nn_radius );
  rgrl_initializer_sptr initializer = inv_initializer;

  // Create the view generator, which is dbicp
  //
  // BeginLatex
  // 
  // The view generator, which handles region growing and model
  // selection, is \code{rgrl\_view\_gen\_dbicp}. The construction of
  // the generator takes a few parameters. \code{max\_growth\_rate}
  // and \code{min\_growth\_rate} are the upper and lower bounds on
  // the growth rate of the registration region, respectively. A
  // growth below the \code{min\_growth\_rate} is treated as no
  // growth. \code{global\_region\_update\_per\_model} limits the
  // number of updates on global region for each estimation model. A
  // global region is the estimated region of overlap between the two
  // images, and the region is computed using the current
  // transformation.
  //
  // EndLatex

  // BeginCodeSnippet
  double max_growth_rate = 2;
  double min_growth_rate = 1.04;
  int global_region_update_per_model = 1;
  rgrl_view_generator_sptr view_gen = 
    new rgrl_view_gen_dbicp( max_growth_rate, min_growth_rate, 
                             global_region_update_per_model);
  // EndCodeSnippet

  // Add the data and model hierarchy needed by the view generator.
  //
  // BeginLatex
  //
  // We construct the single-stage data manager with default robust
  // components. Different from previous examples, we provide 3
  // estimators to the data manager, going from low to high order (and
  // this ordering is crucial).
  //
  // EndLatex

  // BeginCodeSnippet
  rgrl_data_manager_sptr data = new rgrl_data_manager();
  data->add_data( moving_feature_set,  // from data
                  fixed_feature_set ); // to data

  data->add_estimator(new rgrl_est_similarity2d(2) );
  data->add_estimator(new rgrl_est_reduced_quad2d(2) );
  data->add_estimator(new rgrl_est_quadratic(2) );

  rgrl_view_based_registration reg( data, view_gen );
  // EndCodeSnippet

  // Fine-tune some parameters
  //
  reg.set_max_icp_iter(5);
  reg.set_expected_max_geometric_scale(30);
  reg.set_expected_min_geometric_scale(0.5);

  // For debugging
  //
  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());

  reg.run( initializer );

  // Output Results
  //
  if ( reg.has_best_view() ) {
    vcl_cout<<"Final xform: "<<vcl_endl;
    rgrl_transformation_sptr trans = reg.final_transformation();
    rgrl_trans_quadratic* q_xform = rgrl_cast<rgrl_trans_quadratic*>(trans);
    vcl_cout<<"Q =\n"<<q_xform->Q()<<"A = "<<q_xform->A()<<"t ="<<q_xform->t()<<vcl_endl;
    vcl_cout<<"Final alignment error = "<<reg.final_status()->error()<<vcl_endl;
  }

  // BeginLatex
  //
  // Let's execute this example with the same set of features as for
  // section~\ref{sec:init:inv}. When mapping IMG0002 to IMG0004, the
  // registration process succeeds with the first initial estimate,
  // which previously failed withtout DBICP view generation. The final
  // transformation is
  //
  // \begin{verbatim}
  // Final xform: 
  // Q =
  // 0.000101371 0.000100919 8.62385e-06 
  // 1.84413e-05 2.11297e-05 -9.99652e-07 
  // A = 0.859442 -0.0946577 
  // -0.0490676 0.972498 
  // t =-209.967 -8.072
  // Final alignment error = 0.582472
  // \end{verbatim}
  //
  // EndLatex

  // Perform testing
  //
  test_macro( "Registration using DBICP on retinal images" , 
              reg.final_status()->error(), 1 );

  return 0;
}
