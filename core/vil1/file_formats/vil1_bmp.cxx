#ifdef __GNUC__
#pragma implementation "vil_bmp.h"
#endif

#include "vil_bmp.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cstring.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>

#include <vil/vil_stream.h>
#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>
#include <vil/vil_byte_swap.h>

#if VXL_LITTLE_ENDIAN
# define read_int_macro(s, var) s->read(&var, sizeof(var))
#else
# define read_int_macro(s, var) s->read(&var, sizeof(var)); vil_byte_swap(var)
#endif

xBITMAPFILEHEADER::xBITMAPFILEHEADER()
{
  bfType[0] = BMP_SIGNATURE_BYTE_0;
  bfType[1] = BMP_SIGNATURE_BYTE_1;
  bfSize = 0;
  bfOffBits = 0;
  bfReserved1 = 0;
  bfReserved2 = 0;
}

void xBITMAPFILEHEADER::print(ostream &os) const
{
  os << vcl_hex
     << "Type    : "
     << "0x" << unsigned(bfType[0]) << ' ' 
     << "0x" << unsigned(bfType[1]) << endl
     << "Size    : 0x" << bfSize << endl
     << "Res1    : 0x" << bfReserved1 << endl
     << "Res2    : 0x" << bfReserved2 << endl
     << "OffBits : 0x" << bfOffBits << endl
     << vcl_dec;
}

void xBITMAPFILEHEADER::read(vil_stream *s)
{
  s->read(&bfType, sizeof(bfType));
  read_int_macro(s, bfSize);
  read_int_macro(s, bfReserved1);
  read_int_macro(s, bfReserved2);
  read_int_macro(s, bfOffBits);
}

void xBITMAPFILEHEADER::write(vil_stream *s) const
{
  abort();
}

bool xBITMAPFILEHEADER::signature_valid() const
{
  return
    bfType[0] == BMP_SIGNATURE_BYTE_0 &&
    bfType[1] == BMP_SIGNATURE_BYTE_1;
}


xBITMAPCOREHEADER::xBITMAPCOREHEADER()
{
  bcSize = sizeof(*this);
  bcWidth = bcHeight = 0;
  bcPlanes = 1;
  bcBitCount = 0;
}

void xBITMAPCOREHEADER::read(vil_stream *s)
{
  read_int_macro(s, bcSize);
  read_int_macro(s, bcWidth);
  read_int_macro(s, bcHeight);
  read_int_macro(s, bcPlanes);
  read_int_macro(s, bcBitCount);
}

xBITMAPINFOHEADER::xBITMAPINFOHEADER()
{
  biSize = sizeof(*this);
  biWidth = biHeight = 0;
  biPlanes = 1;
  biBitCount = 0;
  biCompression = biSizeImage = 0;
  biXPelsPerMeter = biYPelsPerMeter = 0;
  biClrUsed = biClrImportant = 0;
}

void xBITMAPINFOHEADER::read(vil_stream *s)
{
  read_int_macro(s, biSize);
  read_int_macro(s, biWidth);
  read_int_macro(s, biHeight);
  read_int_macro(s, biPlanes);
  read_int_macro(s, biBitCount);
  read_int_macro(s, biCompression);
  read_int_macro(s, biSizeImage);
  read_int_macro(s, biXPelsPerMeter);
  read_int_macro(s, biYPelsPerMeter);
  read_int_macro(s, biClrUsed);
  read_int_macro(s, biClrImportant);
}

//--------------------------------------------------------------------------------

char const* vil_bmp_format_tag = "bmp";

vil_image_impl* vil_bmp_file_format::make_input_image(vil_stream* is)
{
  // Attempt to read header
  xBITMAPFILEHEADER head;
  is->seek(0);
  head.read(is);
  
  if ( head.signature_valid() )
    return new vil_bmp_generic_image(is);
  
  //cerr << "not a .bmp file" << endl;
  return 0;
}

vil_image_impl* vil_bmp_file_format::make_output_image(vil_stream* is, int planes,
						       int width,
						       int height,
						       int components,
						       int bits_per_component,
						       vil_component_format format)
{
  return new vil_bmp_generic_image(is, planes, width, height, components, bits_per_component, format);
}

