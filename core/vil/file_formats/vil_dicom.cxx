// This is core/vil2/file_formats/vil2_dicom.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil2_dicom.h"

#include <vcl_cassert.h>
#include <vcl_cstdio.h> // for sprintf
#include <vcl_vector.h>

#include <vcl_iostream.h>
#include <vcl_cstring.h>

#include <vxl_config.h> // for VXL_BIG_ENDIAN and vxl_byte

#include <vil2/vil2_stream.h>
#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_memory_chunk.h>
#include <vil2/vil2_crop.h>
#include <vil2/vil2_convert.h>


char const* vil2_dicom_format_tag = "dicom";


vil2_image_resource_sptr vil2_dicom_file_format::make_input_image(vil2_stream* vs)
{
  vil2_dicom_header_format dhf;
  if (dhf.isDicomFormat(*vs))
    return new vil2_dicom_image(vs);
  else
    return 0;
}

vil2_image_resource_sptr vil2_dicom_file_format::make_output_image(vil2_stream* vs,
                                                                   unsigned ni,
                                                                   unsigned nj,
                                                                   unsigned nplanes,
                                                                   vil2_pixel_format format)
{
  vcl_cerr << "ERROR: vil2_dicom_file::make_output_file\n"
           << "       Doesn't support output yet" << vcl_endl;
  return 0;
#if 0

  if (nplanes != 1 || format != VIL2_PIXEL_FORMAT_INT_32)
  {
    vcl_cerr << "ERROR: vil2_dicom_file_format::make_output_image\n" <<
      "       Can only create DICOM images with a single plane\n" <<
      "       and 32-bit integer pixels" << vcl_endl;
    return 0;
  }
  return new vil2_dicom_image(vs, ni, nj, nplanes, format);
#endif
}

