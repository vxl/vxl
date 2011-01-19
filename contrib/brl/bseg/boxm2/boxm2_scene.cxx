#include "boxm2_scene.h"
//:
// \file
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>

/* xml includes */
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <boxm2/io/boxm2_scene_parser.h>    

//vgl includes
#include <vgl/vgl_distance.h>


//: initializes Scene from XML file
boxm2_scene::boxm2_scene(vcl_string filename)
{
    //xml parser 
    xml_path_ = filename; 
    boxm2_scene_parser parser;
    if (filename.size() > 0) {
      vcl_FILE* xmlFile = vcl_fopen(filename.c_str(), "r");
      if (!xmlFile){
        vcl_cerr << filename.c_str() << " error on opening\n";
        return;
      }
      if (!parser.parseFile(xmlFile)) {
        vcl_cerr << XML_ErrorString(parser.XML_GetErrorCode()) << " at line "
                << parser.XML_GetCurrentLineNumber() << '\n';
        return;
      }
    }
    
    //store data path 
    data_path_ = parser.path(); 
    xml_path_  = data_path_ + "scene.xml"; 
    
    //lvcs, origin, block dimension
    parser.lvcs(lvcs_);
    local_origin_ = parser.origin();
    rpc_origin_   = parser.origin();
    
    //store BLOCKS
    blocks_ = parser.blocks();
}


//: add a block meta data...
void boxm2_scene::add_block_metadata(boxm2_block_metadata data)
{
  if ( blocks_.find(data.id_) != blocks_.end() )
  {
    vcl_cout<<"Boxm2 SCENE: Overwriting block metadata for id: "<<data.id_<<vcl_endl;
  }
  blocks_[data.id_] = data; 
}

vcl_vector<boxm2_block_id> boxm2_scene::get_vis_blocks(vpgl_perspective_camera<double>* cam)
{
  //get camera center and order blocks distance from the cam center
  vgl_point_3d<double> cam_center = cam->camera_center(); 

  //Map of distance, id
  vcl_vector<boxm2_dist_id_pair> distances; 
  
  //iterate through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter; 
  for(iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    
    vgl_point_3d<double>    blk_o   = (iter->second).local_origin_; 
    vgl_vector_3d<double>   blk_dim = (iter->second).sub_block_dim_; 
    vgl_vector_3d<unsigned> blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double>   length(blk_dim.x()*blk_num.x(), 
                                   blk_dim.y()*blk_num.y(),
                                   blk_dim.z()*blk_num.z()); 
    vgl_point_3d<double> blk_center = blk_o + length/2.0; 
    double dist = vgl_distance( blk_center, cam_center); 
    
    
    distances.push_back( boxm2_dist_id_pair(dist, iter->first) ); 
  }
    
  //sort distances
  vcl_sort(distances.begin(), distances.end()); 
    
  //put blocks in "vis_order"
  vcl_cout<<"CAM ORDER----------------------------------------"<<vcl_endl;
  vcl_vector<boxm2_block_id> vis_order; 
  vcl_vector<boxm2_dist_id_pair>::iterator di; 
  for(di = distances.begin(); di != distances.end(); ++di) {
    vis_order.push_back(di->id_); 
    vcl_cout<<di->id_<<"("<<di->dist_<<")    ";
  }
  vcl_cout<<vcl_endl<<"-----------------------------------------------"<<vcl_endl;
  return vis_order;
}

//: save scene (xml file)
void boxm2_scene::save_scene()
{
  //write out to XML file
  vcl_ofstream xmlstrm(xml_path_.c_str());
  x_write(xmlstrm, (*this), "scene");
}

//: return a heap pointer to a scene info 
boxm2_scene_info* boxm2_scene::get_blk_metadata(boxm2_block_id id)
{
  if ( blocks_.find(id) == blocks_.end() )
  {
    vcl_cerr<<"\nboxm2_scene::get_blk_metadata: Block doesn't exist: "<<id<<'\n'<<vcl_endl;
    return 0;
  }

  boxm2_block_metadata data = blocks_[id]; 
  boxm2_scene_info* info = new boxm2_scene_info(); 
  
  info->scene_origin[0] = (float) data.local_origin_.x();
  info->scene_origin[1] = (float) data.local_origin_.y();
  info->scene_origin[2] = (float) data.local_origin_.z();
  info->scene_origin[3] = (float) 0.0f;

  info->scene_dims[0] = (int) data.sub_block_num_.x();  // number of blocks in each dimension
  info->scene_dims[1] = (int) data.sub_block_num_.y();  
  info->scene_dims[2] = (int) data.sub_block_num_.z();  
  info->scene_dims[3] = (int) 0; 

  info->block_len = (float) data.sub_block_dim_.x(); 
  info->epsilon   = (float) (info->block_len / 100.0f); 
  
  info->root_level = data.max_level_-1; 
  info->num_buffer = 0; 
  info->tree_buffer_length = 0; 
  info->data_buffer_length = 0; 
  return info; 
}


