#ifndef bstm_scene_h_
#define bstm_scene_h_
//:
// \file
// \brief  bstm scene models a very generic (dynamic) scene, only specifies dimensions
// \author Ali Osman Ulusoy
// \date   07 Aug 2012
//
#include <iostream>
#include <iosfwd>
#include <bstm/basic/bstm_block_id.h>
#include <bstm/bstm_block_metadata.h>
#include <vpgl/vpgl_lvcs.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_box_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>

//smart pointer stuff
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//vpgl camera
#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

//: bstm_scene_scene: simple scene model that maintains (in world coordinates)
//      - scene origin
//      - number of blocks in each dimension
//      - size of each block in each dimension
//      - lvcs information
//      - xml path on disk and data path (directory) on disk
class bstm_scene : public vbl_ref_count
{
  public:
    //: empty scene, needs to be initialized manually
    bstm_scene() = default;

    bstm_scene(std::string data_path, vgl_point_3d<double> const& origin, int version = 2);

    //: initializes scene from xmlFile
    bstm_scene(const std::string& filename);

    //: destructor
    ~bstm_scene() override = default;

    //: save scene xml file
    void save_scene();

    //: return a vector of block ids in visibility order
    std::vector<bstm_block_id> get_vis_blocks(vpgl_generic_camera<double>* cam);
    std::vector<bstm_block_id> get_vis_blocks(vpgl_perspective_camera<double>* cam);
    std::vector<bstm_block_id> get_vis_blocks(vpgl_camera_double_sptr & cam) {
      if ( cam->type_name() == "vpgl_generic_camera" )
        return this->get_vis_blocks( (vpgl_generic_camera<double>*) cam.ptr() );
      else if ( cam->type_name() == "vpgl_perspective_camera" )
        return this->get_vis_blocks( (vpgl_perspective_camera<double>*) cam.ptr() );
      else
        std::cout<<"bstm_scene::get_vis_blocks doesn't support camera type "<<cam->type_name()<<std::endl;
      //else return empty
      std::vector<bstm_block_id> empty;
      return empty;
    }
    //: visibility order from point, blocks must intersect with cam box
    std::vector<bstm_block_id>
    get_vis_order_from_pt(vgl_point_3d<double> const& pt, vgl_box_2d<double> camBox = vgl_box_2d<double>());

    //: return a heap pointer to a scene info
    bool block_exists(bstm_block_id id) const { return blocks_.find(id) != blocks_.end(); }
    bool block_on_disk(bstm_block_id id) const { return vul_file::exists( data_path_ + id.to_string() + ".bin"); }
    bool data_on_disk(bstm_block_id id, std::string data_type) {
      return vul_file::exists(data_path_ + data_type + "_" + id.to_string() + ".bin");
    }

    //: a list of block metadata...
    std::map<bstm_block_id, bstm_block_metadata>& blocks() { return blocks_; }
    unsigned num_blocks() const { return (unsigned) blocks_.size(); }

    //: mutable reference
    bstm_block_metadata& get_block_metadata(bstm_block_id id) { return blocks_[id]; }
    //: const so return a copy
    bstm_block_metadata get_block_metadata_const(const bstm_block_id& id) const;


    std::vector<bstm_block_id> get_block_ids() const;

    //: returns the block ids of blocks that intersect the given bounding box at given time, as well as the local time
    std::vector<bstm_block_id> get_block_ids(vgl_box_3d<double> bb, float time) const;


    //: gets a tight bounding box for the scene
    vgl_box_3d<double>      bounding_box() const;
    //: gets a tight bounding box for the scene
    vgl_box_3d<int>         bounding_box_blk_ids() const;

    //: gets a tight bounding box for the scene
    void      bounding_box_t(double& min_t, double& max_t) const;

    //: gets a tight bounding box of the block ids
    void      blocks_ids_bounding_box_t(unsigned& min_block_id, unsigned& max_block_id) const;

    // returns the dimesnsion of the scene grid where each grid element is a block
    vgl_vector_3d<unsigned int>   scene_dimensions() const;

    //: If a block contains a 3-d point, set the block id, else return false. The local coordinates of the point are also returned
    bool contains(vgl_point_3d<double> const& p, bstm_block_id& bid, vgl_point_3d<double>& local_coords, double const t, double& local_time) const;

    //: returns the local time if t is contained in scene
    bool local_time(double const t, double& local_time) const;


    //: scene dimensions accessors
    vgl_point_3d<double>    local_origin()const { return local_origin_; }
    vgl_point_3d<double>    rpc_origin()  const { return rpc_origin_; }
    vpgl_lvcs               lvcs()        const { return lvcs_; }

    //: scene path accessors
    std::string              xml_path()    const { return xml_path_; }
    std::string              data_path()   const { return data_path_; }

    //: appearance model accessor
    std::vector<std::string> appearances()  const { return appearances_; }
    bool has_data_type(const std::string& data_type);

    //: scene version number
    int version() { return version_; }
    void set_version(int v) { version_ = v; }

    //: scene mutators
    void set_local_origin(vgl_point_3d<double> org) { local_origin_ = org; }
    void set_rpc_origin(vgl_point_3d<double> rpc)   { rpc_origin_ = rpc; }
    void set_lvcs(vpgl_lvcs lvcs)                   { lvcs_ = lvcs; }
    void set_blocks(std::map<bstm_block_id, bstm_block_metadata> blocks) { blocks_ = blocks; }
    void add_block_metadata(bstm_block_metadata data);
    void set_appearances(std::vector<std::string> const& appearances){ this->appearances_ = appearances; }

    //: scene path mutators
    void set_xml_path(std::string path)              { xml_path_ = path; }
    void set_data_path(std::string path)             { data_path_ = path+"/"; }

  private:

    //: world scene information
    vpgl_lvcs               lvcs_;
    vgl_point_3d<double>    local_origin_;
    vgl_point_3d<double>    rpc_origin_;

    //: location on disk of xml file and data/block files
    std::string data_path_, xml_path_;

    //: list of block meta data available to this scene
    std::map<bstm_block_id, bstm_block_metadata> blocks_;

    //: list of appearance models/observation models used by this scene
    std::vector<std::string> appearances_;
    int version_;
};


//: utility class for sorting id's by their distance
class bstm_dist_id_pair
{
  public:
  bstm_dist_id_pair(double dist, bstm_block_id id) : dist_(dist), id_(id) {}
    double dist_;
    bstm_block_id id_;

    inline bool operator < (bstm_dist_id_pair const& v) const {
      return dist_ < v.dist_;
    }
};

typedef vbl_smart_ptr<bstm_scene> bstm_scene_sptr;

//: scene output stream operator
std::ostream& operator<<(std::ostream &s, bstm_scene& scene);

//: scene xml write function
void x_write(std::ostream &os, bstm_scene& scene, std::string name);


//--- IO read/write for sptrs--------------------------------------------------
//: Binary write bstm_scene scene to stream
void vsl_b_write(vsl_b_ostream& os, bstm_scene const& scene);
void vsl_b_write(vsl_b_ostream& os, const bstm_scene* &p);
void vsl_b_write(vsl_b_ostream& os, bstm_scene_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, bstm_scene_sptr const& sptr);

//: Binary load bstm_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, bstm_scene &scene);
void vsl_b_read(vsl_b_istream& is, bstm_scene* p);
void vsl_b_read(vsl_b_istream& is, bstm_scene_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, bstm_scene_sptr const& sptr);


#endif // bstm_scene_h_
