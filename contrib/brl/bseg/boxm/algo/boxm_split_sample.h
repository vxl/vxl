// This is boxm_split_scene.h
#ifndef boxm_split_sample_h
#define boxm_split_sample_h

//:
// \file
// \brief  A class enclosing set of functions to split a boxm_sample scenes/trees into appearance and occupancy scenes/trees
// \author Isabel Restrepo mir@lems.brown.edu
// \date  12/3/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boxm/boxm_scene.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_apm_traits.h>

template<boxm_apm_type APM_MODEL>
class boxm_split_sample
{
public:
  typedef boct_tree<short, boxm_sample<APM_MODEL> > sample_tree_type;
  typedef boct_tree_cell<short, boxm_sample<APM_MODEL> > sample_cell_type;

  typedef typename boxm_apm_traits<APM_MODEL>::apm_datatype apm_datatype;
  typedef boct_tree<short, apm_datatype> apm_tree_type;
  typedef boct_tree_cell<short, apm_datatype> apm_cell_type;

  typedef typename boxm_apm_traits<APM_MODEL>::obs_mathtype alpha_datatype;
  typedef boct_tree<short, alpha_datatype> alpha_tree_type;
  typedef boct_tree_cell<short, alpha_datatype> alpha_cell_type;

  
  //: A function to split an boxm_sample scene, into an appereance and an ocuppancy scenes
  void split_scene(boxm_scene<sample_tree_type> &scene_in, boxm_scene<apm_tree_type> &apm_scene, boxm_scene<alpha_tree_type > &alpha_scene);
  
  //: A function to split an boxm_sample tree, into an appereance and an ocuppancy trees
  void split_tree(sample_tree_type *tree_in, apm_tree_type *apm_tree, alpha_tree_type *alpha_tree);

  
};


//: A function to split an boxm_sample scene, into an appereance and an ocuppancy scenes
template<boxm_apm_type APM_MODEL>
void boxm_split_sample<APM_MODEL>::split_scene(boxm_scene<sample_tree_type> &scene_in, 
                                               boxm_scene<apm_tree_type> &apm_scene,
                                               boxm_scene<alpha_tree_type > &alpha_scene)
{
  //missing:check that dimensions of all scenes agree
  
  //iterate through blocks in the scenes
  boxm_block_iterator<sample_tree_type> iter_in = scene_in.iterator();
  boxm_block_iterator<apm_tree_type> apm_iter = apm_scene.iterator();
  boxm_block_iterator<alpha_tree_type> alpha_iter = alpha_scene.iterator();
  
  iter_in.begin();
  apm_iter.begin();
  alpha_iter.begin();
  while(!iter_in.end() || !apm_iter.end() ||!alpha_iter.end())
  {
    scene_in.load_block(iter_in.index());
    apm_scene.load_block(apm_iter.index());
    alpha_scene.load_block(alpha_iter.index());
    
    sample_tree_type *tree_in= (*iter_in)->get_tree();
    apm_tree_type *apm_tree = tree_in->template clone_to_type<apm_datatype>();
    alpha_tree_type *alpha_tree = tree_in->template clone_to_type<alpha_datatype>();
    this->split_tree(tree_in, apm_tree,alpha_tree);
    (*apm_iter)->init_tree(apm_tree);
    (*alpha_iter)->init_tree(alpha_tree);
    apm_scene.write_active_block();
    alpha_scene.write_active_block();
    
    iter_in++; apm_iter++; alpha_iter++;
  }
}


//: A function to split an boxm_sample tree, into an appereance and an ocuppancy trees
template<boxm_apm_type APM_MODEL>
void boxm_split_sample<APM_MODEL>::split_tree(sample_tree_type *tree_in, apm_tree_type *apm_tree, alpha_tree_type *alpha_tree)
{
  //iterate trhough the trees
  vcl_vector<sample_cell_type*> sample_cells = tree_in->all_cells();
  vcl_vector<apm_cell_type*> apm_cells = apm_tree->all_cells();
  vcl_vector<alpha_cell_type*> alpha_cells = alpha_tree->all_cells();
  
  if(sample_cells.size()!=apm_cells.size() || apm_cells.size()!=alpha_cells.size()){
    vcl_cerr << "Different size vectors in split_sample_scene::split_tree" << vcl_endl;
    return;
  }
    
    
  for(unsigned i = 0; i < sample_cells.size(); i++)
  {
    boxm_sample<APM_MODEL> sample = sample_cells[i]->data();
    apm_cells[i]->set_data(sample.appearance_);
    alpha_cells[i]->set_data(sample.alpha);

  }
}


#endif
