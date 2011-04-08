#ifndef boxm2_scene_h_
#define boxm2_scene_h_
//:
// \file
// \brief  Boxm2 Scene models a very generic scene, only specifies dimensions
// \author Andrew Miller
// \date   26 Oct 2010
//
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/boxm2_block_metadata.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vcl_iosfwd.h>
#include <vul/vul_file.h>

//smart pointer stuff
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//vpgl camera
#include <vpgl/vpgl_perspective_camera.h>


//: block info that can be easily made into a buffer and sent to gpu
struct boxm2_scene_info
{
  //world information
  float    scene_origin[4];          // scene origin (point)
  int      scene_dims[4];            // number of blocks in each dimension
  float    block_len;                // size of each block (can only be 1 number now that we've established blocks are cubes)
  float    epsilon;                  // block_len/100.0 (placed here to avoid using a register)

  // tree meta information
  int      root_level;               // root_level of trees
  int      num_buffer;               // number of buffers (both data and tree)
  int      tree_buffer_length;       // length of tree buffer (number of cells/trees)
  int      data_buffer_length;       // length of data buffer (number of cells)
};

class boxm2_scene_info_wrapper:public vbl_ref_count
{
  public:
    boxm2_scene_info * info;
};

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
    boxm2_scene() {}

    boxm2_scene(vcl_string data_path, vgl_point_3d<double> origin);

    //: initializes scene from xmlFile
    boxm2_scene(vcl_string filename);

    //: destructor
    ~boxm2_scene() { }

    //: save scene xml file
    void save_scene();

    //: return a vector of block ids in visibility order
    vcl_vector<boxm2_block_id> get_vis_blocks(vpgl_perspective_camera<double>* cam);

    //: return a heap pointer to a scene info
    boxm2_scene_info* get_blk_metadata(boxm2_block_id id);
    bool block_exists(boxm2_block_id id) { return blocks_.find(id) != blocks_.end(); }
    bool block_on_disk(boxm2_block_id id) { return vul_file::exists( data_path_ + id.to_string() + ".bin"); }
    bool data_on_disk(boxm2_block_id id, vcl_string data_type) {
      return vul_file::exists(data_path_ + data_type + "_" + id.to_string() + ".bin");
    }

    //: a list of block metadata...
    vcl_map<boxm2_block_id, boxm2_block_metadata>& blocks() { return blocks_; }
    unsigned num_blocks() { return (unsigned) blocks_.size(); }
    boxm2_block_metadata get_block_metadata(boxm2_block_id id) { return blocks_[id]; }
    vcl_vector<boxm2_block_id> get_block_ids();

    //: gets a tight bounding box for the scene
    vgl_box_3d<double>      bounding_box();

    // returns the dimesnsion of the scene grid where each grid element is a block
    vgl_vector_3d<unsigned int>   scene_dimensions();

    //: scene dimensions accessors
    vgl_point_3d<double>    local_origin()const { return local_origin_; }
    vgl_point_3d<double>    rpc_origin()  const { return rpc_origin_; }
    bgeo_lvcs               lvcs()        const { return lvcs_; }

    //: scene path accessors
    vcl_string              xml_path()    const { return xml_path_; }
    vcl_string              data_path()   const { return data_path_; }

    //: appearance model accessor
    vcl_vector<vcl_string> appearances()  const { return appearances_; }
    bool has_data_type(vcl_string data_type);  

    //: scene version number
    static short version_no() { return 1; }

    //: scene mutators
    void set_local_origin(vgl_point_3d<double> org) { local_origin_ = org; }
    void set_rpc_origin(vgl_point_3d<double> rpc)   { rpc_origin_ = rpc; }
    void set_lvcs(bgeo_lvcs lvcs)                   { lvcs_ = lvcs; }
    void set_blocks(vcl_map<boxm2_block_id, boxm2_block_metadata> blocks) { blocks_ = blocks; }
    void add_block_metadata(boxm2_block_metadata data);
	void set_appearances(vcl_vector<vcl_string> const& appearances){ this->appearances_ = appearances; }

    //: scene path mutators
    void set_xml_path(vcl_string path)              { xml_path_ = path; }
    void set_data_path(vcl_string path)             { data_path_ = path+"/"; }

  private:

    //: world scene information
    bgeo_lvcs               lvcs_;
    vgl_point_3d<double>    local_origin_;
    vgl_point_3d<double>    rpc_origin_;

    //: location on disk of xml file and data/block files
    vcl_string data_path_, xml_path_;

    //: list of block meta data available to this scene
    vcl_map<boxm2_block_id, boxm2_block_metadata> blocks_;

    //: list of appearance models/observation models used by this scene
    vcl_vector<vcl_string> appearances_;
};


//: utility class for sorting id's by their distance
class boxm2_dist_id_pair
{
  public:
    boxm2_dist_id_pair(double dist, boxm2_block_id id) : dist_(dist), id_(id) {}
    double dist_;
    boxm2_block_id id_;

    inline bool operator < (boxm2_dist_id_pair const& v) const {
      return dist_ < v.dist_;
    }
};

//Smart_Pointer typedef for boxm2_scene
typedef vbl_smart_ptr<boxm2_scene> boxm2_scene_sptr;
typedef vbl_smart_ptr<boxm2_scene_info_wrapper> boxm2_scene_info_wrapper_sptr;
//: scene output stream operator
vcl_ostream& operator<<(vcl_ostream &s, boxm2_scene& scene);

//: scene xml write function
void x_write(vcl_ostream &os, boxm2_scene& scene, vcl_string name);


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
