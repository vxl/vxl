//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_bmp_file_format_h_
#define vil_bmp_file_format_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author(s): Don Hamilton \and Peter Tu
// Created: 17 Feb 2000
// Modification: 
// 27 May 2000 fsm@robots.ox.ac.uk Numerous endianness and structure-packing
//             bugs fixed.

#include <vcl/vcl_iosfwd.h>
class vil_stream;

typedef unsigned char  uchar; 
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

//=============================================================================
// useful structures for reading a bmp file:

// The signature consists of the two bytes 42, 4D in that order.
// It is not supposed to be read as a 16-bit integer.
#define BMP_SIGNATURE_BYTE_0 0x42
#define BMP_SIGNATURE_BYTE_1 0x4D

// E.g. this is beginning of a .bmp file I 
// downloaded from http://www.cs.columbia.edu/CAVE/curet :
//
//          _____ ___________ _____ _____ ___________ _____>
//00000000: 42 4D 36 10 0E 00 00 00 00 00 36 00 00 00|28 00 BM6.......6...(.
//         <_____ ___________ ___________ _____ _____ _____>
//00000010: 00 00 80 02 00 00 E0 01 00 00 01 00 18 00 00 00 ................
//         >_____ ___________ ___________ ___________ _____>
//00000020: 00 00 00 10 0E 00 00 00 00 00 00 00 00 00 00 00 ................
//         >_____ ___________ _____________________________
//00000030: 00 00 00 00 00 00|0A 0A 09 09 09 08 07 07 06 06 ................ bitmap
//          _______________________________________________
//00000040: 06 05 06 06 05 07 07 06 08 08 07 08 08 07 07 07 ................ bitmap
//          _______________________________________________
//00000050: 06 07 07 06 06 06 05 08 08 07 07 07 06 08 08 07 ................ bitmap
//          _______________________________________________
//00000060: 07 07 06 06 06 05 06 06 05 06 06 05 06 06 05 07 ................ bitmap
//
// The file is 921654 bytes long, which is 0x000e1036 in hex. Note that
// it's written into the file in little-endian format.
//
// The image is 640x480 which is 0x280 by 0x1e0 in hex.
//
// The image is in RGB byte format with 3*640*480 = 0xe1000 bytes of
// bitmap data.
//
// Here is the famous colour "pig" photo converted to bmp :
//          _____ ___________ _____ _____ ___________ _____>
//00000000: 42 4D DA E9 09 00 00 00 00 00 36 00 00 00 28 00 BM........6...(.
//         >_____ ___________ ___________ _____ _____ _____>
//00000010: 00 00 19 02 00 00 93 01 00 00 01 00 18 00 00 00 ................
//         >_____ ___________ ___________ ___________ _____>
//00000020: 00 00 A4 E9 09 00 6D 0B 00 00 6D 0B 00 00 00 00 ......m...m.....
//         >_____ ___________ _____________________________
//00000030: 00 00 00 00 00 00|6F 8C 93 72 8F 96 73 90 97 72 ......o..r..s..r
//          _______________________________________________
//00000040: 8F 96 74 91 98 77 94 9B 75 91 98 70 8C 93 75 8F ..t..w..u..p..u.
//          _______________________________________________
//00000050: 96 77 91 98 77 91 98 73 8D 94 75 8C 94 79 90 98 .w..w..s..u..y..



// Due to padding, you cannot expect to write this structure as a raw 
// sequence of bytes and still get a valid BMP header. The compiler
// will probably place 'bfSize' on a 4-byte boundary, which means it
// will place two bytes of padding after 'bfType'.
//
// Use the read() and write() methods instead.
struct xBITMAPFILEHEADER
{
  uchar  bfType[2];     // signature bytes
  ulong  bfSize;        // file size in bytes
  ushort bfReserved1;   // 0
  ushort bfReserved2;   // 0
  ulong  bfOffBits;     // offset to bitmap

  xBITMAPFILEHEADER( );
  void read(vil_stream *);
  bool signature_valid() const;
  void write(vil_stream *) const;
  void print(ostream &) const;
};

