// This is core/vil1/file_formats/vil1_viff.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil1_viff.h"
extern "C" {
#include "vil1_viff_support.h"
}
#include <vcl_cassert.h>

char const* vil1_viff_format_tag = "viff";

#include <vcl_iostream.h>
#include <vcl_cstring.h>

#include <vil1/vil1_stream.h>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_property.h>

static inline void swap(void* p,int length)
{
  char* t = (char*)p;
#ifdef DEBUG
  if (length == sizeof(vxl_uint_32) && *(vxl_uint_32*)p != 0) {
    vcl_cerr << "Swapping " << *(vxl_uint_32*)p;
    if (length == sizeof(float))
      vcl_cerr << " (or " << *(float*)p << ')';
  }
#endif
  for (int j=0;2*j<length;++j)
  {
    char c = t[j];
    t[j] = t[length-j-1];
    t[length-j-1] = c;
  }
#ifdef DEBUG
  if (length == sizeof(vxl_uint_32) && *(vxl_uint_32*)p != 0) {
    vcl_cerr << " to " << *(vxl_uint_32*)p;
    if (length == sizeof(float))
      vcl_cerr << " (or " << *(float*)p << ')';
    vcl_cerr << '\n';
  }
#endif
}

vil1_image_impl* vil1_viff_file_format::make_input_image(vil1_stream* is)
{
  // Attempt to read header
  if (!is) return 0;
  is->seek(0L);
  vil1_viff_xvimage header;
  if (VIFF_HEADERSIZE != is->read((void*)(&header),VIFF_HEADERSIZE))
    return 0;

  if (header.identifier != (char)XV_FILE_MAGIC_NUM ||
      header.file_type != (char)XV_FILE_TYPE_XVIFF)
    return 0;

  vxl_uint_32 dst = header.data_storage_type;
  if ((dst & 0xff) == 0)
    swap(&dst,sizeof(dst));
  switch (dst)
  {
    case VFF_TYP_BIT:
    case VFF_TYP_1_BYTE:
    case VFF_TYP_2_BYTE:
    case VFF_TYP_4_BYTE:
    case VFF_TYP_FLOAT:
    case VFF_TYP_DOUBLE:
    case VFF_TYP_COMPLEX:
    case VFF_TYP_DCOMPLEX:
      return new vil1_viff_generic_image(is);
    default:
      vcl_cout << "vil1_viff: non supported data type: VFF_TYP "
               << header.data_storage_type << vcl_endl;
      return 0;
  }
}

vil1_image_impl* vil1_viff_file_format::make_output_image(vil1_stream* is, int planes,
                                                          int width,
                                                          int height,
                                                          int components,
                                                          int bits_per_component,
                                                          vil1_component_format format)
{
  return new vil1_viff_generic_image(is, planes, width, height, components, bits_per_component, format);
}

