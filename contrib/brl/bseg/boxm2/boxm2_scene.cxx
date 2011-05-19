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
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_distance.h>


boxm2_scene::boxm2_scene(vcl_string data_path, vgl_point_3d<double> origin)
{
    local_origin_=origin;
    data_path_   = data_path;
    xml_path_  = data_path_ + "/scene.xml";
    vcl_cout<<"PATH "<<xml_path_<<vcl_endl;
    num_illumination_bins_ = -1;
}


//: initializes Scene from XML file
boxm2_scene::boxm2_scene(vcl_string filename)
{
    //xml parser
    xml_path_ = filename;
    boxm2_scene_parser parser;
    if (filename.size() > 0) {
      vcl_FILE* xmlFile = vcl_fopen(filename.c_str(), "r");
      if (!xmlFile) {
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

    //store list of appearances
    appearances_ = parser.appearances();
    num_illumination_bins_ = parser.num_illumination_bins();
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

vcl_vector<boxm2_block_id> boxm2_scene::get_block_ids() const
{
  vcl_map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter;
  vcl_vector<boxm2_block_id> block_ids;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    block_ids.push_back(iter->first);
  }
  return block_ids;
}

boxm2_block_metadata boxm2_scene::
get_block_metadata_const(boxm2_block_id id) const
{
  vcl_map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter)
    if ((*iter).first == id)
      return (*iter).second;
  return boxm2_block_metadata();
}


vcl_vector<boxm2_block_id> boxm2_scene::get_vis_blocks(vpgl_generic_camera<double>* cam)
{
  vcl_vector<boxm2_block_id> vis_order;
  if (!cam) {
    vcl_cout << "null camera in boxm2_scene::get_vis_blocks(.)\n";
    return vis_order;
  }
  //get camera center and order blocks distance from the cam center
  //for non-projective cameras there may not be a single center of projection
  //so instead get the ray origin farthest from the scene origin.
  vgl_point_3d<double> cam_center = cam->max_ray_origin();
  //Map of distance, id
  vcl_vector<boxm2_dist_id_pair> distances;

  //iterate through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
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
  //vcl_cout<<"CAM ORDER----------------------------------------"<<vcl_endl;
  vcl_vector<boxm2_dist_id_pair>::iterator di;
  for (di = distances.begin(); di != distances.end(); ++di) {
    vis_order.push_back(di->id_);
    //vcl_cout<<di->id_<<'('<<di->dist_<<")    ";
  }
  //vcl_cout<<"\n-----------------------------------------------"<<vcl_endl;
  return vis_order;
}

vcl_vector<boxm2_block_id> boxm2_scene::get_vis_blocks(vpgl_perspective_camera<double>* cam)
{
  vcl_vector<boxm2_block_id> vis_order;
  if (!cam) {
    vcl_cout << "null camera in boxm2_scene::get_vis_blocks(.)\n";
    return vis_order;
  }
  //get camera center and order blocks distance from the cam center
  //for non-projective cameras there may not be a single center of projection
  //so instead get the ray origin farthest from the scene origin.
  vgl_point_3d<double> cam_center = cam->camera_center();
  //Map of distance, id
  vcl_vector<boxm2_dist_id_pair> distances;

  //iterate through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
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
  //vcl_cout<<"CAM ORDER----------------------------------------"<<vcl_endl;
  vcl_vector<boxm2_dist_id_pair>::iterator di;
  for (di = distances.begin(); di != distances.end(); ++di) {
    vis_order.push_back(di->id_);
    //vcl_cout<<di->id_<<'('<<di->dist_<<")    ";
  }
  //vcl_cout<<"\n-----------------------------------------------"<<vcl_endl;
  return vis_order;
}

//: find the block containing the specified point, else return false
//  local coordinates are also returned
bool boxm2_scene::contains(vgl_point_3d<double> const& p, boxm2_block_id& bid,
                           vgl_point_3d<double>& local_coords) const
{
    vcl_vector<boxm2_block_id> block_ids = this->get_block_ids();
    for (vcl_vector<boxm2_block_id>::iterator id = block_ids.begin();
         id != block_ids.end(); ++id)
    {
      boxm2_block_metadata md = this->get_block_metadata_const(*id);
      vgl_vector_3d<double> dims(md.sub_block_dim_.x()*md.sub_block_num_.x(),
                                 md.sub_block_dim_.y()*md.sub_block_num_.y(),
                                 md.sub_block_dim_.z()*md.sub_block_num_.z());

      vgl_point_3d<double> lorigin = md.local_origin_;
      vgl_box_3d<double> bbox(lorigin,lorigin+dims);
      if (bbox.contains(p.x(), p.y(), p.z())) {
        bid = (*id);
        double local_x=(p.x()-md.local_origin_.x())/md.sub_block_dim_.x();
        double local_y=(p.y()-md.local_origin_.y())/md.sub_block_dim_.y();
        double local_z=(p.z()-md.local_origin_.z())/md.sub_block_dim_.z();
        local_coords.set(local_x, local_y, local_z);
        return true;
      }
    }
    return false;
}

//: save scene (xml file)
void boxm2_scene::save_scene()
{
  //write out to XML file
  vcl_ofstream xmlstrm(xml_path_.c_str());
  x_write(xmlstrm, (*this), "scene");
  xmlstrm.close();
}

//: return a heap pointer to a scene info
boxm2_scene_info* boxm2_scene::get_blk_metadata(boxm2_block_id id)
{
  if ( blocks_.find(id) == blocks_.end() )
  {
    vcl_cerr<<"\nboxm2_scene::get_blk_metadata: Block doesn't exist: "<<id<<"\n\n";
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


vgl_box_3d<double> boxm2_scene::bounding_box() const
{
  double xmin=10e10, xmax=-10e10;
  double ymin=10e10, ymax=-10e10;
  double zmin=10e10, zmax=-10e10;

  //iterate through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter)
  {
    //determine xmin, ymin, zmin using block_o
    vgl_point_3d<double>  blk_o = (iter->second).local_origin_;
    if (blk_o.x() < xmin) xmin = blk_o.x();
    if (blk_o.y() < ymin) ymin = blk_o.y();
    if (blk_o.z() < zmin) zmin = blk_o.z();

    //get block max point
    vgl_vector_3d<double>   blk_dim = (iter->second).sub_block_dim_;
    vgl_vector_3d<unsigned> blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double>   length(blk_dim.x()*blk_num.x(),
                                   blk_dim.y()*blk_num.y(),
                                   blk_dim.z()*blk_num.z());
    vgl_point_3d<double> blk_max = blk_o + length;
    if (blk_max.x() > xmax) xmax = blk_max.x();
    if (blk_max.y() > ymax) ymax = blk_max.y();
    if (blk_max.z() > zmax) zmax = blk_max.z();
  }

  //: Construct from ranges in \a x,y,z (take care with order of inputs).
  //  The \a x range is given by the 1st and 4th coordinates,
  //  the \a y range is given by the 2nd and 5th coordinates,
  //  the \a z range is given by the 3rd and 6th coordinates.
  return vgl_box_3d<double>(xmin, ymin, zmin,
                            xmax, ymax, zmax);
}

vgl_vector_3d<unsigned int>  boxm2_scene::scene_dimensions() const
{
  vcl_vector<boxm2_block_id> ids = this->get_block_ids();

  if (ids.empty())
    return vgl_vector_3d<unsigned int>(0,0,0);

  int max_i=ids[0].i(),max_j=ids[0].j(),max_k=ids[0].k();
  int min_i=ids[0].i(),min_j=ids[0].j(),min_k=ids[0].k();

  for (unsigned n=0; n<ids.size(); n++) {
    if (ids[n].i() > max_i)
      max_i=ids[n].i();
    if (ids[n].j() > max_j)
      max_j=ids[n].j();
    if (ids[n].k() > max_k)
      max_k=ids[n].k();

    if (ids[n].i() < min_i)
      min_i=ids[n].i();
    if (ids[n].j() < min_j)
      min_j=ids[n].j();
    if (ids[n].k() < min_k)
      min_k=ids[n].k();
  }
  max_i++; max_j++; max_k++;

  return vgl_vector_3d<unsigned int>((max_i-min_i),(max_j - min_j),(max_k-min_k));
}

//: gets the smallest block index
void boxm2_scene::min_block_index (vgl_point_3d<int> &idx,
                                   vgl_point_3d<double> &local_origin)
{
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter= blocks_.begin();

  boxm2_block_id id = iter->first;
  boxm2_block_metadata data = iter->second;

  int min_i=id.i(),min_j=id.j(),min_k=id.k();
  double min_x = data.local_origin_.x(), min_y= data.local_origin_.y(), min_z= data.local_origin_.z();

  for (; iter != blocks_.end(); ++iter) {
    id = iter->first;
    data = iter->second;

    if (id.i() < min_i) {
      min_i=id.i();
      min_x = data.local_origin_.x();
    }
    if (id.j() < min_j) {
      min_j=id.j();
      min_y = data.local_origin_.y();
    }
    if (id.k() < min_k) {
      min_k=id.k();
      min_z = data.local_origin_.z();
    }
  }

  idx.set(min_i,min_j,min_k);
  local_origin.set(min_x, min_y, min_z);
}


//: returns true if the scene has specified data type (simple linear search)
bool boxm2_scene::has_data_type(vcl_string data_type)
{
  for (unsigned int i=0; i<appearances_.size(); ++i)
    if ( appearances_[i] == data_type )
      return true;
  return false;
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
  paths.add_attribute("path", path + '/');
  paths.x_write(os);

  //write list of appearance models

  vcl_vector<vcl_string> apps = scene.appearances();
  for (unsigned int i=0; i<apps.size(); ++i)
  {
    vsl_basic_xml_element apms(APM_TAG);
    apms.add_attribute("apm", apps[i]);
    apms.x_write(os);
  }

  //write block information for each block
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene.blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  for (iter = blocks.begin(); iter != blocks.end(); ++iter) {
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

          //block prob init
    block.add_attribute("random", 0);

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
    <<"world origin:     "<<scene.rpc_origin()<<'\n'
    <<"list of APMs:     "<<'\n';

  //list appearance models for this scene
  vcl_vector<vcl_string> apps = scene.appearances();
  for (unsigned int i=0; i<apps.size(); ++i)
    s << "    " << apps[i] << ", ";
  s << '\n';
  bgeo_lvcs lvcs = scene.lvcs();
  s << lvcs << '\n';

  vgl_box_3d<double> bb = scene.bounding_box();
  vgl_point_3d<double> minp = bb.min_point();
  vgl_point_3d<double> maxp = bb.max_point();
  s << "bounds : ( " << minp.x() << ' ' << minp.y() << ' ' << minp.z()
    << " )==>( " << maxp.x() << ' ' << maxp.y() << ' ' << maxp.z() << " )\n";

  //list of block ids for this scene....
  vgl_vector_3d<unsigned> dims = scene.scene_dimensions();
  s << "block array dims(" << dims.x() << ' ' << dims.y() << ' ' << dims.z() << ")\n";
  vcl_map<boxm2_block_id, boxm2_block_metadata>& blk = scene.blocks();
  s << " blocks:==>\n";
  for (vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator bit=blk.begin();
       bit != blk.end(); ++bit) {
    s << (*bit).second.id_ << ' ';
    vgl_point_3d<double> org = (*bit).second.local_origin_;
    s << ", org( " << org.x() << ' ' << org.y() << ' ' << org.z() << ") ";
    vgl_vector_3d<double> dim = (*bit).second.sub_block_dim_;
    s << ", dim( " << dim.x() << ' ' << dim.y() << ' ' << dim.z() << ") ";
    vgl_vector_3d<unsigned> num = (*bit).second.sub_block_num_;
    s << ", num( " << num.x() << ' ' << num.y() << ' ' << num.z() << ")\n";
  }
  s << "<=====:end blocks\n";
  return s;
}

//: Binary write boxm2_scene to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene const& /*bit_scene*/) {}
//: Binary write boxm2_scene pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene* const& /*ph*/) {}
//: Binary write boxm2_scene smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene_sptr&) {}
//: Binary write boxm2_scene smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene_sptr const&) {}

//: Binary load boxm scene from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene& /*bit_scene*/) {}
//: Binary load boxm scene pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene* /*ph*/) {}
//: Binary load boxm scene smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_sptr&) {}
//: Binary load boxm scene smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_sptr const&) {}

//: Binary write boxm2_scene_info_wrapper to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene_info_wrapper const&) {}
//: Binary write boxm2_scene_info_wrapper pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, const boxm2_scene_info_wrapper* &) {}
//: Binary write boxm2_scene_info_wrapper smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene_info_wrapper_sptr&) {}
//: Binary write boxm2_scene_info_wrapper smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm2_scene_info_wrapper_sptr const&) {}

//: Binary load boxm2_scene_info_wrapper from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_info_wrapper &) {}
//: Binary load boxm2_scene_info_wrapper pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_info_wrapper*) {}
//: Binary load boxm2_scene_info_wrapper smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_info_wrapper_sptr&) {}
//: Binary load boxm2_scene_info_wrapper smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm2_scene_info_wrapper_sptr const&) {}

