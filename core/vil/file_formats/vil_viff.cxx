// This is core/vil/file_formats/vil_viff.cxx
#include <complex>
#include <iostream>
#include <cstring>
#include "vil_viff.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static char const* vil_viff_format_tag = "viff";


#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_exception.h>

static inline void swap(void* p,int length)
{
  char* t = (char*)p;
#ifdef DEBUG
  if (length == sizeof(vxl_uint_32) && *(vxl_uint_32*)p != 0) {
    std::cerr << "Swapping " << *(vxl_uint_32*)p;
    if (length == sizeof(float))
      std::cerr << " (or " << *(float*)p << ')';
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
    std::cerr << " to " << *(vxl_uint_32*)p;
    if (length == sizeof(float))
      std::cerr << " (or " << *(float*)p << ')';
    std::cerr << '\n';
  }
#endif
}

vil_image_resource_sptr vil_viff_file_format::make_input_image(vil_stream* is)
{
  // Attempt to read header
  if (!is) return nullptr;
  is->seek(0L);
  vil_viff_xvimage header;
  if (VIFF_HEADERSIZE != is->read((void*)(&header),VIFF_HEADERSIZE))
    return nullptr;

  if (header.identifier != (char)XV_FILE_MAGIC_NUM ||
      header.file_type != (char)XV_FILE_TYPE_XVIFF)
    return nullptr;

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
      return new vil_viff_image(is);
    default:
      std::cout << "vil_viff: non supported data type: VFF_TYP "
               << header.data_storage_type << std::endl;
      return nullptr;
  }
}

vil_image_resource_sptr vil_viff_file_format::make_output_image(vil_stream* is,
                                                                unsigned int ni, unsigned int nj, unsigned int nplanes,
                                                                vil_pixel_format format)
{
  return new vil_viff_image(is, ni, nj, nplanes, format);
}

char const* vil_viff_file_format::tag() const
{
  return vil_viff_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil_viff_image::vil_viff_image(vil_stream* is)
  : is_(is)
{
  is_->ref();
  if (!read_header())
  {
    std::cerr << "vil_viff: cannot read file header; creating dummy 0x0 image\n";
    start_of_data_ = VIFF_HEADERSIZE; endian_consistent_ = true;
    ni_ = nj_ = 0; nplanes_ = 1;
    format_ = VIL_PIXEL_FORMAT_BYTE;
  }
}

char const* vil_viff_image::file_format() const
{
  return vil_viff_format_tag;
}

vil_viff_image::vil_viff_image(vil_stream* is,
                               unsigned int ni, unsigned int nj, unsigned int nplanes,
                               vil_pixel_format format)
  : is_(is), ni_(ni), nj_(nj),
    nplanes_(nplanes), start_of_data_(VIFF_HEADERSIZE),
    format_(format), endian_consistent_(true)
{
  is_->ref();
  write_header();
}

vil_viff_image::~vil_viff_image()
{
  is_->unref();
}

bool vil_viff_image::get_property(char const * /*tag*/, void * /*prop*/) const
{
  // This is not an in-memory image type, nor is it read-only:
  return false;
}

bool vil_viff_image::read_header()
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

  //Copy ni and nj from header
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

  ni_ = rs;
  nj_ = cs;
  nplanes_ = (int)ndb; // number of colour bands

  // decide on data storage type
  format_ = VIL_PIXEL_FORMAT_UNKNOWN;
  if (dst == VFF_TYP_BIT)
    format_ = VIL_PIXEL_FORMAT_BOOL;
  else if (dst == VFF_TYP_1_BYTE)
    format_ = VIL_PIXEL_FORMAT_BYTE;
  else if (dst == VFF_TYP_2_BYTE)
    format_ = VIL_PIXEL_FORMAT_UINT_16;
  else if (dst == VFF_TYP_4_BYTE)
    format_ = VIL_PIXEL_FORMAT_UINT_32;
  else if (dst == VFF_TYP_FLOAT)
    format_ = VIL_PIXEL_FORMAT_FLOAT;
  else if (dst == VFF_TYP_DOUBLE)
    format_ = VIL_PIXEL_FORMAT_DOUBLE;
  else if (dst == VFF_TYP_COMPLEX)
    format_ = VIL_PIXEL_FORMAT_COMPLEX_FLOAT;
  else if (dst == VFF_TYP_DCOMPLEX)
    format_ = VIL_PIXEL_FORMAT_COMPLEX_DOUBLE;
  else
    std::cout << "vil_viff: non supported data type: VFF_TYP " << dst << '\n';
  return format_ != VIL_PIXEL_FORMAT_UNKNOWN;
}