char const* vil_bmp_file_format::tag() const
{
  return vil_bmp_format_tag;
}

/////////////////////////////////////////////////////////////////////////////
  

vil_bmp_generic_image::vil_bmp_generic_image(vil_stream* is):
  is_(is)
{
  local_color_map_=0;

  read_header();
}

char const* vil_bmp_generic_image::file_format() const
{
  return vil_bmp_format_tag;
}

vil_bmp_generic_image::vil_bmp_generic_image(vil_stream* is, int planes,
					       int width,
					       int height,
					       int components,
					       int bits_per_component,
					       vil_component_format format):
  is_(is)
{
  local_color_map_=0;

  width_ = width;  // TODO what is this suppposed to do?
  height_ = height;
  components_ = components;
  bits_per_component_ = bits_per_component;

  if (components_ == 3) {
    magic_ = 6;
  } else if (components_ == 1) {
    if (bits_per_component_ == 1)
      magic_ = 4;
    else
      magic_ = 5;
  } 
  if (bits_per_component_ <= 8) {
    maxval_ = (1 << 8) - 1;
    bits_per_component_ = 8;
  } else if (bits_per_component_ <= 16) {
    maxval_ = (1 << 16) - 1;
    bits_per_component_ = 16;
  } else if (bits_per_component_ <= 24) {
    maxval_ = (1 << 24) - 1;
    bits_per_component_ = 24;
  } else { 
    cerr << "vil_bmp_generic_image: cannot make  " << bits_per_component_ << " bit x " << components_ << " image\n";
  }

  write_header();
}

vil_bmp_generic_image::~vil_bmp_generic_image()
{
  // we must get rid of the local_color_map_;
  if(local_color_map_){
    delete [] local_color_map_[0];
    delete [] local_color_map_[1];
    delete [] local_color_map_[2];
    delete local_color_map_;
  }
}

bool vil_bmp_generic_image::read_header()
{
  // seek to beginning and read header.
  is_->seek(0);
  fbmp.read(is_);
  if( ! fbmp.signature_valid() )
  {
    cerr <<  "File is not a valid BMP file"<<endl;
    return false;
  }
  // debugging blather
  fbmp.print(cerr);

  // determine which format version is used
  // from structure size field value

  // remember offset  
  long offset = is_->tell();

  long nbytes = 0;
  read_int_macro(is_, nbytes);
  cerr << "nbytes  : " << nbytes << endl;

  // seek back
  is_->seek(offset);
  
  // list bitmap header and palette
  if( nbytes == sizeof(xBITMAPCOREHEADER) ) {
    xBITMAPCOREHEADER bmch;
    bmch.read(is_);
    header.biSize     = bmch.bcSize;
    header.biWidth    = bmch.bcWidth;
    header.biHeight   = bmch.bcHeight;
    header.biPlanes   = bmch.bcPlanes;
    header.biBitCount = bmch.bcBitCount;
  }
  else if( nbytes == sizeof(header) ) {// xBITMAPINFOHEADER
    cerr << "Reading BITMAPINFOHEADER type bmp image" << endl;
    header.read(is_);
  }
  else {
    cerr << "Format error" << endl;
    return false;
  }

  cerr << "Reading BMP header" << endl;
  
  //   SetSizeX(header.biWidth);
  //   SetSizeY(header.biHeight);
  //   SetBitsPixel(header.biBitCount);
  
  width_=header.biWidth;
  height_=header.biHeight;
  bits_per_component_ = header.biBitCount;
  components_ = 1;

  // fsm@robots. this makes it work with rgb bmps.
  if (components_ == 1 && bits_per_component_ == 24) {
    components_ = 3;
    bits_per_component_ = 8;
  }

   
  // Determine the number of colors and set color map if necessary
  int ccount=0;
  
  if (header.biClrUsed != 0)
    ccount = header.biClrUsed;
  else if (header.biBitCount != 24)
    ccount = 1 << header.biBitCount;
  else {
    // SetImageClass('C'); // color image
    // StringArray sa(3);
    // sa.SetEntry(0, "BLUE");
    // sa.SetEntry(1, "GREEN");
    // sa.SetEntry(2, "RED");
    // SetBandOrder(&sa);
    // SetColorMap(nil);
  }
  
  if (ccount != 0) {
    vcl_vector<uchar> cmap; // use vcl_vector<> to avoid coreleak
    if (header.biSize == sizeof(xBITMAPCOREHEADER))
      cmap.insert(cmap.begin(), (unsigned)ccount*3, (int)0);
    else
      cmap.insert(cmap.begin(), (unsigned)ccount*4, (int)0);
    
    if (is_->read(cmap.begin(),1024) != 1024) {
      cerr << "Error reading image palette" << endl;
      return false;
    }
    
    // SetColorNum(ccount);
    // int ncolors = get_color_num();
    int ncolors = ccount; // good guess
    if (ncolors != 0) {
      int **color_map = new int*[3];
      for (int i=0; i<3; ++i) {
	color_map[i] = new int[ncolors];
	for (int j=0; j<ncolors; j++) 
	  color_map[i][j] = (int) cmap[2-i+4*j];
      }
      
      // SetColorMap(color_map);  - TODO find out where to save a color map
      local_color_map_=color_map;
    }
    
  }
  
  // SetOffset(file->Tell());
  start_of_data_ = is_->tell();
  
  // TODO not obvious where the magic number is read 
  
  return true;
}

