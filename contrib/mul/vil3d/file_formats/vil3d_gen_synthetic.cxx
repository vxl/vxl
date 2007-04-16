// This is mul/vil3d/file_formats/vil3d_gen_synthetic.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Reader for simple images generated on the fly.
// \author Ian Scott - Manchester

#include "vil3d_gen_synthetic.h"
#include <vcl_cassert.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_new.h>
#include <vil/vil_pixel_format.h>
#include <vul/vul_reg_exp.h>
#include <vul/vul_string.h>


vil3d_image_resource_sptr vil3d_gen_synthetic_format::make_input_image(const char *filename) const
{
  vul_reg_exp re("^gen:([0-9]+)x([0-9]+)x([0-9]+):([a-z0-9A-Z<>_]+):(-?[0-9\\.]+)$");
  assert(re.is_valid());

  if (! re.find(filename))
    return 0;

  unsigned ni = vul_string_atoi(re.match(1));
  unsigned nj = vul_string_atoi(re.match(2));
  unsigned nk = vul_string_atoi(re.match(3));

  vil_pixel_format pf = vil_pixel_format_from_string(re.match(4).c_str());
  if (pf == VIL_PIXEL_FORMAT_UNKNOWN)
  {
    vcl_cerr << "ERROR: vil3d_gen_synthetic_format unknown pixel format " << re.match(4) << vcl_endl;
    return 0;
  }

  vil3d_gen_synthetic_pixel_value pv;
  switch (pf)
  {
   case VIL_PIXEL_FORMAT_BOOL:
    pv.bool_value = vul_string_to_bool(re.match(5));
    break;
   case VIL_PIXEL_FORMAT_BYTE:
    pv.byte_value = static_cast<vxl_byte>(vul_string_atoi(re.match(5)));
    break;
   case VIL_PIXEL_FORMAT_SBYTE:
    pv.sbyte_value = static_cast<vxl_sbyte>(vul_string_atoi(re.match(5)));
    break;
   case VIL_PIXEL_FORMAT_INT_16:
    pv.int_16_value = vul_string_atoi(re.match(5));
    break;
   case VIL_PIXEL_FORMAT_UINT_16:
    pv.uint_16_value = vul_string_atoi(re.match(5));
    break;
   case VIL_PIXEL_FORMAT_INT_32:
    pv.int_32_value = vul_string_atoi(re.match(5));
    break;
   case VIL_PIXEL_FORMAT_UINT_32:
    pv.uint_32_value = vul_string_atoi(re.match(5));
    break;
   case VIL_PIXEL_FORMAT_FLOAT:
    pv.float_value = static_cast<float>(vul_string_atof(re.match(5)));
    break;
   case VIL_PIXEL_FORMAT_DOUBLE:
    pv.double_value = vul_string_atof(re.match(5));
    break;
   default:
    vcl_cerr << "ERROR: vil3d_gen_synthetic_format Cannot handle pixel format " << re.match(4) << vcl_endl;
    return 0;
  }

  return new vil3d_gen_synthetic_image(ni, nj, nk, pf, pv);
}


//: Make a "generic_image" on which put_section may be applied.
// The file may be opened immediately for writing so that a header can be written.
// The width/height etc are explicitly specified, so that file_format implementors
// know what they need to do...
vil3d_image_resource_sptr vil3d_gen_synthetic_format::make_output_image(const char* /*filename*/,
                                 unsigned /*ni*/, unsigned /*nj*/,
                                 unsigned /*nk*/, unsigned /*nplanes*/,
                                 enum vil_pixel_format /*format*/) const
{
  vcl_cerr << "ERROR: Cannot write to generated synthetic images.\n";
  return 0;
}


vil3d_gen_synthetic_image::vil3d_gen_synthetic_image(
  unsigned ni,
  unsigned nj,
  unsigned nk,
  enum vil_pixel_format format,
  vil3d_gen_synthetic_pixel_value pv):
    ni_(ni), nj_(nj), nk_(nk), format_(format), value_(pv)
{
}

