// BeginLatex
//
// This example is the "Hello World" example for correspondence-based
// registration. The goal of the example is to show steps needed to
// set up the registration process. For now, we treat the registration
// engine as a black box (which is not recommended for any but the
// simplest problems). Please note, most components in the
// registration library are not templated over the dimension of the
// image, since the dimension can be easily derived from the feature
// set.
//
// The problem that we're interested in now is to register two images
// with some simple shapes. One image is in red and one in green. Both
// images have Gaussian noise added, and the green image contains some
// random points which are not present in the red image. Good
// registration should overlay the green on the red image with
// identity transformation\footnote{Different from ITK, the
// transformation maps the moving image to the fixed image.}. To make
// the problem more interesting, we start with an initial
// transformation involving slight rotation, shearing and translation
// (affine transformation), shown in
// figure~\ref{fig:simple_shapes}(a). The final alignment with a close
// to identity transformation is in figure~\ref{fig:simple_shapes}(b).
//
// \begin{figure}[tbp]
// \begin{center}
// \begin{tabular}{cc}
// \includegraphics[width=2.5in]{simple_shapes_init} &
// \includegraphics[width=2.5in]{simple_shapes_final} \\ (a) & (b)
// \end{tabular}
// \end{center}
// \caption{Registration of two simple images. The goal is to register
// the green to the red image. (a) is the initial alignment, and (b) is
// the result of correct registration.}
// \label{fig:simple_shapes}
// \end{figure}
//
// The following header files provide declarations for the basic types
// needed to perform the registration.
//
// EndLatex
//

#include <vcl_iostream.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>

// BeginCodeSnippet
#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_data_manager.h>
#include <rgrl/rgrl_feature_based_registration.h>
// EndCodeSnippet

#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_converge_status.h>

#include "test_util.h"

// vnl_vector_fixed is a fixed-length, stack storage
// vector. vnl_vector_fixed defines most of the operators defined by
// vnl_vector, and does so efficiently.
typedef vnl_vector_fixed<double,2>              vector_2d;
typedef vcl_vector< rgrl_feature_sptr >         feature_vector;

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
    rgrl_trans_affine* xform = rgrl_cast<rgrl_trans_affine*>(trans);
    vcl_cout<<"Xform A = "<<xform->A()<<"\n t= "<<xform->t()<<vcl_endl;
  }
};

// BeginLatex
//
// We start with features. A \emph{feature} is the basic element for
// correspondence-based registration.  It can be as simple as a point
// in space with a location, or as complex as a region with intensity
// information and orientation information encapsulated.  Features can
// be constructed before matching is started, or in the case of
// matching intensities (block matching), constructed as part of the
// matching process.
//
// Different feature types should define different alignment error
// distance measures (see evaluation in~\cite{rusinkiewicz:3ddim01}).
// For example, the alignment error of landmarks matched to landmarks
// is best described as a Euclidean distance, since landmarks are
// fairly stable 3D point locations.  For ``non-distinctive'' points
// on curves and surfaces, however, this is too restrictive a measure
// because the features are often poorly localized in the direction
// along the curve or on the surface.  Thus, the error in the
// direction normal to the curve or surface is the most appropriate
// error distance measure. This is achieved most generally by
// pre-multiplying the standard Euclidean error vector by an
// \emph{error projector matrix}, and then computing the magnitude of
// the projected vector. Error projectors of different types are
// summarized in Table~\ref{tb:error_projector}.
//
// \begin{table}[t]
// \begin{center}
// \begin{tabular}{|l||c|c|c|} \hline
// \textbf{Error Type} & point-to-point & point-to-curve&
// point-to-surface \\ \hline
// \textbf{Error Projector} &
// $\projector = \matx{I}$ &
// $\projector = \matx{I}-\tangentvect \tangentvect^T$ &
// $\projector = \normalvect \normalvect^T$\\ \hline
// \textbf{DoF to Constrain} & m & m-1& 1\\  \hline
// \end{tabular}
// \end{center}
// \caption{Error projector matrix for different types of errors. For
//   a point on a curve, $\tangentvect$ is the tangent to the curve, and
//   for point on a surface, $\normalvect$ is the normal to the
//   surface. $m$ is the number of dimensions of the image.}
// \label{tb:error_projector}
// \end{table}
//
// In this example, we prefer point-to-curve as our error measure,
// since sampled points are less distinctive along the lines and the
// curve. The feature type is \code{rgrl\_feature\_trace\_pt} and the
// code below illustrates the construction of such a feature.
//
// EndLatex