bool vil_bmp_generic_image::write_header()
{

  int rgb_length=0;

  int used_color=0;

  if (header.biClrUsed != 0)
      used_color = header.biClrUsed;
  else
      if (header.biBitCount != 24)
          used_color = 1 << header.biBitCount;

  if (false) {
    cerr << "Writing BMP header" << endl;
    cerr << header.biWidth << ' ' << header.biHeight << ' '
	 << header.biBitCount << endl;
  }

  if (header.biSize == sizeof(xBITMAPINFOHEADER))
      rgb_length = 4;
   else if (header.biSize == sizeof(xBITMAPCOREHEADER))
            rgb_length = 3;
        else
           {
             cerr << "Invalid header size"<<endl;
             return false;
           }

  fbmp.bfOffBits = 14+header.biSize+used_color*rgb_length;
  fbmp.bfSize    = fbmp.bfOffBits + get_area()*pixsize;
  

  // SetSizeX(header.biWidth);
  // SetSizeY(header.biHeight);
  // SetBitsPixel(header.biBitCount);


  if (is_->write(&fbmp.bfType, sizeof(fbmp.bfType)) != sizeof(fbmp.bfType))
  {
      cerr << "Error while writing BMP file header"<< endl;
      return false;
  }
  if (is_->write(&fbmp.bfSize, sizeof(fbmp.bfSize)) != sizeof(fbmp.bfSize))
  {
      cerr << "Error while writing BMP file header"<< endl;
      return false;
  }
  if (is_->write(&fbmp.bfReserved1, sizeof(fbmp.bfReserved1)) != sizeof(fbmp.bfReserved1))
  {
      cerr << "Error while writing BMP file header"<< endl;
      return false;
  }
  if (is_->write(&fbmp.bfReserved2, sizeof(fbmp.bfReserved2)) != sizeof(fbmp.bfReserved2))
  {
      cerr << "Error while writing BMP file header"<< endl;
      return false;
  }
  if (is_->write(&fbmp.bfOffBits, sizeof(fbmp.bfOffBits)) != sizeof(fbmp.bfOffBits))
  {
      cerr << "Error while writing BMP file header"<< endl;
      return false;
  }


  if (header.biSize == sizeof(xBITMAPINFOHEADER))
  {
     rgb_length = 4;
     if (is_->write(&header, sizeof(xBITMAPINFOHEADER)) != sizeof(xBITMAPINFOHEADER))
     {
         cerr << "Error while writing header"<< endl;
         return false;
     }

  }
  else if (header.biSize == sizeof(xBITMAPCOREHEADER))
       {
          rgb_length = 3;
          if (is_->write(&header, sizeof(xBITMAPINFOHEADER)) != sizeof(xBITMAPINFOHEADER))
          {
             cerr << "Error while writing header"<< endl;
             return false;
          }
       }

  if (used_color != 0)
  {

    // int** cm = get_color_map();
    int** cm = local_color_map_;
    
    for(int j=0; j<used_color; j++){
          for(int i=0; i<3; i++)
              is_->write(&cm[2-i][j], 1);
          is_->write(0, 1);
      }

  }


  //  SetOffset(file->Tell());

  return true;

}