vgl_box_3d<double> boxm2_scene::bounding_box()
{
  double xmin=10e10, xmax=-10e10; 
  double ymin=10e10, ymax=-10e10; 
  double zmin=10e10, zmax=-10e10; 
  
  //iterate through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter; 
  for(iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    
    //determine xmin, ymin, zmin using block_o
    vgl_point_3d<double>  blk_o = (iter->second).local_origin_; 
    if(blk_o.x() < xmin) xmin = blk_o.x(); 
    if(blk_o.y() < ymin) ymin = blk_o.y(); 
    if(blk_o.z() < zmin) zmin = blk_o.z(); 


    //get block max point
    vgl_vector_3d<double>   blk_dim = (iter->second).sub_block_dim_; 
    vgl_vector_3d<unsigned> blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double>   length(blk_dim.x()*blk_num.x(), 
                                   blk_dim.y()*blk_num.y(),
                                   blk_dim.z()*blk_num.z()); 
    vgl_point_3d<double> blk_max = blk_o + length; 
    if(blk_max.x() > xmax) xmax = blk_max.x(); 
    if(blk_max.y() > ymax) ymax = blk_max.y(); 
    if(blk_max.z() > zmax) zmax = blk_max.z(); 
  }
  
  //: Construct from ranges in \a x,y,z (take care with order of inputs).
  //  The \a x range is given by the 1st and 4th coordinates,
  //  the \a y range is given by the 2nd and 5th coordinates,
  //  the \a z range is given by the 3rd and 6th coordinates.
  return vgl_box_3d<double>(xmin, ymin, zmin,
                            xmax, ymax, zmax);
}


//---------------------------------------------------------------------
// NON CLASS FUNCTIONS
//---------------------------------------------------------------------
//------------XML WRITE------------------------------------------------
void x_write(vcl_ostream &os, boxm2_scene& scene, vcl_string name)
{
    //open root tag
    vsl_basic_xml_element scene_elm(name);
    scene_elm.x_write_open(os);

    //write lvcs information
    bgeo_lvcs lvcs = scene.lvcs();
    lvcs.x_write(os, LVCS_TAG);
    x_write(os, scene.local_origin(), LOCAL_ORIGIN_TAG);

    //write scene path for (needs to know where blocks are)
    vcl_string path = scene.data_path();
    vsl_basic_xml_element paths(SCENE_PATHS_TAG);
    paths.add_attribute("path", path);
    paths.x_write(os);
    
    //write block informaiton for each block
    vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene.blocks(); 
    vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter; 
    for(iter = blocks.begin(); iter != blocks.end(); iter++) {
      boxm2_block_id id = iter->first; 
      boxm2_block_metadata data = iter->second; 
      vsl_basic_xml_element block(BLOCK_TAG);
      
      //add block id attribute
      block.add_attribute("id_i", id.i());
      block.add_attribute("id_j", id.j()); 
      block.add_attribute("id_k", id.k()); 
      
      //block local origin
      block.add_attribute("origin_x", data.local_origin_.x()); 
      block.add_attribute("origin_y", data.local_origin_.y()); 
      block.add_attribute("origin_z", data.local_origin_.z()); 

      //sub block dimensions
      block.add_attribute("dim_x", data.sub_block_dim_.x()); 
      block.add_attribute("dim_y", data.sub_block_dim_.y()); 
      block.add_attribute("dim_z", data.sub_block_dim_.z()); 

      //sub block numbers
      block.add_attribute("num_x", (int) data.sub_block_num_.x()); 
      block.add_attribute("num_y", (int) data.sub_block_num_.y()); 
      block.add_attribute("num_z", (int) data.sub_block_num_.z()); 
      
      //block init level
      block.add_attribute("init_level", data.init_level_); 
      
      //block max level
      block.add_attribute("max_level", data.max_level_); 
      
      //block max_mb
      block.add_attribute("max_mb", data.max_mb_);
      
      //block prob init
      block.add_attribute("p_init", data.p_init_); 

      //write tag to stream
      block.x_write(os); 
    }
    
    //clse up tag
    scene_elm.x_write_close(os);
}

//------------IO Stream------------------------------------------------
vcl_ostream& operator <<(vcl_ostream &s, boxm2_scene& scene)
{
    s <<"--- BOXM2_SCENE -----------------------------\n"
      <<"xml_path:         "<<scene.xml_path()<<'\n'
      <<"data_path:        "<<scene.data_path()<<'\n'
      <<"world origin:     "<<scene.rpc_origin()<<'\n';
      //<<"number of blocks: "<<scene.block_num()<<'\n'
      //<<"block dimensions: "<<scene.block_dim()<<'\n'; 
    return s;
}

//: Binary write boxm2_scene to stream obj/pointer
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene const& /*bit_scene*/) {}
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene* const& /*ph*/) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_scene_sptr& sptr) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_scene_sptr const& sptr) {}

//: Binary load boxm scene from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene& /*bit_scene*/) {}
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene* /*ph*/) {}
void vsl_b_read(vsl_b_istream& is, boxm2_scene_sptr& sptr) {}
void vsl_b_read(vsl_b_istream& is, boxm2_scene_sptr const& sptr) {}
//: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_scene_info_wrapper const& scene){}
void vsl_b_write(vsl_b_ostream& os, const boxm2_scene_info_wrapper* &p){}
void vsl_b_write(vsl_b_ostream& os, boxm2_scene_info_wrapper_sptr& sptr){} 
void vsl_b_write(vsl_b_ostream& os, boxm2_scene_info_wrapper_sptr const& sptr){}

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_scene_info_wrapper &scene){}
void vsl_b_read(vsl_b_istream& is, boxm2_scene_info_wrapper* p){}
void vsl_b_read(vsl_b_istream& is, boxm2_scene_info_wrapper_sptr& sptr){}
void vsl_b_read(vsl_b_istream& is, boxm2_scene_info_wrapper_sptr const& sptr){}