char const* vil1_viff_file_format::tag() const
{
  return vil1_viff_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil1_viff_generic_image::vil1_viff_generic_image(vil1_stream* is)
  : is_(is)
{
  is_->ref();
  if (!read_header())
  {
    vcl_cerr << "vil1_viff: cannot read file header; creating dummy 0x0 image\n";
    start_of_data_ = VIFF_HEADERSIZE; endian_consistent_ = true;
    width_ = height_ = 0; planes_ = 1; maxval_ = 255;
    bits_per_component_ = 8; format_ = VIL1_COMPONENT_FORMAT_UNSIGNED_INT;
  }
}

bool vil1_viff_generic_image::get_property(char const *tag, void *prop) const
{
  if (0==vcl_strcmp(tag, vil1_property_top_row_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==vcl_strcmp(tag, vil1_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  return false;
}

char const* vil1_viff_generic_image::file_format() const
{
  return vil1_viff_format_tag;
}

vil1_viff_generic_image::vil1_viff_generic_image(vil1_stream* is, int planes,
                                                 int width,
                                                 int height,
                                                 int /*components*/,
                                                 int bits_per_component,
                                                 vil1_component_format format)
  : is_(is), width_(width), height_(height),
    maxval_(255), planes_(planes), start_of_data_(VIFF_HEADERSIZE),
    bits_per_component_(bits_per_component),
    format_(format), endian_consistent_(true)
{
  is_->ref();
  write_header();
}

vil1_viff_generic_image::~vil1_viff_generic_image()
{
  is_->unref();
}

bool vil1_viff_generic_image::read_header()
{
  // Go to start
  is_->seek(0L);
  start_of_data_ = VIFF_HEADERSIZE;

  // Read header
  if (VIFF_HEADERSIZE != is_->read((void*)(&header_),VIFF_HEADERSIZE))
    return false;

  if (header_.identifier != (char)XV_FILE_MAGIC_NUM ||
      header_.file_type != (char)XV_FILE_TYPE_XVIFF)
    return false;

  check_endian();

  //Copy width and height from header
  maxval_ = 0;

  vxl_uint_32 rs = header_.row_size;
  vxl_uint_32 cs = header_.col_size;
  vxl_uint_32 dst = header_.data_storage_type;
  vxl_uint_32 ndb = header_.num_data_bands;

  vxl_uint_32 ispare1 = header_.ispare1;
  vxl_uint_32 ispare2 = header_.ispare2;
  float fspare1 = header_.fspare1;
  float fspare2 = header_.fspare2;

  if (!endian_consistent_)
  {
    swap(&rs,sizeof(rs));
    swap(&cs,sizeof(cs));
    swap(&dst,sizeof(dst));
    swap(&ndb,sizeof(ndb));
    swap(&ispare1,sizeof(ispare1));
    swap(&ispare2,sizeof(ispare2));
    swap(&fspare1,sizeof(fspare1));
    swap(&fspare2,sizeof(fspare2));
  }

  width_ = rs;
  height_ = cs;
  planes_ = (int)ndb; // number of colour bands

  // decide on data storage type
  switch (dst)
  {
    case VFF_TYP_BIT:
      format_ = VIL1_COMPONENT_FORMAT_UNSIGNED_INT;
      bits_per_component_ = 1;
      break;
    case VFF_TYP_1_BYTE:
      format_ = VIL1_COMPONENT_FORMAT_UNSIGNED_INT;
      bits_per_component_ = 8;
      break;
    case VFF_TYP_2_BYTE:
      format_ = VIL1_COMPONENT_FORMAT_UNSIGNED_INT;
      bits_per_component_ = 16;
      break;
    case VFF_TYP_4_BYTE:
      format_ = VIL1_COMPONENT_FORMAT_UNSIGNED_INT;
      bits_per_component_ = 32;
      break;
    case VFF_TYP_FLOAT:
      format_ = VIL1_COMPONENT_FORMAT_IEEE_FLOAT;
      bits_per_component_ = 32;
      break;
    case VFF_TYP_DOUBLE:
      format_ = VIL1_COMPONENT_FORMAT_IEEE_FLOAT;
      bits_per_component_ = 64;
      break;
    case VFF_TYP_COMPLEX:
      format_ = VIL1_COMPONENT_FORMAT_COMPLEX;
      bits_per_component_ = 64;
      break;
    case VFF_TYP_DCOMPLEX:
      format_ = VIL1_COMPONENT_FORMAT_COMPLEX;
      bits_per_component_ = 128;
      break;
    default:
      vcl_cout << "vil1_viff: non supported data type: VFF_TYP "
               <<   header_.data_storage_type << vcl_endl;
      format_ = VIL1_COMPONENT_FORMAT_UNKNOWN;
      return false;
  }

  return true;
}

bool vil1_viff_generic_image::write_header()
{
  is_->seek(0L);
  int type;
  if (bits_per_component_ > 1)
    bits_per_component_ = 8*((bits_per_component_+7)/8); // round to next 8-tuple

  if (component_format()==VIL1_COMPONENT_FORMAT_SIGNED_INT ||
      component_format()==VIL1_COMPONENT_FORMAT_UNSIGNED_INT)
  {
    switch (bits_per_component_)
    {
      case 1: type=VFF_TYP_BIT; break;
      case 8: type=VFF_TYP_1_BYTE; break;
      case 16: type=VFF_TYP_2_BYTE; break;
      case 32: type=VFF_TYP_4_BYTE; break;
      default:
        vcl_cerr << "vil1_viff: non supported data type: " << bits_per_component_ << " bit pixels\n";
        return false;
    }
  }
  else if (component_format()==VIL1_COMPONENT_FORMAT_IEEE_FLOAT)
  {
    switch (bits_per_component_)
    {
      case 32: type=VFF_TYP_FLOAT; break;
      case 64: type=VFF_TYP_DOUBLE; break;
      default:
        vcl_cerr << "vil1_viff: non supported data type: " << bits_per_component_ << " bit float pixels\n";
        return false;
    }
  }
  else if (component_format()==VIL1_COMPONENT_FORMAT_COMPLEX)
  {
    switch (bits_per_component_)
    {
      case 64: type=VFF_TYP_COMPLEX; break;
      case 128: type=VFF_TYP_DCOMPLEX; break;
      default:
        vcl_cerr << "vil1_viff: non supported data type: " << bits_per_component_ << " bit complex pixels\n";
        return false;
    }
  }
  else
  {
    vcl_cout << "vil1_viff: non supported data type: " << (short)component_format() << vcl_endl;
    return false;
  }

  //create header
  vil1_viff_xvimage *imagep = vil1_viff_createimage(height_, width_,
                                                    type, 1, planes_,
                                                    "vil1_viff image writer output",0,0,
                                                    VFF_MS_NONE,VFF_MAPTYP_NONE,VFF_LOC_IMPLICIT,0);

  //make local copy of header
  vcl_memcpy(&header_, imagep, sizeof(header_));
  start_of_data_ = sizeof(header_);

  // release xv header from createimage
  vil1_viff_freeimage(imagep);

  is_->write((void*)(&header_), start_of_data_);
  start_of_data_ = is_->tell();
  return true;
}

bool vil1_viff_generic_image::get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  assert(x0>=0); assert(y0>=0);
  assert((x0+xs)<=width_);
  assert((y0+ys)<=height_);
  if (!buf) return false; // no storage location given
  unsigned char* ib = (unsigned char*) buf;
  if ((x0*bits_per_component_)%8 != 0)
    vcl_cerr << "vil1_viff_generic_image::get_section(): Warning: x0 should be a multiple of 8 for this type of image\n";

  vxl_uint_32 rowsize = (bits_per_component_*xs+7)/8;
  vxl_uint_32 tbytes = rowsize*ys*planes_;
  for (int p = 0; p<planes_; ++p) {
    for (int y = y0; y < y0+ys; ++y) {
      is_->seek(start_of_data_ + p*height_*((width_*bits_per_component_+7)/8)
                               + y*((width_*bits_per_component_+7)/8)
                               + x0*bits_per_component_/8);
      is_->read(ib, rowsize);
      ib += rowsize;
    }
  }
  if (!endian_consistent_) {
    ib = (unsigned char*) buf;
    for (unsigned int i=0;i<tbytes;i+=bits_per_component_/8)
      swap(ib+i,bits_per_component_/8);
  }

  return true;
}

bool vil1_viff_generic_image::put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  assert(x0>=0); assert(y0>=0);
  assert((x0+xs)<=width_);
  assert((y0+ys)<=height_);
  if (!buf) return false; // no storage location given
  unsigned char const* ob = (unsigned char const*) buf;
  if ((x0*bits_per_component_)%8 != 0)
    vcl_cerr << "vil1_viff_generic_image::put_section(): Warning: x0 should be a multiple of 8 for this type of image\n";

  vxl_uint_32 rowsize = (bits_per_component_*xs+7)/8;
  if (endian_consistent_)
    for (int p = 0; p<planes_; ++p)
      for (int y = y0; y < y0+ys; ++y) {
        is_->seek(start_of_data_ + p*height_*((width_*bits_per_component_+7)/8)
                                 + y*((width_*bits_per_component_+7)/8)
                                 + x0*bits_per_component_/8);
        is_->write(ob, rowsize);
        ob += rowsize;
      }
  else {
    unsigned char* tempbuf = new unsigned char[rowsize];
    for (int p = 0; p<planes_; ++p)
      for (int y = y0; y < y0+ys; ++y) {
        vcl_memcpy(tempbuf, ob, rowsize);
        for (unsigned int i=0; i<rowsize; i+=bits_per_component_/8)
          swap(tempbuf+i,bits_per_component_/8);
        is_->seek(start_of_data_ + p*width_*height_*bits_per_component_/8 + bits_per_component_*(y*width_+x0)/8);
        is_->write(tempbuf, rowsize);
        ob += rowsize;
      }
    delete[] tempbuf;
  }
  return true;
}

bool vil1_viff_generic_image::check_endian()
{
  // check if format is consistent
  // Check the data_storage_type in the header
  // If it is between 1 and 255, the "Endian" is consistent with the system
  // if not, we swap and check again

  vxl_uint_32 dst = header_.data_storage_type;

  endian_consistent_ = ((dst & 0xff) != 0);
#ifdef DEBUG
  if (endian_consistent_)
    vcl_cerr << "Endian is Consistent\n";
  else
    vcl_cerr << "Endian is NOT Consistent\n";
#endif
  return endian_consistent_;
}

vil1_image vil1_viff_generic_image::get_plane(unsigned int plane) const
{
  assert((int)plane < planes_);
  vcl_cerr << "FIXME: this should be an adapter that shifts to the plane asked for\n";
  return const_cast<vil1_viff_generic_image*>(this);
}

bool vil1_viff_generic_image::get_section_rgb_byte(void* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) const
{
  vcl_cerr << "FIXME: vil1_viff_generic_image::get_section_rgb_byte() not yet implemented\n";
  return false;
}

bool vil1_viff_generic_image::get_section_float(void* buf, int x0, int y0, int width, int height) const
{
  if (component_format()!=VIL1_COMPONENT_FORMAT_IEEE_FLOAT || bits_per_component_ != 32)
    return false;
  return get_section(buf,x0,y0,width,height);
}

bool vil1_viff_generic_image::get_section_byte(void* buf, int x0, int y0, int width, int height) const
{
  if (component_format()!=VIL1_COMPONENT_FORMAT_UNSIGNED_INT || bits_per_component_ != 8)
    return false;
  return get_section(buf,x0,y0,width,height);
}

void vil1_viff_generic_image::set_ispare1(vxl_uint_32 ispare1)
{
  header_.ispare1 = ispare1;
  int longsize = sizeof(vxl_uint_32);
  unsigned char* bytes = new unsigned char[longsize];
  vcl_memcpy(bytes,&ispare1,longsize);
  if (!endian_consistent_)
    swap(bytes,longsize);

  is_->seek((int)((unsigned char*)&header_.ispare1 - (unsigned char*)&header_));
  is_->write(bytes, longsize);
  delete[] bytes;
}

void vil1_viff_generic_image::set_ispare2(vxl_uint_32 ispare2)
{
  header_.ispare2 = ispare2;
  int longsize = sizeof(vxl_uint_32);
  unsigned char* bytes = new unsigned char[longsize];
  vcl_memcpy(bytes,&ispare2,longsize);
  if (!endian_consistent_)
    swap(bytes,longsize);

  is_->seek((int)((unsigned char*)&header_.ispare2 - (unsigned char*)&header_));
  is_->write(bytes, longsize);
  delete[] bytes;
}

void vil1_viff_generic_image::set_fspare1(float fspare1)
{
  header_.fspare1 = fspare1;
  int floatsize = sizeof(float);
  unsigned char* bytes = new unsigned char[floatsize];
  vcl_memcpy(bytes,&fspare1,floatsize);
  if (!endian_consistent_)
    swap(bytes,floatsize);

  is_->seek((int)((unsigned char*)&header_.fspare1 - (unsigned char*)&header_));
  is_->write(bytes, floatsize);

  delete[] bytes;
}

void vil1_viff_generic_image::set_fspare2(float fspare2)
{
  header_.fspare2 = fspare2;
  int floatsize = sizeof(float);
  unsigned char* bytes = new unsigned char[floatsize];
  vcl_memcpy(bytes,&fspare2,floatsize);
  if (!endian_consistent_)
    swap(bytes,floatsize);

  is_->seek((int)((unsigned char*)&header_.fspare2 - (unsigned char*)&header_));
  is_->write(bytes, floatsize);
  delete[] bytes;
}
