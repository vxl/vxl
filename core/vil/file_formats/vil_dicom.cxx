// This is core/vil/file_formats/vil_dicom.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil_dicom.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h>

#include <vxl_config.h> // for VXL_BIG_ENDIAN and vxl_byte

#include <vil/vil_property.h>
#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>


char const* vil_dicom_format_tag = "dicom";


vil_image_resource_sptr vil_dicom_file_format::make_input_image(vil_stream* vs)
{
  vil_dicom_header_format dhf;
  if (dhf.isDicomFormat(*vs))
    return new vil_dicom_image(vs);
  else
    return 0;
}

vil_image_resource_sptr vil_dicom_file_format::make_output_image(vil_stream* /*vs*/,
                                                                 unsigned /*ni*/,
                                                                 unsigned /*nj*/,
                                                                 unsigned /*nplanes*/,
                                                                 vil_pixel_format )
{
  vcl_cerr << "ERROR: vil_dicom_file doesn't support output yet\n";
  return 0;
#if 0

  if (nplanes != 1 || format != VIL_PIXEL_FORMAT_INT_32)
  {
    vcl_cerr << "ERROR: vil_dicom_file_format::make_output_image\n"
             << "       Can only create DICOM images with a single plane\n"
             << "       and 32-bit integer pixels\n";
    return 0;
  }
  return new vil_dicom_image(vs, ni, nj, nplanes, format);
#endif
}

