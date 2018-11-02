#ifndef boxm_compute_scene_difference_h_
#define boxm_compute_scene_difference_h_
//:
// \file
#include <iostream>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boxm/boxm_block.h>
#include <boxm/boxm_scene.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_operations.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


void boxm_compute_block_difference(boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > * block1,
                                   boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > * block2,
                                   boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > * blockout,
                                   float threshold)
{
  typedef boxm_sample<BOXM_APM_MOG_GREY> data_type;
  typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
  typedef boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> > cell_type;

  tree_type* tree1 = block1->get_tree();
  tree_type* tree2 = block2->get_tree();
  tree_type* treeout = blockout->get_tree();

  std::vector<cell_type*> leaves=tree1->leaf_cells();
  std::vector<cell_type*> leavesout=treeout->leaf_cells();

  for (unsigned i=0;i<leaves.size();i++)
  {
    cell_type *cell1=leaves[i];
    cell_type *cellout=leavesout[i];
    vgl_point_3d<double> p=cell1->local_bounding_box(tree1->root_level()).centroid();
    cell_type *cell2=tree2->locate_point(p);
    if (cell2)
    {
      data_type data1=cell1->data();
      double len1=tree1->cell_bounding_box(cell1).width();

      data_type data2=cell2->data();
      double len2=tree2->cell_bounding_box(cell2).width();

      data_type dataout=cellout->data();

      double p1=1-std::exp(-data1.alpha*len1);
      double p2=1-std::exp(-data2.alpha*len2);

      double p=p1+p2-2*p1*p2;

      double alphaout=-std::log(1-p)/len1;
      if (p<threshold)
          alphaout=0.0;
      dataout.alpha=(float)alphaout;
      if (p1>p2)
        dataout.appearance_=data1.appearance_;
      else
        dataout.appearance_=data2.appearance_;

      cellout->set_data(dataout);
    }
  }
}


void boxm_compute_scene_difference(boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > >&scene1,
                                   boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > >&scene2,
                                   boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > >&sceneout,
                                   float threshold)
{
  typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> >tree_type;
  boxm_block_iterator<tree_type> iter1(&scene1);
  boxm_block_iterator<tree_type> iter2(&scene2);

  // scene out is cloned as scene1.
  scene1.clone_blocks(sceneout);
  boxm_block_iterator<tree_type> iterout(&sceneout);

  for (; !iter1.end(); iter1++,iter2++,iterout++) {
    std::cout<<'.';
    scene1.load_block(iter1.index());
    scene2.load_block(iter2.index());
    sceneout.load_block(iterout.index());

    //boxm_block<tree_type>* block1 = *iter1;
    //boxm_block<tree_type>* block2 = *iter2;
    //boxm_block<tree_type>* blockout = *iterout;

    boxm_compute_block_difference((*iter1),(*iter2),(*iterout), threshold);
    sceneout.write_active_block();
  }
}

#endif // boxm_compute_scene_difference_h_
