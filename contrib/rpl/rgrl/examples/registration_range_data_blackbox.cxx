// BeginLatex
//
// The goal of this example is to demonstrate registration of 3D range
// data using \code{rgrl\_feature\_face\_pt} as the feature
// type. Features are points located on 3D surfaces. Each point is
// associated with a direction normal to the surface at the given
// point. If normal directions are not provided, they can be easily
// estimated at each point location by gathering $m$ closest points
// and fitting a plane. Implementation of triangularization algorithms
// is part of the future work.
//
//  EndLatex

// This program reads in the data file of the format
//      total_number_of_features
//      x y z nx ny nz
//      x y z nx ny nz
//      ...
//
// If the feature file only contains (x, y, z), run
// estimate_normals_3d first to generate the normals.
//

#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector_fixed.h>

// BeginLatex
//
// The set of header files included is very similar to the previous
// example. Since we're interested in point-to-surface error, the only
// difference is the feature type --- \code{rgrl\_feature\_face\_pt},
// instead of \code{rgrl\_feature\_trace\_pt}.
//
// EndLatex

// BeginCodeSnippet
#include <rgrl/rgrl_feature_face_pt.h>
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

typedef vcl_vector< rgrl_feature_sptr >  feature_vector;
typedef vnl_vector_fixed<double,3>       vector_3d;

// BeginLatex
//
// In this example, feature locations and associated normals are
// imported from a file. The construction of
// \code{rgrl\_feature\_face\_pt} takes a location and a normal (both
// are 3D vectors).
//
// EndLatex
void
read_feature_file( const char* filename,
                   feature_vector& feature_points,
                   int sample_spacing )
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
    // BeginCodeSnippet
    rgrl_feature_sptr feature_pt = new rgrl_feature_face_pt(location, normal);
    feature_points.push_back( feature_pt );
    // EndCodeSnippet
  }

  istr.close();

  vcl_cout<<"There are "<<feature_points.size()<<" features"<<vcl_endl;
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
    rgrl_trans_affine* a_xform = rgrl_cast<rgrl_trans_affine*>(trans);
    vcl_cout<<"xform: A = "<<a_xform->A()<<"t = "<<a_xform->t()<<vcl_endl;
  }
};

int
main( int argc, char* argv[] )
{
  if ( argc < 2 ) {
    vcl_cerr << "Missing Parameters" << vcl_endl
             << "Usage: " << argv[0]
             << " ImageFeatureFile\n";
    return 1;
  }

  prepare_testing();

  // Read in the features from the external files.
  //
  feature_vector moving_feature_points;
  feature_vector fixed_feature_points;
  const char* file_name = argv[1];
  read_feature_file( file_name, moving_feature_points, 50);
  read_feature_file( file_name, fixed_feature_points, 1 );

  // Set up the feature sets using kd-tree as the underlying data
  // structure.
  //
  // BeginLatex
  //
  // The default underlying data structure for
  // \code{rgrl\_feature\_set\_location} is N-D bins, which, on
  // average, is quite fast for sparse and scattered data, but
  // inadequate for clustered data (unless the bin size is adjusted
  // accordingly). In the case of range data, points are normalized to
  // the range of $[-1,1]$. To better handle the clustered data,
  // kd-tree should be the choice of data structure for faster
  // performance. During the construction of
  // \code{rgrl\_feature\_set\_location}, the kd-tree is selected if
  // the 2nd input parameter is set to false (\code{!use\_bins}).
  //
  // EndLatex

  // BeginCodeSnippet
  const unsigned int dimension = 3;
  bool use_bins = true;
  rgrl_feature_set_sptr moving_feature_set =
    new rgrl_feature_set_location<dimension>(moving_feature_points, !use_bins);
  rgrl_feature_set_sptr fixed_feature_set =
    new rgrl_feature_set_location<dimension>(fixed_feature_points, !use_bins);
  rgrl_mask_box image_roi = moving_feature_set->bounding_box();
  // EndCodeSnippet


  // Set up the initial transformation and the estimator for affine
  // transformation
  //

  // The initial transform is rotation = [x_angle, y_angle, z_angle],
  // t=[10, 0, 15]
  //
  vnl_matrix<double> A(3,3,vnl_matrix_identity);

  //y_dir
  vnl_matrix<double> Ay(3,3,vnl_matrix_identity);
  double y_angle = 5*vnl_math::pi/180;
  Ay(0,0) = vcl_cos(y_angle); Ay(0,2) = vcl_sin(y_angle);
  Ay(2,0) = -vcl_sin(y_angle); Ay(2,2) = vcl_cos(y_angle);

  //x_dir
  vnl_matrix<double> Ax(3,3,vnl_matrix_identity);
  double x_angle = 0*vnl_math::pi/180;
  Ax(1,1) = vcl_cos(x_angle); Ax(1,2) = -vcl_sin(x_angle);
  Ax(2,1) = vcl_sin(x_angle); Ax(2,2) = vcl_cos(x_angle);

  //z_dir
  vnl_matrix<double> Az(3,3,vnl_matrix_identity);
  double z_angle = 10*vnl_math::pi/180;
  Az(0,0) = vcl_cos(z_angle); Az(0,1) = -vcl_sin(z_angle);
  Az(1,0) = vcl_sin(z_angle); Az(1,1) =  vcl_cos(z_angle);

  A = Ax*Ay*Az;
#if 0 // commented out
  A(0,0) = 0.98106; A(0,1) =  -0.172987; A(0,2)= 0.0871557;
  A(1,0) = 0.173648;  A(1,1) = 0.984808;  A(1,2)= 0;
  A(2,0) = -0.0858317; A(2,1) = 0.0151344; A(2,2)= 0.996195;
#endif // 0
  vector_3d t(10, 0, 15);
  t *= 1/1000;

  rgrl_transformation_sptr init_transform = new rgrl_trans_affine(A, t);
  rgrl_estimator_sptr estimator = new rgrl_est_affine();

  // Store the data in the data manager. Other components in the black
  // box of registration are set to the common default techniques for
  // robustness.
  //
  rgrl_data_manager_sptr data = new rgrl_data_manager();
  data->add_data( moving_feature_set,   // data from moving image
                  fixed_feature_set);   // data from fixed image

  rgrl_feature_based_registration reg( data );
  reg.set_expected_min_geometric_scale( 0.01/1000 );

  // To monitor registration
  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());

  // Run ...
  //
  reg.run( image_roi, estimator, init_transform  );

  // Output Results
  if ( reg.has_final_transformation() ) {
    vcl_cout<<"Final xform: "<<vcl_endl;
    rgrl_transformation_sptr final_trans = reg.final_transformation();
    rgrl_trans_affine* a_xform = rgrl_cast<rgrl_trans_affine*>(final_trans);
    vcl_cout<<"Final xform: A =\n"<<a_xform->A()<<"t = "<<a_xform->t()<<vcl_endl
            <<"Final alignment error = "<<reg.final_status()->error()<<vcl_endl;
  }

  // BeginLatex
  //
  // The rest of the program is extremely similar to
  // \verb+registration_blackbox.cxx+. To execute the example, one may
  // try the feature file
  // \verb+rgrl/examples/dragonStandRight_0_normal.txt+. The original
  // data is from
  // http://graphics.stanford.edu/data/3Dscanrep/. Normals were
  // estimated separately and stored in
  // \verb+dragonStandRight_0_normal.txt+ with the point
  // locations. The result of the execution is a transformation very
  // close to identity.
  //
  //EndLatex

  // Perform testing
  //
  test_macro( "Registration of range data", reg.final_status()->error(), 1.0e-004 );
}
