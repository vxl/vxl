#include "test_utils.h"
//:
// \file
// \author Isabel Restrepo
// \date 15-Aug-2010

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpl/vpl.h>

void init_tree(boct_tree<short,float> *tree, unsigned i, float init_val)
{
  tree-> split(); //now we have 8 cells
  std::vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
  leaves[i]->set_data(init_val);
  leaves[i]->split();

  boct_loc_code<short> code = leaves[i]->get_code() ;
  std::cerr<< "Create Scene Code: " << code << std::endl;
}

boxm_scene<boct_tree<short, float> >* create_scene(unsigned world_dimx,unsigned world_dimy,unsigned world_dimz,
                                                   bool uniform, float val, const std::string& scene_prefix)
{
  float init_val = 0.5f;

  if (uniform)
    init_val = val;

  //crete the input scene
  unsigned int max_tree_level = 3;
  unsigned int init_level = 1;
  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(0,0,0);

  vgl_vector_3d<double> block_dim(10, 10, 10); //world coordinate dimensions of a block

  vgl_vector_3d<unsigned> world_dim(world_dimx,world_dimy,world_dimz); //number of blocks in a scene

  auto *scene = new boxm_scene<boct_tree<short, float> >(lvcs, origin, block_dim, world_dim, max_tree_level, init_level );
  std::string scene_path(vul_file::get_cwd());
  scene->set_paths(scene_path, scene_prefix);
  scene->set_appearance_model(BOXM_FLOAT);
  scene->write_scene(scene_prefix + ".xml");
  unsigned cell_index = 7;
  boxm_block_iterator<boct_tree<short, float> > iter=scene->iterator();
  iter.begin();
  while (!iter.end())
  {
    scene->load_block(iter.index());
    boxm_block<boct_tree<short, float> > *block = scene->get_active_block();
    // Construct an empty tree with 3 maximum levels 1 levele initialized to 0.0
    auto *tree = new boct_tree<short,float>(init_val, 3, 1);
    //tree->print();
    if (uniform)
      init_tree(tree, cell_index, init_val);
    else
      init_tree(tree, cell_index);
    //tree->print();
    block->init_tree(tree);
    scene->write_active_block();
    if (cell_index == 0) cell_index = 7;
    cell_index--;
    ++iter;
  }

#ifdef DEBUG_LEAKS
  std::cerr << "Leaks Created by create_scene() : " << boct_tree_cell<short,float>::nleaks() << std::endl;
#endif
  return scene;
}


void clean_up()
{
  //clean temporary files
  vul_file_iterator file_it("./*.bin");
  for (; file_it; ++file_it)
  {
    vpl_unlink(file_it());// this deletes the file at file_it()
    //    vul_file::delete_file_glob(file_it());
  }
}

void clean_up(const std::string& dir, const std::string& ext)
{
  //clean temporary files
  vul_file_iterator file_it(dir+"/"+ext);
  for (; file_it; ++file_it)
  {
    vpl_unlink(file_it());
    vul_file::delete_file_glob(file_it());
  }
}
