#ifndef boxm2_scene_h_
#define boxm2_scene_h_
//:
// \file
// \brief  Boxm2 Scene models a very generic scene, only specifies dimensions
// \author Andrew Miller
// \date   26 Oct 2010
//
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

//smart pointer stuff
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
 
//: boxm2_scene: simple scene model that maintains (in world coordinates) 
//      - scene origin
//      - number of blocks in each dimension
//      - size of each block in each dimension
//      - lvcs information
//      - xml path on disk and data path (directory) on disk

class boxm2_scene : public vbl_ref_count
{
  public:

    //: initializes scene from xmlFile
    boxm2_scene(vcl_string filename);
    
    //: destructor
    ~boxm2_scene() { }
    
    //: scene dimensions accessors
    vgl_vector_3d<double>   block_dim()   { return block_dim_; }
    vgl_vector_3d<unsigned> block_num()   { return block_num_; }
    vgl_point_3d<double>    local_origin(){ return local_origin_; }
    vgl_point_3d<double>    rpc_origin()  { return rpc_origin_; }
    bgeo_lvcs               lvcs()        { return lvcs_; }
    
    //: scene path accessors  
    vcl_string              xml_path()    { return xml_path_; }
    vcl_string              data_path()   { return data_path_; }

    //: scene version number
    static short version_no() { return 1; }

  private:

    //: world scene information 
    bgeo_lvcs               lvcs_;
    vgl_point_3d<double>    local_origin_;
    vgl_point_3d<double>    rpc_origin_;
    
    //: World dimensions of a block .e.g 1 meter x 1 meter x 1 meter
    vgl_vector_3d<double>   block_dim_;
    
    //: number of blocks in each dimension
    vgl_vector_3d<unsigned> block_num_;
    
    //: location on disk of xml file and data/block files
    vcl_string data_path_, xml_path_;

};

//Smart_Pointer typedef for boxm2_scene
typedef vbl_smart_ptr<boxm2_scene> boxm2_scene_sptr;

//: scene output stream operator 
vcl_ostream& operator <<(vcl_ostream &s, boxm2_scene& scene);

//: scene xml write function
void x_write(vcl_ostream &os, boxm2_scene scene, vcl_string name);

//: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream & os, boxm2_scene const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_scene* &p);

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream & is, boxm2_scene &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_scene* p);

#endif // boxm2_scene_h_
