
#include "clsfy_add_all_loaders.h"
#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_cloneables_factory.h>
#include <clsfy/clsfy_classifier_base.h>
#include <clsfy/clsfy_builder_base.h>

#include <clsfy/clsfy_binary_hyperplane.h>
#include <clsfy/clsfy_binary_hyperplane_ls_builder.h>
#include <clsfy/clsfy_binary_hyperplane_logit_builder.h>
#include <clsfy/clsfy_binary_hyperplane_gmrho_builder.h>
#include <clsfy/clsfy_k_nearest_neighbour.h>
#include <clsfy/clsfy_knn_builder.h>
#include <clsfy/clsfy_rbf_parzen.h>
#include <clsfy/clsfy_parzen_builder.h>
#include <clsfy/clsfy_rbf_svm.h>
#include <clsfy/clsfy_rbf_svm_smo_1_builder.h>
#include <clsfy/clsfy_random_classifier.h>
#include <clsfy/clsfy_random_builder.h>
#include <clsfy/clsfy_null_classifier.h>
#include <clsfy/clsfy_null_builder.h>
#include <clsfy/clsfy_binary_1d_wrapper.h>
#include <clsfy/clsfy_binary_1d_wrapper_builder.h>
#include <clsfy/clsfy_binary_threshold_1d.h>
#include <clsfy/clsfy_binary_threshold_1d_builder.h>

void clsfy_add_all_loaders()
{
  vsl_add_to_binary_loader(clsfy_binary_hyperplane());
  vsl_add_to_binary_loader(clsfy_binary_hyperplane_ls_builder());
  mbl_cloneables_factory<clsfy_builder_base>::add(clsfy_binary_hyperplane_ls_builder());

  vsl_add_to_binary_loader(clsfy_binary_hyperplane_logit_builder());
  mbl_cloneables_factory<clsfy_builder_base>::add(clsfy_binary_hyperplane_logit_builder());

  vsl_add_to_binary_loader(clsfy_binary_hyperplane_gmrho_builder());
  mbl_cloneables_factory<clsfy_builder_base>::add(clsfy_binary_hyperplane_gmrho_builder());

  vsl_add_to_binary_loader(clsfy_k_nearest_neighbour());
  vsl_add_to_binary_loader(clsfy_knn_builder());
  mbl_cloneables_factory<clsfy_builder_base>::add(clsfy_knn_builder());

  vsl_add_to_binary_loader(clsfy_rbf_parzen());
  vsl_add_to_binary_loader(clsfy_parzen_builder());
  mbl_cloneables_factory<clsfy_builder_base>::add(clsfy_parzen_builder());

  vsl_add_to_binary_loader(clsfy_rbf_svm());
  vsl_add_to_binary_loader(clsfy_rbf_svm_smo_1_builder());
  mbl_cloneables_factory<clsfy_builder_base>::add(clsfy_rbf_svm_smo_1_builder());

  vsl_add_to_binary_loader(clsfy_random_classifier());
  vsl_add_to_binary_loader(clsfy_random_builder());
  mbl_cloneables_factory<clsfy_builder_base>::add(clsfy_random_builder());

  vsl_add_to_binary_loader(clsfy_null_classifier());
  vsl_add_to_binary_loader(clsfy_null_builder());
  mbl_cloneables_factory<clsfy_builder_base>::add(clsfy_null_builder());

  vsl_add_to_binary_loader(clsfy_binary_1d_wrapper());
  vsl_add_to_binary_loader(clsfy_binary_1d_wrapper_builder());
  mbl_cloneables_factory<clsfy_builder_base>::add(clsfy_binary_1d_wrapper_builder());


  vsl_add_to_binary_loader(clsfy_binary_threshold_1d());
  vsl_add_to_binary_loader(clsfy_binary_threshold_1d_builder());
  mbl_cloneables_factory<clsfy_builder_1d>::add(clsfy_binary_threshold_1d_builder());

}
