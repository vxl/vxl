//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_bmp_file_format_h_
#define vil_bmp_file_format_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: Don Hamilton Peter Tu
// Created: 17 Feb 00

// useful structures for reading a bmp file

//=============================================================================

#define BMP_SIGNATURE_WORD 0x4D42


struct xBITMAPFILEHEADER
{
   unsigned short bfType;        // signature
   unsigned long  bfSize;        // file size in bytes
   unsigned short bfReserved1;   // 0
   unsigned short bfReserved2;   // 0
   unsigned long  bfOffBits;     // offset to bitmap
   xBITMAPFILEHEADER( )
   {
      bfType = BMP_SIGNATURE_WORD;
      bfSize = bfOffBits = 0;
      bfReserved1 = bfReserved2 = 0;
   }
};

struct xBITMAPCOREHEADER
{  
   long  bcSize;      // size of this struct
   short bcWidth;     // bitmap width in pixels         
   short bcHeight;    // bitmap height in pixels         
   short bcPlanes;    // num planes - always 1
   short bcBitCount;  // bits per pixel
   xBITMAPCOREHEADER( )
   {
      bcSize = sizeof(xBITMAPCOREHEADER);
      bcWidth = bcHeight = 0;
      bcPlanes = 1;
      bcBitCount = 0;
   }
};

struct xBITMAPINFOHEADER
{
   unsigned long  biSize;          // size of this struct
   long           biWidth;         // bmap width in pixels
   long           biHeight;        // bmap height in pixels
   unsigned short biPlanes;        // num planes - always 1
   unsigned short biBitCount;      // bits per pixel
   unsigned long  biCompression;   // compression flag
   unsigned long  biSizeImage;     // image size in bytes
   long           biXPelsPerMeter; // horz resolution
   long           biYPelsPerMeter; // vert resolution
   unsigned long  biClrUsed;       // 0 -> color table size
   unsigned long  biClrImportant;  // important color count
   xBITMAPINFOHEADER( )
   {
      biSize = sizeof(xBITMAPINFOHEADER);
      biWidth = biHeight = 0;
      biPlanes = 1;
      biBitCount = 0;
      biCompression = biSizeImage = 0;
      biXPelsPerMeter = biYPelsPerMeter = 0;
      biClrUsed = biClrImportant = 0;
   }
};





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
  
  //: Return the image interpreted as rgb bytes.
  //virtual bool get_section_rgb_byte(void* buf, int plane, int x0, int y0, int width, int height) const;
  //virtual bool get_section_float(void* buf, int plane, int x0, int y0, int width, int height) const;
  //virtual bool get_section_byte(void* buf, int plane, int x0, int y0, int width, int height) const;

  char const* file_format() const;

  // stuff 
private:
  // members used by BMPImage
   long  offset;
   xBITMAPINFOHEADER header;
   xBITMAPFILEHEADER fbmp;
   int pixsize;
   int** local_color_map_;

  // funcions to emulate base class image
  
  int get_size_x() const {return width();}
  int get_size_y() const {return height();}
  int get_area() const {return width() * height();}
  
};

// classes used to substitute for rekt  - if used later will substitute 




class vil_rekt
{
protected:
   
   int xo_, yo_;			// origin of rectangle
   int xs_, ys_;			// size of rectangle
   
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

};




#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_bmp_file_format.