char const* vil2_dicom_file_format::tag() const
{
  return vil2_dicom_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

bool isEncapsulated(vil2_dicom_header_image_type im_type)
{
  switch (im_type)
  {

  case VIL2_DICOM_HEADER_DITUNKNOWN:
    return false;  // This should be plain DICOM
  case VIL2_DICOM_HEADER_DITJPEGBASE:
  case VIL2_DICOM_HEADER_DITJPEGEXTLOSSY:
  case VIL2_DICOM_HEADER_DITJPEGSPECNH:
  case VIL2_DICOM_HEADER_DITJPEGFULLNH:
  case VIL2_DICOM_HEADER_DITJPEGLOSSLNH:
  case VIL2_DICOM_HEADER_DITJPEGEXTHIER:
  case VIL2_DICOM_HEADER_DITJPEGSPECHIER:
  case VIL2_DICOM_HEADER_DITJPEGFULLHIER:
  case VIL2_DICOM_HEADER_DITJPEGLOSSLHIER:
  case VIL2_DICOM_HEADER_DITJPEGLOSSLDEF:
    return true;
  default:
    return false;
  }
}


void imageSwap(char *in_im, int num_bytes,
                 vil2_dicom_header_info dhi)
{

  char swaps[2];
  int row_num;

  // Only swap if two bytes used and the endians of system and
  // file differ
  if (num_bytes == 2 && dhi.file_endian_ != dhi.sys_endian_)
  {

    // Read two bytes at a time, swapping
    for (int i=0; i<dhi.dimy_; i++)
    {

      row_num = (dhi.dimx_*num_bytes)*i;

      for (int j=0; j<dhi.dimx_*num_bytes; j+=num_bytes)
      {

        swaps[0] = in_im[row_num+j];
        swaps[1] = in_im[row_num+(j+1)];
        in_im[row_num+j]=swaps[1];
        in_im[row_num+(j+1)]=swaps[0];
      }
    }
  }
}


void convertVoidToFloatImage(void *void_im,
                       vil2_image_view<float> &float_im,
                       int num_bytes,
                       vil2_dicom_header_info dhi)
{

  int i,j;
  int next_row;

  // Different types to read from void into
  signed char *char_im=0;
  unsigned char *uchar_im=0;
  signed short *short_im=0;
  unsigned short *ushort_im=0;

  // Size the float image
  float_im.set_size(dhi.dimx_,dhi.dimy_);

  // Move through the void pointer and put the values into
  // the float image
  for (unsigned i=0; i<dhi.dimy_; i++)
  {

    next_row = dhi.dimx_*i;

    for (unsigned j=0; j<dhi.dimx_; j++)
    {

      // Is it signed (1) or unsigned (0)?
      if (dhi.pix_rep_ == 0)
      {

        // Is it a short or char?
        if (num_bytes == 2)
        {
          ushort_im = (unsigned short *)void_im;
          float_im(j,i) =
            (float)ushort_im[next_row+j];
        }
        else
        {

          uchar_im = (unsigned char *)void_im;
          float_im(j,i) =
            (float)uchar_im[next_row+j];
        }
      }
      else
      {

        // Is it a signed short or signed char?
        if (num_bytes == 2)
        {
          short_im = (signed short *)void_im;
          float_im(j,i) =
            (float)short_im[next_row+j];
        }
        else
        {
          char_im = (signed char *)void_im;
          float_im(j,i) =
            (float)char_im[next_row+j];
        }
      }
    }
  }
}



char *convert12to16(char *im, vil2_dicom_header_info dhi,
                  bool del_old=true)
{

  int new_im_size=(dhi.dimx_*2) * dhi.dimy_;
  char *new_im=new char[new_im_size];
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



bool checkReadableFormat(vil2_dicom_header_image_type im_type)
{

  bool retval;
  vcl_string type;

  switch (im_type)
  {

  case VIL2_DICOM_HEADER_DITUNKNOWN:      type = "Unknown or Plain DICOM";
                retval=true; // This should be plain DICOM
                break;
  case VIL2_DICOM_HEADER_DITJPEGBASE:    type = "Baseline JPEG (Process 1)";
                retval=false;
                break;
  case VIL2_DICOM_HEADER_DITJPEGEXTLOSSY:  type = "Extended JPEG (Processes 2, 3, 4 & 5)";
                retval=false;
                break;
  case VIL2_DICOM_HEADER_DITJPEGSPECNH:    type = "Spectral Selection non-hierachical JPEG (Processes 6, 7, 8 and 9)";
                retval=false;
                break;
  case VIL2_DICOM_HEADER_DITJPEGFULLNH:    type = "Full Progression non-hierachical JPEG (Processes 10, 11, 12 and 13)";
                retval=false;
                break;
  case VIL2_DICOM_HEADER_DITJPEGLOSSLNH:    type = "Lossless non-hierachical JPEG (Processes 14 and 15)";
                retval=false;
                break;
  case VIL2_DICOM_HEADER_DITJPEGEXTHIER:    type = "Extended hierachical JPEG (Processes 16, 17, 18 and 19)";
                retval=false;
                break;
  case VIL2_DICOM_HEADER_DITJPEGSPECHIER:  type = "Spectral Selection hierachical JPEG (Processes 20, 21, 22 and 23)";
                retval=false;
                break;
  case VIL2_DICOM_HEADER_DITJPEGFULLHIER:  type = "Full Progression hierachical JPEG (Processes 24, 25, 26 and 27)";
                retval=false;
                break;
  case VIL2_DICOM_HEADER_DITJPEGLOSSLHIER:  type = "Lossless hierarchical JPEG (Processes 28 and 29)";
                retval=false;
                break;
  case VIL2_DICOM_HEADER_DITJPEGLOSSLDEF:  type = "Lossless non-hierachical default (Process 14 [Selection Value 1])";
                retval=true;
                break;
  default:          type = "Undefined";
                retval=false;
                break;

  }

  if (!retval)
  {

    vcl_cerr << "CW_DicomFormat - Image type is " << type.c_str() << ", but is not yet supported."
           << vcl_endl;

  }

  return retval;

}



vil2_dicom_image::vil2_dicom_image(vil2_stream* vs):
  vs_(vs)
{
  vs_->ref();

  vil2_dicom_header_format dhf;
  header_ = dhf.readHeader(*vs);
  start_of_pixels_ = vs->tell();
}

bool vil2_dicom_image::get_property(char const * /*tag*/, void * /*prop*/) const
{
  // Need to write lots of access code for the dicom header.
  return false;
}

char const* vil2_dicom_image::file_format() const
{
  return vil2_dicom_format_tag;
}

vil2_dicom_image::vil2_dicom_image(vil2_stream* vs, unsigned ni, unsigned nj,
                               unsigned nplanes, vil2_pixel_format format):
  vs_(vs)
{
  assert(!"vil2_dicom_image doesn't yet support output");

  vs_->ref();

  assert(nplanes == 1 && format == VIL2_PIXEL_FORMAT_INT_32);
  header_.dimx_=ni;
  header_.dimy_=nj;
  header_.dimz_=1;
}

vil2_dicom_image::~vil2_dicom_image()
{
  //delete vs_;
  vs_->unref();
}



vil2_image_view_base_sptr vil2_dicom_image::get_copy_view(
  unsigned i0, unsigned ni, unsigned j0, unsigned nj) const
{


  void *void_im=0;
  vil2_image_view<float> float_im;
  vil2_image_view<vxl_int_32> im;
  int bytes_read;
  bool readable = checkReadableFormat(header_.image_type_);

  vs_->seek(start_of_pixels_);

  // vs_ should point at the start of the data
  if (vs_->ok() && readable)
  {

    // If it's an encapsulated method, call the encapsulated reader,
    // otherwise just read the data
    if (isEncapsulated(header_.image_type_))
    {

//      bool failed = readEncapsulatedData(im, header_, *vs_);

    }
    else
    {


      // Get the number of rows and columns to read
      int cols=header_.dimx_;
      int rows=header_.dimy_;

      // The number of bytes to read at a time depends on the
      // allocated bits. If 16 or 12 are allocated, then two bytes
      // should be read (with a reduced number of reads for 12
      // bits as each value only takes up 3/4 of the actual number
      // of bits (16) read). Otherwise, one vxl_byte at a time is read

      if (header_.allocated_bits_ == 16 ||
        header_.allocated_bits_ == 12)
        bytes_read = 2;
      else
        bytes_read = 1;

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
    }

  } // End of if(fs.good...
  else
  {
    delete [] (char *) void_im;
    return 0;
  }


  // Convert the image to be floats

  // First, if it's 12 bit convert to 16
  if (header_.allocated_bits_ == 12)
    void_im = (void *)convert12to16((char *) void_im,
                                  header_);

  // Do any swapping necessary
  imageSwap((char *)void_im,bytes_read,header_);

  // And convert to float
  convertVoidToFloatImage(void_im, float_im, bytes_read,
                          header_);

  // Only bother with data we have been asked for.
  vil2_image_view<float> float_im2 = vil2_crop(float_im, i0, ni, j0, nj);

  // Now convert the float from the DICOM compressed values
  // to their real values
  if (header_.res_slope_ != VIL2_DICOM_HEADER_DEFAULTSLOPE)
  {

    // Apply the slope and intercept for scaling the
    // float values back to their original true values
    for (unsigned i=0; i<float_im2.nj(); i++)
      for (unsigned j=0; j<float_im2.ni(); j++)
      {
        float true_value = float_im(j,i);
        true_value = true_value * header_.res_slope_ +
                  header_.res_intercept_;
        float_im(j,i) = true_value;
      }
  }

  // Now the image floatim holds the true DICOM values,
  // but we need it to be an int for our purposes, so convert
  // a final time
  vil2_convert_cast(float_im2, im);


  delete [] (char *) void_im;

  return new vil2_image_view<vxl_int_32>(im);
}


bool vil2_dicom_image::put_view(const vil2_image_view_base& view,
                              unsigned x0, unsigned y0)
{
  assert(!"vil2_dicom_image doesn't yet support output");

  if (!view_fits(view, x0, y0))
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n view does not fit\n";
    return false;
  }
#if 0
  if ((view.pixel_format() == VIL2_PIXEL_FORMAT_UINT_32 && bits_per_component_ < 32) ||
      (view.pixel_format() == VIL2_PIXEL_FORMAT_INT_32  && bits_per_component_ < 32) ||
      (view.pixel_format() == VIL2_PIXEL_FORMAT_UINT_16 && bits_per_component_ < 16) ||
      (view.pixel_format() == VIL2_PIXEL_FORMAT_INT_16  && bits_per_component_ < 16) ||
      (view.pixel_format() == VIL2_PIXEL_FORMAT_BYTE    && bits_per_component_ <  8) ||
      (view.pixel_format() == VIL2_PIXEL_FORMAT_SBYTE   && bits_per_component_ <  8) ||
      (view.pixel_format() == VIL2_PIXEL_FORMAT_BOOL    && bits_per_component_ <  1) ||
       view.pixel_format() == VIL2_PIXEL_FORMAT_DOUBLE ||
       view.pixel_format() == VIL2_PIXEL_FORMAT_FLOAT )
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n Can't fit view into dicom component size\n";
    return false;
  }

  const vil2_image_view<bool>*  bb=0;
  const vil2_image_view<vxl_byte>*  ob = 0;
  const vil2_image_view<vxl_uint_16>* pb = 0;
  const vil2_image_view<vxl_uint_32>*   qb = 0;

  if (view.pixel_format() == VIL2_PIXEL_FORMAT_BOOL)
    bb = &static_cast<const vil2_image_view<bool>& >(view);
  else if (view.pixel_format() == VIL2_PIXEL_FORMAT_BYTE)
    ob = &static_cast<const vil2_image_view<vxl_byte>& >(view);
  else if (view.pixel_format() == VIL2_PIXEL_FORMAT_UINT_16)
    pb = &static_cast<const vil2_image_view<vxl_uint_16>& >(view);
  else if (view.pixel_format() == VIL2_PIXEL_FORMAT_UINT_32)
    qb = &static_cast<const vil2_image_view<vxl_uint_32>& >(view);
  else
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n Do not support putting "
             << view.is_a() << " views into dicom image_resource objects\n";
    return false;
  }

  if (magic_ == 5) // pgm raw image ==> nplanes() == 1
  {
    unsigned bytes_per_sample = (bits_per_component_+7)/8;
    unsigned bytes_per_pixel = bytes_per_sample;
    vil2_streampos byte_start = start_of_data_ + (y0 * ni_ + x0) * bytes_per_pixel;
    unsigned byte_width = ni_ * bytes_per_pixel;
    unsigned byte_out_width = view.ni() * bytes_per_pixel;

    if ( bytes_per_sample==1 )
    {
      assert(ob!=0);
      for (unsigned y = 0; y < view.nj(); ++y)
      {
        vs_->seek(byte_start);
        vs_->write(ob->top_left_ptr() + y * view.ni(), byte_out_width);
        byte_start += byte_width;
      }
    } else if ( bytes_per_sample==2 && VXL_BIG_ENDIAN )
    {
      assert(pb!=0);
      for (unsigned y = 0; y < view.nj(); ++y)
      {
        vs_->seek(byte_start);
        vs_->write(pb->top_left_ptr() + y * view.ni(), byte_out_width);
        byte_start += byte_width;
      }
    } else if ( bytes_per_sample==2 )
    {
      // Little endian host; must convert words to have MSB first.
      //
      // Convert line by line to avoid duplicating a potentially large image.
      vcl_vector<vxl_byte> tempbuf(byte_out_width);
      assert(pb!=0);
      for (unsigned y = 0; y < view.nj(); ++y)
      {
        vs_->seek(byte_start);
        vcl_memcpy(&tempbuf[0], pb->top_left_ptr() + y * view.ni(), byte_out_width);
        ConvertHostToMSB(&tempbuf[0], view.ni());
        vs_->write(&tempbuf[0], byte_out_width);
        byte_start += byte_width;
      }
    } else { // This should never occur...
      vcl_cerr << "ERROR: pgm: writing rawbits format with > 16bit samples\n";
      return false;
    }
  }
  else if (magic_ == 6) // ppm raw image; cannot be written as efficiently as pgm
  {
    unsigned bytes_per_sample = (bits_per_component_+7)/8;
    unsigned bytes_per_pixel = nplanes() * bytes_per_sample;
    vil2_streampos byte_start = start_of_data_ + (y0 * ni_ + x0) * bytes_per_pixel;
    unsigned byte_width = ni_ * bytes_per_pixel;

    if ( bytes_per_sample==1 )
    {
      assert(ob!=0);
      for (unsigned y = 0; y < view.nj(); ++y)
      {
        vs_->seek(byte_start);
        for (unsigned x = 0; x < view.ni(); ++x)
          for (unsigned p = 0; p < ncomponents_; ++p)
            vs_->write(&(*ob)(x,y,p), 1);
        byte_start += byte_width;
      }
    } else if ( bytes_per_sample==2 )
    {
      assert(pb!=0);
      for (unsigned y = y0; y < view.nj(); ++y)
      {
        vs_->seek(byte_start);
        for (unsigned x = x0; x < view.ni(); ++x)
        {
          vxl_uint_16 tempbuf[3];
          for (unsigned p = 0; p < ncomponents_; ++p)
            tempbuf[p] = (*pb)(x,y,p);
          ConvertHostToMSB(tempbuf, ncomponents_);
          vs_->write(tempbuf, bytes_per_pixel);
        }
        byte_start += byte_width;
      }
    } else { // This should never occur...
      vcl_cerr << "ERROR: pgm: writing rawbits format with > 16bit samples\n";
      return false;
    }
  }
  else if (magic_ == 4) // pbm (bitmap) raw image
  {
    int byte_width = (ni_+7)/8;

    assert(bb!=0);
    for (unsigned y = 0; y < view.nj(); ++y)
    {
      vil2_streampos byte_start = start_of_data_ + (y0+y) * byte_width + x0/8;
      int s = x0&7; // = x0%8;
      unsigned char a = 0;
      if (s)
      {
        vs_->read(&a, 1L);
        vs_->seek(byte_start);
        a &= ((1<<s)-1)<<(8-s); // clear the last 8-s bits of a
      }
      for (unsigned x = 0; x < view.ni(); ++x)
      {
        if ((*bb)(x,y)) a |= 1<<(7-s); // single bit; high bit = first
        if (s >= 7) { vs_->write(&a, 1L); ++byte_start; s = 0; a = 0; }
        else ++s;
      }
      if (s)
      {
        if (x0+view.ni() < ni_)
        {
          vs_->seek(byte_start);
          unsigned char c; vs_->read(&c, 1L);
          vs_->seek(byte_start);
          c &= ((1<<(8-s))-1); // clear the first s bits of c
          a |= c;
        }
        vs_->write(&a, 1L);
      }
    }
  }
  else // ascii (non-raw) image data
  {
    if (x0 > 0 || y0 > 0 || view.ni() < ni_ || view.nj() < nj_)
      return false; // can only write the full image in this mode
    vs_->seek(start_of_data_);
    for (unsigned y = 0; y < view.nj(); ++y)
      for (unsigned x = 0; x < view.ni(); ++x)
        for (unsigned p = 0; p < ncomponents_; ++p)
        {
          if (bits_per_component_ <= 1)       (*vs_) << (*bb)(x,y,p);
          else if (bits_per_component_ <= 8)  (*vs_) << (*ob)(x,y,p);
          else if (bits_per_component_ <= 16) (*vs_) << (*pb)(x,y,p);
          else                                (*vs_) << (*qb)(x,y,p);
        }
  }
