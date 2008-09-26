//:
// \file


#include "bvgl_changes.h"
#include "bvgl_change_obj.h"

//TODO: implement this!
vil_image_view<vxl_byte> bvgl_changes::create_mask_from_objs()
{
  vil_image_view<vxl_byte> img;

  return img;
}

void bvgl_changes::add_obj(bvgl_change_obj_sptr obj)
{
  objs_.push_back(obj);
}

/*void bvgl_changes::xml_read()
{

}

void bvgl_changes::xml_write()
{

}*/

//: Return IO version number;
unsigned char
bvgl_changes::version(  ) const
{
  return 1;
}

//: binary IO write
void bvgl_changes::b_write(vsl_b_ostream& os) 
{
  // first write the version number;
  unsigned char ver = version();
  vsl_b_write(os, ver);

  vsl_b_write(os, img_name_);
  vsl_b_write(os, objs_.size());
  for (unsigned i = 0; i < objs_.size(); i++) {
    objs_[i]->b_write(os);
  }

}


//: binary IO read
void bvgl_changes::b_read(vsl_b_istream& is) 
{
  // first read the version number;
  unsigned char ver;
  vsl_b_read(is, ver);

  switch(ver) 
  {
  case 1: 
    {
      vsl_b_read(is, img_name_);
      unsigned size;
      vsl_b_read(is, size);
      for (unsigned i = 0; i < size; i++) {
        bvgl_change_obj o;
        o.b_read(is);
        objs_.push_back(new bvgl_change_obj(o));
      }
      break;
    }
  default: 
    {
      vcl_cout << "In bvgl_changes::b_read() -- Unrecognized version number\n";
      break;
    }
  }

  return;
}