char const* vil_dicom_file_format::tag() const
{
  return vil_dicom_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

bool isEncapsulated(vil_dicom_header_image_type im_type)
{
  switch (im_type)
  {
  case VIL_DICOM_HEADER_DITUNKNOWN:
    return false;  // This should be plain DICOM
  case VIL_DICOM_HEADER_DITJPEGBASE:
  case VIL_DICOM_HEADER_DITJPEGEXTLOSSY:
  case VIL_DICOM_HEADER_DITJPEGSPECNH:
  case VIL_DICOM_HEADER_DITJPEGFULLNH:
  case VIL_DICOM_HEADER_DITJPEGLOSSLNH:
  case VIL_DICOM_HEADER_DITJPEGEXTHIER:
  case VIL_DICOM_HEADER_DITJPEGSPECHIER:
  case VIL_DICOM_HEADER_DITJPEGFULLHIER:
  case VIL_DICOM_HEADER_DITJPEGLOSSLHIER:
  case VIL_DICOM_HEADER_DITJPEGLOSSLDEF:
    return true;
  default:
    return false;
  }
}


void imageSwap(char* in_im, int num_bytes,
               vil_dicom_header_info dhi)
{
  char swaps[2];
  int row_num;

  // Only swap if two bytes used and the endians of system and
  // file differ
  //Note may also need to swap to do some bit shift transforms in some OW modes when using Big Endian
  //Then may reverse swap (eg. when both system and file are big-endian).
  // This appears to be the implication of DICOM standard PS3.5-2001 where for Pixel data
  //the bits allocated, used, and high bit are defined in "virtual" Little Endian prior to potential swapping pre-transfer.
  // See op cit Annex D.
  //Note that post-shifting the bits_stored are the low order bits of the word (viewed as contiguous in Little-Endian)
  //So in packed schemes such as bits_allocated=12 with bits_stored=10, and high_bit=11, the relevant 10 bits are down-shifted by 2.
  // Martin Roberts
  //-------------------------  !!!!!!!!!!!!!!! -------------------------------------
  //!!! Note this code will NOT cope with odd packing schemes across multiple words
  //  (e.g. bits stored=18, high_bit =19, split across 3 bytes) !!
  //-------------------------  !!!!!!!!!!!!!!! -------------------------------------

  if (num_bytes != 2) return;
  bool bSystemReqSwap = dhi.file_endian_ != dhi.sys_endian_; //Need a swap for file to system
  //We may also need to do some shifting where the bit range is some sub-part of a two-byte word
  bool bShiftNeeded =  ((dhi.res_slope_ == VIL_DICOM_HEADER_DEFAULTSLOPE) &&
                        ( (dhi.high_bit_ < dhi.allocated_bits_-1) || //some unused high-bits that may be set
                          (dhi.high_bit_ > dhi.stored_bits_-1)));    //or the relevant sub-portion does not start at bit 0
  //But the shifting needs to be done with a little-endian ordering
  bool bShiftReqSwap = bShiftNeeded && (dhi.file_endian_ == VIL_DICOM_HEADER_DEBIGENDIAN);

  bool bSwap = bSystemReqSwap || bShiftReqSwap;

  if ( bSwap || bShiftNeeded)
  {
    // Read two bytes at a time, swapping and/or shifting unused bits away
    for (int i=0; i<dhi.dimy_; i++)
    {
      row_num = (dhi.dimx_*num_bytes)*i;

      for (int j=0; j<dhi.dimx_*num_bytes; j+=num_bytes)
      {
        if (bShiftReqSwap) //Ensure Little-Endian while we shift
        {
          swaps[0] = in_im[row_num+j];
          swaps[1] = in_im[row_num+(j+1)];
          in_im[row_num+j]=swaps[1];
          in_im[row_num+(j+1)]=swaps[0];
        }

        if (bShiftNeeded)
        {
          char* pRaw = in_im+row_num+j;
          vxl_uint_16* pWord =  reinterpret_cast<vxl_uint_16*>(pRaw);
          vxl_uint_16 pixval = *pWord;
          short bitshift = dhi.allocated_bits_ - dhi.high_bit_ - 1;
          //Potential shifts to remove unused (overlay) high order bits
          if (bitshift>0)
          {
            //Zero any unused high order bits - these may not be zero in the data as they can include overlays
            //Shift left and back right to zero the high order bits at the left end
            pixval<<=bitshift;
            pixval>>=bitshift;
          }
          bitshift = dhi.high_bit_ - short(dhi.stored_bits_+1.0E-12) -1; //Not sure why stored_bit is a float!
          if (bitshift>0)
          {
            //The bits used do not start at the first so bit-shift right to make first used bit the first in word
            pixval>>= bitshift;
          }
          *pWord = pixval;
        }
        //And finally may need to revert the swap if it was done just for the bit shift on Big-Endian,
        // or we impose the system swap if we just shifted without a pre-swap
        if (bShiftReqSwap != bSystemReqSwap)
        {
          swaps[0] = in_im[row_num+j];
          swaps[1] = in_im[row_num+(j+1)];
          in_im[row_num+j]=swaps[1];
          in_im[row_num+(j+1)]=swaps[0];
        }
      }
    }
  }
}


char* convert12to16(char* im, vil_dicom_header_info dhi,
                    bool del_old=true)
{
  int new_im_size=(dhi.dimx_*2) * dhi.dimy_;
  char* new_im=new char[new_im_size];
  int rowstep = dhi.dimx_*2;
  int curr_row;

  if (new_im)
  {
    // Count through the image converting three 16 bit values
    // to four 12 bit values
    for (int i=0; i<dhi.dimy_; i++)
    {
      curr_row = i*rowstep;

      for (int k=0, j=0; k<2*dhi.dimx_; j+=3)
      {
        new_im[curr_row+k] = im[curr_row+j];
        k++;
        new_im[curr_row+k] = im[curr_row+(j+1)] % 16;
        k++;
        new_im[curr_row+k] = im[curr_row+(j+1)]/16+
                             (im[curr_row+(j+2)] % 16)*16;
        k++;
        new_im[curr_row+k] = im[curr_row+(j+2)]/16;
        k++;
      }
    }
  }
  else
    new_im = im;

  if (del_old)
  {
    new_im = NULL;
    delete [] im;
  }

  return new_im;
}


bool checkReadableFormat(vil_dicom_header_image_type im_type)
{
  bool retval;
  vcl_string type;

  switch (im_type)
  {
  case VIL_DICOM_HEADER_DITUNKNOWN:      type = "Unknown or Plain DICOM";
                retval=true; // This should be plain DICOM
                break;
  case VIL_DICOM_HEADER_DITJPEGBASE:    type = "Baseline JPEG (Process 1)";
                retval=false;
                break;
  case VIL_DICOM_HEADER_DITJPEGEXTLOSSY:  type = "Extended JPEG (Processes 2, 3, 4 & 5)";
                retval=false;
                break;
  case VIL_DICOM_HEADER_DITJPEGSPECNH:    type = "Spectral Selection non-hierarchical JPEG (Processes 6, 7, 8 and 9)";
                retval=false;
                break;
  case VIL_DICOM_HEADER_DITJPEGFULLNH:    type = "Full Progression non-hierarchical JPEG (Processes 10, 11, 12 and 13)";
                retval=false;
                break;
  case VIL_DICOM_HEADER_DITJPEGLOSSLNH:    type = "Lossless non-hierarchical JPEG (Processes 14 and 15)";
                retval=false;
                break;
  case VIL_DICOM_HEADER_DITJPEGEXTHIER:    type = "Extended hierarchical JPEG (Processes 16, 17, 18 and 19)";
                retval=false;
                break;
  case VIL_DICOM_HEADER_DITJPEGSPECHIER:  type = "Spectral Selection hierarchical JPEG (Processes 20, 21, 22 and 23)";
                retval=false;
                break;
  case VIL_DICOM_HEADER_DITJPEGFULLHIER:  type = "Full Progression hierarchical JPEG (Processes 24, 25, 26 and 27)";
                retval=false;
                break;
  case VIL_DICOM_HEADER_DITJPEGLOSSLHIER:  type = "Lossless hierarchical JPEG (Processes 28 and 29)";
                retval=false;
                break;
  case VIL_DICOM_HEADER_DITJPEGLOSSLDEF:  type = "Lossless non-hierarchical default (Process 14 [Selection Value 1])";
                retval=true;
                break;
  default:          type = "Undefined";
                retval=false;
                break;
  }

  if (!retval)
    vcl_cerr << "CW_DicomFormat - Image type is " << type.c_str() << ", but is not yet supported.\n";

  return retval;
}


vil_dicom_image::vil_dicom_image(vil_stream* vs):
  vs_(vs)
{
  vs_->ref();

  vil_dicom_header_format dhf;
  header_ = dhf.readHeader(*vs);
  start_of_pixels_ = vs->tell();
}

bool vil_dicom_image::get_property(char const* tag, void* value) const
{
  if (vcl_strcmp(vil_property_quantisation_depth, tag)==0)
  {
    unsigned* depth =  static_cast<unsigned*>(value);
    *depth = header_.allocated_bits_;
    return true;
  }

  if (vcl_strcmp(vil_property_pixel_size, tag)==0)
  {
    float *pixel_size = static_cast<float*>(value);
    pixel_size[0] = header_.xsize_;
    pixel_size[0] = header_.ysize_;
    return true;
  }

  // Need to write lots of access code for the dicom header.
  return false;
}

char const* vil_dicom_image::file_format() const
{
  return vil_dicom_format_tag;
}

vil_dicom_image::vil_dicom_image(vil_stream* vs, unsigned ni, unsigned nj,
                                 unsigned nplanes, vil_pixel_format format):
  vs_(vs)
{
  assert(!"vil_dicom_image doesn't yet support output");

  vs_->ref();

  assert(nplanes == 1 && format == VIL_PIXEL_FORMAT_INT_32);
  header_.dimx_=ni;
  header_.dimy_=nj;
  header_.dimz_=1;
}

vil_dicom_image::~vil_dicom_image()
{
  //delete vs_;
  vs_->unref();
}


enum vil_pixel_format vil_dicom_image::pixel_format() const
{
  unsigned bytes_read;
  if (header_.allocated_bits_ == 16 ||
      header_.allocated_bits_ == 12)
    bytes_read = 2;
  else
    bytes_read = 1;

  if (header_.res_slope_ == VIL_DICOM_HEADER_DEFAULTSLOPE)
    if (header_.pix_rep_ == 0)
      if (bytes_read == 2)
        return VIL_PIXEL_FORMAT_UINT_16;
      else
        return VIL_PIXEL_FORMAT_BYTE;
    else
      if (bytes_read == 2)
        return VIL_PIXEL_FORMAT_INT_16;
      else
        return VIL_PIXEL_FORMAT_SBYTE;
  else return VIL_PIXEL_FORMAT_FLOAT;
}


vil_image_view_base_sptr vil_dicom_image::get_copy_view(
  unsigned x0, unsigned nx, unsigned y0, unsigned ny) const
{
  if (x0+nx > ni() || y0+ny > nj()) return 0;

  void* void_im=0;
  int bytes_read=1;
  bool readable = checkReadableFormat(header_.image_type_);

  vs_->seek(start_of_pixels_);

  // vs_ should point at the start of the data
  if (!vs_->ok() || !readable)
  {
    delete [] (char*) void_im;
    return 0;
  }

  // If it's an encapsulated method, call the encapsulated reader,
  // otherwise just read the data
  if (isEncapsulated(header_.image_type_))
  {
    vcl_cerr<< "ERROR: vil_dicom_image::get_copy_view\n"
            << "       Can't read DICOM images with encapsulated image types\n";
    return 0;
  }

  // Get the number of rows and columns to read
  int cols=header_.dimx_;
  int rows=header_.dimy_;

  // The number of bytes to read at a time depends on the
  // allocated bits. If 16 or 12 are allocated, then two bytes
  // should be read (with a reduced number of reads for 12
  // bits as each value only takes up 3/4 of the actual number
  // of bits (16) read). Otherwise, one byte at a time is read

  if (header_.allocated_bits_ == 16 ||
      header_.allocated_bits_ == 12)
    bytes_read = 2;

  // If 12 bits allocated, read enough sets of 16 bits
  if (header_.allocated_bits_ == 12)
    cols = (int) (3.0*(cols/4));

  unsigned mem_size = (cols*bytes_read)*rows;
  void_im = new char [mem_size];
  vs_->read(void_im,mem_size);

  if (!vs_->ok())
  {
    delete [] (char*)void_im;
    return 0;
  }


  // Convert the image to be floats

  // First, if it's 12 bit convert to 16
  if (header_.allocated_bits_ == 12)
    void_im = (void*)convert12to16((char*)void_im, header_);

  // Do any swapping necessary (also may do any bit shifting needed if part-words are being used
  imageSwap((char*)void_im,bytes_read,header_);

  if (header_.res_slope_ == VIL_DICOM_HEADER_DEFAULTSLOPE)
    if (header_.pix_rep_ == 0) // unsigned
      if (bytes_read == 2) // vxl_uint_16
      {
        vil_image_view<vxl_uint_16> view(nx, ny);
        for (unsigned i=y0; i<(y0+ny); ++i)
        {
          int next_row = header_.dimx_*i;
          for (unsigned j=x0; j<(x0+nx); ++j)
            view(j-x0,i-y0) = static_cast<vxl_uint_16*>(void_im)[next_row+j];
        }
        delete [] (char*) void_im;
        return new vil_image_view<vxl_uint_16>(view);
      }
      else // byte
      {
        vil_image_view<vxl_byte> view(nx, ny);
        for (unsigned i=y0; i<ny; i++)
        {
          int next_row = header_.dimx_*i;
          for (unsigned j=x0; j<nx; j++)
            view(j-x0,i-y0) = static_cast<vxl_byte*>(void_im)[next_row+j];
        }
        delete [] (char*) void_im;
        return new vil_image_view<vxl_byte>(view);
      }
    else // signed
      if (bytes_read == 2) // vxl_int_16
      {
        vil_image_view<vxl_int_16> view(nx, ny);
        for (unsigned i=y0; i<ny; i++)
        {
          int next_row = header_.dimx_*i;
          for (unsigned j=x0; j<nx; j++)
            view(j-x0,i-y0) = static_cast<vxl_int_16*>(void_im)[next_row+j];
        }
        delete [] (char*) void_im;
        return new vil_image_view<vxl_int_16>(view);
      }
      else // vxl_sbyte
      {
        vil_image_view<vxl_sbyte> view(nx, ny);
        for (unsigned i=y0; i<ny; i++)
        {
          int next_row = header_.dimx_*i;
          for (unsigned j=x0; j<nx; j++)
            view(j-x0,i-y0) = static_cast<vxl_sbyte*>(void_im)[next_row+j];
        }
        delete [] (char*) void_im;
        return new vil_image_view<vxl_sbyte>(view);
      }
  else // floating point image.
  {
    vil_image_view<float> view(nx, ny);
    float f;
    for (unsigned i=y0; i<ny; i++)
    {
      int next_row = header_.dimx_*i;
      for (unsigned j=x0; j<nx; j++)
      {
        if (header_.pix_rep_ == 0) // unsigned data
          if (bytes_read == 2) // 2 byte data
            f = static_cast<vxl_uint_16*>(void_im)[next_row+j];
          else // 1 byte data
            f = static_cast<vxl_byte*>(void_im)[next_row+j];
        else // signed data
          if (bytes_read == 2)// 2 byte data
            f = static_cast<vxl_int_16*>(void_im)[next_row+j];
          else // 1 byte data
            f = static_cast<vxl_sbyte*>(void_im)[next_row+j];

        view(j,i) = f * header_.res_slope_ + header_.res_intercept_;
      }
    }
    delete [] (char*)void_im;

    return new vil_image_view<float>(view);
  }
}


bool vil_dicom_image::put_view(const vil_image_view_base& view,
                               unsigned x0, unsigned y0)
{
  assert(!"vil_dicom_image doesn't yet support output yet");

  if (!view_fits(view, x0, y0))
  {
    vcl_cerr << "ERROR: " << __FILE__ << ": view does not fit\n";
    return false;
  }
  return false;
}

#if 0 // remaining of this file commented out
//==================================================================
// readEncapsulatedData
//==================================================================
bool vil_dicom_image::readEncapsulatedData(vimt_image_2d_of<vxl_int_32>&im,
                                           vil_dicom_header_info head_info,
                                           vcl_ifstream &fs)
{
  // We should now be at the start of the encapsulated data.
  // This needs reading as a series of delimeted blocks, each
  // starting with a group/element of delimeter group and
  // delimeter item (0xFFFE, 0xE000). This is followed
  // by the length of the data block (no VR is used). After the
  // series of the items, the pixel data block terminates with
  // the delimeter group and an element of sequence delimeter
  // (0xFFFE, 0xE0DD). All these tags need stripping from the
  // data, and the remaining data sent to a decoder.

  // First set up a data buffer
  unsigned char* data;
  int data_size, num_bytes;

  unsigned short group=0, element=0;  // Each group and element read from
                                  // between the actual data
  unsigned int length=0;   // The length of the next data block
  unsigned long tot_len=0; // The final length of the data
  unsigned int off_entry=0; // For disposing of the offset table entries

  // We can make the data the same as the uncompressed image size
  // so we're sure all the data will fit to save constant resizing

  // Get the number of rows and columns to read
  int cols=head_info.dimx_;
  int rows=head_info.dimy_;

  // The size of the finished data has to be multiplied by
  // the number of bytes - just to make sure.
  if (head_info.allocated_bits_ == 16 ||
      head_info.allocated_bits_ == 12)
    num_bytes = 2;
  else
    num_bytes = 1;

  // If 12 bits allocated, use the right number of 16 bits
  if (head_info.allocated_bits_ == 12)
    cols = 3.0*cols/4;

  data_size = (cols*num_bytes)*rows;
  data = new unsigned char [data_size];

  // Now we're ready to read

  // First ignore the offset table!
  // This may cause some problems if we come across a
  // multiframe image!
  fs.read((char*)&group, sizeof(unsigned short));
  fs.read((char*)&element, sizeof(unsigned short));

  group = shortSwap(group);
  element = shortSwap(element);

  if (group   == CW_DICOM_DELIMITERGROUP &&
      element == CW_DICOM_DLITEM)
  {
    fs.read((char*)&length, sizeof(unsigned int));
    length = intSwap(length);

    // We need to check that the offsets are divisible
    // by 4, otherwise there's and error
    if (length%4 == 0)
    {
      // Now throw away any offset table entries - not
      // needed!
      while (length)
      {
        fs.read((char*)&off_entry, sizeof(unsigned int));
        length -= 4;
      }
    }
    else // Problem with the offset table
      return false;
  }
  else // Problem with the group and element read
    return false;

  // If all's ok, read the image in
  return getDataFromEncapsulation(&data, tot_len, fs);
}

//==================================================================
// getDataFromEncapsulation
//==================================================================
bool vil_dicom_image::getDataFromEncapsulation(unsigned char** data, unsigned long &data_len,
                                               vcl_ifstream &fs)
{
  bool result = true;

  unsigned short group=0, element=0; // The next group and element read
  unsigned int length=0;           // The length of the next data block

  data_len = 0;

  // First read a group and element
  fs.read((char*)&group, sizeof(unsigned short));
  fs.read((char*)&element, sizeof(unsigned short));

  group = shortSwap(group);
  element = shortSwap(element);

  while ((group == CW_DICOM_DELIMITERGROUP &&
       element == CW_DICOM_DLITEM) &&
       result)
  {
    // Read the length
    fs.read((char*)&length, sizeof(unsigned int));
    length = intSwap(length);

    // Now read the data
    fs.read((char*)&((*data)[data_len]), length);

    if (!fs.good())
    {
      result = false;
    }
    else
    {
      // Increase the data length
      data_len += length;

      // Read the next data group and element
      fs.read((char*)&group, sizeof(unsigned short));
      fs.read((char*)&element, sizeof(unsigned short));

      group = shortSwap(group);
      element = shortSwap(element);
    }
  }

  if (result)
  {
    if (group   == CW_DICOM_DELIMITERGROUP &&
        element == CW_DICOM_DLSEQDELIMITATIONITEM)
    {
      // Just read the length and all's ok!
      fs.read((char*)&length, sizeof(unsigned int));
      length = intSwap(length);

      // Try writing it out!
      vcl_ofstream ost("C:\\testout.jpg",vcl_ios_binary);
      ost.write((char*)(*data),data_len);
      ost.close();
    }
    else
    {
      // Something's gone wrong
      result = false;
    }
  }

  return result;
}

//===============================================================
// shortSwap
//===============================================================
short vil_dicom_image::shortSwap(short short_in)
{
  // Only swap if the architecture is different to the
  // file (the logic means that if one is unknown it swaps,
  // if both are unknown, it doesnt)
  if (headerInfo().file_endian_ != headerInfo().sys_endian_)
  {
    // Create a short unioned with two chars
    union short_char
    {
      short short_val;
      char byte_val[2];
    } short_swap;

    // Set the swapper
    short_swap.short_val = short_in;

    // Swap them over
    char temp = short_swap.byte_val[0];
    short_swap.byte_val[0]=short_swap.byte_val[1];
    short_swap.byte_val[1]=temp;

    short_in = short_swap.short_val;
  }

  return short_in;
}

//===============================================================
// intSwap
//===============================================================
int vil_dicom_image::intSwap(int int_in)
{
  // Only swap if the architecture is different to the
  // file (the logic means that if one is unknown it swaps,
  // if both are unknown, it doesnt)
  if (headerInfo().file_endian_ != headerInfo().sys_endian_)
  {
    // Create an int unioned with four chars
    union int_char
    {
      int int_val;
      char byte_val[4];
    } int_swap;

    // Set the swapper
    int_swap.int_val = int_in;

    // Swap them over (end ones first)
    char temp = int_swap.byte_val[0];
    int_swap.byte_val[0]=int_swap.byte_val[3];
    int_swap.byte_val[3]=temp;

    // Now the middle ones
    temp = int_swap.byte_val[1];
    int_swap.byte_val[1] = int_swap.byte_val[2];
    int_swap.byte_val[2] = temp;

    int_in = int_swap.int_val;
  }

  return int_in;
}

//===============================================================
// charSwap
//===============================================================
void vil_dicom_image::charSwap(char* char_in, int val_size)
{
  // Only swap if the architecture is different to the
  // file (the logic means that if one is unknown it swaps,
  // if both are unknown, it doesnt)
  if (headerInfo().file_endian_ != headerInfo().sys_endian_)
  {
    // Create a char the same size to swap
    char* temp = new char[val_size];

    if (temp)
    {
      // Copy from the first vcl_string into the temp
      for (int i=0; i<val_size; i++)
        temp[i]=char_in[i];

      // Now put back in reverse
      for (int i=0; i<val_size; i++)
        char_in[(val_size-i)-1] = temp[i];

      delete [] temp;
    }
    else
    {
      vcl_cerr << "Couldn't create temp in charSwap!\n"
               << "Value remains unswapped!\n";
    }
  }
}

#endif // 0
