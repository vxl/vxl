//:
// \file
// \author Isabel Restrepo
// \date 11-Apr-2011

#include "bvpl_global_taylor.h"

#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>

#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>

#include <bvpl/bvpl_octree/bvpl_block_vector_operator.h>
#include <bvpl/functors/bvpl_algebraic_functor.h>
#include <bvpl/kernels/bvpl_taylor_basis_factory.h>

#include <vul/vul_file.h>

//: Constructor  from xml file
bvpl_global_taylor::bvpl_global_taylor(const vcl_string &path){
  
  vcl_cout << "Loading taylor info from xml-file" << vcl_endl;
  int valid = 0;
  path_out_ = path;
  vcl_ifstream xml_ifs(xml_path().c_str());
  if(!xml_ifs.is_open()){
    vcl_cerr << "Error: could not open xml info file: " << xml_path() << " \n";
    throw;
  }
  bxml_document doc = bxml_read(xml_ifs);
  bxml_element query("taylor_global_info");
  bxml_data_sptr root = bxml_find_by_name(doc.root_element(), query);
  if (!root) {
    vcl_cerr << "Error: bvpl_discover_pca_kernels - could not parse xml root\n";
    throw;
  }
  
  //Parse scenes
  bxml_element scenes_query("scene");
  vcl_vector<bxml_data_sptr> scenes_data = bxml_find_all_with_name(root, scenes_query);
  
  unsigned nscenes=scenes_data.size();
  vcl_cout << "Number of scenes: " << nscenes <<vcl_endl;
  
  scenes_.clear();
  scenes_.resize(nscenes);
  
  aux_dirs_.clear();
  aux_dirs_.resize(nscenes);
  
  finest_cell_length_.clear();
  finest_cell_length_.resize(nscenes);
  
  for(unsigned si = 0; si < nscenes; si++)
  {
    bxml_element* scenes_elm = dynamic_cast<bxml_element*>(scenes_data[si].ptr());
    int id = -1;
    scenes_elm->get_attribute("id", id);
    scenes_elm->get_attribute("path", scenes_[id]);
    scenes_elm->get_attribute("aux_dir", aux_dirs_[id]);
    scenes_elm->get_attribute("cell_length" , finest_cell_length_[id]);
    
    vcl_cout << "Scene " << id << " is " << path << "\n";
  }
  
  //Parse kernels
  bxml_element kernels_query("kernels");
  bxml_data_sptr kernels_data = bxml_find_by_name(root, kernels_query);
  if(kernels_data)
  {
    
    bxml_element* kernels_elm = dynamic_cast<bxml_element*>(kernels_data.ptr());
     
    kernels_elm->get_attribute("path", kernels_path_);
    
    vcl_cout << "Kernels path is " << kernels_path_ << "\n";
    kernel_vector_ = new bvpl_kernel_vector;
    
    if(vul_file::exists(kernels_path_) && vul_file::is_directory(kernels_path_)){
      //read the kernels

      kernel_vector_->kernels_.clear();

      kernel_vector_->kernels_.resize(10, NULL);
      vcl_string kernel_names[]={"I0", "Ix", "Iy", "Iz", "Ixx", "Iyy", "Izz", "Ixy", "Ixz", "Iyz"};
      for (unsigned ki = 0; ki<10; ki++){

        vcl_string filename = kernels_path_ + "/" + kernel_names[ki] + ".txt";
        vcl_cout << "Reading kernel file : " << filename << vcl_endl;
        bvpl_taylor_basis_factory factory(filename);
        bvpl_kernel_sptr kernel = new bvpl_kernel(factory.create());
        kernel_vector_->kernels_[ki] = kernel;
      }
    }
  } 
  else {
    vcl_cerr << " In Gobal Taylor, path to kernels does not exist\n";
  }


}

