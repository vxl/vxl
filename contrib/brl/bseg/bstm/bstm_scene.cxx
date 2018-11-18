
#include "bstm_scene.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

/* xml includes */
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <bstm/bstm_scene_parser.h>
#include <vpgl/xio/vpgl_xio_lvcs.h>

//vgl includes
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>

//vsph include
#include <vsph/vsph_camera_bounds.h>


bstm_scene::bstm_scene(std::string data_path, vgl_point_3d<double> const& origin, int version)
{
    local_origin_=origin;
    data_path_   = std::move(data_path);
    xml_path_  = data_path_ + "/scene.xml";
    version_ = version;
}

//: initializes Scene from XML file
bstm_scene::bstm_scene(const std::string& filename)
{
    //xml parser
    xml_path_ = filename;
    bstm_scene_parser parser;
    if (filename.size() > 0) {
      std::FILE* xmlFile = std::fopen(filename.c_str(), "r");
      if (!xmlFile) {
        std::cerr << filename.c_str() << " error on opening\n";
        return;
      }
      if (!parser.parseFile(xmlFile)) {
        std::cerr << XML_ErrorString(parser.XML_GetErrorCode()) << " at line "
                 << parser.XML_GetCurrentLineNumber() << '\n';
        return;
      }
      std::fclose(xmlFile);
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
    version_ = parser.version();


}


//: add a block meta data...
void bstm_scene::add_block_metadata(bstm_block_metadata data)
{
  if ( blocks_.find(data.id_) != blocks_.end() )
  {
    std::cout<<"Boxm2 SCENE: Overwriting block metadata for id: "<<data.id_<<std::endl;
  }
  blocks_[data.id_] = data;
}


std::vector<bstm_block_id> bstm_scene::get_block_ids() const
{
  std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  std::vector<bstm_block_id> block_ids;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    block_ids.push_back(iter->first);
  }
  return block_ids;
}

std::vector<bstm_block_id> bstm_scene::get_block_ids(vgl_box_3d<double> bb, float time) const
{
  std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  std::vector<bstm_block_id> block_ids;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {

    vgl_point_3d<double>  blk_o = (iter->second).local_origin_;
    vgl_vector_3d<double>   blk_dim = (iter->second).sub_block_dim_;
    vgl_vector_3d<unsigned> blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double>   length(blk_dim.x()*blk_num.x(),
                                   blk_dim.y()*blk_num.y(),
                                   blk_dim.z()*blk_num.z());
    vgl_box_3d<double> block_bb(blk_o,length.x(),length.y(),length.z(),vgl_box_3d<double>::min_pos);
    double local_time;
    if(!vgl_intersection(bb,block_bb).is_empty() && (iter->second).contains_t(time,local_time) )
      block_ids.push_back(iter->first);
  }
  return block_ids;
}

bstm_block_metadata bstm_scene::
get_block_metadata_const(const bstm_block_id& id) const
{
  std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter)
    if ((*iter).first == id)
      return (*iter).second;
  return bstm_block_metadata();
}

std::vector<bstm_block_id> bstm_scene::get_vis_blocks(vpgl_generic_camera<double>* cam)
{
  std::vector<bstm_block_id> vis_order;
  std::vector<bstm_dist_id_pair> distances;
  if (!cam) {
    std::cout << "null camera in boxm2_scene::get_vis_blocks(.)\n";
    return vis_order;
  }
  std::map<bstm_block_id, bstm_block_metadata>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    vgl_point_3d<double>&    blk_o   = (iter->second).local_origin_;
    vgl_vector_3d<double>&   blk_dim = (iter->second).sub_block_dim_;
    vgl_vector_3d<unsigned>& blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double>    length(blk_dim.x()*blk_num.x(),
                                    blk_dim.y()*blk_num.y(),
                                    blk_dim.z()*blk_num.z());

    double min_depth = 1e10;
    for(unsigned i = 0;  i<=1 ; i++)
        for(unsigned j = 0;  j<=1 ; j++)
            for(unsigned k = 0;  k<=1 ; k++)
            {
                vgl_vector_3d<double>    length(blk_dim.x()*blk_num.x()*(double)i,
                                                blk_dim.y()*blk_num.y()*(double)j,
                                                blk_dim.z()*blk_num.z()*(double)k);
                vgl_point_3d<double> pt = blk_o + length;
                double u,v;
                cam->project(pt.x(),pt.y(),pt.z(),u,v);
                if ( u >= 0 && v >=0 && u < cam->cols() && v <cam->rows() )
                {
                    vgl_point_3d<double> ro =  cam->ray(u,v).origin();
                    double depth = (ro-pt).length();
                    if(depth <  min_depth)
                        min_depth = depth ;
                }
            }
            if (min_depth <1e10)
                distances.emplace_back(min_depth, iter->first );

  }

  //sort distances
  std::sort(distances.begin(), distances.end());

  //put blocks in "vis_order"
  std::vector<bstm_dist_id_pair>::iterator di;
  for (di = distances.begin(); di != distances.end(); ++di)
    vis_order.push_back(di->id_);
  return vis_order;
}