bool vil_bmp_generic_image::get_section(void* ib, int x0, int y0, int xs, int ys) const
{
  assert(ib);

  if (bits_per_component_ != 8) {
    cerr << "vil_bmp_generic_image::get_section() : cannot read " 
	 << bits_per_component_ << "-bit, " << components_ << "-component image\n";
    return false;
  }

  // fsm: an easy case that works.
  if (x0==0 && y0==0 && xs==width() && ys==height()) {
    cerr << "fred\'s easy case" << endl;
    char *bp = static_cast<char*>(ib);
    is_->seek(start_of_data_);
    unsigned want = xs*(bits_per_component_*components_/8); // number of bytes we want per raster.
    unsigned have = ((want + 3)>>2)<<2; // number of bytes in file per raster.
    for (unsigned i=0; i<ys; ++i) {
      is_->read(bp + want*i, want);
      char wibble[4]; is_->read(wibble, have-want); // discard padding
    }
    return true;
  }
  // dunno about the rest....


  vil_rekt r(x0, y0, xs, ys);		// construct rectangle from params
  
  vil_rekt ir, x1, x2, y1, y2;
  
  subtract(r, vil_rekt(get_size_x(),get_size_y()), ir, x1, x2, y1, y2);
    
  int rowbytes  = (get_size_x() * pixsize + 3) / 4;
  rowbytes *= 4;
  
  int bytes_to_read;
  if (ir.get_orig_x() + ir.get_size_x() == get_size_x())
    bytes_to_read = ir.get_size_x() * pixsize + rowbytes - get_size_x() * pixsize;
  else
    bytes_to_read  = ir.get_size_x() * pixsize;
        
  
  uchar* dp  = (uchar*)ib;
  uchar* dp2 = new uchar [ir.get_size_y() * pixsize + bytes_to_read];
  
  int skip;
  if (xs >= ys) { // if horizontal bytes are requested
    skip =
      int(offset) +
      ( (rowbytes * get_size_y()) - ( (ir.get_orig_y() * rowbytes) + (rowbytes-ir.get_orig_x()) * (ir.get_size_y())));
  }
  else {
    skip =
      int(offset) + 
      ( (rowbytes * get_size_y()) - (rowbytes*ir.get_orig_y() + rowbytes - ir.get_orig_x()*pixsize ));
  }
  is_->seek(skip);
  
  
  int y1c,y2c;
  if (xs >= ys) {
    y1c	= y1.get_size_x()*pixsize;
    y2c	= y2.get_size_x()*pixsize;
  }
  else {
    y1c	= x1.get_size_y()*pixsize;
    y2c	= x2.get_size_y()*pixsize;
  }
  
  ushort def	= 0;                                    // XXX
  
  register int xc, yc;					// counters
  
  int tc,tr;
  if (xs >= ys) {
    tc = ir.get_size_y();
    tr = ir.get_size_x();
  }
  else {
    tc = ir.get_size_x(); 
    tr = ir.get_size_y();
  }
  
  if (xs >= ys) {
    
    for (xc = x1.get_area()*pixsize; xc; xc--) 
      *dp++ = def;// fill x1 with default
    
    for (yc = ir.get_size_y(); yc; yc--) {
      for (xc = y1c; xc; xc--) *dp++ = def;		// fill a row of y1
      
      //   file->ReadBytes(dp2, bytes_to_read);	// read a row of ir
      is_->read(dp2, bytes_to_read);	// read a row of ir
      for (int i=0; i<bytes_to_read; i++)
	dp[i] = (uchar)dp2[i];
      
      skip = offset + ((get_size_x() * get_size_y()) - ((ir.get_orig_y() * get_size_x() + (get_size_x()-ir.get_orig_x()) * (ir.get_size_y())))) * pixsize;
      
      is_->seek(skip);
      dp += ir.get_size_x()*pixsize;
      
      for (xc = y2c; xc; xc--) 
	*dp++ = def;		// fill a row of y2
    }
    
    for (xc = x2.get_area()*pixsize; xc; xc--) *dp++ = def;	// fill x2
  }
  else {
    //for (xc = y1.get_area()*pixsize; xc; xc--) *dp++ = def; // fill x1 with default
    
    int s = 0;
    for (yc = tr; yc; yc--) {
      //for (xc = y1c; xc; xc--) *dp++ = def;             // fill a row of y1
      
      is_->read(&dp[s], tc*pixsize); // read a row of ir
      s+=tc*pixsize;
      
      
      // file->SkipBytes(-(rowbytes+tc*pixsize));
      is_->seek(is_->tell()-(rowbytes+tc*pixsize));
    }
    
    //for (xc = x2.get_area()*pixsize; xc; xc--) *dp++ = def;       // fill x2
    
  }

  return true;
}






