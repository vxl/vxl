// This is core/vil/file_formats/vil_nitf.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Implementation of methods for class vil_nitf.
// See vil_nitf.h for a description of this file.
//
// \author    laymon@crd.ge.com
// \date: 2003/12/26
//
// \verbatim
//  Modifications:
// \endverbatim

#include <vcl_ctime.h>  // for CLOCKS_PER_SEC
#include <vxl_config.h> // for VXL_BIG_ENDIAN and vxl_byte
#include <vcl_cassert.h>

#include <vcl_cstring.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>

#include <vil/vil_decimate.h>
#include <vil/vil_image_view.h>
#include <vil/vil_property.h>
#include <vil/vil_save.h>
#include <vil/vil_stream.h>

#include "vil_nitf.h"
#include "vil_nitf_message_header_v20.h"
#include "vil_nitf_image_subheader_v20.h"
#include "vil_nitf_util.h"

// Constants
char const* const vil_nitf_format_tag = "NITF v2.0";
static int debug_level = 0;

static const unsigned long TOTAL_BYTE_LIMIT = 100000000;  // 100MB - glutMainLoop seems to choke with images larger than this.

// Functions
// Ported from NITFImage.  Comment out to avoid compiler warnings
// until we actually start using.
#if 0
static bool xxproblem(char const* linefile, char const* msg)
{
  vcl_cerr << linefile << "[PROBLEM " <<msg << ']';
  return false;
}
#define xproblem(x, l) xxproblem(__FILE__ ": " #l ": ", x)
#define yxproblem(x, l) xproblem(x, l)
#define problem(x) yxproblem(x, __LINE__)
#endif


////////////////////////////////////////////////////////////////////
//: Calculate and display elapsed time given start and end time.
//
// \param start structure containing start time
// \param end structure containing end time
// \param msg string message to be displayed
////////////////////////////////////////////////////////////////////
static
void display_elapsed_time(
    vcl_clock_t start,
    vcl_clock_t end,
    const vcl_string &msg)
{
  long elapsed = (end - start) * 1000 / CLOCKS_PER_SEC;
  if (msg.length() > 0) {
    vcl_cout << msg << " - ";
  }
  vcl_cout << "elapsed milli-seconds = " << elapsed << vcl_endl;
}


// Probe file associated with input stream.
//
// \return true if associated file is NITF.

bool is_nitf_file_format(vil_stream* is)
{
  bool is_nitf_file = false;

  // FOR NOW, JUST GET V 2.0 WORKING

  vil_streampos curpos = is->tell();
  vil_nitf_message_header_v20 * message_header = new vil_nitf_message_header_v20();

  if (message_header->read_version(is)) {
    is_nitf_file = true;
  }
  delete message_header;

  is->seek(curpos);

  return is_nitf_file;
}

//: Read input data from vil_stream and return smart pointer to vil_image_resource.
// In this case, this will actually be a pointer to the subclass vil_nitf_image.
//
//  \param is vil_stream from which the data will be read
//
//  \return smart pointer to vil_nitf_image as pointer to base class vil_image_resource
//       return null if is does not contain a NITF file
//
vil_image_resource_sptr vil_nitf_file_format::make_input_image(vil_stream* is)
{
  static vcl_string method_name = "vil_nitf_file_format::make_input_image: ";

  io_stream_ = is;
  vil_image_resource_sptr nitf_image_resource = 0;

  // FOR NOW, is_nitf_file_format JUST CHECKS FOR v2.0
  // TBD: ADD CODE TO CHECK FOR BOTH V2.0 AND V2.1 -- MAL 26sep2003

  if (! is_nitf_file_format(is)) {
    return 0;
  }

  //  READ IN HEADERS HERE RATHER THAN IN vil_nitf_image TO ALLOW POSSIBLITY
  //  OF MULTIPLE IMAGES IN FUTURE.
  read_header_data();

  //  FOR NOW, JUST RETURN FIRST IMAGE FROM FILE.
  if (message_header_->NUMI > 0)
  {
    if (image_subheader_vector_.size() > 1) {
      vcl_cerr << method_name << "WARNING: size of vector image_subheader_vector_ = "
               << image_subheader_vector_.size() << ".  Should be 1.  Taking last element.\n";
    }
    vil_nitf_image_subheader_sptr image_subheader = image_subheader_vector_.back();

    // Smart pointers do not provide inheritance like corresponding classes.
    // Use regular pointer to access non-inherited methods in vil_nitf_image.
    vil_nitf_image * nitf_image = new vil_nitf_image(is, message_header_,
                                                     image_subheader,
                                                     io_stream_->tell());

    nitf_image_resource = nitf_image;

    if (debug_level > 0) {
        vcl_cout << method_name << "ni = " << nitf_image->ni()
                 << " nj = " << nitf_image->nj() << vcl_endl;

        nitf_image->display_image_attributes(method_name);
    }
    unsigned bits_per_pixel = 8 * vil_pixel_format_sizeof_components(nitf_image->pixel_format());

    //  IS THIS TRUE ?  For now, we are forcing this to be true.
    //  See method pixel_format.  MAL 28oct2003
    if (bits_per_pixel != image_subheader->NBPP) {
      vcl_cerr << method_name
               << "WARNING: bits_per_pixel = " << bits_per_pixel
               << " calculated from pixel_format does not equal attribute NBPP = "
               << image_subheader->NBPP << " in image subheader.\n";
    }
  }

  return nitf_image_resource;
}

vil_image_resource_sptr vil_nitf_file_format::make_output_image(
    vil_stream* vs,
    unsigned nx,
    unsigned ny,
    unsigned nplanes,
    enum vil_pixel_format format)
{
  if (true) {
    vcl_cerr << "ERROR: vil_nitf_file_format doesn't support output yet\n";
    return 0;
  }
  else
    return new vil_nitf_image(vs, nx, ny, nplanes, format);
}

char const* vil_nitf_file_format::tag() const
{
  return vil_nitf_format_tag;
}

///////////////////////////////////////////////////////////////////////
//: Read header data from input stream.  There are multiple headers.
// For now, we read file/message header, plus image subheaders.
//
// \param return true if header data read successfully.
//////////////////////////////////////////////////////////////////////
bool vil_nitf_file_format::read_header_data()
{
  static vcl_string method_name = "vil_nitf_file_format::read_header_data: ";

  bool success = false;

  message_header_ = new vil_nitf_message_header_v20();
  StatusCode status = message_header_->Read(io_stream_);

  image_subheader_vector_.clear();

  if (status == STATUS_GOOD)
  {
    if (debug_level > 0) {
        message_header_->display_header_info(method_name);
    }
    vil_streampos save_pos = io_stream_->tell();

    // CAST BELOW IS OK.  CURRENT POSITION IN FILE WILL ALWAYS BE > 0.
    if (message_header_->GetHeaderLength() != static_cast<unsigned long>(save_pos)) {
      vcl_cerr << method_name << "WARNING: "
               << "after reading message header, file position = "
               << save_pos << " is not equal to message header length = "
               << message_header_->GetHeaderLength()
               << vcl_endl;
    }

    // FOR NOW, JUST READ FIRST IMAGE SUBHEADER.  MAL 20oct2003
    vil_nitf_image_subheader_sptr image_subheader = new vil_nitf_image_subheader_v20();
    status = image_subheader->Read(io_stream_);

    if (status == STATUS_GOOD)
    {
      if (debug_level > 0) {
        image_subheader->display_attributes(method_name);
        vcl_cout << method_name
                 << "after read message header, file position = "
                 << save_pos << vcl_endl
                 << method_name
                 << "after first image header, file position = "
                 << io_stream_->tell() << vcl_endl;
      }

      image_subheader->set_data_length(message_header_->get_image_data_length(0));
      image_subheader_vector_.push_back(image_subheader);

      success = true;
    }
  }  // end if (status == STATUS_GOOD)

  return success;
}

/////////////////////////////////////////////////////////////////////////////
// Code for vil_nitf_image
/////////////////////////////////////////////////////////////////////////////

vil_nitf_image::vil_nitf_image(vil_stream* is)
  : in_stream_(is), out_stream_(0)
{
  in_stream_->ref();
}

vil_nitf_image::vil_nitf_image(vil_stream* is,
                               vil_nitf_message_header_sptr message_header,
                               vil_nitf_image_subheader_sptr image_subheader,
                               vil_streampos image_data_offset)
  : nplanes_(0), in_stream_(is), out_stream_(0),
    image_data_offset_(image_data_offset),
    message_header_(message_header),
    image_subheader_(image_subheader)
{
  static vcl_string method_name = "vil_nitf_image::vil_nitf_image: ";

  in_stream_->ref();

  this->set_image_data();

  if (debug_level > 0) {
      check_image_data_offset(vcl_cout, "constructor");
  }

}  // end constructor

vil_nitf_image::vil_nitf_image(
    vil_stream* is,
    unsigned ni,
    unsigned nj,
    unsigned nplanes,
    vil_pixel_format format)
