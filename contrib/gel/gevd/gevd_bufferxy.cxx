// This is gel/gevd/gevd_bufferxy.cxx
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include "gevd_bufferxy.h"
//:
// \file

#include <vcl_compiler_detection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_pixel_format.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>

#if defined(_MSC_VER) || defined(__GNUC__) || defined(__INTEL_COMPILER)
#define iostream_char char
#else
#define iostream_char unsigned char
#endif

//============== Constructors and Destructors ======================

void gevd_bufferxy::Init(int x, int y, int b)
{
  SetBitsPixel(b);
  SetSizeX(x);
  SetSizeY(y);

  // Set the pointers appropriately
  typedef unsigned char * byteptr;
  yra = new byteptr[y];
  xra = new unsigned int[x];
  for (int i=0; i < x; i++)
    xra[i] = i * GetBytesPixel();

  for (int j=0; j < y; j++)
    yra[j] = GetBufferPtr() + j*x*GetBytesPixel();
}

//: Construct a gevd_bufferxy of width x, height y, and b bits per entry.
gevd_bufferxy::gevd_bufferxy(int x, int y, int b) : gevd_memory_mixin(x*y*(int)((b+7)/8))
{
  Init(x, y, b);
}

//: Construct a gevd_bufferxy of width x, height y, and b bits per entry, and load data from memptr.  Object will not free memptr.
gevd_bufferxy::gevd_bufferxy(int x, int y, int b, void* memptr) : gevd_memory_mixin(x*y*(int)((b+7)/8),memptr)
{
  Init(x, y, b);
  // Don't delete the buffer when destructing.  MM_PROTECTED should be
  // avoided, but with this constructor we do not know how memptr was
  // allocated, so we should not attempt to free or delete or delete[]
  // it.  If required, the interface could be expanded so the caller
  // has a way of telling the object whether to and how to free
  // memptr.
  SetStatus(MM_PROTECTED);
}

//: Construct a gevd_bufferxy from a vil1_image
gevd_bufferxy::gevd_bufferxy(vil1_image const& image) : gevd_memory_mixin( image.get_size_bytes() )
{
  int sizey= image.rows();
  int sizex= image.cols();

  Init(sizex, sizey, image.bits_per_component());

  image.get_section(GetBufferPtr(),     // copy bytes image into buf
                    0, 0, sizex, sizey);
}

//: Construct a gevd_bufferxy from a vil_image
gevd_bufferxy::gevd_bufferxy(vil_image_resource_sptr const& image_s) :
  gevd_memory_mixin(image_s->nplanes()*image_s->ni()*image_s->nj()*vil_pixel_format_sizeof_components(image_s->pixel_format()))
{
  if (!image_s)
  {
    std::cout << "In gevd_bufferxy - null image_resource\n";
    return;
  }
  vil_image_resource& image = *image_s;
  if (image.nplanes()!=1)
  {
    std::cout << "In gevd_bufferxy - can't handle image format, buffer invalid\n";
    return;
  }
  unsigned n_rows= image.nj();
  unsigned n_cols= image.ni();

  vil_pixel_format fmt = image.pixel_format();
  unsigned n_bytes = vil_pixel_format_sizeof_components(fmt);
#if 0
  unsigned n_bits = 8*n_bytes;
  Init(n_cols, n_rows, n_bits);
#endif
  Init(n_cols, n_rows, 8);
  //two cases of interest
  switch (n_bytes)
  {
   case 1:// unsigned byte pixels
   {
    unsigned char* buf = gevd_memory_mixin::GetBufferPtr();
    vil_image_view<unsigned char> view = image.get_view(0, n_cols,
                                                        0, n_rows);
    std::ptrdiff_t istep=view.istep(),jstep=view.jstep();
    const unsigned char* row = view.top_left_ptr();
    for (unsigned j=0;j<n_rows;++j,row += jstep, buf += jstep)
    {
      const unsigned char* pixel = row;
      unsigned char* buf_pixel = buf;
      for (unsigned i=0;i<n_cols;++i,pixel+=istep, buf_pixel += istep)
        *buf_pixel = *pixel;
    }
    break;
   }
   case 2://unsigned short pixels - convert to byte range for consistency
   {
    vil_image_view<unsigned short> view = image.get_view(0, n_cols,
                                                         0, n_rows);
    unsigned short imin=0, imax=0;
    vil_math_value_range<unsigned short>(view, imin, imax);
    auto fmin = static_cast<float>(imin), fmax = static_cast<float>(imax);
    float scale = fmax-fmin;
    if (scale != 0.f)
      scale = 255.f/scale;
    else
      scale = 1.f;
    for (unsigned j=0;j<n_rows;++j)
      for (unsigned i=0;i<n_cols;++i)
        *((unsigned char*)GetElementAddr(i,j)) =
          static_cast<unsigned char>((view(i,j)-imin)*scale);
    break;
   }
   default:
    std::cout << "In gevd_bufferxy - can't handle pixel type, buffer invalid\n";
    return;
  }
}

