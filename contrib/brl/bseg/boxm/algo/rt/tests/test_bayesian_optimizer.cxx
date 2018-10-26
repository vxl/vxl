#include <testlib/testlib_test.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_aux_scene.h>
#include <boxm/sample/boxm_rt_sample.h>
#include <boxm/algo/rt/boxm_opt_rt_bayesian_optimizer.h>
#include <boct/boct_tree.h>
#include <vul/vul_file.h>

static void test_bayesian_optimizer()
{
  typedef boxm_sample<BOXM_APM_SIMPLE_GREY> data_type;
  typedef boxm_rt_sample<float> aux_type;

  // create the main scene
  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(1,1,1);
  boxm_scene<boct_tree<short,data_type> > scene(lvcs, origin, block_dim, world_dim);
  scene.set_octree_levels(3,2);
  scene.set_paths("./boxm_scene", "block");
  scene.set_appearance_model(BOXM_APM_SIMPLE_GREY);
  x_write(std::cout, scene, "scene");
  vul_file::make_directory("./boxm_scene");
  scene.write_scene();
  //vsl_b_ofstream os("scene.bin");
  //scene.b_write(os);
  //os.close();

  boxm_block_iterator<boct_tree<short,data_type> > iter(&scene);
  // default model
  boxm_simple_grey apm(0.3f, 0.4f, 0.5f);

  boxm_sample<BOXM_APM_SIMPLE_GREY> default_sample;
  default_sample.alpha=0.001f;
  default_sample.set_appearance(apm);

  // sample 1
  boxm_sample<BOXM_APM_SIMPLE_GREY> s1_sample;
  s1_sample.alpha=0.6f;
  s1_sample.set_appearance(apm);

  // sample 2
  boxm_sample<BOXM_APM_SIMPLE_GREY> s2_sample;
  s2_sample.alpha=0.6f;
  s2_sample.set_appearance(apm);

  while (!iter.end())
  {
    scene.load_block(iter.index().x(),iter.index().y(),iter.index().z());
    boxm_block<boct_tree<short,data_type> > * block=scene.get_active_block();
    boct_tree<short,data_type> * tree= block->get_tree();//new boct_tree<short,data_type>(3,2);
    boct_tree_cell<short,data_type>* cel11=tree->locate_point(vgl_point_3d<double>(0.01,0.01,0.01));
    cel11->set_data(s2_sample);
    boct_tree_cell<short,data_type>* cell2=tree->locate_point(vgl_point_3d<double>(0.51,0.51,0.01));
    cell2->set_data(s1_sample);
    block->init_tree(tree);
    block->get_tree()->print();
    scene.write_active_block();
    iter++;
  }

  // create the auxiliary scenes for each image
  std::vector<std::string> image_names;
  image_names.emplace_back("image1");
  image_names.emplace_back("image2");
  image_names.emplace_back("image3");
  std::vector<boxm_scene<boct_tree<short,aux_type> >*> aux_scenes(image_names.size());

  // create aux scenes
  for (unsigned i=0; i<image_names.size(); i++) {
    aux_scenes[i] = new boxm_scene<boct_tree<short,aux_type> >(scene.lvcs(), scene.origin(), scene.block_dim(), scene.world_dim());
    aux_scenes[i]->set_path(scene.path(),  image_names[i]);
    iter.begin();
    while (!iter.end()) {
      scene.load_block(iter.index().x(),iter.index().y(),iter.index().z());
      boxm_block<boct_tree<short,data_type> > * block=scene.get_active_block();
      boct_tree<short,data_type>* tree=block->get_tree();
      boct_tree<short,aux_type>* aux_tree = tree->clone_to_type<aux_type>();
      boct_tree_cell<short,aux_type>* cell1=aux_tree->locate_point(vgl_point_3d<double>(0.01,0.01,0.01));
      boxm_rt_sample<float> rt;
      rt.seg_len_=1.0;
      cell1->set_data(rt);
      boct_tree_cell<short,aux_type>* cell2=aux_tree->locate_point(vgl_point_3d<double>(0.51,0.51,0.01));
      rt.seg_len_=0.5;
      cell2->set_data(rt);
      aux_scenes[i]->set_block(iter.index(), new boxm_block<boct_tree<short,aux_type> >(block->bounding_box(), aux_tree));
      aux_scenes[i]->write_active_block();
      ++iter;
    }
  }

  boxm_opt_rt_bayesian_optimizer<short,BOXM_APM_SIMPLE_GREY,BOXM_AUX_OPT_RT_GREY> opt(scene, image_names);
  opt.optimize_cells(0.01);

  for (unsigned i=0; i<image_names.size(); i++) {
    delete aux_scenes[i];
  }
}

TESTMAIN(test_bayesian_optimizer);
