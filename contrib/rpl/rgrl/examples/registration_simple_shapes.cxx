// BeginLatex
//
// This simple example of registration of images with 2 simple shapes
// illustrate the main component of non-robust feature-based
// registration.
//
// EndLatex

#include <vcl_sstream.h>
#include <vcl_fstream.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>

#include <rgrl/rgrl_feature_based_registration.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_convergence_on_median_error.h>

#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_data_manager.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_event.h>
#include <rgrl/rgrl_command.h>

#include <vnl/vnl_random.h>

#include "test_util.h"

typedef vcl_vector< rgrl_feature_sptr >         feature_vector;
typedef vnl_vector_fixed<double,2>              vector_2d;

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

#if 0 // commented out
    static unsigned count = 0;
    ++count;

    // Output the transformation estimate
    //
    vcl_ostringstream s;
    s << "xform-dump-"<<count;
    vcl_ofstream xform_out( s.str().c_str() );

    xform_out<<"Xform A = "<<xform->A()<<"\n t= "<<xform->t()<<vcl_endl;
    xform_out.close();

    // Output the matches
    //
    typedef rgrl_match_set::from_iterator  from_iter;
    typedef from_iter::to_iterator         to_iter;

    vcl_ostringstream ss;
    ss << "matches-dump-"<<count;
    vcl_ofstream fout( ss.str().c_str() );

    for ( unsigned ms=0; ms < match_sets.size(); ++ms ) {
      rgrl_match_set_sptr match_set = match_sets[ms];
      //  for each from image feature being matched
      for ( from_iter fitr = match_set->from_begin();
            fitr != match_set->from_end(); ++fitr ){
        if ( fitr.size() == 0 )  continue;

        rgrl_feature_sptr from_feature = fitr.from_feature();
        fout<<from_feature->location()[0]<<' '<<from_feature->location()[1];
        rgrl_feature_sptr mapped_from = fitr.mapped_from_feature();
        fout<<' '<<mapped_from->location()[0]<<' '<<mapped_from->location()[1];
        for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
          //  for each match with a "to" image feature
          rgrl_feature_sptr to_feature = titr.to_feature();
          fout<<' '<<to_feature->location()[0]<<' '
              <<to_feature->location()[1]<<vcl_endl;
        }
      }
    }
    fout.close();
#endif // 0
  }
};

void
generate_data(feature_vector& feature_set)
{
  // Build a rectangle of size 200x100 with (50,50) at the top-left corner.
  // Gaussian noise of sigma=0.1 is added to the point set.
  //
  vnl_random random;
  double sigma = 0.1;
  double org_x = 50;
  double org_y = 50;

  // The edge of y = org_y
  //
  for (unsigned int xi = 0; xi<200; xi+=2 ) {
    vector_2d pt;
    pt[0] = org_x + xi + random.normal()*sigma;
    pt[1] = org_y + random.normal()*sigma;
    feature_set.push_back( new rgrl_feature_point(pt) );
  }

  // The edge of y = org_y + 99
  //
  for (unsigned int xi = 0; xi<200; xi+=2 ) {
    vector_2d pt;
    pt[0] = org_x + xi + random.normal()*sigma;
    pt[1] = org_y + 99 + random.normal()*sigma;
    feature_set.push_back( new rgrl_feature_point(pt) );
  }

  // The edge of x = org_x
  //
  for (unsigned int yi = 0; yi<100; yi+=2 ) {
    vector_2d pt;
    pt[0] = org_x + random.normal()*sigma;
    pt[1] = org_y + yi + random.normal()*sigma;
    feature_set.push_back( new rgrl_feature_point(pt) );
  }

  // The edge of x = org_x+199
  //
  for (unsigned int yi = 0; yi<100; yi+=2 ) {
    vector_2d pt;
    pt[0] = org_x + 199 + random.normal()*sigma;
    pt[1] = org_y + yi + random.normal()*sigma;
    feature_set.push_back( new rgrl_feature_point(pt) );
  }

  // Draw the circle, centered at (115, 115), with radius 50
  //
  double radius = 50;
  double center_x = 115;
  double center_y = 115;
  for ( unsigned int ci = 0; ci<360; ci++ ) {
    vector_2d pt;
    double angle = ci*2*vnl_math::pi/180;
    pt[0] = center_x + radius*vcl_cos(angle);
    pt[1] = center_y + radius*vcl_sin(angle);
    feature_set.push_back( new rgrl_feature_point(pt) );
  }
}

int
main()
{
  prepare_testing();

  // Set up the feature sets
  //
  feature_vector  moving_feature_points;
  feature_vector  fixed_feature_points;

  generate_data( moving_feature_points );
  fixed_feature_points = moving_feature_points;

  const unsigned int  dimension = 2;
  rgrl_feature_set_sptr moving_feature_set;
  rgrl_feature_set_sptr fixed_feature_set;
  moving_feature_set = new rgrl_feature_set_location<dimension>(moving_feature_points);
  fixed_feature_set =  new rgrl_feature_set_location<dimension>(fixed_feature_points);
  rgrl_mask_box image_roi = moving_feature_set->bounding_box();

  // Set up the ICP matcher
  //
  unsigned int k = 1;
  rgrl_matcher_sptr cp_matcher = new rgrl_matcher_k_nearest( k );

  // Set up the convergence tester
  //
  double tolerance = 1.5;
  rgrl_convergence_tester_sptr conv_test =
    new rgrl_convergence_on_median_error( tolerance );

  // Set up the estimator for affine transformation
  //
  rgrl_estimator_sptr estimator = new rgrl_est_affine();

  // Set up components for initialization
  //
  rgrl_transformation_sptr init_transform;
  vnl_matrix<double> A(2,2);
  A(0,0) = 0.98;  A(0,1) = -0.17;
  A(1,0) = -0.17;  A(1,1) =0.98;
  vector_2d t( 5, -3);
  init_transform = new rgrl_trans_affine(A, t);

  // Store the data in the data manager
  //
  rgrl_data_manager_sptr data = new rgrl_data_manager();
  data->add_data( moving_feature_set, // data from moving image
                  fixed_feature_set,  // data from fixed image
                  cp_matcher );       // matcher for this data

  rgrl_feature_based_registration reg( data, conv_test );

  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());

  // Run ...
  //
  reg.run( image_roi, estimator, init_transform );

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
  test_macro( "Registration of simple shapes" , reg.final_status()->error(), 1 );
}