: ni_(ni), nj_(nj), nplanes_(nplanes),
  bits_per_component_(8*vil_pixel_format_sizeof_components(format)),
  in_stream_(is), out_stream_(0)
{
  // BELOW IS NOT CORRECT.
  in_stream_->ref();
  write_header();
}

vil_nitf_image::~vil_nitf_image()
{
  // NEED TO DO SOMETHING ELSE ?? MAL 22oct2003
    in_stream_->unref();
    if (out_stream_ != 0 && out_stream_ != in_stream_) {
      out_stream_->unref();
    }
}

/**
 * Set image data from headers.  Copied from TargetJr class NITFImage.
 * Was plain Read in NITFImage.  Method used to read image header data,
 * but that should already have been done by the time this method is invoked.
 * Since we are just setting attribute values from header attributes, rename
 * to set_image_data.
 */
StatusCode vil_nitf_image::set_image_data()
{
  static vcl_string method_name = "vil_nitf_file_format::set_image_data: ";

  // Header data should already have been read.
#if 0
  if (ReadHeader(input_stream) == STATUS_BAD) {
    return STATUS_BAD;
  }
#endif
  // If the Header has not yet been created just return STATUS_GOOD.
  //
  vil_nitf_image_subheader_sptr image_subheader = getHeader();

  // SHOULDN'T WE RETURN STATUS_BAD IF IMAGE HEADER HAS NOT BEEN READ ?? MAL 20oct2003
  if (image_subheader == static_cast<vil_nitf_image_subheader_sptr>(0)) {
    return STATUS_GOOD;
  }

  // If we read the header successfully, set up
  // the Image information.
  //
  // NOTE: All code for setting color maps and band order is commented out
  //     in TIFF code in both vil1 and vil.
  //     Good enough for TIFF, good enough for me.  This allows us to
  //     dispense with class BandRep.  MAL 22oct2003

  this->ni_ = image_subheader->NCOLS;
  this->nj_ = image_subheader->NROWS;

  //  IS THIS CORRECT ?  Talk to Glen Booksby.  Glen says probably yes. MAL 20oct2003
  this->set_nplanes(image_subheader_->NBANDS);

  // SetBitsPixel(image_subheader->ABPP * image_subheader->NBANDS);
  bits_per_component_ = image_subheader->ABPP;

  unsigned temp_uival = image_subheader->ABPP * image_subheader->NBANDS;
  if (debug_level > 1) {
      vcl_cout << method_name << "NBPP = " << image_subheader->NBPP
               << "  (ABPP * NBANDS) = " << temp_uival << vcl_endl;
  }

  // PUT OUT WARNING FOR NOW BECAUSE VIL DOES NOT DEAL WELL WITH CASE WHERE
  // ACTUAL BITS < STORED BITS.
  if (temp_uival != image_subheader->NBPP) {
    vcl_cout << method_name << "WARNING: (ABPP * NBANDS) = " << temp_uival
             << " != NBPP = " << image_subheader->NBPP
             << vcl_endl;
  }

  blocking_info_.set_num_blocks_x(getHeader()->NBPR);
  blocking_info_.set_block_size_x(getHeader()->NPPBH);

  blocking_info_.set_num_blocks_y(getHeader()->NBPC);
  blocking_info_.set_block_size_y(getHeader()->NPPBV);

  SetRepFormat(image_subheader->IMODE_ == PIXEL_INTERLEAVED ? INTERLEAVED : BANDED);

  ifilePos_ = in_stream_->tell();

  // Set up for Image data fetching methods.
  //
  set_top_pad(0);
  set_bottom_pad((image_subheader->NBPC * image_subheader->NPPBV) - image_subheader->NROWS);
  set_left_pad(0);
  set_right_pad((image_subheader->NBPR * image_subheader->NPPBH) - image_subheader->NCOLS);

  // setInterleaveType(image_subheader->IMODE_);
  // setvil_stream(in_stream_, ifilePos_);
  set_row_modulus(1);

  // Make sure that the files stay in sync if they are the same.
  //
  if (in_stream_ == out_stream_) {
    ofilePos_ = ifilePos_;
  }

  if (image_subheader->ICORDS == vil_nitf_image_subheader::GEOCENTRIC ||
      image_subheader->ICORDS == vil_nitf_image_subheader::GEOGRAPHIC)
  {
    vcl_cerr << method_name << "WARNING: ICORDS = " << image_subheader->ICORDS
             << "\n    Code to set GeoPt values in super-class Image not implemented.\n";

#if 0 // commented out  MAL 22oct2003
    // These methods deal with attributes from TargetJr base class ImageTemplate.
    // They do not appear to have counterparts in the VXL image classes.
    SetUL(GeoPt(image_subheader->IGEOLO));
    SetUR(GeoPt(image_subheader->IGEOLO+15));
    SetLR(GeoPt(image_subheader->IGEOLO+30));
    SetLL(GeoPt(image_subheader->IGEOLO+45));
    SetGeographic(true);
#endif
  }

  // If we don't know the length of the image and since the v1.1
  // header doesn't tell us, we'll assume it is the length of the file
  // minus the length of the header.

  //  WE ARE ONLY DEALING WITH VERSION 2.0 AND ABOVE.
  //  IF DATA LENGTH NOT SET, LOG AN ERROR.

  if (image_subheader && image_subheader->get_data_length() == 0) {
    vcl_cerr << method_name << "WARNING: data length not set in image subheader.\n";
  }
#if 0
  SetStatusGood(); // This method is on super-class Tag (through super-class Image of class NITFImage.
                   // See if we actually need status attribute in this class.
#endif
  return STATUS_GOOD;
}  // end method init_image_data

bool vil_nitf_image::read_header()
{
  return false;
}

bool vil_nitf_image::write_header()
{
  return false;
}

///////////////////////////////////////////////////////////////////////
//: Find and set value for named property.
//
// \param tag name of property to look up.
// \param value pointer to value to be set if property is found
//
//     NOTE: At present time, only valid property for NITF is
//         vil_property_quantisation_depth.  Type of value is unsigned int *
//
// \return true if property was found.
//
//////////////////////////////////////////////////////////////////////
bool vil_nitf_image::get_property(char const * tag, void * value) const
{
  static vcl_string method_name = "vil_nitf_file_format::get_property: ";
  bool found_property = false;

  if (vcl_strcmp (vil_property_quantisation_depth, tag) == 0) {
    found_property = true;
    if (value != 0) {
      unsigned int * depth = static_cast<unsigned int*>(value);
      *depth = get_bits_per_component();
    }
    else {
      vcl_cerr << method_name << "WARNING: property " << tag
               << " found, but passed pointer value is null.\n";
    }
  }
// This is not an in-memory image type, nor is it read-only:
  return found_property;
}

/////////////////////////////////////////////////////////////
//: Get pixel format.
/////////////////////////////////////////////////////////////
vil_pixel_format vil_nitf_image::pixel_format() const
{
  vil_pixel_format format = VIL_PIXEL_FORMAT_UNKNOWN;

  if (image_subheader_ != static_cast<vil_nitf_image_subheader_sptr>(0))
  {
    switch(get_bits_per_pixel())
    {
      case 8:
        format = VIL_PIXEL_FORMAT_BYTE;
        break;
      case 16:
        format = VIL_PIXEL_FORMAT_UINT_16;
        break;
      case 32:
        format = VIL_PIXEL_FORMAT_UINT_32;
        break;
      default:
        format = VIL_PIXEL_FORMAT_UNKNOWN;
        break;
    }
    // Deal separately with RBG case
    if (image_subheader_->NBANDS == 3 &&
        image_subheader_->ABPP == 8)
    {
      // Mike P. thinks this should be  VIL_PIXEL_FORMAT_RGB_INT_32.
      // Look at TargetJr code to verify.  MAL 29oct2003
      format = VIL_PIXEL_FORMAT_RGB_BYTE;
    }
  }
  return format;
}

vil_nitf_image_subheader_sptr vil_nitf_image::getHeader()
{
  return image_subheader_;
}

char const* vil_nitf_image::file_format() const
{
  return vil_nitf_format_tag;
}

//:  Get vil_image_view for NITF.
//
// \param i0 x origin
// \param ni number of pixels in x
// \param j0 y origin
// \param nj number of pixels in y
//
// \return smart pointer to vil_image_view_base
//     for NITF image.  Actual sub-class should be vil_image_view<T>.

