//:
// \file
// \brief : Tests bvpl/PCA operations
// \author Isabel Restrepo
// \date 17-Nov-2010

#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

#include <bvpl/bvpl_octree/bvpl_discover_pca_kernels.h>
  
#include "test_utils.h"

void test_kernel_set_up()
{
  bool result=true;;
  //create scene
  boxm_scene<boct_tree<short, float> > *scene = create_scene(4,4,4);
  //neighborhood box for volume "patches" 3x3x3 
   vgl_box_3d<int> neighborhood(vgl_point_3d<int>(-1,-1,-1), vgl_point_3d<int>(1,1,1));
  //number of samples - 10% of total number of leaf-cells
  unsigned long nsamples = (unsigned long)((double)scene->size() * 0.1);

  clean_up("./evd", "*.txt");
  vpl_mkdir("./evd",0777);
  clean_up("./svd", "*.txt");
  vpl_mkdir("./svd",0777);

  bvpl_discover_pca_kernels pca_extractor(neighborhood, nsamples, scene, "./evd");
   bvpl_discover_pca_kernels pca_extractor_svd(neighborhood, nsamples, scene, "./svd", false);


  //XML write
  pca_extractor.xml_write();
  pca_extractor_svd.xml_write();
  
  //training errorr
  vnl_vector<double> verror;
  vnl_vector<double> t_verror;
  vnl_vector<double> testing_error;
  pca_extractor.compute_training_error(verror);
  pca_extractor.theoretical_training_error(t_verror);
  pca_extractor.compute_testing_error(testing_error);
  
#ifdef DEBUG_LEAKS
  vcl_cerr << "Leaks at test_pca_kernels - 1 " << boct_tree_cell<short, float >::nleaks() << vcl_endl;
#endif

  result = verror[verror.size()-1] < 1.0e-7;
  TEST("Test training error:", result, true);  
   
  bvpl_discover_pca_kernels pca_extractor2("./evd");
  vnl_vector<double> verror2;
  pca_extractor2.compute_training_error(verror2);
  vnl_vector<double> test_error2;
  pca_extractor2.compute_testing_error(test_error2);
  
  //Load from XML file - and check that clases are the same
  result = pca_extractor.principal_comps().is_equal(pca_extractor2.principal_comps(), 1.0e-9);
  TEST("Test xml_write PC:", result, true);
    
  result = pca_extractor.weights().is_equal(pca_extractor2.weights(), 1.0e-9);
  TEST("Test xml_write weights:", result, true);
    
  result = pca_extractor.mean().is_equal(pca_extractor2.mean(), 1.0e-9);
  TEST("Test xml_write mean:", result, true);
  
  result = pca_extractor.feature_dim()== pca_extractor2.feature_dim();
  TEST("Test xml_write feature dim:", result, true);
  
  result = pca_extractor.nsamples()== pca_extractor2.nsamples();
  TEST("Test xml_write nsamples :", result, true);
  
  result = verror.is_equal(t_verror, 1.0e-9);
  TEST("Test training errors agrees with theoretical value:", result, true);
  
  result = testing_error.squared_magnitude() > t_verror.squared_magnitude();
  TEST("Test testing error > training error:", result, true);
  
  result = testing_error.is_equal(test_error2, 1.0e-9);
  TEST("Testing errors are equal:", result, true);

#ifdef DEBUG_LEAKS
  vcl_cerr << "Leaks at test_pca_kernels - 2 " << boct_tree_cell<short, float >::nleaks() << vcl_endl;
#endif
  
#if 0
  
  vcl_cout<< "error : " << verror << vcl_endl;
  vcl_cout<< "t_error : " << t_verror << vcl_endl;
  vcl_cout<< "weights : " << pca_extractor.weights() << vcl_endl;
  
  vcl_cout<< "error 1: " << verror << vcl_endl;
  vcl_cout<< "error 2: " << verror2 << vcl_endl;

  vcl_cout<< "mean 1: " << pca_extractor.mean() << vcl_endl;
  vcl_cout<< "mean 2: " << pca_extractor2.mean() << vcl_endl;
  
  vcl_cout<< "path 1: " << pca_extractor.scene_path() << vcl_endl;
  vcl_cout<< "path 2: " << pca_extractor2.scene_path() << vcl_endl;
#endif
}


void test_pca_kernels()
{
  clean_up();

  //make up a matrix
  vnl_matrix<double> data(2, 10);
  unsigned i =0;
  data.set_column(i++, vnl_vector_fixed<double, 2>(0.69f, 0.49f));
  data.set_column(i++, vnl_vector_fixed<double, 2>(-1.31f, -1.21f));
  data.set_column(i++, vnl_vector_fixed<double, 2>(0.39, 0.99));
  data.set_column(i++, vnl_vector_fixed<double, 2>(0.09, 0.29));
  data.set_column(i++, vnl_vector_fixed<double, 2>(1.29, 1.09));
  data.set_column(i++, vnl_vector_fixed<double, 2>(0.49, 0.79));
  data.set_column(i++, vnl_vector_fixed<double, 2>(0.19, -0.31));
  data.set_column(i++, vnl_vector_fixed<double, 2>(-0.81, -0.81));
  data.set_column(i++, vnl_vector_fixed<double, 2>(-0.31, -0.31));
  data.set_column(i++, vnl_vector_fixed<double, 2>(-0.71, -1.01));

  bvpl_discover_pca_kernels pca_extractor(data);
  
  vnl_matrix<double> pc = pca_extractor.principal_comps();
  
  vnl_matrix<double> true_pc(2,2);
  i =0;
  true_pc.set_row(i++, vnl_vector_fixed<double, 2>(-0.677873, -0.735179)); 
  true_pc.set_row(i++, vnl_vector_fixed<double, 2>(-0.735179, 0.677873));
  
  bool result = true_pc.is_equal(pc, 0.001); 
  TEST("Test pc equals true_pc", result, true);
  
  test_kernel_set_up();
  clean_up();
}


TESTMAIN(test_pca_kernels);