#endif
  return true;
}

#if 0
//==================================================================
// readEncapsulatedData
//==================================================================
bool vil2_dicom_image::readEncapsulatedData(vimt_image_2d_of<vxl_int_32>&im,
                      vil2_dicom_header_info head_info,
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
  unsigned char *data;
  int data_size, num_bytes;

  unsigned short group=0, element=0;  // Each group and element read from
                                  // between the actual data
  unsigned int length=0;   // The length of the next data block
  unsigned long tot_len=0; // The final length of the data
  unsigned int off_entry=0; // For disposing of the offset table entries
  bool result=true;


  // We can make the data the same as the uncompressed image size
  // so we're sure all the data will fit to save constant resizing

  // Get the number of rows and columns to read
  int cols=head_info.dimx_;
  int rows=head_info.dimy_;

  // The size of the finished data has to be multiplied by
  // the number of bytes - just to make sure.
  if (head_info.allocated_bits_ == 16 ||
    head_info.allocated_bits_ == 12)
  {

    num_bytes = 2;

  } // End of if(head_info.allocated...
  else
  {

    num_bytes = 1;

  } // End of else

  // If 12 bits allocated, use the right number of 16 bits
  if (head_info.allocated_bits_ == 12)
  {

    cols = 3.0*cols/4;

  } // End of if(head_info...
  
  data_size = (cols*num_bytes)*rows;
  data = new unsigned char [data_size];

  // Now we're ready to read

  // First ignore the offset table!
  // This may cause some problems if we come across a
  // multiframe image!
  fs.read((char *)&group, sizeof(unsigned short));
  fs.read((char *)&element, sizeof(unsigned short));

  group = shortSwap(group);
  element = shortSwap(element);

  if (group == CW_DICOM_DELIMITERGROUP && 
    element == CW_DICOM_DLITEM)
  {

    fs.read((char *)&length, sizeof(unsigned int));
    length = intSwap(length);

    // We need to check that the offsets are divisible
    // by 4, otherwise there's and error
    if (length%4 == 0)
    {
        
      // Now throw away any offset table entries - not
      // needed!
      while (length)
      {

        fs.read((char *)&off_entry, sizeof(unsigned int));
        length -= 4;
        
      }
      
    }
    else
    {

      // Problem with the offset table
      result = false;

    }

  }
  else
  {

    // Problem with the group and element read
    result = false;

  }

  // If all's ok, read the image in
  if (result)
  {

    result = getDataFromEncapsulation(&data, tot_len, fs);

  }

  return result;

}

