// This is brl/bseg/boxm/ocl/boxm_ocl_utils.txx
#ifndef boxm_ocl_utils_txx_
#define boxm_ocl_utils_txx_
//:
// \file
#include "boxm_ocl_utils.h"
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vcl_cstdlib.h> // for rand()
#if defined(unix) || defined(__unix) || defined(__unix__)
#include <malloc.h>
#endif
template<class T>
void boxm_ocl_convert<T>::pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >* cell_ptr, vnl_vector_fixed<float, 16> &data)
{
  data.fill(0.0f);
  boxm_sample<BOXM_APM_SIMPLE_GREY> cell_data = cell_ptr->data();
  data[0]=cell_data.alpha; // alpha

  boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype appear=cell_data.appearance_;
  data[1]=appear.color();
  data[2]=appear.sigma();
  data[3]=appear.gauss_weight();
}

template<class T>
void boxm_ocl_convert<T>::pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* cell_ptr, vnl_vector_fixed<float, 16> &data)
{
  data.fill(0.0f);
  boxm_sample<BOXM_APM_MOG_GREY> cell_data = cell_ptr->data();
  data[0]=cell_data.alpha; // alpha
  boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype appear=cell_data.appearance_;
  //: assuming 3 components
  data[1]=appear.distribution(0).mean();
  data[2]=vcl_sqrt(appear.distribution(0).var());
  data[3]=appear.weight(0);
  data[4]=appear.distribution(0).num_observations;
  data[5]=appear.distribution(1).mean();
  data[6]=vcl_sqrt(appear.distribution(1).var());
  data[7]=appear.weight(1);
  data[8]=appear.distribution(1).num_observations;
  data[9]=appear.distribution(2).mean();
  data[10]=vcl_sqrt(appear.distribution(2).var());
  //data[11]=appear.weight(2);
  data[11]=appear.distribution(2).num_observations;
  data[12]=appear.num_observations;
}

template<class T>
void boxm_ocl_convert<T>::pack_cell_data(boct_tree_cell<short, float> * cell_ptr, vnl_vector_fixed<float, 16> &data)
{
  data.fill(0.0f);
  float cell_data = cell_ptr->data();
  data[0]=cell_data; // alpha
}


//: Recursive algorithm to take in a tree cell and copy structure and data into two arrays
template<class T>
void boxm_ocl_convert<T>::copy_to_arrays(boct_tree_cell<short, T >* cell_ptr,
                                       vcl_vector<vnl_vector_fixed<int, 4> >& cell_array,
                                       vcl_vector<vnl_vector_fixed<float, 16> >& data_array,
                                       int cell_input_ptr)
{
  // cell_input_ptr is the array index for the cell being constructed
  // it already exists in the cell array but only has the parent index set
  // no data or child pointers


  // convert the data to 16 vector size
 if (cell_ptr->is_leaf()) {
  vnl_vector_fixed<float, 16> data;
  boxm_ocl_convert<T>::pack_cell_data(cell_ptr,data);

  // data pointer will be at index == size after the push_back
  cell_array[cell_input_ptr][2] = data_array.size();
  data_array.push_back(data);
  cell_array[cell_input_ptr][3] = cell_ptr->level();
 }
  // if the cell has chidren then they must be copied
  if (!cell_ptr->is_leaf()) {
    // initialize data values to null
    //data_array[cell_array[cell_input_ptr][2]].fill(0.0);
    // create the children on the cell array
    int child_ptr = -1;
    boxm_ocl_utils::split(cell_array, cell_input_ptr, child_ptr);
    cell_array[cell_input_ptr][1]=child_ptr;
    cell_array[cell_input_ptr][3]=0;
    boct_tree_cell<short,T >* children = cell_ptr->children();
    for (unsigned i = 0; i<8; ++i) {
      boct_tree_cell<short, T >* child_cell_ptr = children + i;
      int child_cell_input_ptr = child_ptr +i;
      copy_to_arrays(child_cell_ptr, cell_array, data_array, child_cell_input_ptr);
    }
  }
}