bool vil_viff_image::write_header()
{
  is_->seek(0L);
  vil_viff_data_storage type;

  if (format_==VIL_PIXEL_FORMAT_UINT_32 ||
      format_==VIL_PIXEL_FORMAT_INT_32)
    type=VFF_TYP_4_BYTE;
  else if (format_==VIL_PIXEL_FORMAT_UINT_16 ||
           format_==VIL_PIXEL_FORMAT_INT_16)
    type=VFF_TYP_2_BYTE;
  else if (format_==VIL_PIXEL_FORMAT_BYTE ||
           format_==VIL_PIXEL_FORMAT_SBYTE)
    type=VFF_TYP_1_BYTE;
  else if (format_==VIL_PIXEL_FORMAT_BOOL)
    type=VFF_TYP_BIT;
  else if (format_==VIL_PIXEL_FORMAT_FLOAT)
    type=VFF_TYP_FLOAT;
  else if (format_==VIL_PIXEL_FORMAT_DOUBLE)
    type=VFF_TYP_DOUBLE;
  else if (format_==VIL_PIXEL_FORMAT_COMPLEX_FLOAT)
    type=VFF_TYP_COMPLEX;
  else if (format_==VIL_PIXEL_FORMAT_COMPLEX_DOUBLE)
    type=VFF_TYP_DCOMPLEX;
  else
  {
    std::cout << "vil_viff: non supported data type: " << (short)format_ << '\n';
    return false;
  }

  //create header
  vil_viff_xvimage image(ni_, nj_, type, nplanes_);

  //make local copy of header
  std::memcpy(&header_, &image, sizeof(header_));
  start_of_data_ = sizeof(header_);


  is_->write((void*)(&header_), start_of_data_);
  start_of_data_ = is_->tell();
  return true;
}

vil_image_view_base_sptr vil_viff_image::get_copy_view(unsigned int x0, unsigned int xs,
                                                       unsigned int y0, unsigned int ys) const
{
  assert(x0+xs<=ni_);
  assert(y0+ys<=nj_);
  unsigned int pix_size = 8*vil_pixel_format_sizeof_components(format_);
  if (format_==VIL_PIXEL_FORMAT_BOOL) pix_size = 1;
  if (format_==VIL_PIXEL_FORMAT_BOOL && x0%8 != 0)
    std::cerr << "vil_viff_image::get_copy_view(): Warning: x0 should be a multiple of 8 for this type of image\n";

  vxl_uint_32 rowsize = (pix_size*xs+7)/8;
  vxl_uint_32 tbytes = rowsize*ys*nplanes_;
  vil_memory_chunk_sptr buf = new vil_memory_chunk(tbytes,format_);
  auto* ib = reinterpret_cast<vxl_byte*>(buf->data());
  for (unsigned int p = 0; p<nplanes_; ++p) {
    for (unsigned int y = y0; y < y0+ys; ++y) {
      is_->seek(start_of_data_ + p*nj_*((ni_*pix_size+7)/8)
                               + y*((ni_*pix_size+7)/8)
                               + x0*pix_size/8);
      is_->read(ib, rowsize);
      ib += rowsize;
    }
  }
  if (!endian_consistent_) {
    ib = reinterpret_cast<vxl_byte*>(buf->data());
    for (unsigned int i=0;i<tbytes;i+=(pix_size+7)/8)
      swap(ib+i,(pix_size+7)/8);
  }

#define ARGS(T) buf, reinterpret_cast<T*>(buf->data()), xs,ys,nplanes_, 1,xs,xs*ys
  if (format_ == VIL_PIXEL_FORMAT_BOOL)                return new vil_image_view<bool>                (ARGS(bool));
  else if (format_ == VIL_PIXEL_FORMAT_BYTE)           return new vil_image_view<vxl_byte>            (ARGS(vxl_byte));
  else if (format_ == VIL_PIXEL_FORMAT_UINT_16)        return new vil_image_view<vxl_uint_16>         (ARGS(vxl_uint_16));
  else if (format_ == VIL_PIXEL_FORMAT_UINT_32)        return new vil_image_view<vxl_uint_32>         (ARGS(vxl_uint_32));
  else if (format_ == VIL_PIXEL_FORMAT_FLOAT)          return new vil_image_view<float>               (ARGS(float));
  else if (format_ == VIL_PIXEL_FORMAT_DOUBLE)         return new vil_image_view<double>              (ARGS(double));
  else if (format_ == VIL_PIXEL_FORMAT_COMPLEX_FLOAT)  return new vil_image_view<std::complex<float> > (ARGS(std::complex<float>));
  else if (format_ == VIL_PIXEL_FORMAT_COMPLEX_DOUBLE) return new vil_image_view<std::complex<double> >(ARGS(std::complex<double>));
  else return nullptr;
#undef ARGS
}