//: Compute the 10 taylor kernels for this scene at current block. The ouput is saved to the projection scene as a 10-d vector
void bvpl_global_taylor::compute_taylor_coefficients(int scene_id, int block_i, int block_j, int block_k)
{
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;
  
  typedef boct_tree<short,vnl_vector_fixed<double,10> > taylor_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > taylor_cell_type;
  
  boxm_scene_base_sptr data_scene_base =load_scene(scene_id);
  boxm_scene_base_sptr proj_scene_base =load_projection_scene(scene_id);
  boxm_scene_base_sptr valid_scene_base = load_valid_scene(scene_id);
  
  boxm_scene<boct_tree<short, float> >* data_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*>(data_scene_base.as_pointer());
  boxm_scene<taylor_tree_type>* proj_scene = dynamic_cast<boxm_scene<taylor_tree_type>*>(proj_scene_base.as_pointer());
  boxm_scene<boct_tree<short, bool> >* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
  if(!(data_scene && proj_scene && valid_scene))
  {
    vcl_cerr << "Error in bvpl_global_taylor::sample_statistics: Could not cast input scenes" << vcl_endl;
    return;
  }
  
  //init variables
  data_scene->unload_active_blocks();
  proj_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  
  //operate on scene
  bvpl_block_vector_operator block_oper;
  bvpl_algebraic_functor functor;
  block_oper.operate(*data_scene, functor, kernel_vector_, block_i, block_j, block_k, *proj_scene, *valid_scene, finest_cell_length_[scene_id]);
  
  //clean memory
  data_scene->unload_active_blocks();
  proj_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();

  
}

//: Init auxiliary scenes and smallest cell length values
void bvpl_global_taylor::init()
{
  for(unsigned i = 0; i < aux_dirs_.size(); i++)
  {
    boxm_scene_base_sptr data_scene_base = load_scene(i);
    boxm_scene<boct_tree<short, float> >* data_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (data_scene_base.as_pointer());
    if (!data_scene){
      vcl_cerr << "Error in bvpl_global_taylor::init(): Could not cast data scene \n";
      return;
    }
    double finest_cell_length = data_scene->finest_cell_length();
    finest_cell_length_[i] = finest_cell_length;
    
    if (!(vul_file::exists(aux_dirs_[i]) && vul_file::is_directory(aux_dirs_[i]))){
      vul_file::make_directory(aux_dirs_[i]);
    }

    {
      vcl_stringstream aux_scene_ss;
      aux_scene_ss << "valid_scene_" << i ;
      vcl_string aux_scene_path = aux_dirs_[i] + "/" + aux_scene_ss.str() + ".xml";
      if(!vul_file::exists(aux_scene_path)){
        vcl_cout<< "Scene: " << aux_scene_path << " does not exist, initializing" << vcl_endl;
        boxm_scene<boct_tree<short, bool> > *aux_scene =
        new boxm_scene<boct_tree<short, bool> >(data_scene->lvcs(), data_scene->origin(), data_scene->block_dim(), data_scene->world_dim(), data_scene->max_level(), data_scene->init_level());
        aux_scene->set_appearance_model(BOXM_BOOL);
        aux_scene->set_paths(aux_dirs_[i], aux_scene_ss.str());
        aux_scene->write_scene("/" + aux_scene_ss.str() +  ".xml");
      }    
    }
    
    {
      vcl_stringstream proj_scene_ss;
      proj_scene_ss << "proj_taylor_scene_" << i ;
      vcl_string proj_scene_path = aux_dirs_[i] + "/" + proj_scene_ss.str() + ".xml";
      if(!vul_file::exists(proj_scene_path)){
        vcl_cout<< "Scene: " << proj_scene_path << " does not exist, initializing" << vcl_endl;
        typedef boct_tree<short,vnl_vector_fixed<double,10> > taylor_tree_type;
        boxm_scene<taylor_tree_type > *proj_scene =
        new boxm_scene<taylor_tree_type >(data_scene->lvcs(), data_scene->origin(), data_scene->block_dim(), data_scene->world_dim(), data_scene->max_level(), data_scene->init_level());
        proj_scene->set_appearance_model(VNL_DOUBLE_10);
        proj_scene->set_paths(aux_dirs_[i], proj_scene_ss.str());
        proj_scene->write_scene("/" + proj_scene_ss.str() +  ".xml");
      }
    }
  }
  
  xml_write();
}