//: Converts boxm_scene to a boxm_ocl_scene
// Currently requires that the scene be 
template<class T>
void boxm_ocl_convert<T>::convert_scene(boxm_scene<boct_tree<short, T> >* scene, int num_buffers, boxm_ocl_scene &ocl_scene)
{
  typedef boct_tree<short, T> tree_type;
  typedef boxm_scene<tree_type> scene_type; 
  typedef boxm_block<tree_type> block_type;
  typedef vnl_vector_fixed<int, 4> int4;
  typedef vnl_vector_fixed<int, 2> int2;
  typedef vnl_vector_fixed<float, 16> float16;
  int SMALL_BLK_MAX_LEVEL = 4, SMALL_BLK_INIT_LEVEL = 1;
  
  vgl_point_3d<double> origin = scene->origin();
  vgl_vector_3d<double> block_dim = scene->block_dim();
  int x_num, y_num, z_num;
  scene->block_num(x_num, y_num, z_num);
  vgl_vector_3d<int> block_num(x_num, y_num, z_num);
  bgeo_lvcs lvcs = scene->lvcs();
  
  vcl_cout<<"---bocm_ocl_convert::convert_scene-----------"<<vcl_endl
          <<"---ORIGINAL SCENE ---------------------------"<<vcl_endl
          <<"---origin: "<<origin<<vcl_endl
          <<"---block_dim: "<<block_dim<<vcl_endl
          <<"---block_num: "<<block_num<<vcl_endl
          <<"---max_level: "<<scene->max_level()<<vcl_endl
          <<"---init_level: "<<scene->init_level()<<vcl_endl
          <<"---------------------------------------------"<<vcl_endl;
  
  /* calculate number of small blocks along one side of one big block*/
  int max_level = (int) scene->max_level(); //equals rootLevel + 1
  int def_init = max_level - SMALL_BLK_MAX_LEVEL; //allows for trees to grow to depth 4
  int scene_init = scene->init_level(); 
  int init_level = vcl_max(def_init, scene_init);
  //Large scene level which becomes small scene roots
  int small_blk_root_level = max_level-init_level;
  vcl_cout<<"Big block scene init level: "<<init_level<<vcl_endl
          <<"big block scene level: "<<small_blk_root_level
          <<" to become small block scene root"<<vcl_endl;
  
  //finest blocks that lie along one side of the big (or small) blocks
  float finestPerBig = (float) vcl_pow((float)2, (float)(max_level-1));
  float finestPerSmall = (float) vcl_pow((float)2, (float)(max_level-init_level));
  //float finestPerSmall = (float) vcl_pow((float)2, (float(SMALL_BLK_MAX_LEVEL));
  int sm_n = (int) (finestPerBig/finestPerSmall);
  vcl_cout<<"Num small blocks per side of big block: "<<sm_n<<vcl_endl;
  vgl_vector_3d<int> block_num_small(sm_n*x_num, sm_n*y_num, sm_n*z_num);
  vgl_vector_3d<double> block_dim_small(block_dim.x()/sm_n, block_dim.y()/sm_n, block_dim.z()/sm_n);
  
  /* compute total tree space to allocate = number of blocks + size of all existing trees */
  int total_blocks = block_num_small.x() * block_num_small.y() * block_num_small.z();
  int total_tree_cells = 0;
  int total_leaf_cells = 0;
  boxm_block_iterator<tree_type> it(scene);
  for(it.begin(); !it.end(); it++) {
    vgl_point_3d<int> blk_ind = it.index();
    scene->load_block(blk_ind.x(), blk_ind.y(), blk_ind.z());
    block_type* block = scene->get_active_block();
    tree_type* tree = block->get_tree();
    total_tree_cells += tree->all_cells().size();
    total_leaf_cells += tree->leaf_cells().size();
    vcl_cout<<"# tree cells "<<tree->all_cells().size()<<vcl_endl;
  }
  vcl_cout<<"Total number of tree cells = "<<total_tree_cells<<vcl_endl;
  int buff_length = (int) (total_tree_cells/num_buffers);
  
  /* 1. Set up 3D array of blocks (small blocks), assuming all blocks are similarly sized */
  //vcl_cout<<"Small block dimensions = "<<block_num_small<<vcl_endl;
  int4 init(-1);
  vbl_array_3d<int4> blocks(block_num_small.x(), block_num_small.y(), block_num_small.z(), init);

  /* 2. set up 2d array of tree_buffers */
  //vcl_cout<<"tree_buffer dimensions = "<<num_buffers<<" buffers by "<<buff_length<<vcl_endl;
  vbl_array_2d<int4> tree_buffers(num_buffers, buff_length, init);
  
  /* 3. set up 1d array of mem ptrs */
  int2 mem_init; mem_init[0] = 0; mem_init[1] = 1;
  vbl_array_1d<int2> mem_ptrs(num_buffers, mem_init);
  
  /* 4. set up 2d array of data buffers */
  float16 dat_init(0.0f);
  dat_init[1] = .1;
  vbl_array_2d<float16> data_buffers(num_buffers, buff_length, dat_init);
  
  /* 5. Go through each block and convert it to smaller blocks */
  bool buffFull = false; 
  vnl_random random(9667566);
  boxm_block_iterator<tree_type> iter(scene); 
  for(iter.begin(); !iter.end(); iter++) {
    vcl_cout<<"Converting big block at "<<iter.index()<<vcl_endl;
    vgl_point_3d<int> blk_ind = iter.index();
    scene->load_block(blk_ind.x(), blk_ind.y(), blk_ind.z());
    block_type* block = scene->get_active_block();
    tree_type* tree = block->get_tree();
        
    //cells at level 3 (4th level from the bottom) make up the new roots 
    vcl_vector<boct_tree_cell<short,T>*> blk_roots = tree->cells_at_level(small_blk_root_level);
    vcl_cout<<"  number small blocks: "<<blk_roots.size()<<vcl_endl;
    int tot_alloc = 0, tot_child = 0;
    for(int blk_i=0; blk_i<blk_roots.size(); blk_i++) {
      
      //figure out which small_block i,j,k this root corresponds to
      boct_tree_cell<short,T>* root = blk_roots[blk_i];
      boct_loc_code<short> loc_code = root->get_code();
      vgl_point_3d<double> back_left = loc_code.get_point(tree->number_levels());
      int i = (int) (back_left.x()*sm_n) + blk_ind.x()*sm_n;
      int j = (int) (back_left.y()*sm_n) + blk_ind.y()*sm_n;
      int k = (int) (back_left.z()*sm_n) + blk_ind.z()*sm_n;
      vgl_point_3d<int> sm_ind(i,j,k);
      
      //create a tree and data arrays from block root
      int4 arr_root;
      arr_root[0] = -1; //no parent for root
      arr_root[1] = -1; // no children yet
      arr_root[2] = 0;  // points to datum
      arr_root[3] = 0;  // not used yet
      vcl_vector<int4> cell_array; 
      cell_array.push_back(arr_root); 
      vcl_vector<float16> data_array;
      data_array.push_back(dat_init);
      boxm_ocl_convert<T>::copy_to_arrays(root, cell_array, data_array, 0);
      tot_alloc += cell_array.size();
      
      //randomly choose a buffer, and get the first free spot in memory (update blocks)
      int buffIndex = random.lrand32(0, num_buffers-1);
      int buffOffset = mem_ptrs[buffIndex][1]-1; //minus one cause mem_end points to one past the last one
      int4 blk(0);
      blk[0] = buffIndex;            //buffer index 
      blk[1] = buffOffset;           //buffer offset to root 
      blk[2] = cell_array.size();    //tree size
      blk[3] = 0;                    //nothign for now
      blocks(i,j,k) = blk;
      
      //copy cell_array and data_array to buffer 
      //make sure there's enough room
      int start=mem_ptrs[buffIndex][0];
      int end = mem_ptrs[buffIndex][1];
      int freeSpace = (start >= end)? start-end : buff_length - (end-start);
      if(freeSpace > cell_array.size()) {
        for(int c=0; c<cell_array.size(); c++) {
          tree_buffers(buffIndex, buffOffset+c) = cell_array[c];
          
          //only copy data if it exists for this cell (not inner cells)
          int dat_index = cell_array[c][2];
          if(dat_index >= 0) {
            data_buffers(buffIndex,buffOffset+c) = data_array[dat_index];
            tree_buffers(buffIndex,buffOffset+c)[2] = buffOffset+c;
          }
        }
        //update end of tree data in mem_ptrs
        mem_ptrs[buffIndex][1] += cell_array.size();
      }
      else {
        buffFull = true;
      }
      cell_array.clear();
    }
    vcl_cout<<"  allocated "<<tot_alloc<<" cells"<<vcl_endl;
    if(buffFull) vcl_cout<<"boxm_ocl_utils::convert_scene: BLOCK @ "<<blk_ind<<" FILLED BUFFER "<<vcl_endl;
    buffFull = false;
  }
  

  /* make a pass to make sure all small blocks were initialized */
  int nonInitCount = 0;
  vcl_cout<<"init blocks: "<<vcl_endl;
  for(int i=0; i<blocks.get_row1_count(); i++) {
    for(int j=0; j<blocks.get_row2_count(); j++) {
      for(int k=0; k<blocks.get_row3_count(); k++) {
        
        int buffIndex = blocks(i,j,k)[0];
        //if this block isn't initialized... initialize it
        if(buffIndex < 0) {
          nonInitCount++;
          
          //choose a random buffIndex and see how much free space is left 
          buffIndex = random.lrand32(0, num_buffers-1);
          int start=mem_ptrs[buffIndex][0];
          int end = mem_ptrs[buffIndex][1];
          int freeSpace = (start >= end)? start-end : buff_length - (end-start);
          if(freeSpace > 0) {
          
            int buffOffset = mem_ptrs[buffIndex][1]-1; //minus one cause mem_end points to one past the last one
            int4 blk_ptr(0);
            blk_ptr[0] = buffIndex;  //buffer index 
            blk_ptr[1] = buffOffset; //buffer offset to root 
            blk_ptr[2] = 1;          //tree has size of 1 now
            blk_ptr[3] = 0;          //nothign for now
            blocks(i,j,k) = blk_ptr;

            //put root in that memory
            int4 blk_root(0); 
            blk_root[0] = -1; //no parent for root
            blk_root[1] = -1; // no children yet
            blk_root[2] = buffOffset; // points to datum
            blk_root[3] = 0;  // not used yet
            tree_buffers(buffIndex, buffOffset) = blk_root;

            //put data in memory 
            float16 datum(0.0f);
            datum[0] = .001;  //TODO have some p_init determine this alpha val
            data_buffers(buffIndex, buffOffset) = datum;

            //make sure mem spot is now taken
            mem_ptrs[buffIndex][1]++;
          }
          else {
            //notify that you ran out of space
            nonInitCount = -666;
          }
        }
      }
    }
  }
  if(nonInitCount < 0) 
    vcl_cout<<"Initializing uninitialized blocks failed.  Your scene is no good."<<vcl_endl;
  else
    vcl_cout<<"Initialized "<<nonInitCount<<" uninitialized blocks"<<vcl_endl;

  ocl_scene.init_scene(blocks, tree_buffers, data_buffers, mem_ptrs, lvcs, origin, block_dim_small);
  ocl_scene.set_max_level(max_level-init_level+1);
}




    
#define BOXM_OCL_UTILS_INSTANTIATE(T) \
  template class boxm_ocl_convert<T >\

#endif