std::vector<bstm_block_id> bstm_scene::get_vis_blocks(vpgl_perspective_camera<double>* cam)
{
  std::vector<bstm_block_id> vis_order;
  if (!cam) {
    std::cout << "null camera in bstm_scene::get_vis_blocks(.)\n";
    return vis_order;
  }

  //---------------------------------------
  //find intersection box
  //---------------------------------------
  vgl_box_3d<double> sceneBB = this->bounding_box();
  vgl_box_2d<double> lowBox, highBox;
  vsph_camera_bounds::planar_bounding_box(*cam, lowBox, sceneBB.min_z());
  vsph_camera_bounds::planar_bounding_box(*cam, highBox, sceneBB.max_z());
  vgl_box_2d<double> camBox;
  camBox.add(lowBox);
  camBox.add(highBox);

  //grab visibility order from camera center
  vgl_point_3d<double> cam_center = cam->camera_center();
  return get_vis_order_from_pt(cam_center, camBox);
}

std::vector<bstm_block_id>
bstm_scene::get_vis_order_from_pt(vgl_point_3d<double> const& pt,
                                  vgl_box_2d<double> camBox)
{
  //Map of distance, id
  std::vector<bstm_block_id> vis_order;
  std::vector<bstm_dist_id_pair> distances;

  //get camera center and order blocks distance from the cam center
  //for non-projective cameras there may not be a single center of projection
  //so instead get the ray origin farthest from the scene origin.
  std::map<bstm_block_id, bstm_block_metadata>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    vgl_point_3d<double>&    blk_o   = (iter->second).local_origin_;
    vgl_vector_3d<double>&   blk_dim = (iter->second).sub_block_dim_;
    vgl_vector_3d<unsigned>& blk_num = (iter->second).sub_block_num_;
    vgl_vector_3d<double>    length(blk_dim.x()*blk_num.x(),
                                    blk_dim.y()*blk_num.y(),
                                    blk_dim.z()*blk_num.z());

    //make sure there is a non empty intersection here
    vgl_box_2d<double> blkBox(blk_o.x(), blk_o.x()+length.x(),
                              blk_o.y(), blk_o.y()+length.y());
    vgl_box_2d<double> intersect = vgl_intersection(camBox, blkBox);

      vgl_point_3d<double> blk_center = blk_o + length/2.0;

      double dist = vgl_distance( blk_center, pt);
      distances.emplace_back(dist, iter->first );

  }

  //sort distances
  std::sort(distances.begin(), distances.end());

  //put blocks in "vis_order"
  std::vector<bstm_dist_id_pair>::iterator di;
  for (di = distances.begin(); di != distances.end(); ++di)
    vis_order.push_back(di->id_);
  return vis_order;
}


//: find the block containing the specified point, else return false
//  Local coordinates are also returned
bool bstm_scene::contains(vgl_point_3d<double> const& p, bstm_block_id& bid,
                          vgl_point_3d<double>& local_coords, double const t, double& local_time) const
{
    std::vector<bstm_block_id> block_ids = this->get_block_ids();
    for (auto & block_id : block_ids)
    {
      bstm_block_metadata md = this->get_block_metadata_const(block_id);
      vgl_vector_3d<double> dims(md.sub_block_dim_.x()*md.sub_block_num_.x(),
                                 md.sub_block_dim_.y()*md.sub_block_num_.y(),
                                 md.sub_block_dim_.z()*md.sub_block_num_.z());

      vgl_point_3d<double> lorigin = md.local_origin_;
      vgl_box_3d<double> bbox(lorigin,lorigin+dims);
      if (bbox.contains(p.x(), p.y(), p.z())) {

        //now check for time
        if (md.contains_t(t,local_time))
        {
          bid = block_id;
          double local_x=(p.x()-md.local_origin_.x())/md.sub_block_dim_.x();
          double local_y=(p.y()-md.local_origin_.y())/md.sub_block_dim_.y();
          double local_z=(p.z()-md.local_origin_.z())/md.sub_block_dim_.z();
          local_coords.set(local_x, local_y, local_z);
          return true;
        }
      }
    }
    return false;
}