//: Load scene info
boxm_scene_base_sptr bvpl_global_taylor::load_scene (int scene_id)
{
  if(scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    vcl_cerr << "Error in bvpl_global_pca::load_scene: Invalid scene id" << vcl_endl;
    return NULL;
  }  
  //load scene
  boxm_scene_base_sptr scene_base = new boxm_scene_base();
  boxm_scene_parser scene_parser;
  scene_base->load_scene(scenes_[scene_id], scene_parser);
  
  //cast scene
  boxm_scene<boct_tree<short, float > > *scene= new boxm_scene<boct_tree<short, float > >();
  if (scene_base->appearence_model() == BOXM_FLOAT){     
    scene->load_scene(scene_parser);
    scene_base = scene;
  }else {
    vcl_cerr << "Error in bvpl_global_pca::load_scene: Invalid apperance model" << vcl_endl;
    return NULL;
  }
  
  return scene_base;
}

//: Load auxiliary scene indicating if a cell is valid. e.g border cells are not valid
boxm_scene_base_sptr bvpl_global_taylor::load_valid_scene (int scene_id)
{
  if(scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    vcl_cerr << "Error in bvpl_global_taylor::load_scene: Invalid scene id" << vcl_endl;
    return NULL;
  }  
  //load scene
  boxm_scene_base_sptr aux_scene_base = new boxm_scene_base();
  boxm_scene_parser aux_parser;
  vcl_stringstream aux_scene_ss;
  aux_scene_ss << aux_dirs_[scene_id] << "/valid_scene_" << scene_id << ".xml";
  aux_scene_base->load_scene(aux_scene_ss.str(), aux_parser);
  
  //cast scene
  boxm_scene<boct_tree<short, bool > > *aux_scene= new boxm_scene<boct_tree<short, bool > >();
  if (aux_scene_base->appearence_model() == BOXM_BOOL){     
    aux_scene->load_scene(aux_parser);
    aux_scene_base = aux_scene;
  }else {
    vcl_cerr << "Error in bvpl_global_taylor::load_aux_scene: Invalid apperance model" << vcl_endl;
    return NULL;
  }
  
  return aux_scene_base;  
}

#if 0
//: Load auxiliary scene indicating if a cell is should be used for training
boxm_scene_base_sptr bvpl_global_taylor::load_train_scene (int scene_id)
{
  if(scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    vcl_cerr << "Error in bvpl_global_taylor::load_scene: Invalid scene id" << vcl_endl;
    return NULL;
  }  
  //load scene
  boxm_scene_base_sptr aux_scene_base = new boxm_scene_base();
  boxm_scene_parser aux_parser;
  vcl_stringstream aux_scene_ss;
  aux_scene_ss << aux_dirs_[scene_id] << "/train_scene_" << scene_id << ".xml";
  aux_scene_base->load_scene(aux_scene_ss.str(), aux_parser);
  
  //cast scene
  boxm_scene<boct_tree<short, bool > > *aux_scene= new boxm_scene<boct_tree<short, bool > >();
  if (aux_scene_base->appearence_model() == BOXM_BOOL){     
    aux_scene->load_scene(aux_parser);
    aux_scene_base = aux_scene;
  }else {
    vcl_cerr << "Error in bvpl_global_taylor::load_aux_scene: Invalid apperance model" << vcl_endl;
    return NULL;
  }
  
  return aux_scene_base;  
}
#endif


//: Load auxiliary scene info

boxm_scene_base_sptr bvpl_global_taylor::load_projection_scene (int scene_id)
{
  if(scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    vcl_cerr << "Error in bvpl_global_taylor::load_projection_scene: Invalid scene id" << vcl_endl;
    return NULL;
  }
  //load scene
  boxm_scene_base_sptr proj_scene_base = new boxm_scene_base();
  boxm_scene_parser proj_parser;
  vcl_stringstream proj_scene_ss;
  proj_scene_ss << aux_dirs_[scene_id] << "/proj_taylor_scene_" << scene_id << ".xml";
  proj_scene_base->load_scene(proj_scene_ss.str(), proj_parser);
  
  //cast scene
  typedef boct_tree<short,vnl_vector_fixed<double,10> > taylor_tree_type;
  boxm_scene<taylor_tree_type > *proj_scene= new boxm_scene<taylor_tree_type >();
  if (proj_scene_base->appearence_model() == VNL_DOUBLE_10){     
    proj_scene->load_scene(proj_parser);
    proj_scene_base = proj_scene;
  }else {
    vcl_cerr << "Error in bvpl_global_taylor::load_proj_scene: Invalid apperance model" << vcl_endl;
    return NULL;
  }
  
  return proj_scene_base;
  
}    