bool vil_bmp_generic_image::put_section(void const *ib, int x0, int y0, int xs, int ys)
{
  assert(ib);

  if (bits_per_component_ != 8) {
    cerr << "vil_bmp_generic_image: cannot save " << bits_per_component_ << " bit x " << components_ << " image\n";
    return false;
  }
  

   vil_rekt r(x0, y0, xs, ys);		// construct rectangle from params

   vil_rekt ir = r * vil_rekt(get_size_x(),get_size_y());	// intersect with image extent

   int rowbytes = (get_size_x() * pixsize + 3) / 4;
   rowbytes *= 4;
   
// int bytes_to_write;
// if (ir.get_orig_x() + ir.get_size_x() == get_size_x())
//   bytes_to_write = ir.get_size_x() * pixsize + rowbytes - get_size_x() * pixsize;
// else
//    bytes_to_write  = ir.get_size_x() * pixsize; 
    
   // if (get_access() == 'r')
   // {
   //   fprintf(stderr, "PutSection: image is read only\n");
   //   return -1;
   // }

   //(void) bytes_to_write;

   int skip = 
     int(offset) +
     ( (rowbytes * get_size_y()) - ( (ir.get_orig_y() * rowbytes) + (rowbytes - ir.get_orig_x()) * (ir.get_size_y())));

   // file->SkipToByte(skip);
   is_->seek(skip);

#ifndef hpux
   //   intPoint2D ro = ir.get_orig() - r.get_orig();  // origin of ir relative to r
   //   uchar* sp    = (uchar*) ib + (r.get_size_x() * ro.X() + ro.Y());
   uchar* sp    = (uchar*) ib + (r.get_size_x() * (ir.get_orig_x() -r.get_orig_x()) 
                                 + (ir.get_orig_y() -r.get_orig_y()) );
#else
   int roX = ir.get_orig_x() - r.get_orig_x();
   int roY = ir.get_orig_y() - r.get_orig_y();

   uchar* sp    = (uchar*) ib + (r.get_size_x() * roX + roY);
#endif
   
   int spinc	= r.get_size_x();
   skip	= (rowbytes - ir.get_size_x()) * pixsize;
   int nshorts	= ir.get_size_x();
   int nbytes	= nshorts * pixsize;

   for (int yc = ir.get_size_y(); yc; yc--) {
     // file->WriteBytes(sp, nbytes);			// write it
     // file->SkipBytes(skip);
     is_->write(sp, nbytes);			// write it
     is_->seek(is_->tell() + skip);
     sp += spinc;
   }

   return true;
}





// Intersection of r1 and r2.

