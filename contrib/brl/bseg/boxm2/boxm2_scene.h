#ifndef boxm2_scene_h_
#define boxm2_scene_h_
//:
// \file
// \brief  Boxm2 Scene models a very generic scene, only specifies dimensions
// \author Andrew Miller
// \date   26 Oct 2010
//
#include <iostream>
#include <iosfwd>
#include <vector>
#include <string>
#include <cassert>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/boxm2_block_metadata.h>
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
#include <vpgl/vpgl_affine_camera.h>


//: block info that can be easily made into a buffer and sent to gpu
struct boxm2_scene_info
{
  //world information
  float    scene_origin[4];          // scene origin (point)
  int      scene_dims[4];            // number of blocks in each dimension
  float    block_len;                // size of each block (can only be 1 number now that we've established blocks are cubes)
  float    epsilon;                  // block_len/100.0 (placed here to avoid using a register)
  float    pinit;
  // tree meta information
  int      root_level;               // root_level of trees
  int      num_buffer;               // number of buffers (both data and tree)
  int      tree_buffer_length;       // length of tree buffer (number of cells/trees)
  int      data_buffer_length;       // length of data buffer (number of cells)
};

class boxm2_scene_info_wrapper : public vbl_ref_count
{
  public:
    boxm2_scene_info * info;
};
//Smart_Pointer typedef for boxm2_scene
class boxm2_scene;
typedef vbl_smart_ptr<boxm2_scene> boxm2_scene_sptr;
typedef vbl_smart_ptr<boxm2_scene_info_wrapper> boxm2_scene_info_wrapper_sptr;

//: boxm2_scene: simple scene model that maintains (in world coordinates)
//      - scene origin
//      - number of blocks in each dimension
//      - size of each block in each dimension
//      - lvcs information
//      - xml path on disk and data path (directory) on disk
class boxm2_scene : public vbl_ref_count
{
  public:
    //: empty scene, needs to be initialized manually
    boxm2_scene() {boxm2_scene::get_count()++;}

    boxm2_scene(std::string const& data_path, vgl_point_3d<double> const& origin, int version = 2);

    //: create a scene with a single block
    // scene_dir      - path to scene directory
    // scene_name     - path to xml file is scene_dir + scene_name.xml
    // data_path      - binary data is stored in scene_dir + data_path
    // prefixes       - the set of prefix strings defining the databases stored in the model(appearances)
    // scene_box      - the 3-d bounding box for the scene in global coordinates
    // sub_block_len  - the length of the octree block (sub_block) assumed to be a cube
    // init/max level - the number of levels in the refined tree
    // max_mb         - the maximum storage for a block (not typically used)
    // p_init         - the initial probability of surface occupancy
    // n_illum_bins   - number of illumination bins
    // version        - scene format version
    boxm2_scene(std::string const& scene_dir, std::string const& scene_name, std::string const& data_path, std::vector<std::string> const& prefixes,
                vgl_box_3d<double> const& scene_box, double sub_block_len, int init_level = 1,
                int max_level = 4, double max_mb=1200, double p_init=0.001, int n_illum_bins = 1, int version = 2);

    //: initializes the scene from the buffer that loaded an XML file in.
    // this is added for decoupling from the local filesystem to load the scene
    boxm2_scene(const char* buffer);

    //: initializes scene from xmlFile
    boxm2_scene(std::string const& filename);

    //: copy constructor
    boxm2_scene(boxm2_scene const& other_scene);

    //: destructor
    ~boxm2_scene() override = default;

    //:create an in-memory copy of the scene with unique id
    boxm2_scene_sptr clone_no_disk();

    //: save scene xml file
    void save_scene();