void
generate_data( feature_vector& feature_points )
{
  // Build a rectangle of size 200x100 with (50,50) at the top-left corner.
  // Gaussian noise of sigma=0.1 is added to the point set.
  //
  vnl_random random;
  double sigma  = 0.2;
  double org_x = 50;
  double org_y = 50;

  // The edge of y = org_y
  //
  for (unsigned int xi = 0; xi<200; xi+=2 ) {
    // BeginCodeSnippet
    vector_2d pt, tangent_dir;
    pt[0] = org_x + xi + random.normal()*sigma;
    pt[1] = org_y + random.normal()*sigma;
    tangent_dir[0] = 1;
    tangent_dir[1] = 0;
    rgrl_feature_sptr feature_pt = new rgrl_feature_trace_pt(pt, tangent_dir);
    feature_points.push_back( feature_pt );
    // EndCodeSnippet
  }

  // The edge of y = org_y + 99
  //
  for (unsigned int xi = 0; xi<200; xi+=2 ) {
    vector_2d pt, tangent_dir(-1,0);
    pt[0] = org_x + xi + random.normal()*sigma;
    pt[1] = org_y + 99 + random.normal()*sigma;
    feature_points.push_back( new rgrl_feature_trace_pt(pt, tangent_dir) );
  }

  // The edge of x = org_x
  //
  for (unsigned int yi = 0; yi<100; yi+=2 ) {
    vector_2d pt, tangent_dir(0,1);
    pt[0] = org_x + random.normal()*sigma;
    pt[1] = org_y + yi + random.normal()*sigma;
    feature_points.push_back( new rgrl_feature_trace_pt(pt, tangent_dir) );
  }

  // The edge of x = org_x+199
  //
  for (unsigned int yi = 0; yi<100; yi+=2 ) {
    vector_2d pt,tangent_dir(0,-1);
    pt[0] = org_x + 199 + random.normal()*sigma;
    pt[1] = org_y + yi + random.normal()*sigma;
    feature_points.push_back( new rgrl_feature_trace_pt(pt, tangent_dir) );
  }

  // Draw the circle, centered at (115, 115), with radius 50
  //
  double radius = 50;
  double center_x = 115;
  double center_y = 115;
  for ( unsigned int ci = 0; ci<360; ci++ ) {
    vector_2d pt,tangent_dir;
    double angle = ci*2*vnl_math::pi/180;
    double next_angle = (ci+1)*2*vnl_math::pi/180;
    pt[0] = center_x + radius*vcl_cos(angle);
    pt[1] = center_y + radius*vcl_sin(angle);
    tangent_dir[0] = vcl_cos(next_angle) - vcl_cos(angle) ;
    tangent_dir[1] = vcl_sin(next_angle) - vcl_sin(angle) ;
    tangent_dir.normalize(); //make the tangent a unit vector
    feature_points.push_back( new rgrl_feature_trace_pt(pt, tangent_dir) );
  }
}

// BeginLatex
//
// Since we're dealing with 2D images, both \code{pt} for the location
// of the point and \code{tangent\_dir} for the direction of the
// tangent are both 2D vectors. Let's not to worry how \code{pt} and
// \code{tangent\_dir} are generated, but concentrate on construction
// of the feature. \code{rgrl\_feature} is the abstract base class of
// various feature types, and it provides the common interface for
// interaction with other components of the registration
// library. \code{rgrl\_feature\_sptr} defines a smart pointer of type
// \code{rgrl\_feature} (\code{\_sptr} stands for smart pointer).
// \code{rgrl\_feature\_sptr} can also store a pointer to a derived
// class. Here we create an object of type
// \code{rgrl\_feature\_trace\_pt} with the point location and the
// approximated tangent direction. A pointer of such an object is
// assigned to \code{rgrl\_feature\_sptr}, which is stored in a
// vector, \code{feature\_points}.
//
// EndLatex

void
add_outliers( feature_vector& feature_points )
{
  vnl_random random;

  unsigned int num_outliers = 200;
  for (unsigned int i = 0; i<num_outliers; i++) {
    vnl_vector<double> v(2), n(2);
    v[0] = random.drand32(0, 300);
    v[1] = random.drand32(0, 300);
    n = v;
    feature_points.push_back( new rgrl_feature_trace_pt(v, n.normalize()) );
  }
}

