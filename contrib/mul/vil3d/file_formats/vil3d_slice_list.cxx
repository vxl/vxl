#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Reader/Writer a vlolume made up of a list of slices.
// \author Ian Scott - Manchester

#include "vil3d_slice_list.h"
#include <vcl_cstdlib.h>
#include <vul/vul_file.h>
#include <vil2/vil2_load.h>
#include <vil2/vil2_copy.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_slice.h>
#include <vil3d/file_formats/vil3d_slice_list.h>

vil3d_slice_list_format::vil3d_slice_list_format() {}

// The destructor must be virtual so that the memory chunk is destroyed.
vil3d_slice_list_format::~vil3d_slice_list_format()
{
}


void parse_multiple_filenames(const vcl_string & input,
                              vcl_vector<vcl_string> &filenames)
{
  filenames.clear();
  unsigned start=0;
  for (unsigned i=0; i != input.size(); ++i)
  {
    if (input[i]==':')
    {
      filenames.push_back(input.substr(start, i-start));
      start=i+1;
    }
  }
  filenames.push_back(input.substr(start, input.size() - start));
}

vil3d_image_resource_sptr
vil3d_slice_list_format::make_input_image(const char * filename) const
{
  vcl_vector<vcl_string> filenames;
  parse_multiple_filenames(filename, filenames);

  for (unsigned i=0; i<filenames.size(); ++i)
    if (!vul_file::exists(filenames[i]))
      return 0;

  if (filenames.empty()) return 0;

  // load all the slices
  vcl_vector<vil2_image_resource_sptr> images(filenames.size());
  for (unsigned i=0; i<filenames.size(); ++i)
  {
    vil2_image_resource_sptr im  =
      vil2_load_image_resource(filenames[i].c_str());
    images[i]=im;
    // make sure all slices are consistent,
    if (!im ||
        im->nplanes() != images.front()->nplanes() ||
        im->ni() != images.front()->ni() ||
        im->nj() != images.front()->nj() ||
        im->pixel_format() != images.front()->pixel_format())
      return 0;
  }
  // everything seems fine so create the volume
  return new vil3d_slice_list_image(images);

  return 0;
}


vil3d_image_resource_sptr
vil3d_slice_list_to_volume(const vcl_vector<vil2_image_resource_sptr> & images)
{
  if (!images.empty() && !images.front()) return 0;
  for (unsigned i=1; i<images.size(); ++i)
  {
    // make sure all slices are consistent,
    if (!images[i] ||
        images[i]->nplanes() != images.front()->nplanes() ||
        images[i]->ni() != images.front()->ni() ||
        images[i]->nj() != images.front()->nj() ||
        images[i]->pixel_format() != images.front()->pixel_format())
      return 0;
  }
  // everything seems fine so create the volume
  return new vil3d_slice_list_image(images);

  return 0;
}


//: Make a "generic_image" on which put_section may be applied.
// The file may bo openned immediately for writing so that a header can be written.
// The width/height etc are explicitly specified, so that file_format implementors
// know what they need to do...
vil3d_image_resource_sptr
vil3d_slice_list_format::make_output_image(const char* filename,
                                           unsigned ni, unsigned nj,
                                           unsigned nk, unsigned nplanes,
                                           enum vil2_pixel_format) const
{
  vcl_cerr <<"vil3d_slice_list_format::make_output_image() NYI\n";
  vcl_abort();
  return 0;
}


vil3d_slice_list_image::vil3d_slice_list_image(const vcl_vector<vil2_image_resource_sptr>& images):
slices_(images)
{
}

vil3d_slice_list_image::~vil3d_slice_list_image()
{
}

//: Dimensions:  nplanes x ni x nj x nk.
// This concept is treated as a synonym to components.
unsigned vil3d_slice_list_image::nplanes() const
{
  return slices_.empty()?0:slices_.front()->nplanes();
}
//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each row.
unsigned vil3d_slice_list_image::ni() const
{
  return slices_.empty()?0:slices_.front()->ni();
}
//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each column.
unsigned vil3d_slice_list_image::nj() const
{
  return slices_.empty()?0:slices_.front()->nj();
}
//: Dimensions:  nplanes x ni x nj x nk.
// The number of slices per image.
unsigned vil3d_slice_list_image::nk() const
{
  return slices_.size();
}

//: Pixel Format.
enum vil2_pixel_format vil3d_slice_list_image::pixel_format() const
{
  return slices_.empty() ? VIL2_PIXEL_FORMAT_UNKNOWN :
    slices_.front()->pixel_format();
}

//: Get the properties (of the first slice)
bool vil3d_slice_list_image::get_property(char const *key, void * value) const
{
  return slices_.empty()?false:slices_.front()->get_property(key, value);
}

//: Get some oor all of the volume.
vil3d_image_view_base_sptr
vil3d_slice_list_image::get_copy_view(unsigned i0, unsigned ni,
                                      unsigned j0, unsigned nj,
                                      unsigned k0, unsigned nk) const
{
  if (i0+ni > this->ni() || j0+nj > this->nj() || k0+nk > this->nk()) return 0;

#define macro( type ) { \
  vil3d_image_view< type > vv(ni, nj, nk, nplanes()); \
  for (unsigned k=0; k<nk; ++k)  { \
    vil2_image_view< type > src(slices_[k+k0]->get_view(i0, ni, j0, nj)); \
    vil2_image_view< type > dest(vil3d_slice_ij(vv, k)); \
    vil2_copy_reformat(src, dest); } \
  return new vil3d_image_view< type >(vv); \
}

  switch (pixel_format())
  {
  case VIL2_PIXEL_FORMAT_BYTE:
    macro( vxl_byte );
  case VIL2_PIXEL_FORMAT_SBYTE:
    macro( vxl_sbyte );
  case VIL2_PIXEL_FORMAT_UINT_16:
    macro( vxl_uint_16 );
  case VIL2_PIXEL_FORMAT_INT_16:
    macro( vxl_int_16 );
  case VIL2_PIXEL_FORMAT_UINT_32:
    macro( vxl_uint_32 );
  case VIL2_PIXEL_FORMAT_INT_32:
    macro( vxl_int_32 );
  case VIL2_PIXEL_FORMAT_FLOAT:
    macro( float );
  case VIL2_PIXEL_FORMAT_DOUBLE:
    macro( double );
#if 0 // Several missing templates needed before these can be used
  case VIL2_PIXEL_FORMAT_COMPLEX_FLOAT:
    macro( vcl_complex<float> );
  case VIL2_PIXEL_FORMAT_COMPLEX_DOUBLE:
    macro( vcl_complex<double> );
#endif
  default:
    vcl_cerr << "ERROR: vil3d_slice_list_image::get_copy_view\n"
             << "       Can't deal with pixel_format " << pixel_format() << vcl_endl;
    return 0;
  }
}

//: Set the contents of the volume.
bool vil3d_slice_list_image::put_view(const vil3d_image_view_base& vv,
                                      unsigned i0, unsigned j0, unsigned k0)
{
  vcl_cerr << "ERROR: vil3d_slice_list_image::put_view NYI\n\n";
  return false;
}
