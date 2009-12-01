// This is bvpl_scene_vector_operator.h
#ifndef bvpl_scene_vector_operator_h
#define bvpl_scene_vector_operator_h

//:
// \file
// \brief  A class that operates a vector of bvpl_kernels on a boxm_scene
// \author Isabel Restrepo mir@lems.brown.edu
// \date  November 30, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim



class bvpl_scene_vector_operator
{
public:
    
  typedef  tree_in_type;
  typedef boct_tree<short, T_data_out> tree_out_type;
  typedef boct_tree_cell<short,T_data_in> cell_in_type;
  typedef boct_tree_cell<short,T_data_out> cell_out_type;
  
  
  //: Main interface funtion. Applies a vector of bvpl_kernels on a boxm_scene
  template<class T_data, class F >
  bool operate(boxm_scene<boct_tree<short, T_data> > &scene_in, 
               bvpl_octree_vector_operator<T_data> * vec_opertor,
               F functor,
               bvpl_kernel_vector_sptr kernel_vector,
               boxm_scene<boct_tree<short, T_data> > &scene_out,
               boct_tree<short ,int>* id_tree, short level, double cell_length)
  {
    //iterate through the scene
    boxm_block_iterator<boct_tree<short, T_data> > iter_in = scene_in_.iterator();
    boxm_block_iterator<boct_tree<short, T_data> > iter_out = scene_out_.iterator();
    iter_in.begin();
    iter_out.begin();
    
    bvpl_local_max_functor<tree_out_type> func_max;

    for (; !iter_in.end(); iter_in++, iter_out++) {
      scene_in.load_block(iter_in.index());
      scene_out.load_block(iter_out.index());
      boct_tree<short, T_data>  *tree_in= (*iter_in)->get_tree();
      boct_tree<short, T_data> *tree_out = tree_in -> clone();
      tree_out->init_cells(func_max.min_response());
      id_tree->init_cells(-1)

      boct_tree<short, T_data> *temp_tree_out = tree_in->clone();
           
      //at each tree, run the vector of kernels
      vec_opertor->operate(tree_in, functor, kernel_vector, tree_out, id_tree, level, cell_length);      
      (*iter_out)->init_tree(tree_out);
      scene_out.write_active_block();
    }
    
    return true;
    
  }
protected:
  F functor_;
};

#endif