vil_rekt operator * (const vil_rekt& r1, const vil_rekt& r2)
{
  //   int x0 = max(r1.xo_, r2.xo_);			// upper-left of intersection
  // int y0 = max(r1.yo_, r2.yo_);
  // int x1 = min(r1.xo_ + r1.xs_, r2.xo_ + r2.xs_);	// lower-right of intersection
  // int y1 = min(r1.yo_ + r1.ys_, r2.yo_ + r2.ys_);

  int x0 = r1.xo_;
  if(r2.xo_>r1.xo_){
    x0 = r2.xo_;
  }
  
  int y0 = r1.yo_;
  if(r2.yo_>r1.yo_){
    y0 = r2.yo_;
  }
  
  int x1 = r1.xo_ + r1.xs_;
  if(r2.xo_ + r2.xs_ <x1){
    x1 = r2.xo_ + r2.xs_;
  }

  int y1 = r1.yo_ + r1.ys_;
  if(r2.yo_ + r2.ys_ <y1){
    y1 = r2.yo_ + r2.ys_;
  }

   if (x1 > x0 && y1 > y0)
      return vil_rekt(x0, y0, x1 - x0, y1 - y0);
   else
      return vil_rekt(0, 0, 0, 0);			// no intersection
}

//-----------------------------------------------------------------------------
// Difference of r1 and r2.

/*
  This description assumes an x-axis pointing to the "right" and a y-axis
  pointing "down".  The code, however, does not depend on axis directions.

  Return parameters:

	ir = intersection of r1 and r2
	x1 = portion of r1 above ir, width = width of r1
	x2 = portion of r1 below ir, width = width of r1
	y1 = portion of r1 to left of ir, height = height of ir
	y2 = portion of r1 to right of ir, height = height of ir

  Special case:  if r1 and r2 don't intersect, x1 = r1, ir = x2 = y1 = y2 = nil

  Ex.1: r1 & r2 partially intersect    Ex.2: r1 fully contains r2

     r1 ~~~~~~~~~~~~~~~~~	     r1 ~~~~~~~~~~~~~~~~~~~~~~~~~
	|            	|		|			|
	|	x1	|		|	    x1		|
	|		|		|			|
	|-------~~~~~~~~~~~~~~~~~	|-------~~~~~~~~~~~-----|
	|	|	|	|	|	|	  |	|
	|   y1	|   ir	|	|	|   y1	| r2 = ir |  y2	|
	|	|	|	|	|	|	  |	|
	|-------~~~~~~~~~~~~~~~~~ r2	|-------~~~~~~~~~~~-----|
	|	x2	|		|	    x2		|
	~~~~~~~~~~~~~~~~~		~~~~~~~~~~~~~~~~~~~~~~~~~

        (width of y2 = 0)
*/

void subtract(const vil_rekt& r1, const vil_rekt& r2,
	      vil_rekt& ir, vil_rekt &x1, vil_rekt& x2, vil_rekt& y1, vil_rekt& y2)
{
   ir = r1 * r2;		// intersection

   // if (!ir)			// if r1 and r2 don't intersect
   if(!ir.get_area())
   {
      x1 = r1;
      x2 = y1 = y2 = vil_rekt(0, 0, 0, 0);
   }
   else
   {
      x1.xo_ = r1.xo_;
      x1.yo_ = r1.yo_;
      x1.xs_ = r1.xs_;
      x1.ys_ = ir.yo_ - r1.yo_;

      y1.xo_ = r1.xo_;
      y1.yo_ = r1.yo_ + x1.ys_;
      y1.xs_ = ir.xo_ - r1.xo_;
      y1.ys_ = ir.ys_;

      int xsum = y1.xs_ + ir.xs_;
      int ysum = x1.ys_ + ir.ys_;

      x2.xo_ = r1.xo_;
      x2.yo_ = r1.yo_ + ysum;
      x2.xs_ = r1.xs_;
      x2.ys_ = r1.ys_ - ysum;

      y2.xo_ = r1.xo_ + xsum;
      y2.yo_ = y1.yo_;
      y2.xs_ = r1.xs_ - xsum;
      y2.ys_ = ir.ys_;
   }
}