//  Local coordinates are also returned
bool bstm_scene::local_time(double const t,  double& local_time) const
{
  std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
    if ((iter->second).contains_t(t,local_time))
      return true;

  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

//: save scene (xml file)
void bstm_scene::save_scene()
{
  //write out to XML file
  std::ofstream xmlstrm(xml_path_.c_str());
  x_write(xmlstrm, (*this), "scene");
  xmlstrm.close();
}

void bstm_scene::bounding_box_t(double& min_t, double& max_t) const
{
  min_t = 10e10; max_t=-10e10;

  std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter)
  {
    double blk_min, blk_max;
    (iter->second).bbox_t(blk_min,blk_max);
    if (blk_min < min_t)
      min_t = blk_min;

    if (blk_max > max_t)
      max_t = blk_max;
  }
}

//: gets a tight bounding box of the block ids
void  bstm_scene::blocks_ids_bounding_box_t(unsigned& min_block_id, unsigned& max_block_id) const
{
  min_block_id = std::numeric_limits<unsigned>::max() ;
  max_block_id = std::numeric_limits<unsigned>::min() ;

  std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter)
  {
    if (iter->first.t_ < (int)min_block_id)
      min_block_id = iter->first.t_;

    if (iter->first.t_ > (int)max_block_id)
      max_block_id = iter->first.t_;
  }
}


vgl_box_3d<double> bstm_scene::bounding_box() const
{
  double xmin=10e10, xmax=-10e10;
  double ymin=10e10, ymax=-10e10;
  double zmin=10e10, zmax=-10e10;

  //iterate through each block
  std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
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
  return {xmin, ymin, zmin,
                            xmax, ymax, zmax};
}

vgl_box_3d<int> bstm_scene::bounding_box_blk_ids() const
{
  vgl_box_3d<int> bbox;
  //iterate through each block
  std::map<bstm_block_id, bstm_block_metadata>::const_iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); ++iter)
    bbox.add(vgl_point_3d<int> ( iter->first.i(),iter->first.j(), iter->first.k()) ) ;

  return bbox;
}

vgl_vector_3d<unsigned int>  bstm_scene::scene_dimensions() const
{
  std::vector<bstm_block_id> ids = this->get_block_ids();

  if (ids.empty())
    return {0,0,0};

  int max_i=ids[0].i(),max_j=ids[0].j(),max_k=ids[0].k();
  int min_i=ids[0].i(),min_j=ids[0].j(),min_k=ids[0].k();

  for (auto & id : ids) {
    if (id.i() > max_i)
      max_i=id.i();
    if (id.j() > max_j)
      max_j=id.j();
    if (id.k() > max_k)
      max_k=id.k();

    if (id.i() < min_i)
      min_i=id.i();
    if (id.j() < min_j)
      min_j=id.j();
    if (id.k() < min_k)
      min_k=id.k();
  }
  max_i++; max_j++; max_k++;

  return {static_cast<unsigned int>((max_i-min_i)),static_cast<unsigned int>((max_j - min_j)),static_cast<unsigned int>((max_k-min_k))};
}

//: returns true if the scene has specified data type (simple linear search)
bool bstm_scene::has_data_type(const std::string& data_type)
{
  for (const auto & appearance : appearances_)
    if ( appearance == data_type )
      return true;
  return false;
}