vil_image_view_base_sptr vil_nitf_image::get_copy_view(
    unsigned i0,
    unsigned ni,
    unsigned j0,
    unsigned nj) const
{
  static vcl_string method_name = "vil_nitf_image::get_copy_view: ";

  vil_image_view_base_sptr image_view = 0;  // RETURN VALUE

  // ACCORDING TO DOCUMENTATION, IF PARAMETERS ARE BAD, WE SHOULD RETURN NULL POINTER.
  if ((i0 + ni > ni_) || (j0 + nj > nj_)) {
    return image_view;
  }

  assert(image_subheader_ != static_cast<vil_nitf_image_subheader_sptr>(0));

  if (debug_level > 0) {
      vcl_cout << method_name
               << "i0 = " << i0
               << "  j0 = " << j0
               << "  ni = " << ni
               << "  nj = "  << nj
               << vcl_endl;
  }

  // vil_pixel_format_sizeof_components should give # of bytes per pixel.
  unsigned bytes_per_pixel = vil_pixel_format_sizeof_components(this->pixel_format());
  unsigned long total_bytes = this->get_image_length();
  // ONLY CALCULATE NUMBER OF BYTES IF NOT READING TOTAL IMAGE.
  if (ni < this->ni_ || nj < this->nj_) {
    total_bytes = ni * nj * bytes_per_pixel;
  }
  else {
    if (debug_level > 1) {
      vcl_cout << method_name << "Reading entire image - use image size from header.\n";
    }
  }

  if (debug_level > 1) {
    vcl_cout << method_name << "image size get_image_length = "
             << get_image_length() << vcl_endl
             << method_name << "image size from get_image_data_length(0) = "
             << message_header_->get_image_data_length(0) << vcl_endl

             << method_name << "bits_per_pixel = " << get_bits_per_pixel() << vcl_endl
             << method_name << "bits_per_component = " << get_bits_per_component() << vcl_endl

             << method_name << "pixel_format    = " << pixel_format() << vcl_endl
             << method_name << "bytes_per_pixel = " << bytes_per_pixel << vcl_endl
             << method_name << "total_bytes = " << total_bytes << vcl_endl;
  }
  if (debug_level > 1) {
    vcl_cout << method_name << "origin = (" << i0 << ", " << j0 << ")\n"
             << method_name << "size = (" << ni << ", " << nj << ", "
             << this->nplanes() << ")\n"
             << method_name << "block_size_x = " << get_block_size_x() << vcl_endl;
  }

  if (image_subheader_->NBANDS == 1) {
    image_view = get_single_band_view(i0, ni, j0, nj);
  }
  else {
    vcl_cerr << method_name << "WARNING: logic for NBANDS != 1 not implemented.\n";
  }

  return image_view;
}

//////////////////////////////////////////////////////////////////////////////////
//: Get vil_image_view for NITF image with single band.
//  Helper method for get_copy_view.
//
// \param i0 x origin
// \param ni number of pixels in x
// \param j0 y origin
// \param nj number of pixels in y
//
// \return smart pointer to vil_image_view_base
//     for NITF image.  Actual sub-class should be vil_image_view<T>.

vil_image_view_base_sptr vil_nitf_image::get_single_band_view(
    unsigned i0,
    unsigned ni,
    unsigned j0,
    unsigned nj) const
{
  static vcl_string method_name = "vil_nitf_image::get_single_band_view: ";

  vil_image_view_base_sptr image_view = 0;  // RETURN VALUE

  vil_memory_chunk_sptr buffer = read_single_band_data(i0, ni, j0, nj);

  switch(pixel_format())
  {
    case VIL_PIXEL_FORMAT_BYTE:
      image_view = new vil_image_view<vxl_byte>(
                       buffer, (const vxl_byte * ) buffer->data(),
                       ni, nj, nplanes(), nplanes(), nplanes() * ni, 1);
      break;

    case VIL_PIXEL_FORMAT_UINT_16:
      image_view = new vil_image_view<vxl_uint_16>(
                       buffer, (const vxl_uint_16 * ) buffer->data(),
                       ni, nj, nplanes(), nplanes(), nplanes() * ni, 1);
      break;

    case VIL_PIXEL_FORMAT_UINT_32:
      image_view = new vil_image_view<vxl_uint_32>(
                       buffer, (const vxl_uint_32 * ) buffer->data(),
                       ni, nj, nplanes(), nplanes(), nplanes() * ni, 1);
      break;
#if 0
    case VIL_PIXEL_FORMAT_RGB_BYTE:
      image_view = new vil_image_view<vil_rgb<vxl_byte> >(
                       buffer, (const vil_rgb<vxl_byte> * ) buffer->data(),
                       ni, nj, nplanes(), nplanes(), nplanes() * ni, 1);
      break;
#endif
    default:
      vcl_cerr << method_name << "Logic not implemented for pixel format = "
               << pixel_format() << vcl_endl;
      break;
  }  // end switch pixel_format

  return image_view;

}  // end method get_single_band_view

