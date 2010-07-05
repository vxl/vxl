#include "boxm_ocl_scene.h"

//Initializes empty scene
boxm_ocl_scene::boxm_ocl_scene(int nb_x, int nb_y, int nb_z, 
                   int num_tree_buffs, int tb_len)
{
  num_blocks_x_ = nb_x;
  num_blocks_y_ = nb_y;
  num_blocks_z_ = nb_z;
  num_tree_buffers_ = num_tree_buffs;
  tree_buff_length_ = tb_len;
  
  //init values in tree buffers
  int4 init_cell(-1);
  tree_buffers_ = vbl_array_2d<int4>(num_tree_buffers_, tree_buff_length_, init_cell);

  //init blocks_
  blocks_ = vbl_array_3d<int4>(nb_x, nb_y, nb_z);
  for(int i=0; i<nb_x; i++) {
    for(int j=0; j<nb_y; j++) {
      for(int k=0; k<nb_z; k++) {
      
        //randomly pick one of the tree_buffers_, and assign it 
          
      } 
    }
  }
}                 











