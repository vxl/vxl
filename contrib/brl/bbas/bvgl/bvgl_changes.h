#ifndef bvgl_changes_h_
#define bvgl_changes_h_
//:
// \file
// \brief Helper classes to annotate polygonal regions in images a set of bvgl_change_obj
// \author Ozge C. Ozcanli ozge@lems.brown.edu
// \date   September 26, 2008
//
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_view_base.h>

#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>
#include "bvgl_change_obj_sptr.h"

class bvgl_changes : public vbl_ref_count
{
 public:
  bvgl_changes() {}

  ~bvgl_changes() {}

  unsigned size() const { return objs_.size(); }

  bvgl_change_obj_sptr obj(unsigned int i);

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

  //: Return IO version number;
  unsigned char version() const;
  //void xml_read();
  //void xml_write();

  vil_image_view_base_sptr create_mask_from_objs(unsigned ni, unsigned nj, vcl_string change_type);
  vil_image_view_base_sptr create_mask_from_objs_all_types(unsigned ni, unsigned nj);

  void add_obj(bvgl_change_obj_sptr obj);
  void remove_obj(bvgl_change_obj_sptr obj);

 private:
  vcl_vector<bvgl_change_obj_sptr> objs_;
  vcl_string img_name_;
};

#endif  //bvgl_changes_h_