//////////////////////////////////////////////////////////////////////////////////
//
//:  Get vil_image_view for NITF image with single band.
//  Helper method for get_copy_view.
//
// \param i0 x origin
// \param ni number of pixels in x
// \param j0 y origin
// \param nj number of pixels in y
//
// \return smart pointer to vil_memory_chunk containing data
//     for NITF image.
//
vil_memory_chunk_sptr vil_nitf_image::read_single_band_data(
    unsigned i0,
    unsigned ni,
    unsigned j0,
    unsigned nj) const
{
  static vcl_string method_name = "vil_nitf_image::read_single_band_data: ";

  vil_memory_chunk_sptr buffer = 0;    // RETURN VALUE

  unsigned blocks_read = 0;

  unsigned start_block_x = 0;
  unsigned start_block_x_offset = 0;  // offset to first column in starting x block
  unsigned max_block_x = get_num_blocks_x();
  unsigned max_block_x_pixels = get_block_size_x();  //!< # of pixels to use in last block in row

  unsigned start_block_y = 0;
  unsigned start_block_row_offset = 0;  // offset to first row in starting y  block

  unsigned max_block_y = get_num_blocks_y();
  unsigned max_block_y_pixels = get_block_size_y();  //!< # of pixels to use in last block in column

  unsigned long last_image_offset = 0;

  // vil_pixel_format_sizeof_components should give # of bytes per pixel.
  unsigned long bytes_per_pixel = vil_pixel_format_sizeof_components(this->pixel_format());
  unsigned long bytes_per_block = get_block_size_x() * get_block_size_y() * bytes_per_pixel;

  // # of bytes for all blocks with one row of blocks
  unsigned long bytes_per_block_row = bytes_per_block * get_num_blocks_x();

  // # of bytes per unified image row
  unsigned long bytes_per_image_row = ni * bytes_per_pixel;

  // BLOCK_SIZE_X IS NUMBER OF PIXELS IN EACH ROW OF THE BLOCK.
  // WE WANT TO READ ONE ROW OF A BLOCK AT A TIME.
  unsigned long bytes_per_read = get_block_size_x() * bytes_per_pixel;
  unsigned char * block_buffer = new unsigned char[bytes_per_block];  // BUFFER TO HOLD BYTES FOR ONE BLOCK

  unsigned long total_bytes = this->get_image_length();
  // ONLY CALCULATE NUMBER OF BYTES IF NOT READING TOTAL IMAGE.
  if (ni < this->ni_ || nj < this->nj_) {
    total_bytes = ni * nj * bytes_per_pixel;
  }
  else {
    if (debug_level > 1) {
      vcl_cout << method_name << "Reading entire image - use image size from header.\n";
    }
  }

  buffer = new vil_memory_chunk(total_bytes, pixel_format());

  // ALWAYS CHECK IMAGE OFFSET FOR NOW.
  if (debug_level > -1) {
    check_image_data_offset (vcl_cout, method_name);
  }

  if (debug_level > 0) {
      vcl_cout << '\n' << method_name
               << "i0 = " << i0
               << "  j0 = " << j0
               << "  ni = " << ni
               << "  nj = " << nj
               << "  bytes_per_pixel = " << bytes_per_pixel
               << "  total_bytes for image = " << total_bytes
               << vcl_endl

               << method_name
               << "num_blocks_x = " << get_num_blocks_x()
               << "  num_blocks_y = " << get_num_blocks_y()
               << "  bytes_per_block = " << bytes_per_block
               << "  bytes_per_block_row = " << bytes_per_block_row
               << "  bytes_per_image_row = " << bytes_per_image_row
               << "  bytes_per_read = " << bytes_per_read
               << "  reverse_bytes = " << reverse_bytes() << vcl_endl
               << "  block_buffer = " << &block_buffer << vcl_endl;
  }

  unsigned int within_row_offset = 0;  // offset with uniform image row

  unsigned long total_read_count = 0;
  unsigned long read_count = 0;

  calculate_start_block(i0, get_block_size_x(), start_block_x, start_block_x_offset);
  calculate_max_block(i0, ni, get_block_size_x(), get_num_blocks_x(),
                      max_block_x, max_block_x_pixels);
  calculate_start_block(j0, get_block_size_y(), start_block_y, start_block_row_offset);
  calculate_max_block(j0, nj, get_block_size_y(), get_num_blocks_y(),
                      max_block_y, max_block_y_pixels);

  // NOTE: MUST SUBTRACT ONE TO GET LAST PIXEL COORDINATE.
  // FOR EXAMPLE, IF i0 = 0 AND ni = 256, HIGHEST X PIXEL IS 255.
  // SIMILAR CALCULATIONS FOR Y.

  unsigned long calculated_pixels = get_num_blocks_x() * get_block_size_x();
  unsigned long diff = calculated_pixels - ni;

  calculated_pixels = get_num_blocks_y() * get_block_size_y();
  diff = calculated_pixels - nj;  // NEED TO CHECK THIS.
  unsigned long last_image_col_num = 0;
  if (ni < get_block_size_x()) {
    last_image_col_num = i0 + ni - 1;
  }
  else {
    last_image_col_num = (max_block_x * get_block_size_x()) - 1;
  }
  unsigned long last_image_row_num = (max_block_y * get_block_size_x()) - 1;
  unsigned long display_pixels = (max_block_x - start_block_x) * get_block_size_x();
  display_pixels = (max_block_y - start_block_y) * get_block_size_y();

  if (debug_level > 1) {
// Please do not combine the vcl_out statements below into one statement. 
// I prefer it the way it is.   -- MAL 2004mar1.
      vcl_cout << method_name << "start_block_x = " << start_block_x
               << "  max_block_x = " << max_block_x 
               << "  start_block_row_offset = " << start_block_row_offset
               << vcl_endl;
      vcl_cout << method_name << "  start_block_x_offset = " << start_block_x_offset
               << "  expected last X pixel = " << last_image_col_num << vcl_endl;
      vcl_cout << method_name << "start_block_y = " << start_block_y
               << "  max_block_y = " << max_block_y << vcl_endl;
      vcl_cout << "expected last Y pixel = " << last_image_row_num << vcl_endl;

      vcl_cout << method_name << "display pixels = " << display_pixels
               << " rows by " << display_pixels << " columns" << vcl_endl;
  }

  vcl_clock_t start = vcl_clock();

  // ITERATE OVER BLOCKS LEFT TO RIGHT, TOP TO BOTTOM, READING IMAGE BYTES.
  // INSERT BYTES INTO ONE UNIFORM GRID

  // OUTER LOOP - ITERATE OVER BLOCKS IN Y (== J == BLOCK ROW) DIMENSION.
  for (unsigned int block_row = start_block_y; block_row < max_block_y; ++block_row)
  {
    unsigned int block_col;   // DEFINE OUT HERE SO WE CAN DISPLAY AT END OF EACH BLOCK ROW
                              // DO NOT INITIALIZE BECAUSE VALUE IS REASSIGNED BEFORE
                              // INITIAL VALUE IS EVER USED.  SOME COMPILERS COMPLAIN ABOUT THIS.

    unsigned long bytes_copied = 0;  // bytes copied for first pixel row

    // ITERATE OVER BLOCKS IN X (== I == BLOCK COLUMN) DIMENSION.
    for (block_col = start_block_x; block_col < max_block_x; ++block_col)
    {
      unsigned long image_block_col = block_col - start_block_x;  // BLOCK COLUMN RELATIVE TO IMAGE.

      // CALCULATE OFFSET TO CURRENT BLOCK IN BLOCK ROW
      unsigned block_offset = (block_row * bytes_per_block_row)
                            + (block_col * bytes_per_block);
      in_stream_->seek(get_image_data_offset() + block_offset);
    // READ IN ONE BLOCK OF DATA
      unsigned long bytes_read = in_stream_->read(block_buffer, bytes_per_block);

      total_read_count += bytes_read;
      //unsigned long last_bytes_read = bytes_read;
      ++read_count;
      ++blocks_read;

      if (bytes_read < bytes_per_block) {
        vcl_cerr << method_name
                 << "WARNING: number of bytes read = " << bytes_read
                 << ", less than requested = " << bytes_per_block
                 << "\nimage_block column = " << image_block_col
                 << "  block row = " << block_row
                 << "  block column = " << block_col
                 << "\nimage_data_offset = " << get_image_data_offset()
                 << "  block_offset = " << block_offset
                 << vcl_endl;
        continue;
      }
      if (bytes_per_pixel > 1 && reverse_bytes()) {
        reverse_bytes(block_buffer, bytes_read, bytes_per_pixel);
      }

      within_row_offset = (block_col - start_block_x) * get_block_size_x() * bytes_per_pixel;
      if (start_block_x_offset > 0 && ((block_col - start_block_x) > 0)) {
        within_row_offset -= start_block_x_offset * bytes_per_pixel;
      }

      //  block_size_y == # of rows per block
      unsigned int max_row = get_block_size_y();
      if ((block_row == (max_block_y - 1)) &&
          max_block_y_pixels < get_block_size_y())
      {
        max_row = max_block_y_pixels;
      }
    // THIS LOOP COPIES PIXELS FROM READ BUFFER INTO IMAGE BUFFER.
    // ITERATE OVER ROWS OF PIXELS WITHIN BLOCK
    // SKIP OVER THOSE PIXEL ROWS WHICH ARE NOT NEEDED, I.E. IF (row < start_block_row_offset)
      for (unsigned int row = 0; row < max_row; ++row)
      {
        if (start_block_row_offset > 0 && block_row == start_block_y &&
            row < start_block_row_offset)
        {
          continue;
        }

        unsigned block_buffer_offset = (row * bytes_per_read);
        unsigned char * buffer_pos = block_buffer + block_buffer_offset;

        unsigned long image_row_num = ((block_row - start_block_y) * get_block_size_y())
          + row - start_block_row_offset;

        unsigned long image_row_offset = image_row_num * bytes_per_image_row;
        unsigned char * image_offset = (unsigned char *) buffer->data()
                                       + image_row_offset + within_row_offset;

        last_image_row_num = image_row_num;  // SAVE FOR DEBUGGING
        last_image_offset = image_row_offset + within_row_offset;

        if ((image_row_offset + within_row_offset) >= total_bytes)
        {
          //  ASSERTION WILL FAIL.  PRINT OUT INFORMATION BEFORE INVOKING ASSERTION.
          vcl_cout << "\n##### ERROR #####\n" << method_name << vcl_endl
                   << "  block_row = " << block_row
                   << "  start_block_y = " << start_block_y
                   << "  block_col = " << block_col << vcl_endl
                   << "  row = " << row
                   << "  image_row_num = " << image_row_num
                   << "  within_row_offset = " << within_row_offset << vcl_endl
                   << "  image_offset = " << (image_row_offset + within_row_offset)
                   << " larger than total bytes in image = " << total_bytes
                   << vcl_endl;
          assert((image_row_offset + within_row_offset) < total_bytes);
        }

        unsigned char * first_byte = buffer_pos;
        unsigned long copy_count = bytes_per_read;

        if (block_col == start_block_x) {
            if (start_block_x_offset > 0) {
                copy_count = (get_block_size_x() - start_block_x_offset) * bytes_per_pixel;
                first_byte += (start_block_x_offset * bytes_per_pixel);
            }
            if (ni < get_block_size_x()) {
                copy_count = ni * bytes_per_pixel;
            }
        }
        if ((block_col > 0) && (block_col == (max_block_x - 1)) && (max_block_x_pixels < get_block_size_x())) {
          copy_count = max_block_x_pixels * bytes_per_pixel;
        }

        assert(copy_count != 0);
        vcl_memcpy(image_offset, first_byte, copy_count);

        if (row == start_block_row_offset) {
          bytes_copied += copy_count;
        }
#ifdef DEBUG
        // display_copied_bytes IS SET ABOVE IF BYTES IN BUFFER ARE DISPLAYED.
        if (display_copied_bytes)
        {
          vcl_cout << "image_row_num = " << image_row_num
                   << "  image_row_offset = " << image_row_offset
                   << "  within_row_offset = " << within_row_offset << vcl_endl
                   << "  image_offset = " << (image_row_offset + within_row_offset)
                   << "  copy_count = " << copy_count
                   << "  block_buffer_offset = " << block_buffer_offset
                   << vcl_endl;
#if 1
          unsigned char * image_display_offset = image_offset;
          unsigned char * buffer_offset = buffer_pos;
#else
          unsigned char * image_display_offset = image_offset + bytes_per_read - pixels_to_display;
          unsigned char * buffer_offset = buffer_pos + bytes_per_read - pixels_to_display;
#endif
          unsigned long err_count = compare_bytes(buffer_offset, image_display_offset,
                                                  pixels_to_display, bytes_per_pixel,
                                                  "after memcpy");
          if (err_count > 0) {
            display_as_hex(buffer_offset, pixels_to_display, bytes_per_pixel,
                           "bytes from read buffer after mcmcpy");
            display_as_hex(image_display_offset, pixels_to_display, bytes_per_pixel,
                           "bytes from image memory chunk after mcmcpy");
          }
        }  // end if (row == 0)
#endif
      }  // end for row < get_block_size_y()
    }  // end for block_col < max_block_y

    if (debug_level > 1) {
        last_image_col_num = (block_col * get_block_size_x()) - 1;
        vcl_cout << method_name << "end for loop - block_row = " << block_row
                 << "  block_col = " << (block_col - 1)
                 << "  start_block_y = " << start_block_y << '\n'
                 << "last_image_row_num = " << last_image_row_num
                 << "  last_image_col_num = " << last_image_col_num << '\n'
                 << "blocks_read = " << blocks_read 
                 << "    bytes_copied = " << bytes_copied
                 << vcl_endl;
    }

    if (bytes_copied != bytes_per_image_row) {
      vcl_cout << method_name << "ERROR: "
               << "block_row = " << block_row
               << "  block_col = " << block_col
               << "  bytes_copied = " << bytes_copied
               << " != bytes_per_image_row = " << bytes_per_image_row
               << vcl_endl;
    }
  }  // end for block_row < max_block_x

  if (debug_level > 1) {
    vcl_cout << "##### " << method_name << "finish read loops\n";

    vcl_clock_t finish = vcl_clock();

    vcl_cout << "finish reading input = " << finish << vcl_endl;
    vcl_string msg_str = "read image bytes";
    display_elapsed_time(start, finish, msg_str);

    vcl_cout << method_name << "start_block_x = " << start_block_x
             << "  max_block_x = " << max_block_x << vcl_endl
             << method_name << "start_block_y = " << start_block_y
             << "  max_block_y = " << max_block_y << vcl_endl
             << "blocks_read = " << blocks_read << vcl_endl;
    unsigned long expected_read_count = blocks_read * get_block_size_y();
    vcl_cout << "buffers read = " << read_count
             << "  expected value = " << expected_read_count << vcl_endl
             << method_name << "total bytes read = " << total_read_count << vcl_endl
             << method_name << "last_image_row_num = " << last_image_row_num << vcl_endl
             << method_name << "buffer->size = " << buffer->size() << vcl_endl
             << "last_image_offset = " << last_image_offset
             << "  last within_row_offset = " << within_row_offset << vcl_endl;
    diff = buffer->size() - last_image_offset;
    vcl_cout << method_name << "buffer end - last_image_offset = " << diff << vcl_endl;
  }

  if ((total_read_count != total_bytes)
      && (ni > get_block_size_x())
      && (nj > get_block_size_y())) {

    vcl_cerr << method_name << "WARNING: image size = " << total_bytes
             << " != bytes read = " << total_read_count << vcl_endl;
  }

#if (defined DEBUG) && DEBUG
  const unsigned int DEFAULT_PIXELS_TO_DISPLAY = 8;

  unsigned char * offset = (unsigned char *) buffer->data()
                           + last_image_offset;
//                         + buffer->size() - DEFAULT_PIXELS_TO_DISPLAY;
  display_as_hex(offset, DEFAULT_PIXELS_TO_DISPLAY, bytes_per_pixel,
                 "first 8 bytes from last row of last block from memory chunk");
  vcl_cout << "buffer size - DEFAULT_PIXELS_TO_DISPLAY = "
           << (buffer->size() - DEFAULT_PIXELS_TO_DISPLAY)
           << vcl_endl
           << "last_image_offset + bytes_per_read - DEFAULT_PIXELS_TO_DISPLAY = "
           << (last_image_offset + bytes_per_read - DEFAULT_PIXELS_TO_DISPLAY)
           << vcl_endl;
#endif

  return buffer;

}  // end method read_single_band_data

