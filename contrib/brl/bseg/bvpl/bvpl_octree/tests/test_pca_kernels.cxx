//:
// \file
// \brief : Tests bvpl/PCA operations
// \author Isabel Restrepo
// \date 17-Nov-2010

#include <testlib/testlib_test.h>

#include <bvpl/bvpl_octree/bvpl_discover_pca_kernels.h>
#include <bvpl/bvpl_octree/bvpl_global_pca.h>
#include "test_utils.h"

#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>

#include <vul/vul_file.h>
#include <vpl/vpl.h>

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
  std::cerr << "Leaks at test_pca_kernels - 1 " << boct_tree_cell<short, float >::nleaks() << '\n';
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
  std::cerr << "Leaks at test_pca_kernels - 2 " << boct_tree_cell<short, float >::nleaks() << '\n';
#endif

#if 0
  std::cout<< "error : " << verror << '\n'
          << "t_error : " << t_verror << '\n'
          << "weights : " << pca_extractor.weights() << '\n'

          << "error 1: " << verror << '\n'
          << "error 2: " << verror2 << '\n'

          << "mean 1: " << pca_extractor.mean() << '\n'
          << "mean 2: " << pca_extractor2.mean() << '\n'

          << "path 1: " << pca_extractor.scene_path() << '\n'
          << "path 2: " << pca_extractor2.scene_path() << std::endl;
#endif
}