struct xBITMAPCOREHEADER
{  
  ulong  bcSize;      // size of this struct
  ushort bcWidth;     // bitmap width in pixels
  ushort bcHeight;    // bitmap height in pixels
  ushort bcPlanes;    // num planes - always 1
  ushort bcBitCount;  // bits per pixel
  xBITMAPCOREHEADER( );
  void read(vil_stream *);
};

struct xBITMAPINFOHEADER
{
  ulong  biSize;          // size of this struct
  long           biWidth;         // bmap width in pixels
  long           biHeight;        // bmap height in pixels
  ushort biPlanes;        // num planes - always 1
  ushort biBitCount;      // bits per pixel
  ulong  biCompression;   // compression flag
  ulong  biSizeImage;     // image size in bytes
  long           biXPelsPerMeter; // horz resolution
  long           biYPelsPerMeter; // vert resolution
  ulong  biClrUsed;       // 0 -> color table size
  ulong  biClrImportant;  // important color count
  xBITMAPINFOHEADER( );
  void read(vil_stream *);
};

//--------------------------------------------------------------------------------

#include <vil/vil_file_format.h>
#include <vil/vil_image_impl.h>

//: Loader for BMP files
class vil_bmp_file_format : public vil_file_format {
public:
  virtual char const* tag() const;
  virtual vil_image_impl* make_input_image(vil_stream* vs);
  virtual vil_image_impl* make_output_image(vil_stream* vs, int planes,
					       int width,
					       int height,
					       int components,
					       int bits_per_component,
					       vil_component_format format);
};

//: Generic image implementation for BMP files
class vil_bmp_generic_image : public vil_image_impl {
  vil_stream* is_;
  int magic_;
  int width_;
  int height_;
  int maxval_;

  int start_of_data_;
  int components_;
  int bits_per_component_;

  bool read_header();
  bool write_header();

  friend class vil_bmp_file_format;
public:

  vil_bmp_generic_image(vil_stream* is);
  vil_bmp_generic_image(vil_stream* is, int planes,
					       int width,
					       int height,
					       int components,
					       int bits_per_component,
					       vil_component_format format);
  
  ~vil_bmp_generic_image();

  //: Dimensions.  Planes x W x H x Components
  virtual int planes() const { return 1; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return components_; }

  virtual int bits_per_component() const { return bits_per_component_; }
  virtual enum vil_component_format component_format() const { return VIL_COMPONENT_FORMAT_UNSIGNED_INT; }
  
  //: Copy plane PLANE of this to BUF, 
  virtual bool get_section(void* buf, int x0, int y0, int width, int height) const;
  virtual bool put_section(void const* buf, int x0, int y0, int width, int height);
  
  char const* file_format() const;

  // stuff 
private:
  // members used by vil_bmp
   long  offset;
   xBITMAPINFOHEADER header;
   xBITMAPFILEHEADER fbmp;
   int pixsize;
   int** local_color_map_;

  // functions to emulate base class image
  
  int get_size_x() const {return width();}
  int get_size_y() const {return height();}
  int get_area() const {return width() * height();}
  
};

// classes used to substitute for rekt  - if used later will substitute 




class vil_rekt {
public:
  vil_rekt(int x, int y, int xsz, int ysz)   : xo_(x), yo_(y), xs_(xsz), ys_(ysz) {}
  vil_rekt(int xsz, int ysz)	  : xo_(0), yo_(0), xs_(xsz), ys_(ysz) {}
  vil_rekt()			  : xo_(0), yo_(0), xs_(0),   ys_(0)   {}
  
  int get_orig_x() const	{ return xo_; }
  int get_orig_y() const	{ return yo_; }
  
  int get_size_x() const	{ return xs_; }
  int get_size_y() const	{ return ys_; }
  int get_area() const		{ return xs_ * ys_; }
   
  friend vil_rekt operator * (const vil_rekt& r1, const vil_rekt& r2);	// intersection
  
  friend void subtract(const vil_rekt& r1, const vil_rekt& r2,		// subtraction
		       vil_rekt& ir, vil_rekt& x1, vil_rekt& x2, vil_rekt& y1, vil_rekt& y2);
  
protected:
   int xo_, yo_;			// origin of rectangle
   int xs_, ys_;			// size of rectangle
};

#endif