///////////////////////////////////////////////////////////////////////
//: Create set of pyramid images.
//     NOTE: Currently, this method only works for 16 bit images.
//
// \param levels number of levels of pyramid images
// \param file_name base file
// \param dir_name optional parameter for directory where output files should
//      be written.   If missing, same directory as base file will be used.
//
// \return true if no problems.
//
//////////////////////////////////////////////////////////////////////
bool vil_nitf_image::construct_pyramid_images(
    unsigned int levels,
    vcl_string file_name,
    vcl_string dir_name)
{
  static vcl_string method_name = "vil_nitf_file_format::construct_pyramid_images: ";

  bool success = true;

  char * tag_name = vil_property_quantisation_depth;
  unsigned int bits_per_component = 0;
  bool got_property = this->get_property(tag_name, &bits_per_component);

  if (got_property == false) {
    vcl_cout << method_name
             << "WARNING: failed to get property <" << tag_name << ">\n";
  }

  vil_image_view_base_sptr image_view = this->get_view();  // RETURN VALUE

  vcl_cout << method_name << "vil_image_view parameters:\n"
           << "  ni = " << image_view->ni()
           << "  nj = " << image_view->nj()
           << "  nplanes = " << image_view->nplanes() << vcl_endl
           << "  pixel_format = " << image_view->pixel_format()
           << "  bits_per_component from get_property = " << bits_per_component
           << vcl_endl;
  vcl_size_t dot_pos = file_name.rfind(".");
  vcl_cout << method_name << "file_name = <" << file_name
           << ">  dot_pos = " << dot_pos << vcl_endl;

  if (dot_pos == vcl_string::npos) {
    vcl_cout << "No dot in file name.\n";
  }
  else {
    if (dot_pos > 0) {
      file_name = file_name.substr(0, dot_pos);
      vcl_cout << method_name<< "modified file_name = <"<< file_name<< ">\n";
    }
  }
  file_name += ".";

  vil_image_view<vxl_uint_16> * uint_16_view =
      dynamic_cast<vil_image_view<vxl_uint_16> *>(image_view.as_pointer());

  for (unsigned int i = 0; i < levels; ++i)
  {
    unsigned int factor = 2 * (i + 1);
    vil_image_view<vxl_uint_16> decimated_image_view =
        vil_decimate(*uint_16_view, factor, factor);

    vil_image_view_base_sptr save_view =
        new vil_image_view<vxl_uint_16>(decimated_image_view);

    vcl_cout << method_name << "save_view parameters:\n"
             << "  ni = " << save_view->ni()
             << "  nj = " << save_view->nj()
             << "  nplanes = " << save_view->nplanes()
             << "  pixel_format = " << save_view->pixel_format()
             << vcl_endl;

    vcl_ostringstream ext_stream;
    ext_stream << 'R' << (i + 1); // Set current file extension
    vcl_string ext = ext_stream.str();

    vcl_string out_file_name = file_name;
    out_file_name += ext;

    vcl_cout << "ext = <" << ext << ">  file name = <"
             << out_file_name << ">\n";
#if 1
    vcl_clock_t start = vcl_clock();

    bool write_success = vil_save(*save_view, out_file_name.c_str(), "png");

    vcl_clock_t finish = vcl_clock();
    vcl_string msg_str = "save 2-byte PNG file";
    display_elapsed_time(start, finish, msg_str);

    vcl_cout << "Saved image as 2-byte PNG to file <" << out_file_name
             << "> using vil_save.  Success = " << write_success << vcl_endl;
#endif
  }  // end for (i)

  return success;
}

///////////////////////////////////////////////////////////////////////////
//: Helper method to calculate values for start block in Y.
//
// \param j0 origin
// \param block_size size of block in pixels
// \param start_block calculated starting block
// \param start_block_offset first pixel coordinate if origin is not on block boundary
//
///////////////////////////////////////////////////////////////////////////
void  vil_nitf_image::calculate_start_block(
    unsigned j0,
    unsigned long block_size,
    unsigned & start_block,
    unsigned & start_block_offset) const
{
  static vcl_string method_name = "vil_nitf_image::calculate_start_block: ";

  start_block = 0;
  start_block_offset = 0;

  if (j0 != 0)
  {
    start_block = j0 / block_size;
    if (debug_level > 1) {
        vcl_cout << method_name << "set start_block to " << start_block << vcl_endl;
    }
    if (j0 > 0 && j0 % block_size != 0)
    {
      start_block_offset = j0 - (start_block * block_size);
      if (debug_level > 1) {
          vcl_cout << method_name << "first pixel not on block boundary - "
                   << "set offset for starting block = "
                   << start_block_offset << vcl_endl;
      }
    }
  }  // end if (j0 != 0)
}

///////////////////////////////////////////////////////////////////////////
//: Helper method to calculate values for start block in Y.
//
// \param j0 origin
// \param nj number of pixels
// \param block_size size of block in pixels
// \param start_block calculated starting block
// \param start_block_offset first pixel coordinate if origin is not on block boundary
//
///////////////////////////////////////////////////////////////////////////
void  vil_nitf_image::calculate_max_block(
    unsigned j0,
    unsigned nj,
    unsigned long block_size,
    unsigned long num_blocks,
    unsigned & max_block,
    unsigned & max_block_pixels) const
{
  static vcl_string method_name = "vil_nitf_image::calculate_max_block: ";

  max_block = num_blocks;
  max_block_pixels = block_size;

  max_block = (j0 + nj) / block_size;

  if (debug_level > 1) {
    vcl_cout << method_name << "initial value of max_block = "
             << max_block << vcl_endl;
  }

  if ((j0 + nj) % block_size != 0)
  {
    max_block_pixels = (j0 + nj) - (max_block * block_size);
    ++max_block;

    if (debug_level > 1) {
      vcl_cout << method_name << "rightmost pixel not on block boundary\n"
               << "  set max block = " << max_block
               << ", offset for max block = " << max_block_pixels << vcl_endl;
    }
  }
}  // end calculate_max_block

