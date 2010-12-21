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
#include <vgl/vgl_vector_3d.h>
#include <vcl_iosfwd.h>

//smart pointer stuff
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//vpgl camera
#include <vpgl/vpgl_perspective_camera.h>

//cl include for boxm2_scene_info
#include <bocl/bocl_cl.h>

//: block info that can be easily made into a buffer and sent to gpu
struct boxm2_scene_info
{
  //world information
  cl_float    scene_origin[4];          // scene origin (point)
  cl_int      scene_dims[4];            // number of blocks in each dimension
  cl_float    block_len;                // size of each block (can only be 1 number now that we've established blocks are cubes)
  cl_float    epsilon;                  // block_len/100.0 (placed here to avoid using a register)

  //tree meta information
  cl_int      root_level;               // root_level of trees
  cl_int      num_buffer;               // number of buffers (both data and tree)
  cl_int      tree_buffer_length;       // length of tree buffer (number of cells/trees)
  cl_int      data_buffer_length;       // length of data buffer (number of cells)
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
    
    //: initializes scene from xmlFile
    boxm2_scene(vcl_string filename);

    //: destructor
    ~boxm2_scene() { }
    
    //: save scene xml file
    void save_scene(); 
    
    //: return a vector of block ids in visibility order
    vcl_vector<boxm2_block_id> get_vis_blocks(vpgl_perspective_camera<double>* cam) ; 
    
    //: return a heap pointer to a scene info 
    boxm2_scene_info* get_blk_metadata(boxm2_block_id id); 

    //: a list of block metadata...
    vcl_map<boxm2_block_id, boxm2_block_metadata> blocks() { return blocks_; }
    unsigned num_blocks() { return blocks_.size(); }

    //: scene dimensions accessors
    vgl_point_3d<double>    local_origin()const { return local_origin_; }
    vgl_point_3d<double>    rpc_origin()  const { return rpc_origin_; }
    bgeo_lvcs               lvcs()        const { return lvcs_; }

    //: scene path accessors
    vcl_string              xml_path()    const { return xml_path_; }
    vcl_string              data_path()   const { return data_path_; }
    
    //: scene version number
    static short version_no() { return 1; }

    //: scene mutators 
    void set_local_origin(vgl_point_3d<double> org) { local_origin_ = org; }
    void set_rpc_origin(vgl_point_3d<double> rpc)   { rpc_origin_ = rpc; }
    void set_lvcs(bgeo_lvcs lvcs)                   { lvcs_ = lvcs; }
    void set_blocks(vcl_map<boxm2_block_id, boxm2_block_metadata> blocks) { blocks_ = blocks; }
    void add_block_metadata(boxm2_block_metadata data); 
    
    //: scene path mutators
    void set_xml_path(vcl_string path)              { xml_path_ = path; }
    void set_data_path(vcl_string path)             { data_path_ = path; }

  private:

    //: world scene information
    bgeo_lvcs               lvcs_;
    vgl_point_3d<double>    local_origin_;
    vgl_point_3d<double>    rpc_origin_;

    //: location on disk of xml file and data/block files
    vcl_string data_path_, xml_path_;
    
    //: list of block meta data available to this scene
    vcl_map<boxm2_block_id, boxm2_block_metadata> blocks_; 

};

//Smart_Pointer typedef for boxm2_scene
typedef vbl_smart_ptr<boxm2_scene> boxm2_scene_sptr;

//: scene output stream operator
vcl_ostream& operator<<(vcl_ostream &s, boxm2_scene& scene);

//: scene xml write function
void x_write(vcl_ostream &os, boxm2_scene& scene, vcl_string name);

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

#endif // boxm2_scene_h_