bool vil_viff_image::put_view(vil_image_view_base const& buf, unsigned int x0, unsigned int y0)
{
  assert(buf.pixel_format() == format_); // pixel formats of image and buffer must match
  if (!view_fits(buf, x0, y0) || buf.nplanes() != nplanes())
  {
    vil_exception_warning(vil_exception_out_of_bounds("vil_viff_image::put_view"));
    return false;
  }
  unsigned int ni = buf.ni();
  unsigned int nj = buf.nj();
#ifdef DEBUG
  std::cerr << "vil_viff_image::put_view() : buf="
           << ni<<'x'<<nj<<'x'<< buf.nplanes()<<'p'
           << " at ("<<x0<<','<<y0<<")\n";
#endif
  //std::cout << "buf=" << buf << '\n';
  auto const& ibuf = reinterpret_cast<vil_image_view<vxl_byte> const&>(buf);
  //std::cout << "ibuf=" << ibuf << '\n';
  if (ibuf.istep() != 1 || ibuf.jstep() != int(ni) ||
      (ibuf.planestep() != int(ni*nj) && nplanes() != 1))
  {
    std::cerr << "ERROR: " << __FILE__ << ":\n"
             << " view does not fit: istep="<<ibuf.istep()
             << ", jstep="<<ibuf.jstep()
             << ", planestep="<<ibuf.planestep()
             << " instead of 1,"<<ni<<','<<ni*nj<<'\n';
    return false;
  }
  const vxl_byte* ob = ibuf.top_left_ptr();
  unsigned int pix_size = 8*vil_pixel_format_sizeof_components(format_);
  if (format_==VIL_PIXEL_FORMAT_BOOL) pix_size = 1;
  if (format_==VIL_PIXEL_FORMAT_BOOL && x0%8 != 0)
    std::cerr << "vil_viff_image::put_view(): Warning: x0 should be a multiple of 8 for this type of image\n";

  vxl_uint_32 rowsize = (pix_size*ni+7)/8;
  if (endian_consistent_ || pix_size <= 8)
    for (unsigned int p = 0; p<nplanes_; ++p)
      for (unsigned int y = y0; y < y0+nj; ++y) {
        is_->seek(start_of_data_ + p*nj_*((ni_*pix_size+7)/8)
                                 + y*((ni_*pix_size+7)/8)
                                 + x0*pix_size/8);
        if ((vil_streampos)rowsize != is_->write(ob, rowsize))
          std::cerr << "WARNING: " << __FILE__ << ":\n"
                   << " could not write "<<rowsize<<" EC bytes to stream;\n"
                   << " p="<<p<<", y="<<y<<'\n';
#ifdef DEBUG
        else
          std::cerr << "written "<<rowsize<<" EC bytes to stream; p="<<p<<", y="<<y<<'\n';
#endif
        ob += rowsize;
      }
  else { // !endian_consistent_ ==> must swap bytes
    auto* tempbuf = new vxl_byte[rowsize];
    for (unsigned int p = 0; p<nplanes_; ++p)
      for (unsigned int y = y0; y < y0+nj; ++y) {
        std::memcpy(tempbuf, ob, rowsize);
        for (unsigned int i=0; i<rowsize; i+=pix_size/8)
          swap(tempbuf+i,pix_size/8);
        is_->seek(start_of_data_ + p*ni_*nj_*pix_size/8 + pix_size*(y*ni_+x0)/8);
        if ((vil_streampos)rowsize != is_->write(tempbuf, rowsize))
          std::cerr << "WARNING: " << __FILE__ << ":\n"
                   << " could not write "<<rowsize<<" NEC bytes to stream;\n"
                   << " p="<<p<<", y="<<y<<'\n';
#ifdef DEBUG
        else
          std::cerr << "written "<<rowsize<<" NEC bytes to stream; p="<<p<<", y="<<y<<'\n';
#endif
        ob += rowsize;
      }
    delete[] tempbuf;
  }
  return true;
}