///////////////////////////////////////////////////////////////////////////
//: Test to see if size of image exceeds display limits.
//
// \param ni number of pixels in i dimension
// \param nj number of pixels in j dimension
// \param bytes_per_pixel bytes per pixel for image
//
// \return true if image exceeds display limits.
//
///////////////////////////////////////////////////////////////////////////
bool vil_nitf_image::exceeds_display_limits(
    unsigned ni,
    unsigned nj,
    unsigned bytes_per_pixel) const
{
//  static vcl_string method_name = "vil_nitf_image::exceeds_display_limits: ";

    bool exceeds_limits = false;

    unsigned long total_bytes = ni * nj * bytes_per_pixel;

    if (total_bytes > TOTAL_BYTE_LIMIT) {
      exceeds_limits = true;
    }

    return exceeds_limits;
}

///////////////////////////////////////////////////////////////////////////
//:
// Calculate factor for vil_decimate.  Value is amount image will be reduced,
// e.g. if value is 2, a 1024 X 1024 image will become 512 X 512.
// For now, assume same factor is used for both i and j dimensions.
//
// \param ni number of pixels in i dimension
// \param nj number of pixels in j dimension
//
// \return factor to use for vil_decimate.
//
///////////////////////////////////////////////////////////////////////////
unsigned int vil_nitf_image::calculate_decimate_factor(
    unsigned ni,
    unsigned nj,
    unsigned bytes_per_pixel) const
{
//  static vcl_string method_name = "vil_nitf_image::calculate_decimate_factor: ";

    unsigned int decimate_factor = 1;

    unsigned long total_bytes = ni * nj * bytes_per_pixel;

    if (total_bytes > TOTAL_BYTE_LIMIT)
    {
      decimate_factor = 2;
      unsigned display_ni = ni / decimate_factor;
      unsigned display_nj = nj / decimate_factor;

      while ((display_ni * display_nj * bytes_per_pixel) > TOTAL_BYTE_LIMIT) {
        decimate_factor *= 2;
        display_ni /= decimate_factor;
        display_nj /= decimate_factor;
      }
    }

    return decimate_factor;
}


bool vil_nitf_image::put_view(const vil_image_view_base &/*im*/,
                              unsigned /*i0*/, unsigned /*j0*/)
{
  vcl_cerr << "vil_nitf_image::put_view() NYI\n";
  return false;
}

/////////////////////////////////////////////////////////////////
//
//: Fill in passed vectors with RPC camera data from data in this image header
//
// \param matx (4, 20) matrix to hold co-oefficients of rational cubics
//        There are twenty values for each of line numerator, line denominator
//        sample numerator, sample denominator.
// \param scalex (2) longitude scale/offset
// \param scaley (2) latitude scale/offset
// \param scalez (2) height scale/offset
// \param scales (2) line scale/offset
// \param scalel (2) sample scale/offset
// \param init_pt (3) centroid of the four world corner points
// \param rescales (2) vector to hold rescale parameters for sample
// \param rescale1 (2) vector to hold rescale parameters for line
//
// \return true if vectors filled in successfully
//
/////////////////////////////////////////////////////////////////
bool vil_nitf_image::get_rational_camera_data(
    vcl_vector<double>& samp_num,
    vcl_vector<double>& samp_denom,
    vcl_vector<double>& line_num,
    vcl_vector<double>& line_denom,
    vcl_vector<double>& scalex,
    vcl_vector<double>& scaley,
    vcl_vector<double>& scalez,
    vcl_vector<double>& scales,
    vcl_vector<double>& scalel,
    vcl_vector<double>& init_pt,
    vcl_vector<double>& rescales,
    vcl_vector<double>& rescalel,
    int scale_index,
    int offset_index) const
{
  bool success = false;

  // JUST DELEGATE TO METHOD IN UNDERLYING IMAGE SUBHEADER.

  if (image_subheader_ != static_cast<vil_nitf_image_subheader_sptr>(0)) {
    success = image_subheader_->get_rational_camera_data(
          samp_num, samp_denom, line_num, line_denom,
          scalex, scaley, scalez, scales, scalel, init_pt, rescales, rescalel,
          scale_index, offset_index);
  }
  return success;
}

////////////////////////////////////////////////////////
//:
// Get the upper left, upper right, lower right, lower left coordinates of an image
// as four vectors containing longitude, latitude and elevation for each corner.
// Used to verify rational camera class, but may be useful for other stuff.
//
//  \param UL vector to be filled in with longitude, latitude and elevation for upper left
//  \param UR vector to be filled in with longitude, latitude and elevation for upper right
//  \param LR vector to be filled in with longitude, latitude and elevation for lower right
//  \param LL vector to be filled in with longitude, latitude and elevation for lower left
//
//  \return true if vectors filled in successfully
//
////////////////////////////////////////////////////////
bool vil_nitf_image::get_image_corners(
    vcl_vector<double>& UL,
    vcl_vector<double>& UR,
    vcl_vector<double>& LR,
    vcl_vector<double>& LL) const
{
  bool success = false;

  // JUST DELEGATE TO METHOD IN UNDERLYING IMAGE SUBHEADER.

  if (image_subheader_ != static_cast<vil_nitf_image_subheader_sptr>(0)) {
    success = image_subheader_->get_image_corners(UL, UR, LR, LL);
  }
  return success;
}

void vil_nitf_image::set_nplanes(unsigned int new_val)
{
  nplanes_ = new_val;
}

void vil_nitf_image::set_nplanes(int new_val)
{
  if (new_val > -1) {
    nplanes_ = new_val;
  }
}

//====================================================================
//: Method to set padding at the top of the Image.
// The equation
// top_pad_ + nj_ + bottom_pad_ == get_num_blocks_y() * get_block_size_y()
// must hold.
// NOTE: this is in PIXELS, not bytes.
//====================================================================
void vil_nitf_image::set_top_pad(unsigned new_val)
{
  top_pad_ = new_val;
}

//====================================================================
//: Method to set padding at the bottom of the Image.
// The equation
// top_pad_ + nj_ + bottom_pad_ == get_num_blocks_y() * get_block_size_y()
// must hold.
// NOTE: this is in PIXELS, not bytes.
//====================================================================
void vil_nitf_image::set_bottom_pad(unsigned new_val)
{
  bottom_pad_ = new_val;
}

//====================================================================
//: Method to set padding at the top of the Image.
// The equation
// left_pad_ + ni_ + right_pad_ == get_num_blocks_x() * get_block_size_x()
// must hold.
// NOTE: this is in PIXELS, not bytes.
//====================================================================
void vil_nitf_image::set_left_pad(unsigned new_val)
{
  left_pad_ = new_val;
}

//====================================================================
//: Method to set padding at the top of the Image.
// The equation
// left_pad_ + ni_ + right_pad_ == get_num_blocks_x() * get_block_size_x()
// must hold.
// NOTE: this is in PIXELS, not bytes.
//====================================================================
void vil_nitf_image::set_right_pad(unsigned new_val)
{
  right_pad_ = new_val;
}

//====================================================================
//: Method to set the size in BYTES that each line of each block of the image must be a multiple of.
//====================================================================
void vil_nitf_image::set_row_modulus(int new_val)
{
  if (new_val >= 1) {
    row_modulus_ = new_val;
  }
}

//  Get interleave type of image.  TargetJr used attribute in Image super-class.
//  We will just use IMODE value from image subheader.

InterleaveType vil_nitf_image::get_interleave_type() const
{
  InterleaveType ret_val = NOT_INTERLEAVED;
  if (image_subheader_ != static_cast<vil_nitf_image_subheader_sptr>(0)) {
    ret_val = image_subheader_->IMODE_;
  }
  return ret_val;
}

///////////////////////////////////////////////////////////////
//:
// Compare value of image data offset (which was set after reading
// the message and image headers) and the value calculated by adding
//  the header lengths read from the file.
//
// \return calculated length - current file position.
//
///////////////////////////////////////////////////////////////
int vil_nitf_image::check_image_data_offset (vcl_ostream& out, vcl_string caller) const
{
  static vcl_string method_name = "vil_nitf_image::check_image_data_offset: ";

  // At this point, we should have read message header and first image header,
  // so file position should be equal to sum of length of these two headers.

  unsigned header_length_sum = message_header_->GetHeaderLength()
    + message_header_->get_image_header_length();

  // For now, assume file position is correct, because someone might have
  // mucked with the header.  Do put out warning message.

  int diff = header_length_sum - get_image_data_offset();

  if (diff != 0)
  {
    out << method_name;
    if (caller.length() > 0) {
      out << " from " << caller << ": ";
    }
    out << "WARNING:\n"
        << "  file position and calculated offset differ by "
        << diff << " bytes.\n"
        << "  Using current file position = " << get_image_data_offset() << ".\n"
        << "  message header length = "
        << message_header_->GetHeaderLength() << '\n'
        << "  image header length   = "
        << message_header_->get_image_header_length() << '\n'
        << "  total header length   = " << header_length_sum
        << vcl_endl;
  }
  else {
    if (debug_level > 1) {
        out << method_name;
        if (caller.length() > 0) {
          out << " from " << caller << ": ";
        }
        out << "file position and calculated header offset agree = "
            << get_image_data_offset() << vcl_endl;
    }
  }

  return diff;
}

unsigned int vil_nitf_image::get_block_size_x() const
{
  return blocking_info_.get_block_size_x();
}