    //: return a vector of block ids in visibility order
    std::vector<boxm2_block_id> get_vis_blocks(vpgl_generic_camera<double>* cam, double dist = -1.0);
    std::vector<boxm2_block_id> get_vis_blocks(vpgl_perspective_camera<double>* cam, double dist = -1.0);
    std::vector<boxm2_block_id> get_vis_blocks(vpgl_affine_camera<double>* cam);
    std::vector<boxm2_block_id> get_vis_blocks(vpgl_camera_double_sptr & cam, double dist = -1.0) {
      if ( cam->type_name() == "vpgl_generic_camera" )
        return this->get_vis_blocks( (vpgl_generic_camera<double>*) cam.ptr(), dist );
      else if ( cam->type_name() == "vpgl_perspective_camera" )
        return this->get_vis_blocks( (vpgl_perspective_camera<double>*) cam.ptr(), dist );
      else if ( cam->type_name() == "vpgl_affine_camera" )
        return this->get_vis_blocks( (vpgl_affine_camera<double>*) cam.ptr() );
      else
        std::cout<<"boxm2_scene::get_vis_blocks doesn't support camera type "<<cam->type_name()<<std::endl;
      //else return empty
      std::vector<boxm2_block_id> empty;
      return empty;
    }
    //: visibility order from point, blocks must intersect with cam box
    std::vector<boxm2_block_id>
    get_vis_order_from_pt(vgl_point_3d<double> const& pt,
                          vgl_box_2d<double> camBox = vgl_box_2d<double>(), double distance=-1.0);
    std::vector<boxm2_block_id> get_vis_blocks_opt(vpgl_perspective_camera<double>* cam, unsigned int ni, unsigned int nj);
    //: visibility order using a ray given by origin and direction vector, the block needs to be in the front direction as given by this ray
    std::vector<boxm2_block_id>
    get_vis_order_from_ray(vgl_point_3d<double> const& origin, vgl_vector_3d<double> const& dir, double distance);

    //: return all blocks with center less than dist from the given point
    std::vector<boxm2_block_id> get_vis_blocks(vgl_point_3d<double> const& pt, double dist);

    //: return a heap pointer to a scene info
    boxm2_scene_info* get_blk_metadata(boxm2_block_id const& id);
    bool block_exists(boxm2_block_id const& id) const { return blocks_.find(id) != blocks_.end(); }
    bool block_on_disk(boxm2_block_id const& id) const { return vul_file::exists( data_path_ + id.to_string() + ".bin"); }
    bool data_on_disk(boxm2_block_id const& id, std::string const& data_type) {
      return vul_file::exists(data_path_ + data_type + "_" + id.to_string() + ".bin");
    }

    //: a list of block metadata...
    std::map<boxm2_block_id, boxm2_block_metadata>& blocks() { return blocks_; }
    unsigned num_blocks() const { return (unsigned) blocks_.size(); }

    float finest_resolution();
    //: mutable reference
    boxm2_block_metadata& get_block_metadata(boxm2_block_id const& id)
    {
        assert(blocks_.find(id) != blocks_.end());
        return blocks_[id];
    }
    //: const so return a copy
    boxm2_block_metadata get_block_metadata_const(boxm2_block_id const& id) const;

    //: return number of trees in block
    int num_trees_in_block(boxm2_block_id const& id) {
      assert(blocks_.find(id) != blocks_.end());
      boxm2_block_metadata& d = blocks_[id];
      return d.sub_block_num_.x() * d.sub_block_num_.y() * d.sub_block_num_.z();
    }

    std::vector<boxm2_block_id> get_block_ids() const;

    //: gets a tight bounding box for the scene
    vgl_box_3d<double>      bounding_box() const;
    //: gets a tight bounding box for the scene
    vgl_box_3d<int>         bounding_box_blk_ids() const;

    //: gets the smallest block index in all dimensions
    void min_block_index(vgl_point_3d<int> &idx,
                         vgl_point_3d<double> &local_origin) const;
    //: gets max block index in all dims
    void max_block_index(vgl_point_3d<int> &idx,
                         vgl_point_3d<double> &local_origin) const;

    // returns the dimesnsion of the scene grid where each grid element is a block
    vgl_vector_3d<unsigned int>   scene_dimensions() const;

    //: If a block contains a 3-d point, set the local coordinates of the point
    bool block_contains(vgl_point_3d<double> const& p, boxm2_block_id const& bid,
                        vgl_point_3d<double>& local_coords) const;

