// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

//:
// \file

#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>

#include "gevd_bufferxy.h"
#include <vcl_cstdio.h> // For sscanf()

#include <vcl_compiler.h>
#if defined(VCL_VC) || defined(VCL_SUNPRO_CC_50) || defined(VCL_SGI_CC) || defined(VCL_GCC_30)
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

  // Set the pointers apropriately
  typedef unsigned char * byteptr;
  yra = new byteptr[y];
  xra = new unsigned int[x];
  for(int i=0; i < x; i++)
    xra[i] = i * GetBytesPixel();

  for(int j=0; j < y; j++)
    yra[j] = GetBufferPtr() + j*x*GetBytesPixel();
}

//: Construct a gevd_bufferxy of width x, height y, and b bits per entry.
gevd_bufferxy::gevd_bufferxy(int x, int y, int b) : gevd_memory_mixin(x*y*(int)((b+7)/8))
{
  Init(x, y, b);
}

//: Construct a gevd_bufferxy of width x, height y, and b bits per entry, and load data from memptr.
gevd_bufferxy::gevd_bufferxy(int x, int y, int b, void* memptr) : gevd_memory_mixin(x*y*(int)((b+7)/8),memptr)
{
  Init(x, y, b);
}

//: Construct a gevd_bufferxy from a vil_image
gevd_bufferxy::gevd_bufferxy(vil_image &image) : gevd_memory_mixin( image.get_size_bytes() )
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