unsigned int vil_nitf_image::get_block_size_y() const
{
  return blocking_info_.get_block_size_y();
}

unsigned int vil_nitf_image::get_num_blocks_x() const
{
  return blocking_info_.get_num_blocks_x();
}

unsigned int vil_nitf_image::get_num_blocks_y() const
{
 return blocking_info_.get_num_blocks_y();
}

unsigned int vil_nitf_image::get_image_length() const
{
  unsigned int image_length = 0;
  if (message_header_ != static_cast<vil_nitf_message_header_sptr>(0)) {
    image_length = message_header_->get_image_data_length();
  }
  return image_length;
}

///////////////////////////////////////////////////////////////////
//:  Display message header attributes.
///////////////////////////////////////////////////////////////////
void vil_nitf_image::display_message_attributes(vcl_string caller)
{
  static vcl_string method_name = "vil_nitf_image::display_message_attributes: ";

  if (message_header_ == static_cast<vil_nitf_message_header_sptr>(0)) {
    vcl_cout << method_name
             << "ERROR: Cannot message attributes.  Message header is null.\n";
  }
  else {
    message_header_->display_header_info(caller);
  }
}

///////////////////////////////////////////////////////////////////
//:  Display image attributes.
///////////////////////////////////////////////////////////////////
void vil_nitf_image::display_image_attributes(vcl_string caller)
{
  static vcl_string method_name = "vil_nitf_image::display_image_attributes: ";

  if (image_subheader_ == static_cast<vil_nitf_image_subheader_sptr>(0)) {
    vcl_cout << method_name
             << "ERROR: Cannot display image attributes.  Image subheader is null.\n";
  }
  else {
    image_subheader_->display_size_attributes(caller);
    vcl_cout << "bits_per_component = " << this->bits_per_component_ << vcl_endl
             << "pixel_format = " << this->pixel_format() << vcl_endl;
    this->display_block_attributes("");
  }
}

////////////////////////////////////////////////////////////////////
//:
// Return boolean value indicating if bytes need to be reversed
// because of big endian/little endian issues.

bool vil_nitf_image::reverse_bytes()
{
  // NITF BYTES ARE STORED IN BIG ENDIAN.
  // IF PLATFORM IS LITTLE ENDIAN, NEED TO REVERSE BYTES.
#if VXL_LITTLE_ENDIAN
  return true;
#else
  return false;
#endif
}

////////////////////////////////////////////////////////////////////
//:
// Reverse bytes in vxl_uint_16.  Used to convert little endian to
// big endian and vice versa.

vxl_uint_16 vil_nitf_image::reverse_bytes(vxl_uint_16 value) const
{
  vcl_string method_name = "vil_nitf_image::reverse_bytes: ";

  static const unsigned bytes_per_value = 2;

  vxl_uint_16 byte0_mask = 0x00FF;

  vxl_uint_16 inbytes[bytes_per_value];

  static const unsigned int DEBUG_LEVEL = 2;
  if (DEBUG_LEVEL < 2) {
    vcl_cout << method_name << "initial value as vxl_uint_16 = " << value
             << vcl_hex << " = 0x" << value << vcl_dec << vcl_endl;
  }

  for (unsigned int i = 0; i < bytes_per_value; ++i)
  {
    inbytes[i] = (value >> (8 * i)) & byte0_mask;
    if (DEBUG_LEVEL < 2) {
      vcl_cout << method_name << "inbytes[" << i << "] = 0x"
               << vcl_hex << inbytes[i] << vcl_dec << vcl_endl;
    }
  }

  vxl_uint_16 ret_value = 0;

  for (unsigned int i = 0; i < bytes_per_value; ++i)
  {
    int shift_count = 8 * ((bytes_per_value - 1) - i);
    vxl_uint_16 temp_long = (inbytes[i] << shift_count);
    ret_value = ret_value | temp_long;
  }

  if (DEBUG_LEVEL < 2) {
    vcl_cout << method_name << "return value as vxl_uint_16 = " << ret_value
             << vcl_hex << " = 0x" << ret_value << vcl_dec << vcl_endl;
  }

  return ret_value;
}  // end method reverse_bytes

////////////////////////////////////////////////////////////////////
//:
// Reverse bytes in buffer.  Used to convert little endian to
// big endian and vice versa.
//
// \param buffer buffer containing data for one or more values
// \param buf_len number of bytes in buffer
// \param bytes_per_value number of bytes per numeric value
//
void vil_nitf_image::reverse_bytes(
    unsigned char * buffer,
    unsigned long buf_len,
    unsigned int bytes_per_value) const
{
  vcl_string method_name = "vil_nitf_image::reverse_bytes: ";

  if (buf_len % bytes_per_value != 0) {
    vcl_cerr << method_name << "WARNING: number of bytes in buffer = "
             << buf_len << " is not a multiple of bytes per value = "
             << bytes_per_value << ".\n";
  }
  vcl_vector<unsigned char> temp_buf( bytes_per_value );

  unsigned long curr_pos = 0;

  unsigned count = 0;
  while (curr_pos < buf_len)
  {
    ++count;
    for (unsigned int i = 0; i < bytes_per_value; ++i) {
      temp_buf[i] = buffer[curr_pos + i];
    }
    for (unsigned int i = 0; i < bytes_per_value; ++i) {
      buffer[curr_pos + i] = temp_buf[bytes_per_value - i - 1];
    }

    bool debug_output = false;  // SET TO TRUE TO SEE OUTPUT
    if (count < 4 && debug_output)
    {
      bool not_zero = false;

      for (unsigned int i = 0; i < bytes_per_value; ++i) {
        vxl_uint_16 uint_val = vxl_uint_16(temp_buf[i]);
        if (uint_val > 0) {
          not_zero = true;
        }
      }

      if (not_zero)
      {
        vcl_cout << method_name << "curr_pos = " << curr_pos
                 << "  initial value = " << vcl_hex;
        for (unsigned int i = 0; i < bytes_per_value; ++i) {
          vxl_uint_16 uint_val = vxl_uint_16(temp_buf[i]);
          vcl_cout << ' ' << uint_val;
        }
        vcl_cout << vcl_endl;

        vcl_cout << "  reversed value = ";
        for (unsigned int i = 0; i < bytes_per_value; ++i) {
          vxl_uint_16 uint_val = vxl_uint_16(buffer[curr_pos + i]);
          vcl_cout << ' ' << uint_val;
        }
        vcl_cout << vcl_dec << vcl_endl;
      }  // end if (not_zero)
    }  // end if (count < 4)

    curr_pos += bytes_per_value;
  }  // end while (curr_pos < buf_len)
}  // end method reverse_bytes

////////////////////////////////////////////////////////////////////
//
//: Check high order byte value.
// NOTE: Right now, this is just for checking first byte of 11 bit value
//    stored as 16 bits in 2 bytes.
//
// \param buffer buffer containing data for one or more values
// \param buf_len number of bytes in buffer
// \param bytes_per_value number of bytes per numeric value
//
// \return true if any bits other than 3 least significant bits in first byte are being used.
//
bool vil_nitf_image::using_upper_bits(
    unsigned char * buffer,
    unsigned long buf_len,
    unsigned int bytes_per_value,
    unsigned block_row,
    unsigned block_col) const
{
  vcl_string method_name = "vil_nitf_image::using_upper_bits: ";

  bool using_upper_bits = false;

  if (buf_len % bytes_per_value != 0) {
    vcl_cerr << method_name << "WARNING: number of bytes in buffer = "
             << buf_len << " is not a multiple of bytes per value = "
             << bytes_per_value << ".\n";
  }
  unsigned long curr_pos = reverse_bytes() ? 1 : 0;

  vxl_uint_16 byte_mask = 0x0007;  // MASK FOR LOWER THREE BITS OF BYTE.

  while (curr_pos < buf_len)
  {
    vxl_uint_16 uint_val = vxl_uint_16(buffer[curr_pos]);
    vxl_uint_16 masked_val = uint_val & byte_mask;

    if ((uint_val > 0) && (uint_val != masked_val))
    {
      using_upper_bits = true;

      vcl_cout << method_name
               << "block_row = " << block_row
               << "  block_col = " << block_col
               << "  curr_pos = " << curr_pos
               << vcl_hex << "  mask = " << byte_mask
               << "  value = " << vcl_hex
               << uint_val << "  masked value = "
               << masked_val << vcl_dec << vcl_endl;
    }

    curr_pos += bytes_per_value;
  }  // end while (curr_pos < buf_len)

  return using_upper_bits;
}  // end method using_upper_bits

