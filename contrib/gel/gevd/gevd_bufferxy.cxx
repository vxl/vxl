//:
// \file

#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>

#include "gevd_bufferxy.h"

#include <vcl_compiler.h>
#if defined(VCL_VC) || defined(VCL_SUNPRO_CC_5) || defined(VCL_SGI_CC) || defined(VCL_GCC_30) || defined(__INTEL_COMPILER)
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
gevd_bufferxy::gevd_bufferxy(vil1_image &image) : gevd_memory_mixin( image.get_size_bytes() )
{
  int sizey= image.rows();
  int sizex= image.cols();

  Init(sizex, sizey, image.bits_per_component());

  gevd_bufferxy image_buf(sizex, sizey, image.bits_per_component());

  image.get_section(GetBufferPtr(),     // copy bytes image into buf
                    0, 0, sizex, sizey);
}

gevd_bufferxy::~gevd_bufferxy()
{
        delete [] yra;
        delete [] xra;
}

gevd_bufferxy::gevd_bufferxy(gevd_bufferxy const& buf) : gevd_memory_mixin(buf)
{
  Init(buf.GetSizeX(), buf.GetSizeY(), buf.GetBitsPixel());
  vcl_memcpy(yra[0], buf.yra[0], GetSizeX()*GetSizeY()*GetBytesPixel());
}

//: Write to file.  Note that this can be OS-specific!
void gevd_bufferxy::dump(const char* filename)
{
  vcl_ofstream f(filename,vcl_ios_out|vcl_ios_binary);
  if (!f) { vcl_cerr << "Cannot open "<< filename <<" for writing\n"; return; }
  f << "BUFFERXYDUMP "<< GetSizeX() <<" "<< GetSizeY() <<" "<< GetBitsPixel()
#ifdef WORDS_BIGENDIAN
    << " BIGENDIAN DATA\n";
#else
    << " LITTLEENDIAN DATA\n";
#endif
  iostream_char const* buf = (iostream_char const*)GetBuffer();
  f.write(buf, gevd_memory_mixin::GetSize());
}

static int read_from_file(const char* filename)
{
  vcl_ifstream f(filename,vcl_ios_in|vcl_ios_binary); // ios::nocreate is on by default for VCL_WIN32
  if (!f) { vcl_cerr <<"Cannot open "<< filename <<" for reading\n"; return -1; }
  char l[1024];
  f.get(l, 1024); // read single line
  int x=-1, y=-1, b=-1; char w;
  if ( 4 > vcl_sscanf(l, "BUFFERXYDUMP %d %d %d %c", &x, &y, &b, &w)
       || x <= 0 || y <= 0 || b <= 0 )
    { vcl_cerr << filename << " is not a gevd_bufferxy dump file\n"; return -1; }
#ifdef WORDS_BIGENDIAN
  if (w != 'B')
#else
  if (w != 'L')
#endif
    vcl_cerr << "Warning: "<<filename<<" was created on a different platform\n";
  return x*y*(int)((b+7)/8);
}

//: Read from file.  Note that this can be OS-specific!
gevd_bufferxy::gevd_bufferxy(const char* filename) : gevd_memory_mixin(read_from_file(filename)),
  yra(0), xra(0)
{
  if (gevd_memory_mixin::GetSize() > 0) {
    vcl_ifstream f(filename,vcl_ios_in|vcl_ios_binary); // ios::nocreate is on by default for VCL_WIN32
    char l[1024];
    f.get(l, 1024); // read single line
    int x=-1, y=-1, b=-1;
    vcl_sscanf(l, "BUFFERXYDUMP %d %d %d", &x, &y, &b);
    f.get(l[0]); // read end-of-line
    Init(x, y, b);
    iostream_char* buf = (iostream_char*)GetBuffer();
    f.read(buf, gevd_memory_mixin::GetSize());
  }
  else
    vcl_cerr<< "ERROR: This should not happen in gevd_bufferxy::gevd_bufferxy(char const*)\n";
}
