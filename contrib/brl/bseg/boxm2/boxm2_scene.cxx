#include "boxm2_scene.h"
//:
// \file
#include <vcl_iostream.h>
#include <vcl_string.h>

/* xml includes */
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <boxm2/boxm2_scene_parser.h>    


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
    vcl_string dir, pref;
    parser.paths(dir, pref);
    data_path_ = dir;
    
    //lvcs, origin, block dimension
    parser.lvcs(lvcs_);
    local_origin_ = parser.origin();
    rpc_origin_   = parser.origin();
    block_dim_    = parser.block_dim();
    block_num_    = parser.block_nums();
}


//: save scene (xml file)
void boxm2_scene::save_scene()
{
  //write out to XML file
  vcl_ofstream xmlstrm(xml_path_.c_str());
  x_write(xmlstrm, (*this), "scene");
}

//: return a heap pointer to a scene info 
boxm2_scene_info* boxm2_scene::get_scene_info()
{
  boxm2_scene_info* info = new boxm2_scene_info(); 
  
  //world information
  info->scene_origin[0] = (cl_float) (local_origin_.x()); 
  info->scene_origin[1] = (cl_float) (local_origin_.y()); 
  info->scene_origin[2] = (cl_float) (local_origin_.z());  
  info->scene_origin[3] = 0.0f;
  
  info->scene_dims[0] = (cl_int) (block_num_.x());  // number of blocks in each dimension
  info->scene_dims[1] = (cl_int) (block_num_.y());            
  info->scene_dims[2] = (cl_int) (block_num_.z());            
  info->scene_dims[3] = 0;

  info->block_len = (cl_float) (block_dim_.x()); 
  info->epsilon   = (cl_float) (info->block_len / 100.0f); 
  
  info->root_level = 3; 
  info->num_buffer = 0; 
  info->tree_buffer_length = 0; 
  info->data_buffer_length = 0; 
  
  return info; 
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

    //write block numbers for x,y,z
    vsl_basic_xml_element blocks(BLOCK_NUM_TAG);
    vgl_vector_3d<unsigned> nums = scene.block_num();
    blocks.add_attribute("x_dimension", (int) (nums.x()));
    blocks.add_attribute("y_dimension", (int) (nums.y()));
    blocks.add_attribute("z_dimension", (int) (nums.z()));
    blocks.x_write(os);

    //write block dimensions for each
    vsl_basic_xml_element bnum(BLOCK_DIMENSIONS_TAG);
    vgl_vector_3d<double> dims = scene.block_dim();
    bnum.add_attribute("x", dims.x());
    bnum.add_attribute("y", dims.y());
    bnum.add_attribute("z", dims.z());
    bnum.x_write(os);

    //write scene path for (needs to know where blocks are)
    vcl_string path = scene.data_path();
    vsl_basic_xml_element paths(SCENE_PATHS_TAG);
    paths.add_attribute("path", path);
    paths.x_write(os);

    //Not specifying octree levels here - octree levels should be specified
    //at the block level... 
    //write octree levels tag
    //vsl_basic_xml_element tree(OCTREE_LEVELS_TAG);
    //tree.add_attribute("max", (int) scene.max_level());
    //tree.add_attribute("init", (int) scene.init_level());
    //tree.x_write(os);

    //write max MB for scene
    //vsl_basic_xml_element max_mb(MAX_MB_TAG);
    //max_mb.add_attribute("mb", (int) scene.max_mb());
    //max_mb.x_write(os);

    //write p_init for scene
    //vsl_basic_xml_element pinit(P_INIT_TAG);
    //pinit.add_attribute("val", (float) scene.pinit());
    //pinit.x_write(os);

    scene_elm.x_write_close(os);
}

//------------IO Stream------------------------------------------------
vcl_ostream& operator <<(vcl_ostream &s, boxm2_scene& scene)
{
    s <<"--- BOXM2_SCENE -----------------------------\n"
      <<"xml_path:         "<<scene.xml_path()<<'\n'
      <<"data_path:        "<<scene.data_path()<<'\n'
      <<"world origin:     "<<scene.rpc_origin()<<'\n'
      <<"number of blocks: "<<scene.block_num()<<'\n'
      <<"block dimensions: "<<scene.block_dim()<<'\n'; 
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