void test_global()
{
  //create scene
  boxm_scene<boct_tree<short, float> > *scene = create_scene(3,3,3);
  //neighborhood box for volume "patches" 5x5x5
  vgl_box_3d<int> neighborhood(vgl_point_3d<int>(-2,-2,-2), vgl_point_3d<int>(2,2,2));
  //number of samples - 10% of total number of leaf-cells
  double training_fraction = 1.0;
  unsigned long nsamples = (unsigned long)((double)scene->size() * training_fraction);
  double finest_cell_length = scene->finest_cell_length();

  clean_up("./pca_info", "*.txt");
  vpl_mkdir("./pca_info",0777);
  clean_up("./pca_global_info", "*.txt");
  vpl_mkdir("./pca_global_info",0777);

  bvpl_discover_pca_kernels pca_info(neighborhood, nsamples, scene, "./pca_info");
  pca_info.xml_write();

  std::vector<std::string> scenes;
  std::vector<std::string> aux_paths;
  std::vector<double> cell_lengths;
  scenes.push_back("./test_scene.xml");
  aux_paths.push_back(".");
  cell_lengths.push_back(finest_cell_length);

  boxm_scene<boct_tree<short, int> > *aux_scene =
  new boxm_scene<boct_tree<short, int> >(scene->lvcs(), scene->origin(), scene->block_dim(), scene->world_dim(), scene->max_level(), scene->init_level());
  aux_scene->set_appearance_model(BOXM_INT);

  std::string scene_name = "test_aux_scene";
  std::string scene_path(vul_file::get_cwd());
  aux_scene->set_paths(scene_path, scene_name);
  aux_scene->write_scene("./test_aux_scene.xml");

  bvpl_global_pca<125> pca_global_info(scenes, aux_paths,cell_lengths, neighborhood,training_fraction, "./pca_global_info");
  pca_global_info.xml_write();

  vnl_matrix<double> s1 = pca_info.compute_scatter_matrix(scene);

  unsigned file =0;
  for (unsigned i = 0; i < 3; i++)
    for (unsigned j = 0; j < 3; j++)
      for (unsigned k = 0; k < 3; k++) {
        vnl_matrix_fixed<double, 125, 125> S;
        vnl_vector_fixed<double, 125> mean;
        unsigned long nfeature =0L;
        pca_global_info.sample_statistics(0, i, j, k, S, mean, nfeature);

        std::stringstream scatter_ss;
        scatter_ss << pca_global_info.path_out() << "/S_" << 0 << '_' <<file<< ".txt";
        std::ofstream scatter_ofs(scatter_ss.str().c_str());
        scatter_ofs.precision(15);
        scatter_ofs << S;

        std::stringstream mean_ss;
        mean_ss <<  pca_global_info.path_out() << "/mean_" << 0 << '_'  <<file << ".txt";
        std::ofstream mean_ofs ( mean_ss.str().c_str() );
        mean_ofs.precision(15);
        mean_ofs << nfeature << '\n' << mean;
        file++;
      }

  //read pairwise

  unsigned nrepeat = 0; // will become ceil(log_2(file)) after this iteration
  unsigned orig_file = file;
  for (unsigned pow_of_2 = 1; pow_of_2 < orig_file; pow_of_2*=2,++nrepeat) // i.e.: ceil(log_2(file)) iterations
  {
    unsigned file_p = 0;
    unsigned nfiles = 0;
    for (; file_p < file; file_p+=2, ++nfiles)
    {
      vnl_matrix_fixed<double, 125, 125> S1(0.0);
      vnl_vector_fixed<double, 125> mean1(0.0);
      unsigned long nfeatures1 =0;

      {
        std::stringstream scatter_ss;
        scatter_ss <<pca_global_info.path_out()  << "/S_" << nrepeat << '_' << file_p << ".txt";
        std::ifstream scatter_ifs(scatter_ss.str().c_str());
        scatter_ifs >> S1;
        std::stringstream mean_ss;
        mean_ss << pca_global_info.path_out() << "/mean_" << nrepeat << '_'  << file_p << ".txt";
        std::ifstream mean_ifs ( mean_ss.str().c_str() );
        mean_ifs >> nfeatures1;
        mean_ifs >> mean1;
#if 0
        vul_file::delete(scatter_ss.str());
        vul_file::delete(mean_ss.str());
#endif
      }

      vnl_matrix_fixed<double, 125, 125> S2(0.0);
      vnl_vector_fixed<double, 125> mean2(0.0);
      unsigned long nfeatures2 =0;

      {
        std::stringstream scatter_ss;
        scatter_ss <<pca_global_info.path_out()  << "/S_" << nrepeat << '_' << file_p + 1  << ".txt";
        std::ifstream scatter_ifs(scatter_ss.str().c_str());
        scatter_ifs >> S2;
        std::stringstream mean_ss;
        mean_ss << pca_global_info.path_out() << "/mean_" << nrepeat << '_' << file_p + 1 << ".txt";
        std::ifstream mean_ifs ( mean_ss.str().c_str() );
        mean_ifs >> nfeatures2;
        mean_ifs >> mean2;
      }

      vnl_matrix_fixed<double, 125, 125> S_out(0.0);
      vnl_vector_fixed<double, 125> mean_out(0.0);
      double nfeatures_out =0.0;
      bvpl_global_pca<125>::combine_pairwise_statistics(mean1, S1, (double)nfeatures1, mean2, S2, (double)nfeatures2, mean_out, S_out, nfeatures_out);

      std::stringstream scatter_ss;
      scatter_ss <<pca_global_info.path_out()  << "/S_" << (nrepeat+1) << '_' << file_p/2  << ".txt";
      std::ofstream scatter_ofs(scatter_ss.str().c_str());
      scatter_ofs.precision(15);
      scatter_ofs << S_out;

      std::stringstream mean_ss;
      mean_ss << pca_global_info.path_out() << "/mean_" <<  (nrepeat+1) << '_' << file_p/2<< ".txt";
      std::ofstream mean_ofs ( mean_ss.str().c_str() );
      mean_ofs.precision(15);
      mean_ofs << nfeatures_out << '\n' << mean_out;
    }
    file = nfiles;
  }

  //load final mean and scatter
  vnl_matrix_fixed<double, 125, 125> S_total(0.0);
  vnl_vector_fixed<double, 125> mean_total(0.0);
  unsigned long nfeatures_total =0;


  std::stringstream scatter_ss;
  scatter_ss <<pca_global_info.path_out()  << "/S_" << nrepeat<< '_' << 0 << ".txt";
  std::ifstream scatter_ifs(scatter_ss.str().c_str());
  scatter_ifs >> S_total;
  std::stringstream mean_ss;
  mean_ss << pca_global_info.path_out() << "/mean_" << nrepeat << '_'  << 0 << ".txt";
  std::ifstream mean_ifs ( mean_ss.str().c_str() );
  mean_ifs >> nfeatures_total;
  mean_ifs >> mean_total;


  std::cout << "Scatter diff:\n " << ((s1  - S_total).array_two_norm()) << '\n'
           << "Mean diff:\n " << (pca_info.mean()  - mean_total).two_norm() << '\n';

  clean_up("./pca_info", "*.txt");
  clean_up("./pca_global_info", "*.txt");
#if 0
  pca_global_info.compute_statistics();
#endif
  //compare results
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
  test_global();
  clean_up();
}


TESTMAIN(test_pca_kernels);