//: Write this class to xml file
void bvpl_global_taylor::xml_write()
{
  bxml_document doc;
  bxml_element *root = new bxml_element("taylor_global_info");
  doc.set_root_element(root);
  root->append_text("\n");
  
  
  //write the scenes
  for(unsigned i =0; i<scenes_.size(); i++) 
  {
    bxml_element* scenes_elm = new bxml_element("scene");
    scenes_elm->append_text("\n");
    scenes_elm->set_attribute("id", i);
    scenes_elm->set_attribute("path", scenes_[i]);
    scenes_elm->set_attribute("aux_dir", aux_dirs_[i]);
    scenes_elm->set_attribute("cell_length", finest_cell_length_[i]);
    root->append_data(scenes_elm);
    root->append_text("\n");
  }
  
  //write path to kernels 
  bxml_element* kernel_elm = new bxml_element("kernels");
  kernel_elm->append_text("\n");
  kernel_elm->set_attribute("path", kernels_path_);
  root->append_data(kernel_elm);
  root->append_text("\n");
  
  //write to disk  
  vcl_ofstream os(xml_path().c_str());
  bxml_write(os, doc);
  os.close();
   
}


//: Extract a particular coefficient scene 
void bvpl_global_taylor::extract_coefficient_scene(int scene_id, int coefficient_id, boxm_scene<boct_tree<short, float > > *float_scene)
{
  
  typedef boct_tree<short, float> float_tree_type;
  typedef boct_tree_cell<short, float> float_cell_type;
  
  typedef boct_tree<short,vnl_vector_fixed<double,10> > projection_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > projection_cell_type;
  vcl_vector<vnl_vector_fixed<double,10> > projections;
  
  boxm_scene_base_sptr projection_scene_base = load_projection_scene(scene_id);
  
  boxm_scene<projection_tree_type>* projection_scene = dynamic_cast<boxm_scene<projection_tree_type>* >(projection_scene_base.as_pointer());
 

  if(!(projection_scene && float_scene))
  {
    vcl_cerr << "Error in bof_util::random_label_for_training: Could not cast scenes" << vcl_endl;
    return;
  }
  
  //init variables
  projection_scene->unload_active_blocks();
  float_scene->unload_active_blocks();
  
  //get the cells for these blocks
  boxm_block_iterator<projection_tree_type> it = projection_scene->iterator();
  for (it.begin(); !it.end(); ++it) 
  {
    if(!(projection_scene->valid_index(it.index()) && float_scene->valid_index(it.index()))){
      vcl_cerr << "In bof_util::random_label_for_training: invalid block" << vcl_endl;
      return;
    }
    
    projection_scene->load_block(it.index());
    float_scene->load_block(it.index());
    
    //get the trees
    projection_tree_type* projection_tree = projection_scene->get_block(it.index())->get_tree();
    float_tree_type* float_tree = projection_tree->clone_to_type<float>();
    
    
    //get leaf cells
    vcl_vector<projection_cell_type *> projection_leaves = projection_tree->leaf_cells();
    vcl_vector<float_cell_type *> float_leaves = float_tree->leaf_cells();
    
    float tree_ncells = projection_leaves.size();
    bool zero_mean = false;

    for(unsigned long i =0; i<tree_ncells; i++)
    {
     float_leaves[i]->set_data((float)(projection_leaves[i]->data())[coefficient_id]);  
    }
    float_scene->get_block(it.index())->init_tree(float_tree);
    float_scene->write_active_block();

  }
    
    //release memory
    projection_scene->unload_active_blocks();
    float_scene->unload_active_blocks();
}