int
main( int argc, char* argv[] )
{
  prepare_testing();

  // Generate the feature points
  //
  feature_vector  moving_feature_points;
  feature_vector  fixed_feature_points;

  generate_data( moving_feature_points);
  generate_data( fixed_feature_points );

  // Add random data points to create outliers
  //
  add_outliers(moving_feature_points);

  // Set up the feature sets
  //
  // BeginLatex
  //
  // All features of the same type from one image are stored in a
  // \emph{feature set}.  Fundamentally, feature sets answer queries
  // in support of correspondence generation.  Examples include
  // finding the nearest feature in the fixed image to a given feature
  // (mapped from the moving image), finding the k-nearest neighbors,
  // or even finding all features in a given region when multiple
  // correspondences are required for a feature (see for
  // example~\cite{granger:eccv02}).  Spatial data structures such as
  // k-d trees and binning support fast querying. In this example, the
  // type of feature set is \code{rgrl\_feature\_set\_location}, which
  // answers the queries based on the feature location. The
  // construction of a feature set \code{rgrl\_feature\_set\_location}
  // takes a vector of smart pointers to features, which we previously
  // constructed.
  //
  // EndLatex

  // BeginCodeSnippet
  const unsigned int  dimension = 2;
  rgrl_feature_set_sptr moving_feature_set =
    new rgrl_feature_set_location<dimension>(moving_feature_points);
  rgrl_feature_set_sptr fixed_feature_set =
    new rgrl_feature_set_location<dimension>(fixed_feature_points);
  rgrl_mask_box image_roi = moving_feature_set->bounding_box();
  // EndCodeSnippet

  // Set up the initial transformation and the estimator for affine
  // transformation
  //

  // BeginLatex
  //
  // Next, we initialize the registration process with a prior affine
  // transformation, and define the affine estimator. More detailed
  // discussion on other common initialization schemes will be
  // discussed in section~\ref{sec:init}.
  //
  // EndLatex

  // BeginCodeSnippet
  vnl_matrix<double> A(2,2);
  A(0,0) = 0.996;   A(0,1) = -0.087;
  A(1,0) = -0.087;  A(1,1) = 0.996;
  vector_2d t( 10, -13 );
  rgrl_transformation_sptr init_transform = new rgrl_trans_affine(A, t);
  rgrl_estimator_sptr      estimator = new rgrl_est_affine();
  // EndCodeSnippet

  // Store the data in the data manager. Other components in the black
  // box of registration are set to the common default techniques for
  // robustness.
  //
  // BeginLatex
  //
  // All the components required for registration are stored in a data
  // structure, \code{rgrl\_data\_manager}. This is useful for
  // illustration purposes, but not for many real problems. To make it
  // more useful, the data manager also supports robust estimation,
  // multi-resolution and multi-model registration (topics to be
  // discussed later). In the simplest form, the data manager takes
  // only the two feature sets. The data manager is passed to the
  // registration engine, \code{rgrl\_feature\_based\_registration}
  // during construction.
  //
  // EndLatex

  // BeginCodeSnippet
  rgrl_data_manager_sptr data = new rgrl_data_manager();
  data->add_data( moving_feature_set, fixed_feature_set );
  rgrl_feature_based_registration reg( data );
  // EndCodeSnippet

  reg.set_expected_min_geometric_scale( 0.1 );

  // To monitor registration
  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());

  // Run ...
  //

  // BeginLatex
  //
  // Now, we're ready to perform the registration. With the same sets
  // of features, we may obtain different results depending on the
  // region of interest (\code{image\_roi}), transformation estimator
  // ({\code{estimator}) and initial transformation estimate
  // (\code{init\_transform}). Please note, the initial estimate is
  // only for generation of the first set of matches. Therefore, it
  // can belong to a model different from that of the transformation
  // estimator.
  //
  // EndLatex

  // BeginCodeSnippet
  reg.run( image_roi, estimator, init_transform );
  // EndCodeSnippet

  if ( reg.has_final_transformation() ) {
    vcl_cout<<"Final xform: "<<vcl_endl;
    rgrl_transformation_sptr trans = reg.final_transformation();
    rgrl_trans_affine* a_xform = rgrl_cast<rgrl_trans_affine*>(trans);
    vcl_cout<<"A = "<<a_xform->A()<<vcl_endl
            <<"t = "<<a_xform->t()<<vcl_endl
            <<"Final alignment error = "<<reg.final_status()->error()<<vcl_endl;
  }

  // Perform testing
  //
  test_macro( "black-box Registration of simple shapes" ,
              reg.final_status()->error(), 1 );
}