//==================================================================
// getDataFromEncapsulation
//==================================================================
bool vil2_dicom_image::getDataFromEncapsulation(unsigned char **data, unsigned long &data_len, 
                        vcl_ifstream &fs)
{

  bool result = true;

  unsigned short group=0, element=0; // The next group and element read
  unsigned int length=0;           // The length of the next data block

  data_len = 0;

  // First read a group and element
  fs.read((char *)&group, sizeof(unsigned short));
  fs.read((char *)&element, sizeof(unsigned short));

  group = shortSwap(group);
  element = shortSwap(element);

  while ((group == CW_DICOM_DELIMITERGROUP &&
       element == CW_DICOM_DLITEM) &&
       result)
  {

    // Read the length
    fs.read((char *)&length, sizeof(unsigned int));
    length = intSwap(length);

    // Now read the data
    fs.read((char *)&((*data)[data_len]), length);

    if (!fs.good())
    {

      result = false;

    }
    else
    {

      // Increase the data length
      data_len += length;

      // Read the next data group and element
      fs.read((char *)&group, sizeof(unsigned short));
      fs.read((char *)&element, sizeof(unsigned short));

      group = shortSwap(group);
      element = shortSwap(element);

    }

  }

  if (result)
  {

    if (group == CW_DICOM_DELIMITERGROUP &&
      element == CW_DICOM_DLSEQDELIMITATIONITEM)
    {

      // Just read the length and all's ok!
      fs.read((char *)&length, sizeof(unsigned int));
      length = intSwap(length);

      // Try writing it out!
      vcl_ofstream ost("C:\\testout.jpg",vcl_ios_binary);
      ost.write((char *)(*data),data_len);
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
short vil2_dicom_image::shortSwap(short short_in)
{

  short result = short_in;

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

    result = short_swap.short_val;

  } // End of if(head_info.file_endian_...

  return result;

} // End of vil2_dicom_image::shortSwap

//===============================================================
// intSwap
//===============================================================
int vil2_dicom_image::intSwap(int int_in)
{

  int result = int_in;

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

    result = int_swap.int_val;

  } // End of if(head_info.file_endian_...

  return result;

} // End of vil2_dicom_image::intSwap

//===============================================================
// charSwap
//===============================================================
void vil2_dicom_image::charSwap(char *char_in, int val_size)
{

  // Only swap if the architecture is different to the
  // file (the logic means that if one is unknown it swaps,
  // if both are unknown, it doesnt)
  if (headerInfo().file_endian_ != headerInfo().sys_endian_)
  {

    // Create a char the same size to swap
    char *temp = new char [val_size];

    if (temp)
    {
      int i;

      // Copy from the first vcl_string into the temp
      for (i=0; i<val_size; i++)
      {

        temp[i]=char_in[i];

      } // End of for

      // Now put back in reverse
      for (i=0; i<val_size; i++)
      {

        char_in[(val_size-i)-1] = temp[i];

      } // End of for

      delete [] temp;

    } // End of if(temp)
    else
    {

      vcl_cerr << "Couldn't create temp in charSwap!" << vcl_endl;
      vcl_cerr << "Value remains unswapped!" << vcl_endl;

    }
    
  } // End of if(head_info.file_endian_...

} // End of vil2_dicom_image::charSwap
#endif

