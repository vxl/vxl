#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Reader/Writer for a volume made up of a list of slices.
// \author Ian Scott - Manchester

#include "vil3d_slice_list.h"
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_algorithm.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>
#include <vil/vil_copy.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_slice.h>
#include <vil3d/file_formats/vil3d_slice_list.h>
#include <vil3d/file_formats/vil3d_dicom.h>

vil3d_slice_list_format::vil3d_slice_list_format() {}

vil3d_slice_list_format::~vil3d_slice_list_format()
{
}

// Look for a set of filenames that match the glob spec in filename
// The globbing format expects only '#' to represent numbers.
// Do not use "*" or "?"
// All "#" should be in one contiguous group.
void parse_globbed_filenames(const vcl_string & input,
                             vcl_vector<vcl_string> &filenames)
{
  filenames.clear();
  vcl_string filename = input;

  // Avoid confusing globbing functions
  if (filename.find("*") != filename.npos) return;
  if (filename.find("?") != filename.npos) return;

  // Check that all the #s are in a single group.
  vcl_size_t start = filename.find_first_of("#");
  if (start == filename.npos) return;
  vcl_size_t end = filename.find_first_not_of("#", start);
  if (filename.find_first_of("#",end) != filename.npos) return;
  if (end == filename.npos) end = filename.length();
  for (vcl_size_t i=start, j=start; i!=end; ++i, j+=12)
    filename.replace(j,1,"[0123456789]");


  // Search for the files
  for (vul_file_iterator fit(filename); fit; ++fit)
    filenames.push_back(fit());


  if (filenames.empty()) return;

  start = (start + filenames.front().size()) - input.size();
  end = (end + filenames.front().size()) - input.size();

  // Put them all in numeric order.
  vcl_sort(filenames.begin(), filenames.end());

  // Now discard non-contiguously numbered files.
  long count = vcl_atol(filenames.front().substr(start, end-start).c_str());
  vcl_vector<vcl_string>::iterator it=filenames.begin()+1;
  while (it != filenames.end())
  {
    if (vcl_atol(it->substr(start, end-start).c_str()) != ++count)
      break;
    ++it;
  }
  filenames.erase(it, filenames.end());
}


void parse_multiple_filenames(const vcl_string & input,
                              vcl_vector<vcl_string> &filenames)
{
  filenames.clear();
  unsigned start=0;
  for (unsigned i=0; i != input.size(); ++i)
  {
    if (input[i]==';')
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
    {
      filenames.clear();
      break;
    }


  if (filenames.empty())
    parse_globbed_filenames(filename, filenames);

  if (filenames.empty()) return 0;

  // load all the slices
  vcl_vector<vil_image_resource_sptr> images(filenames.size());

  bool same = true;

  for (unsigned i=0; i<filenames.size(); ++i)
  {
    vil_image_resource_sptr im  =
      vil_load_image_resource(filenames[i].c_str());

    images[i]=im;
    // make sure all slices are consistent,
    if (!im ||
        im->nplanes() != images.front()->nplanes() ||
        im->ni() != images.front()->ni() ||
        im->nj() != images.front()->nj() ||
        im->pixel_format() != images.front()->pixel_format())
      return 0;
    // decide if all slices are of the same type.
    if (vcl_strcmp(im->file_format(), images.front()->file_format())!=0)
      same=true;
  }

  // everything seems fine so create the volume

  // If they are all dicom images, create an explicit dicom volume.
  if (same && vcl_strcmp("dicom", images.front()->file_format())==0)
    return new vil3d_dicom_image(images);

  return new vil3d_slice_list_image(images);
}


vil3d_image_resource_sptr
vil3d_slice_list_to_volume(const vcl_vector<vil_image_resource_sptr> & images)
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
}


//: Not implemented - Make a "generic_image" on which put_section may be applied.
// The file may be opened immediately for writing so that a header can be written.
// The width/height etc are explicitly specified, so that file_format implementors
// know what they need to do...
vil3d_image_resource_sptr
vil3d_slice_list_format::make_output_image(const char* /*filename*/,
                                           unsigned /*ni*/, unsigned /*nj*/,
                                           unsigned /*nk*/, unsigned /*nplanes*/,
                                           enum vil_pixel_format) const
{
  // This should follow the pattern of make_input_image.
  // Construct a load of image resources using vil_new_image_resource
  // If you are able to construct them all, then create the slice_image.
  vcl_cerr <<"vil3d_slice_list_format::make_output_image() NYI\n";
  vcl_abort();
  return 0;
}


vil3d_slice_list_image::vil3d_slice_list_image(const vcl_vector<vil_image_resource_sptr>& images):
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
enum vil_pixel_format vil3d_slice_list_image::pixel_format() const
{
  return slices_.empty() ? VIL_PIXEL_FORMAT_UNKNOWN :
    slices_.front()->pixel_format();
}

//: Get the properties (of the first slice)
bool vil3d_slice_list_image::get_property(char const *key, void * value) const
{
  return slices_.empty()?false:slices_.front()->get_property(key, value);
}

//: Get some or all of the volume.
vil3d_image_view_base_sptr
vil3d_slice_list_image::get_copy_view(unsigned i0, unsigned ni,
                                      unsigned j0, unsigned nj,
                                      unsigned k0, unsigned nk) const
{
  if (i0+ni > this->ni() || j0+nj > this->nj() || k0+nk > this->nk()) return 0;

#define macro( type ) { \
  vil3d_image_view< type > vv(ni, nj, nk, nplanes()); \
  for (unsigned k=0; k<nk; ++k)  { \
    vil_image_view< type > src(slices_[k+k0]->get_view(i0, ni, j0, nj)); \
    vil_image_view< type > dest(vil3d_slice_ij(vv, k)); \
    vil_copy_reformat(src, dest); } \
  return new vil3d_image_view< type >(vv); \
}

  switch (pixel_format())
  {
  case VIL_PIXEL_FORMAT_BYTE:
    macro( vxl_byte );
  case VIL_PIXEL_FORMAT_SBYTE:
    macro( vxl_sbyte );
  case VIL_PIXEL_FORMAT_UINT_16:
    macro( vxl_uint_16 );
  case VIL_PIXEL_FORMAT_INT_16:
    macro( vxl_int_16 );
  case VIL_PIXEL_FORMAT_UINT_32:
    macro( vxl_uint_32 );
  case VIL_PIXEL_FORMAT_INT_32:
    macro( vxl_int_32 );
  case VIL_PIXEL_FORMAT_FLOAT:
    macro( float );
  case VIL_PIXEL_FORMAT_DOUBLE:
    macro( double );
#if 0 // Several missing templates needed before these can be used
  case VIL_PIXEL_FORMAT_COMPLEX_FLOAT:
    macro( vcl_complex<float> );
  case VIL_PIXEL_FORMAT_COMPLEX_DOUBLE:
    macro( vcl_complex<double> );
#endif
  default:
    vcl_cerr<< "ERROR: vil3d_slice_list_image::get_copy_view\n"
            << "       Can't deal with pixel_format " << pixel_format() << '\n';
    return 0;
  }
}

//: Set the contents of the volume.
bool vil3d_slice_list_image::put_view(const vil3d_image_view_base& /*vv*/,
                                      unsigned /*i0*/, unsigned /*j0*/, unsigned /*k0*/)
{
  vcl_cerr << "ERROR: vil3d_slice_list_image::put_view NYI\n\n";
  return false;
}