////////////////////////////////////////////////////////////////////
//
//: Check high order byte value.
// NOTE: Right now, this is just for checking first byte of 11 bit value
//    stored as 16 bits in 2 bytes.
//    This method uses raw data, which is stored as big endian.
//    Cannot be used after bytes reversed to little endian.
//
// \param buffer buffer containing data for one or more values
// \param buf_len number of bytes in buffer
// \param bytes_per_value number of bytes per numeric value
//
// \return true if any bits other than 3 least significant bits in first byte are being used.
//
vxl_uint_16 vil_nitf_image::check_max_value(
    unsigned char * buffer,
    unsigned long buf_len,
    unsigned int bytes_per_value,
    unsigned block_row,
    unsigned block_col) const
{
  vcl_string method_name = "vil_nitf_image::check_max_value: ";

  vxl_uint_16 max_value = 0;

  if (buf_len % bytes_per_value != 0) {
    vcl_cerr << method_name << "WARNING: number of bytes in buffer = "
             << buf_len << " is not a multiple of bytes per value = "
             << bytes_per_value << ".\n";
  }
  unsigned long curr_pos = 0;

  vxl_uint_16 byte_mask = 0x0007;

  while (curr_pos < buf_len)
  {
    vxl_uint_16 uint_val = vxl_uint_16(buffer[curr_pos]);
    vxl_uint_16 masked_val = uint_val & byte_mask;

    if ((uint_val > 0) && (uint_val != masked_val))
    {
      vcl_cout << method_name
               << "block_row = " << block_row
               << "  block_col = " << block_col
               << "  curr_pos = " << curr_pos
               << " first byte using upper five bytes.\n"
               << vcl_hex << "  mask = " << byte_mask
               << "  value = " << vcl_hex
               << uint_val << "  masked value = "
               << masked_val << vcl_endl
               << vcl_dec << vcl_endl;
    }
    else {
      uint_val = (vxl_uint_16(buffer[curr_pos]) << 8) + vxl_uint_16(buffer[curr_pos + 1]);
      if (uint_val > max_value) {
#if 0
        vcl_cout << method_name
                 << "block_row = " << block_row
                 << "  block_col = " << block_col
                 << "  curr_pos = " << curr_pos
                 << " old max value = " << max_value
                 << " new max value = " << uint_val
                 << vcl_endl;
#endif
        max_value = uint_val;
      }
    }

    curr_pos += bytes_per_value;
  }  // end while (curr_pos < buf_len)

    return max_value;
}  // end method check_max_value

////////////////////////////////////////////////////////////////////
//:  Display image pixels. (For image with vxl_uint_16 values only.)
///////////////////////////////////////////////////////////////////
void vil_nitf_image::display_image_values(
    unsigned long row,
    unsigned long column,
    unsigned pixels_per_row,
    unsigned char * buffer,
    const vil_image_view<vxl_uint_16>& image_view,
    unsigned int pixel_count) const
{
  static vcl_string method_name = "vil_nitf_image::display_image_values: ";

  vxl_uint_16 mask = 0xF800;

  unsigned long bytes_per_pixel = vil_pixel_format_sizeof_components(this->pixel_format());
  unsigned long row_offset = row * pixels_per_row * bytes_per_pixel;
  unsigned long col_offset = column * bytes_per_pixel;
  unsigned long image_offset = row_offset + col_offset;

  vcl_cout << method_name << "display first " << pixel_count
           << " pixel values starting with i = "
           << row << " and j = " << column << vcl_endl;

  unsigned value_error_count = 0;
  unsigned invalid_error_count = 0;

  for (unsigned int i = 0; i < pixel_count; ++i)
  {
    unsigned int offset = i + column;
    vxl_uint_16 view_pixel_value = image_view(row, offset, 0);
    vxl_uint_16 buffer_pixel_value = (vxl_uint_16(buffer[image_offset + i]) << 8)
                                    + vxl_uint_16(buffer[image_offset + i + 1]);

    vxl_uint_16 masked_value = view_pixel_value & mask;
    if (masked_value > 0)
    {
      vcl_cout << "ERROR: upper five bits should be zero.\n"
               << method_name << "i = " << i
               << " view value = " << vcl_hex << view_pixel_value
               << " masked value = " << masked_value << vcl_dec << vcl_endl;
      ++invalid_error_count;
    }
    if (view_pixel_value != buffer_pixel_value)
    {
      vcl_cout << "ERROR: value from image and value from buffer not equal.\n"
               << method_name << "i = " << i
               << " view value = " << vcl_hex << view_pixel_value
               << " buffer value = " << buffer_pixel_value << vcl_dec << vcl_endl;

      ++value_error_count;
    }

    reverse_bytes(view_pixel_value);
  }  // end for i

  vcl_cout << method_name << "invalid error count = " << invalid_error_count
           << "  value error count = " << value_error_count << vcl_endl;
}  // end method display_image_values

////////////////////////////////////////////////////////////////////
//:  Display image pixels. (For image with vxl_uint_16 values only.)
///////////////////////////////////////////////////////////////////
unsigned int vil_nitf_image::check_image_values(
    unsigned long row,
    unsigned long column,
    unsigned long row_offset,
    unsigned char * read_buffer,
    vil_memory_chunk_sptr mem_chunk,
    unsigned int pixel_count) const
{
  static vcl_string method_name = "vil_nitf_image::check_image_values: ";

  unsigned int error_count = 0;

  for (unsigned int i = 0; i < pixel_count; ++i)
  {
    unsigned int offset = i + column;
    // THIS IS BIG ENDIAN.  WHY DOES THIS WORK ON THE PC, WHICH SHOULD BE LITTLE ENDIAN ??
    //  MAL 5oct2003
    vxl_uint_16 buffer_pixel_value = (vxl_uint_16(read_buffer[i]) << 8)
                                    + vxl_uint_16(read_buffer[i + 1]);

    unsigned char * image_offset = (unsigned char *) mem_chunk->data()
                                   + row_offset + offset;
    vxl_uint_16 view_pixel_value = (vxl_uint_16(*image_offset) << 8)
                                  + vxl_uint_16(*(image_offset + 1));

    if (view_pixel_value != buffer_pixel_value)
    {
      vcl_cout << method_name
               << "ERROR: value from read_buffer and value from memory chunk not equal.\n"
               << method_name << "row = " << row << "  column = " << offset
               << " view value = " << vcl_hex << view_pixel_value
               << " buffer value = " << buffer_pixel_value << vcl_dec << vcl_endl;

      ++error_count;
    }
  }  // end for i

  if (error_count > 0)
  {
    vcl_cout << method_name
             << "pixel values starting with i = "
             << row << " and j = " << column
             << " error count = " << error_count << vcl_endl;
  }

  return error_count;
}  // end method check_image_values

////////////////////////////////////////////////////////////////////
//:  Display attributes related to blocking.
///////////////////////////////////////////////////////////////////
void vil_nitf_image::display_block_attributes(vcl_string caller)
{
  static vcl_string method_name = "vil_nitf_image::display_block_attributes: ";

  vcl_cout << method_name;
  if (caller.length() > 0) {
    vcl_cout << "from " << caller << ":\n";
  }
  vcl_cout << "  num blocks X = " << get_num_blocks_x()
           << "  block size X = " << get_block_size_x() << vcl_endl
           << "  num blocks Y = " << get_num_blocks_y()
           << "  block size Y = " << get_block_size_y() << vcl_endl
           << "  right pad  = " << get_right_pad() << vcl_endl
           << "  bottom pad = " << get_bottom_pad() << vcl_endl;
}


////////////////////////////////////////////////////////////////////
//
//: Compare bytes in two buffers.
//
// \param buffer_1 buffer containing first set ofraw bytes
// \param buffer_2 buffer containing second set ofraw bytes
// \param compare_count number of values to compare
// \param bytes_per_pixel number of bytes per pixel
// \param label optional string to display explaining meaning of byes.
//
// \return number of bytes which are not equal
//
////////////////////////////////////////////////////////////////////
unsigned long vil_nitf_image::compare_bytes(
    const unsigned char * buffer_1,
    const unsigned char * buffer_2,
    unsigned long compare_count,
    unsigned int bytes_per_value,
    vcl_string label) const
{
  static vcl_string method_name = "vil_nitf_image::compare_bytes: ";

    unsigned short temp_short;
    unsigned long err_count = 0;

    unsigned long loop_count = compare_count * bytes_per_value;

    for (unsigned count = 0; count < loop_count; ++count)
    {
      if (buffer_1[count] != buffer_2[count])
      {
        ++err_count;
        if (err_count == 1) {
          vcl_cout << method_name;
          if (label.length() > 0) {
            vcl_cout << label << " - ";
          }
          vcl_cout << vcl_endl;
        }

        temp_short = (unsigned short) buffer_1[count];
        vcl_cout << "ERROR: Difference at position " << count << vcl_endl
                 << "  buffer 1 byte = " << vcl_hex << temp_short;
        temp_short = (unsigned short) buffer_2[count];
        vcl_cout << "  buffer 2 byte = " << temp_short << vcl_dec << vcl_endl;
      }  // end if (buffer_1[count] != buffer_2[count])
    }  // end for count

    vcl_cout << vcl_endl;

    // RESET OUTPUT FORMAT
    vcl_cout << vcl_dec << vcl_endl;
#if 0
    if (err_count > 0) {
      display_as_hex(buffer_1, compare_count, bytes_per_value, "buffer_1");
      display_as_hex(buffer_2, compare_count, bytes_per_value, "buffer_2");
    }
#endif
    return err_count;
}  // end method

