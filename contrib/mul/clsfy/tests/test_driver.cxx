#include <testlib/testlib_register.h>

DECLARE( test_adaboost );
DECLARE( test_binary_hyperplane );
DECLARE( test_binary_pdf_classifier );
DECLARE( test_binary_threshold_1d );
DECLARE( test_direct_boost );
DECLARE( test_k_nearest_neighbour );
DECLARE( test_mean_square_1d );
DECLARE( test_rbf_svm_smo );
DECLARE( test_smo_1 );
DECLARE( test_logit_loss_function );
DECLARE( test_binary_hyperplane_logit );
DECLARE( test_binary_1d_wrapper );
DECLARE( test_binary_tree );
DECLARE( test_random_forest );

void
register_tests()
{
  REGISTER( test_adaboost );
  REGISTER( test_binary_hyperplane );
  REGISTER( test_binary_pdf_classifier );
  REGISTER( test_binary_threshold_1d );
  REGISTER( test_direct_boost );
  REGISTER( test_k_nearest_neighbour );
  REGISTER( test_mean_square_1d );
  REGISTER( test_rbf_svm_smo );
  REGISTER( test_smo_1 );
  REGISTER( test_logit_loss_function );
  REGISTER( test_binary_hyperplane_logit );
  REGISTER( test_binary_1d_wrapper );
  REGISTER( test_binary_tree );
  REGISTER( test_random_forest );

}

DEFINE_MAIN;