    //: If a scene contains a 3-d point, set the block id, else return false. The local coordinates of the point are also returned
    bool contains(vgl_point_3d<double> const& p, boxm2_block_id& bid,
                  vgl_point_3d<double>& local_coords) const;

    //: scene dimensions accessors
    vgl_point_3d<double>    local_origin()const { return local_origin_; }
    vgl_point_3d<double>    rpc_origin()  const { return rpc_origin_; }
    vpgl_lvcs               lvcs()        const { return lvcs_; }

    //: scene path accessors
    std::string              xml_path()    const { return xml_path_; }
    std::string              data_path()   const { return data_path_; }

    //: appearance model accessor
    std::vector<std::string> appearances()  const { return appearances_; }
    bool has_data_type(std::string const& data_type);
    int num_illumination_bins() const {return num_illumination_bins_;}

    //: scene version number
    int version() const { return version_; }
    void set_version(int v) { version_ = v; }
    //: unique scene id
    unsigned id() const {return id_;}
    //: scene mutators
    void set_local_origin(vgl_point_3d<double> org) { local_origin_ = org; }
    void set_rpc_origin(vgl_point_3d<double> rpc)   { rpc_origin_ = rpc; }
    void set_lvcs(vpgl_lvcs lvcs)                   { lvcs_ = lvcs; }
    void set_blocks(std::map<boxm2_block_id, boxm2_block_metadata> blocks) { blocks_ = blocks; }
    void add_block_metadata(boxm2_block_metadata data);
    void set_appearances(std::vector<std::string> const& appearances){ this->appearances_ = appearances; }
    void set_num_illumination_bins(int num_bins) { this->num_illumination_bins_ = num_bins; }

    //: scene path mutators
    void set_xml_path(std::string const& path)              { xml_path_ = path; }
    void set_data_path(std::string const& path)             { data_path_ = path+"/"; }

  private:
    //: unique scene id
    unsigned id_;
    // count of constructed scenes to generate a unique id
    static unsigned& get_count();
    //: world scene information
    vpgl_lvcs               lvcs_;
    vgl_point_3d<double>    local_origin_;
    vgl_point_3d<double>    rpc_origin_;

    //: location on disk of xml file and data/block files
    std::string data_path_, xml_path_;

    //: list of block meta data available to this scene
    std::map<boxm2_block_id, boxm2_block_metadata> blocks_;

    //: list of appearance models/observation models used by this scene
    std::vector<std::string> appearances_;
    int num_illumination_bins_;
    int version_;

    bool is_block_visible(boxm2_block_metadata & data, vpgl_camera<double> const& cam, unsigned ni, unsigned nj );
};


//: utility class for sorting id's by their distance
class boxm2_dist_id_pair
{
  public:
    boxm2_dist_id_pair(double dist, boxm2_block_id const& id) : dist_(dist), id_(id) {}
    double dist_;
    boxm2_block_id id_;

    inline bool operator < (boxm2_dist_id_pair const& v) const {
      return dist_ < v.dist_;
    }
};

//: scene output stream operator
std::ostream& operator<<(std::ostream &s, boxm2_scene& scene);

//: scene xml write function
void x_write(std::ostream &os, boxm2_scene& scene, std::string const& name);


//--- IO read/write for sptrs--------------------------------------------------
//: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_scene const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_scene* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_scene_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, boxm2_scene_sptr const& sptr);

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_scene &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_scene* p);
void vsl_b_read(vsl_b_istream& is, boxm2_scene_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_scene_sptr const& sptr);

//: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_scene_info_wrapper const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_scene_info_wrapper* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_scene_info_wrapper_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, boxm2_scene_info_wrapper_sptr const& sptr);

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_scene_info_wrapper &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_scene_info_wrapper* p);
void vsl_b_read(vsl_b_istream& is, boxm2_scene_info_wrapper_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_scene_info_wrapper_sptr const& sptr);


#endif // boxm2_scene_h_