//: Dimensions:  nplanes x ni x nj x nk.
// This concept is treated as a synonym to components.
unsigned vil3d_gen_synthetic_image::nplanes() const
{
  return vil_pixel_format_num_components(format_);
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each row.
unsigned vil3d_gen_synthetic_image::ni() const
{
  return ni_;
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each column.
unsigned vil3d_gen_synthetic_image::nj() const
{
  return nj_;
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of slices per image.
unsigned vil3d_gen_synthetic_image::nk() const
{
  return nk_;
}

//: Pixel Format.
enum vil_pixel_format vil3d_gen_synthetic_image::pixel_format() const
{
  return format_;
}

//: Get some or all of the volume.
vil3d_image_view_base_sptr vil3d_gen_synthetic_image::get_copy_view(
                               unsigned i0, unsigned ni, unsigned j0, unsigned nj,
                               unsigned k0, unsigned nk) const
{
  if (i0+ni > this->ni() || j0+nj > this->nj() || k0+nk > this->nk()) return 0;

  switch (format_)
  {
    case VIL_PIXEL_FORMAT_BOOL:
    {
      vil3d_image_view<bool> *p =
        new vil3d_image_view<bool>(ni, nj, nk, nplanes());
      p->fill(value_.bool_value);
      return p;
    }
    case VIL_PIXEL_FORMAT_SBYTE:
    {
      vil3d_image_view<vxl_sbyte> *p =
        new vil3d_image_view<vxl_sbyte>(ni, nj, nk, nplanes());
      p->fill(value_.sbyte_value);
      return p;
    }
    case VIL_PIXEL_FORMAT_BYTE:
    {
      vil3d_image_view<vxl_byte> *p =
        new vil3d_image_view<vxl_byte>(ni, nj, nk, nplanes());
      p->fill(value_.byte_value);
      return p;
    }
    case VIL_PIXEL_FORMAT_INT_16:
    {
      vil3d_image_view<vxl_int_16> *p =
        new vil3d_image_view<vxl_int_16>(ni, nj, nk, nplanes());
      p->fill(value_.int_16_value);
      return p;
    }
    case VIL_PIXEL_FORMAT_UINT_16:
    {
      vil3d_image_view<vxl_uint_16> *p =
        new vil3d_image_view<vxl_uint_16>(ni, nj, nk, nplanes());
      p->fill(value_.uint_16_value);
      return p;
    }
    case VIL_PIXEL_FORMAT_INT_32:
    {
      vil3d_image_view<vxl_int_32> *p =
        new vil3d_image_view<vxl_int_32>(ni, nj, nk, nplanes());
      p->fill(value_.int_32_value);
      return p;
    }
    case VIL_PIXEL_FORMAT_UINT_32:
    {
      vil3d_image_view<vxl_uint_32> *p =
        new vil3d_image_view<vxl_uint_32>(ni, nj, nk, nplanes());
      p->fill(value_.uint_32_value);
      return p;
    }
    case VIL_PIXEL_FORMAT_FLOAT:
    {
      vil3d_image_view<float> *p =
        new vil3d_image_view<float>(ni, nj, nk, nplanes());
      p->fill(value_.float_value);
      return p;
    }
    case VIL_PIXEL_FORMAT_DOUBLE:
    {
      vil3d_image_view<double> *p =
        new vil3d_image_view<double>(ni, nj, nk, nplanes());
      p->fill(value_.double_value);
      return p;
    }
    default:
    vcl_cout<<"ERROR: vil3d_gen_synthetic_format::get_image_data()\n"
            <<"Can't deal with pixel type " << pixel_format() << vcl_endl;
    return 0;
  }
}

bool vil3d_gen_synthetic_image::get_property(char const *, void *) const
{
  return false;
}

//: Set the contents of the volume.
bool vil3d_gen_synthetic_image::put_view(const vil3d_image_view_base&,
  unsigned, unsigned, unsigned)
{
  vcl_cerr << "ERROR: Cannot write to generated synthetic images.\n";
  return false;
}