//---------------------------------------------------------------------
// NON CLASS FUNCTIONS
//---------------------------------------------------------------------
//------------XML WRITE------------------------------------------------
void x_write(std::ostream &os, bstm_scene& scene, std::string name)
{
  //open root tag
  vsl_basic_xml_element scene_elm(std::move(name));
  scene_elm.x_write_open(os);

  //write lvcs information
  vpgl_lvcs lvcs = scene.lvcs();
  x_write(os, lvcs, LVCS_TAG);
  x_write(os, scene.local_origin(), LOCAL_ORIGIN_TAG);

  //write scene path for (needs to know where blocks are)
  std::string path = scene.data_path();
  vsl_basic_xml_element paths(SCENE_PATHS_TAG);
  paths.add_attribute("path", path + '/');
  paths.x_write(os);

  vsl_basic_xml_element ver(VERSION_TAG);
  ver.add_attribute("number", scene.version());
  ver.x_write(os);

  //write list of appearance models

  std::vector<std::string> apps = scene.appearances();
  for (const auto & app : apps)
  {
    vsl_basic_xml_element apms(APM_TAG);
    apms.add_attribute("apm", app);
    apms.x_write(os);
  }

  //write block information for each block
  std::map<bstm_block_id, bstm_block_metadata> blocks = scene.blocks();
  std::map<bstm_block_id, bstm_block_metadata>::iterator iter;
  for (iter = blocks.begin(); iter != blocks.end(); ++iter) {
    bstm_block_id id = iter->first;
    bstm_block_metadata data = iter->second;
    vsl_basic_xml_element block(BLOCK_TAG);

    //add block id attribute
    block.add_attribute("id_i", id.i());
    block.add_attribute("id_j", id.j());
    block.add_attribute("id_k", id.k());
    block.add_attribute("id_t", id.t());

    //block local origin
    block.add_attribute("origin_x", data.local_origin_.x());
    block.add_attribute("origin_y", data.local_origin_.y());
    block.add_attribute("origin_z", data.local_origin_.z());
    block.add_attribute("origin_t", data.local_origin_t_ );

    //sub block dimensions
    block.add_attribute("dim_x", data.sub_block_dim_.x());
    block.add_attribute("dim_y", data.sub_block_dim_.y());
    block.add_attribute("dim_z", data.sub_block_dim_.z());
    block.add_attribute("dim_t", data.sub_block_dim_t_);

    //sub block numbers
    block.add_attribute("num_x", (int) data.sub_block_num_.x());
    block.add_attribute("num_y", (int) data.sub_block_num_.y());
    block.add_attribute("num_z", (int) data.sub_block_num_.z());
    block.add_attribute("num_t", (int) data.sub_block_num_t_);

    //block init level
    block.add_attribute("init_level", data.init_level_);
    block.add_attribute("init_level_t", data.init_level_t_);

    //block max level
    block.add_attribute("max_level", data.max_level_);
    block.add_attribute("max_level_t", data.max_level_t_);

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
std::ostream& operator <<(std::ostream &s, bstm_scene& scene)
{
  s <<"--- bstm_scene -----------------------------\n"
    <<"xml_path:         "<<scene.xml_path()<<'\n'
    <<"data_path:        "<<scene.data_path()<<'\n'
    <<"world origin:     "<<scene.rpc_origin()<<'\n'
    <<"list of APMs:     "<<'\n';

  //list appearance models for this scene
  std::vector<std::string> apps = scene.appearances();
  for (const auto & app : apps)
    s << "    " << app << ", ";
  s << '\n';
  vpgl_lvcs lvcs = scene.lvcs();
  s << lvcs << '\n';

  vgl_box_3d<double> bb = scene.bounding_box();
  vgl_point_3d<double> minp = bb.min_point();
  vgl_point_3d<double> maxp = bb.max_point();
  s << "bounds : ( " << minp.x() << ' ' << minp.y() << ' ' << minp.z()
    << " )==>( " << maxp.x() << ' ' << maxp.y() << ' ' << maxp.z() << " )\n";

  //list of block ids for this scene....
  vgl_vector_3d<unsigned> dims = scene.scene_dimensions();
  s << "block array dims(" << dims.x() << ' ' << dims.y() << ' ' << dims.z() << ")\n";
  std::map<bstm_block_id, bstm_block_metadata>& blk = scene.blocks();
  s << " blocks:==>\n";
  for (auto & bit : blk) {
    s << bit.second.id_ << ' ';
    vgl_point_3d<double> org = bit.second.local_origin_;
    s << ", org( " << org.x() << ' ' << org.y() << ' ' << org.z() << ' ' << bit.second.local_origin_t_  << ") ";
    vgl_vector_3d<double> dim = bit.second.sub_block_dim_;
    s << ", dim( " << dim.x() << ' ' << dim.y() << ' ' << dim.z() << ' ' << bit.second.sub_block_dim_t_ << ") ";
    vgl_vector_3d<unsigned> num = bit.second.sub_block_num_;
    s << ", num( " << num.x() << ' ' << num.y() << ' ' << num.z()  << ' ' << bit.second.sub_block_num_t_ << ")\n";
  }
  s << "<=====:end blocks\n";
  return s;
}


//: Binary write bstm_scene to stream
void vsl_b_write(vsl_b_ostream& /*os*/, bstm_scene const& /*bit_scene*/) {}
//: Binary write bstm_scene pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, bstm_scene* const& /*ph*/) {}
//: Binary write bstm_scene smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, bstm_scene_sptr&) {}
//: Binary write bstm_scene smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, bstm_scene_sptr const&) {}

//: Binary load boxm scene from stream.
void vsl_b_read(vsl_b_istream& /*is*/, bstm_scene& /*bit_scene*/) {}
//: Binary load boxm scene pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, bstm_scene* /*ph*/) {}
//: Binary load boxm scene smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, bstm_scene_sptr&) {}
//: Binary load boxm scene smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, bstm_scene_sptr const&) {}
