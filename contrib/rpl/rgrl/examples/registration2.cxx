// BeginLatex
//
// It is a good practice to monitor the progress of the registration
// process. There are two ways of doing it. The first is the use of
// command/observer mechanism\cite{gamma:book95}. The second is the
// setting of the debug flag, which is a technique used by the
// developers of RRL library. We'll only discuss the first approach in
// this section.
//
// The command/observer design pattern depicts the interaction among 3
// classes: \code{rgrl\_object}, \code{rgrl\_command}, and
// \code{rgrl\_event}. \code{rgrl\_object} is the base class of most
// objects in RRL. This class maintains a list of event
// observers. Each event observer bundles a \code{rgrl\_event} and
// \code{rgrl\_command}, and is created via the call to
// \code{rgrl\_object::add\_observer()}. By doing so, the command
// registers itself with the object, declaring that it is interested
// in receiving notification when a particular event happens. Events
// defined in RRL are \code{rgrl\_event\_start},
// \code{rgrl\_event\_end}, \code{rgrl\_event\_iteration}.
//
// \begin{figure}[tbp]
//  \center{\includegraphics[width=5in]{command_observer}}
// \caption{Interaction between the registration engine and the command
// object. The command is added to the engine via a call to
// add\_observer() before the registration process begins, and is
// associated to the iteration event. In each iteration, the
// execute() function of the command object is triggered.}
// \label{observer}
// \end{figure}
//
// Inside the registration engine, the iteration event
// (\code{rgrl\_event\_iteration}) is invoked at the end of each
// iteration. When an event is invoked by an object, this object goes
// through its list of observers and checks whether any one of them
// has expressed interest in the current event type. Whenever such an
// observer is found, its corresponding \code{execute()} method is
// invoked. In this context, \code{execute()} methods should be
// considered callbacks, and are assumed to execute rapidly, for
// example, printing out a transformation. The following code
// illustrates a simple way of creating a command to monitor a
// registration process. This new class, called
// \code{command\_iteration\_update}, derives from
// \code{rgrl\_command} and provides a specific implementation of the
// \code{execute()} methods.
//
// EndLatex

#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector_fixed.h>

#include <rrel/rrel_muset_obj.h>
#include <rrel/rrel_tukey_obj.h>

#include <rgrl/rgrl_feature_based_registration.h>
#include <rgrl/rgrl_feature_set_location_masked.h>
#include <rgrl/rgrl_initializer_prior.h>
#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_trans_quadratic.h>
#include <rgrl/rgrl_est_quadratic.h>

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

void
read_feature_file( const char* filename,
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
// BeginCodeSnippet
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

    if ( !reg_engine ) return;

    rgrl_transformation_sptr trans = reg_engine->current_transformation();

    if ( trans->is_type( rgrl_trans_quadratic::type_id() ) ) {
      rgrl_trans_quadratic* q_xform = rgrl_cast<rgrl_trans_quadratic*>(trans);
      vcl_cout<<"xform: Q\n"<<q_xform->Q()<<"A = "<<q_xform->A()
              <<"t = "<<q_xform->t()<<vcl_endl;
    }
    else
      vcl_cout<<"ERROR: Incorrect type!"<<vcl_endl;
  }
};
// EndCodeSnippet

int
main( int argc, char* argv[] )
{
  if ( argc < 3 ) {
    vcl_cerr << "Missing Parameters " << vcl_endl
             << "Usage: " << argv[0]
             << " FixedImageFeatureFile MovingImageFeatureFile\n";
    return 1;
  }

  prepare_testing();

  // Preparing the feature sets
  //
  feature_vector moving_feature_points;
  feature_vector fixed_feature_points;
  const char* fixed_file_name = argv[1];
  const char* moving_file_name = argv[2];

  read_feature_file( moving_file_name, moving_feature_points );
  read_feature_file( fixed_file_name, fixed_feature_points );

  const unsigned int dimension = 2;
  rgrl_feature_set_sptr moving_feature_set;
  rgrl_feature_set_sptr fixed_feature_set;
  moving_feature_set =
    new rgrl_feature_set_location<dimension>(moving_feature_points);
  fixed_feature_set =
    new rgrl_feature_set_location<dimension>(fixed_feature_points);
  rgrl_mask_box moving_image_region = moving_feature_set->bounding_box();

  // Transformation estimator
  //
  rgrl_estimator_sptr estimator = new rgrl_est_quadratic();

  //set the initial transformation to identity
  //
  rgrl_transformation_sptr init_trans = new rgrl_trans_quadratic(dimension);


  // Set up the data manager
  //
  rgrl_data_manager_sptr data = new rgrl_data_manager();
  data->add_data( moving_feature_set,   // data from moving image
                  fixed_feature_set );  // data from fixed image

  // Set up the registration engine
  //
  rgrl_feature_based_registration reg( data );

  // Monitoring the registration process
  //
  // BeginLatex
  //
  // Before the registration process begins, the
  // \code{command\_iteration\_update} is added to the observer list
  // of the engine and specifies its interest in catching
  // \code{rgrl\_event\_iteration}.
  //
  // EndLatex

  // BeginCodeSnippet
  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());
  // EndCodeSnippet

  reg.run( moving_image_region, estimator, init_trans  );

  // Output Results
  if ( reg.has_final_transformation() ) {
    vcl_cout<<"Final xform: "<<vcl_endl;
    rgrl_transformation_sptr trans = reg.final_transformation();
    rgrl_trans_quadratic* q_xform = rgrl_cast<rgrl_trans_quadratic*>(trans);
    vcl_cout<<"Q\n"<<q_xform->Q()<<"A = "<<q_xform->A()
            <<"t = "<<q_xform->t()<<vcl_endl
            <<"Final alignment error = "<<reg.final_status()->error()<<vcl_endl;
  }

  // BeginLatex
  //
  // One may try running this example with the feature files
  // \verb+rgrl/examples/IMG0002.pgm.txt+ and
  // \verb+rgrl/examples/IMG0003.pgm.txt+.
  //
  // EndLatex

  // Perform testing
  //
  test_macro( "Registration with robustness", reg.final_status()->error(), 1 );
}
