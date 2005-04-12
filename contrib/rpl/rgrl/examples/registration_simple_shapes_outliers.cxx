// BeginLatex
//
// It is never a good idea to treat the registration engine as a black
// box, and expect the default techniques to work robustly for images
// with noise. In this section, we'll look at components which were
// previously hidden in the registration engine.
//
// Back to the same registration problem as for
// section~\ref{sec:features:ex1}. The purpose of the example is to
// demonstrate customized robust registration. The focus of the
// discussion is on matching, robust transformation estimation, and
// convergence testing. We'll skip the part on construction of feature
// sets and initialization of the transformation (which were explained
// in section~\ref{sec:features:ex1}).
//
// EndLatex

#include <vcl_iostream.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>

#include <rrel/rrel_muset_obj.h>
#include <rrel/rrel_tukey_obj.h>

#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_data_manager.h>
#include <rgrl/rgrl_feature_based_registration.h>

#include <rgrl/rgrl_matcher_k_nearest.h>
#include <rgrl/rgrl_scale_est_all_weights.h>
#include <rgrl/rgrl_scale_est_closest.h>
#include <rgrl/rgrl_weighter_m_est.h>
#include <rgrl/rgrl_converge_status.h>
#include <rgrl/rgrl_convergence_on_weighted_error.h>

#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_cast.h>

#include <testlib/testlib_test.h>
void testlib_enter_stealth_mode(); // defined in core/testlib/testlib_main.cxx

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

  void execute(const rgrl_object* caller, const rgrl_event & /*event*/ )
  {
    const rgrl_feature_based_registration* reg_engine =
      dynamic_cast<const rgrl_feature_based_registration*>(caller);
    rgrl_transformation_sptr trans = reg_engine->current_transformation();
    rgrl_trans_affine* xform = rgrl_cast<rgrl_trans_affine*>(trans);
    vcl_cout<<"Xform A = "<<xform->A()<<"\n t= "<<xform->t()<<vcl_endl;
  }
};


void
generate_data( feature_vector& feature_set )
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
    vector_2d pt, tangent_dir(1,0);
    pt[0] = org_x + xi + random.normal()*sigma;
    pt[1] = org_y + random.normal()*sigma;
    feature_set.push_back( new rgrl_feature_trace_pt(pt, tangent_dir) );
  }

  // The edge of y = org_y + 99
  //
  for (unsigned int xi = 0; xi<200; xi+=2 ) {
    vector_2d pt, tangent_dir(-1, 0);
    pt[0] = org_x + xi + random.normal()*sigma;
    pt[1] = org_y + 99 + random.normal()*sigma;
    feature_set.push_back( new rgrl_feature_trace_pt(pt, tangent_dir) );
  }

  // The edge of x = org_x
  //
  for (unsigned int yi = 0; yi<100; yi+=2 ) {
    vector_2d pt, tangent_dir(0,1);
    pt[0] = org_x + random.normal()*sigma;
    pt[1] = org_y + yi + random.normal()*sigma;
    feature_set.push_back( new rgrl_feature_trace_pt(pt, tangent_dir) );
  }

  // The edge of x = org_x+199
  //
  for (unsigned int yi = 0; yi<100; yi+=2 ) {
    vector_2d pt,tangent_dir(0,-1);
    pt[0] = org_x + 199 + random.normal()*sigma;
    pt[1] = org_y + yi + random.normal()*sigma;
    feature_set.push_back( new rgrl_feature_trace_pt(pt, tangent_dir) );
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
    feature_set.push_back( new rgrl_feature_trace_pt(pt, tangent_dir) );
  }
}

void
add_outliers( feature_vector& feature_set )
{
  vnl_random random;

  unsigned int num_outliers = 200;
  for (unsigned int i = 0; i<num_outliers; i++) {
    vnl_vector<double> v(2), n(2);
    v[0] = random.drand32(0, 300);
    v[1] = random.drand32(0, 300);
    n = v;
    feature_set.push_back( new rgrl_feature_trace_pt(v, n.normalize()) );
  }
}

