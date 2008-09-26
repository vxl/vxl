//:
// \file
// \brief Helper classes to annotate polygonal regions in images
//        a set of bvgl_change_obj
// \author Ozge C. Ozcanli ozge@lems.brown.edu
// \date   09/26/08
//
// \verbatim
//  Modifications
//
// \endverbatim
//

#ifndef bvgl_changes_h_
#define bvgl_changes_h_

#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>
#include "bvgl_change_obj_sptr.h"

class bvgl_changes : public vbl_ref_count
{
public:
  bvgl_changes() {}

  unsigned size() { return objs_.size(); }

	//: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);
  
  //: Return IO version number;
  unsigned char version() const;
	//void xml_read();
	//void xml_write();
	
  vil_image_view<vxl_byte> create_mask_from_objs();
	void add_obj(bvgl_change_obj_sptr obj);

  vcl_vector<bvgl_change_obj_sptr> objs_;
	vcl_string img_name_;
	
};

#endif  //bvgl_changes_h_