bool vil_viff_image::check_endian()
{
  // check if format is consistent
  // Check the data_storage_type in the header
  // If it is between 1 and 255, the "Endian" is consistent with the system
  // if not, we swap and check again

  vxl_uint_32 dst = header_.data_storage_type;

  endian_consistent_ = ((dst & 0xff) != 0);
#ifdef DEBUG
  if (endian_consistent_)
    std::cerr << "Endian is Consistent\n";
  else
    std::cerr << "Endian is NOT Consistent\n";
#endif
  return endian_consistent_;
}

void vil_viff_image::set_ispare1(vxl_uint_32 ispare1)
{
  header_.ispare1 = ispare1;
  int longsize = sizeof(vxl_uint_32);
  auto* bytes = new vxl_byte[longsize];
  std::memcpy(bytes,&ispare1,longsize);
  if (!endian_consistent_)
    swap(bytes,longsize);

  is_->seek((int)((vxl_byte*)&header_.ispare1 - (vxl_byte*)&header_));
  is_->write(bytes, longsize);
  delete[] bytes;
}

void vil_viff_image::set_ispare2(vxl_uint_32 ispare2)
{
  header_.ispare2 = ispare2;
  int longsize = sizeof(vxl_uint_32);
  auto* bytes = new vxl_byte[longsize];
  std::memcpy(bytes,&ispare2,longsize);
  if (!endian_consistent_)
    swap(bytes,longsize);

  is_->seek((int)((vxl_byte*)&header_.ispare2 - (vxl_byte*)&header_));
  is_->write(bytes, longsize);
  delete[] bytes;
}

void vil_viff_image::set_fspare1(float fspare1)
{
  header_.fspare1 = fspare1;
  int floatsize = sizeof(float);
  auto* bytes = new vxl_byte[floatsize];
  std::memcpy(bytes,&fspare1,floatsize);
  if (!endian_consistent_)
    swap(bytes,floatsize);

  is_->seek((int)((vxl_byte*)&header_.fspare1 - (vxl_byte*)&header_));
  is_->write(bytes, floatsize);

  delete[] bytes;
}

void vil_viff_image::set_fspare2(float fspare2)
{
  header_.fspare2 = fspare2;
  int floatsize = sizeof(float);
  auto* bytes = new vxl_byte[floatsize];
  std::memcpy(bytes,&fspare2,floatsize);
  if (!endian_consistent_)
    swap(bytes,floatsize);

  is_->seek((int)((vxl_byte*)&header_.fspare2 - (vxl_byte*)&header_));
  is_->write(bytes, floatsize);
  delete[] bytes;
}