int
main()
{
  // Don't allow Visual Studio to open critical error dialog boxes
  testlib_enter_stealth_mode(); // defined in core/testlib/testlib_main.cxx

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
  const unsigned int  dimension = 2;
   rgrl_feature_set_sptr moving_feature_set =
    new rgrl_feature_set_location<dimension>(moving_feature_points);
  rgrl_feature_set_sptr fixed_feature_set =
    new rgrl_feature_set_location<dimension>(fixed_feature_points);
  rgrl_mask_box image_roi = moving_feature_set->bounding_box();

  // Set up the initial transformation and the estimator for affine
  // transformation
  //
  rgrl_transformation_sptr init_transform;
  vnl_matrix<double> A(2,2);
  A(0,0) = 0.996;   A(0,1) = -0.087;
  A(1,0) = -0.087;  A(1,1) = 0.996;
  vector_2d t( 10, -13);
  init_transform = new rgrl_trans_affine(A, t);
  rgrl_estimator_sptr estimator = new rgrl_est_affine();

  // Set up the ICP matcher
  //
  // BeginLatex
  //
  // The \emph{matcher} is responsible for computing the
  // correspondences and constructing a \emph{match set} data
  // structure to store them. One of the derived class of the matcher
  // is \code{rgrl\_matcher\_k\_nearest}. This matcher can use
  // different ``nearness'' measures, such as Euclidean distance and
  // appearance-based similarity
  // measures~\cite{feldmar:ijcv96,schutz:icpr98}, to determine the
  // $k$ nearest neighbours (allowing multiple correspondences per
  // feature). When $k=1$ and ``nearness'' is defined by Euclidean
  // distance, this becomes the classical ICP
  // matcher~\cite{besl:pami92,chen:ivc92,zhang:ijcv94}, as used in
  // this example.
  //
  // EndLatex

  // BeginCodeSnippet
  unsigned int k = 1;
  rgrl_matcher_sptr cp_matcher = new rgrl_matcher_k_nearest( k );
  // EndCodeSnippet

  // Set up the weighter
  //
  // BeginLatex
  //
  // To robustly handle outliers (wrong matches) generated by the
  // matcher, proper weights should be assigned to the matches, such
  // that matches with higher errors have less weight.  ``Weighter''
  // objects compute the weights for each match and store them back
  // with the matches in the match set.  Several types of weight
  // calculations are implemented, including standard M-estimator
  // weighting, weighting based on feature similarities, and
  // competitive weighting, if there are multiple matches for each
  // feature.
  //
  // The M-estimator weight function depends on the loss function for
  // the estimation of the transformation \cite{stewart:siamrev99}.
  // The choice of loss function, \code{m\_est\_obj}, is motivated by
  // looking at several different loss functions. and their associated
  // weight functions illustrated in Figure~\ref{fig:robust-loss}. The
  // least-squares loss function has a constant weight (unit weight),
  // the Cauchy weight function descends and asymptotes at 0, while
  // the Beaton-Tukey biweight function has a hard limit beyond which
  // the weight is 0.  This limit is set to about 4 error scale
  // (robust standard deviation of the alignment errors) in the
  // statistics literature \cite{holland:stats77}.  This is important
  // for rejecting errors due to mismatches, and therefore it is the
  // one we choose here. Implementation of a variety of loss functions
  // can be found in \verb+vxl/contrib/rpl/rrel+.
  //
  // EndLatex

  // BeginCodeSnippet
  vcl_auto_ptr<rrel_m_est_obj>  m_est_obj( new rrel_tukey_obj(4) );
  rgrl_weighter_sptr wgter = new rgrl_weighter_m_est(m_est_obj, false, false);
  // EndCodeSnippet

  // BeginLatex
  //
  // \begin{figure}[tbp]
  // \begin{center}
  // \begin{tabular}{cc}
  // \includegraphics[width=2.3in]{m_est_rho} &
  // \includegraphics[width=2.3in]{m_est_wgts} \\ (a) & (b)
  // \end{tabular}
  // \end{center}
  // \caption{Plots of (a) the robust loss function $\rho(u)$ and (b) weight
  // function $w(u) = \rho'(u) / u $ for the Beaton-Tukey biweight loss
  // function, the Cauchy loss function and the quadratic loss function,
  // which equates to least-squares estimation.  The Beaton-Tukey is chosen
  // because it most aggressively rejects outliers, providing no weight to
  // matches with normalized distances greater than about 4 standard
  // deviations.}
  // \label{fig:robust-loss}
  // \end{figure}
  //
  // EndLatex

  // Set up the scale estimators, both weighted and unweighted
  //
  // BeginLatex
  //
  // The formulation of a weight function is $w(u) = \rho'(u) / u $,
  // where $\rho(u)$ is the loss function and $u$ is the registration
  // error measured in error scale (scale normalized residual). There
  // are several ways to compute the scale, and the methods can be
  // classified into weighted and unweighted. Weighted scale estimator
  // has precedence over unweighted, but it requires initialization
  // (to get the first set of weights). This can be done either by
  // providing a prior scale to the initializer, or by using an
  // unweighted scale estimator. In the example, we take the second
  // approach by declaring both types of scale estimator.
  //
  // EndLatex

  // BeginCodeSnippet
  vcl_auto_ptr<rrel_objective> muset_obj( new rrel_muset_obj(0, false) );

  rgrl_scale_estimator_unwgted_sptr unwgted_scale_est =
    new rgrl_scale_est_closest( muset_obj );
  rgrl_scale_estimator_wgted_sptr wgted_scale_est =
    new rgrl_scale_est_all_weights();
  // EndCodeSnippet

  // BeginLatex
  //
  // The unweighted scale estimator uses MUSE \cite{miller:cvpr96},
  // which is similar to MAD (Median Absolute Deviation), but much
  // more robust. It automatically determines the fraction of
  // (approximately) correct matches.
  //
  // EndLatex

  // Set up the convergence tester
  //
  // BeginLatex
  //
  // A \emph{convergence tester} determines whether the registration
  // process should terminate for the current initial estimate.  The
  // result of testing is stored in a \emph{convergence status}
  // object. Two termination criteria are currently implemented: (1)
  // the estimation has converged with the change of alignment error
  // below certain threshold, and (2) the estimation is
  // oscillating. Derived classes of the convergence tester include
  // weighted and unweighted. The alignment error computed by the
  // weighted tester is the weighted average of the errors, while the
  // unweighted tester returns the median error. The input parameter,
  // \code{tolerance}, to
  // \code{rgrl\_convergence\_on\_weighted\_error} does not affect the
  // termination of the registration process, but allows the result to
  // be labeled as acceptable or not.
  //
  // EndLatex

  // BeginCodeSnippet
  double tolerance = 1.5;
  rgrl_convergence_tester_sptr conv_test =
    new rgrl_convergence_on_weighted_error( tolerance );
  // EndCodeSnippet

  // Store the data in the data manager
  //
  // BeginLatex
  //
  // As before, all the components, except the convergence tester, are
  // stored in the data manager. Table~\ref{tb:defaults} contains the
  // list of default techniques for the major components if not set.
  //
  // \begin{table}[t]
  // \begin{center}
  // \begin{tabular}{|l||c|c|} \hline
  // Component & RRL Class & Technique \\ \hline \hline
  // matcher & rgrl\_matcher\_k\_nearest & ICP (k=1) \\ \hline
  // weighter& rgrl\_weighter\_m\_est & Beaton-Tukey Bi-weight \\ \hline
  // unweighted scale estimator & rgrl\_scale\_est\_closest & MAD (Median Abs. Dev.) \\ \hline
  // convergence tester & rgrl\_convergence\_on\_median\_error & Median error\\ \hline
  // \end{tabular}
  // \end{center}
  // \caption{Default techniques for major components if not set}
  // \label{tb:defaults}
  // \end{table}
  //
  // EndLatex

  // BeginCodeSnippet
  rgrl_data_manager_sptr data = new rgrl_data_manager();
  data->add_data( moving_feature_set, // data from moving image
                  fixed_feature_set,  // data from fixed image
                  cp_matcher,         // matcher for this data
                  wgter,              // weighter
                  unwgted_scale_est,  // unweighted scale estimator
                  wgted_scale_est);   // weighted scale estimator
  // EndCodeSnippet

  // Construct the registration engine and adjust some parameters
  //
  // BeginLatex
  //
  // Construction of the registration engine takes the data manager
  // and the convergence tester. The Engine also allows adjustment of
  // some parameters, such as \code{set\_max\_icp\_iter()} to limit
  // the maximum amount of looping in Figure~\ref{fb-diagram}.
  //
  // EndLatex

  // BeginCodeSnippet
  rgrl_feature_based_registration reg( data, conv_test );
  reg.set_expected_min_geometric_scale( 0.1 ); //scale cannot go lower than 0.1
  reg.set_max_icp_iter( 10 );
  // EndCodeSnippet

  reg.add_observer( new rgrl_event_iteration(), new command_iteration_update());

  // Run ...
  //
  reg.run( image_roi, estimator, init_transform );

  if ( reg.has_final_transformation() ) {
    vcl_cout<<"Final xform:"<<vcl_endl;
    rgrl_transformation_sptr trans = reg.final_transformation();
    rgrl_trans_affine* a_xform = rgrl_cast<rgrl_trans_affine*>(trans);
    vcl_cout<<"A = "<<a_xform->A()<<vcl_endl
            <<"t = "<<a_xform->t()<<vcl_endl
            <<"Final alignment error = "<<reg.final_status()->error()<<vcl_endl;
  }

  // Perform testing
  //
  testlib_test_start( "Registration of simple shapes with outliers" );
  testlib_test_assert_near("", reg.final_status()->error(), 0.0, 1.0 );
  return testlib_test_summary();
}