gevd_bufferxy::~gevd_bufferxy()
{
        delete [] yra;
        delete [] xra;
}

gevd_bufferxy::gevd_bufferxy(gevd_bufferxy const& buf) : gevd_memory_mixin(buf)
{
  Init(buf.GetSizeX(), buf.GetSizeY(), buf.GetBitsPixel());
  std::memcpy(yra[0], buf.yra[0], GetSizeX()*GetSizeY()*GetBytesPixel());
}

//: Write to file.  Note that this can be OS-specific!
void gevd_bufferxy::dump(const char* filename)
{
  std::ofstream f(filename,std::ios::out|std::ios::binary);
  if (!f) { std::cerr << "Cannot open "<< filename <<" for writing\n"; return; }
  f << "BUFFERXYDUMP "<< GetSizeX() <<' '<< GetSizeY() <<' '<< GetBitsPixel()
#ifdef WORDS_BIGENDIAN
    << " BIGENDIAN DATA\n";
#else
    << " LITTLEENDIAN DATA\n";
#endif
  auto const* buf = (iostream_char const*)GetBuffer();
  f.write(buf, gevd_memory_mixin::GetSize());
}

static int read_from_file(const char* filename)
{
  std::ifstream f(filename,std::ios::in|std::ios::binary); // ios::nocreate is on by default for _WIN32
  if (!f) { std::cerr <<"Cannot open "<< filename <<" for reading\n"; return -1; }
  char l[1024];
  f.get(l, 1024); // read single line
  int x=-1, y=-1, b=-1; char w;
  if ( 4 > std::sscanf(l, "BUFFERXYDUMP %d %d %d %c", &x, &y, &b, &w)
       || x <= 0 || y <= 0 || b <= 0 )
    { std::cerr << filename << " is not a gevd_bufferxy dump file\n"; return -1; }
#ifdef WORDS_BIGENDIAN
  if (w != 'B')
#else
  if (w != 'L')
#endif
    std::cerr << "Warning: "<<filename<<" was created on a different platform\n";
  return x*y*(int)((b+7)/8);
}

//: Read from file.  Note that this can be OS-specific!
gevd_bufferxy::gevd_bufferxy(const char* filename) : gevd_memory_mixin(read_from_file(filename)),
  yra(nullptr), xra(nullptr)
{
  if (gevd_memory_mixin::GetSize() > 0) {
    std::ifstream f(filename,std::ios::in|std::ios::binary); // ios::nocreate is on by default for _WIN32
    char l[1024];
    f.get(l, 1024); // read single line
    int x=-1, y=-1, b=-1;
    std::sscanf(l, "BUFFERXYDUMP %d %d %d", &x, &y, &b);
    f.get(l[0]); // read end-of-line
    Init(x, y, b);
    auto* buf = (iostream_char*)GetBuffer();
    f.read(buf, gevd_memory_mixin::GetSize());
  }
  else
    std::cerr<< "ERROR: This should not happen in gevd_bufferxy::gevd_bufferxy(char const*)\n";
}
