#ifdef __GNUC__
#pragma implementation "vil_bmp.h"
#endif

#include "vil_bmp.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cstdio.h>  // sprintf()
#include <vcl/vcl_cstring.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>

#include <vil/vil_stream.h>
#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>

typedef unsigned char uchar; 
typedef unsigned char byte;
typedef unsigned short ushort;

char const* vil_bmp_format_tag = "bmp";

vil_image_impl* vil_bmp_file_format::make_input_image(vil_stream* is)
{
  // Attempt to read header

   xBITMAPFILEHEADER head;
   
   is->read(&head, sizeof(head));
   
   if(head.bfType != BMP_SIGNATURE_WORD)
     {
       return 0;
     }
   
   return new vil_bmp_generic_image(is);
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
// I don't know why but, when I read bitmap file header with structure
// variables don't take the values that they should have.
// So, I a, reading one by one and move variables into structure variables
   short sv; 
   long  lv;
   
   is_->seek(0);
   
   is_->read(&sv, sizeof(sv));
   fbmp.bfType = sv;
   is_->read(&lv, sizeof(lv));
   fbmp.bfSize = lv;
   is_->read(&sv, sizeof(sv));
   fbmp.bfReserved1 = sv;
   is_->read(&sv, sizeof(sv));
   fbmp.bfReserved2 = sv;
   is_->read(&lv, sizeof(lv));
   fbmp.bfOffBits = lv;

// This is much better but doesn't work, I really wonder why ?
/*
   if (file->ReadBytes(&fbmp, sizeof(fbmp)) != sizeof(fbmp))
   {
      cout << "Error reading file " << endl;
      return false;
   }

   if( fbmp.bfType != BMP_SIGNATURE_WORD )
   {
      cout <<  "File is not a valid BMP file"<<endl;
      return false;
   }

*/
   // determine which format version is used
   // from structure size field value

   long offset = is_->tell();
   long nbytes = 0;
   
    if (is_->read(&nbytes, sizeof(nbytes)) != sizeof(nbytes))
     {
       cout << "Error reading BMP file size information" << endl;
       return false;
     }

   is_->seek(offset);

cout << "Type    : " << fbmp.bfType<<endl;
cout << "Size    : " << fbmp.bfSize<<endl;
cout << "Res1    : " << fbmp.bfReserved1<<endl;
cout << "Res2    : " << fbmp.bfReserved2<<endl;
cout << "OffBits : " << fbmp.bfOffBits<<endl;
cout << "nbytes  : " << nbytes<<endl;

   // list bitmap header and palette
   if( nbytes == sizeof(xBITMAPCOREHEADER) )
   {
      xBITMAPCOREHEADER bmch;

     cout << "Reading BITMAPCOREHEADER type bmp image" << endl;
     if (is_->read(&bmch, sizeof(bmch)) != sizeof(bmch))
     {
        cout << "Error reading BMP core image header "<<endl;
        return false;
     }

     header.biSize     = bmch.bcSize;
     header.biWidth    = bmch.bcWidth;
     header.biHeight   = bmch.bcHeight;
     header.biPlanes   = bmch.bcPlanes;
     header.biBitCount = bmch.bcBitCount;
   }
   else if( nbytes == sizeof(header) )
        {
          cout << "Reading BITMAPINFOHEADER type bmp image" << endl;
          if (is_->read(&header, sizeof(header)) != sizeof(header))
          {
              cout << "Error reading BMP info image header "<<endl;
              return false;
          }
        }
          else { cout << "Format error" << endl;
                 return false;
               }

   cout << "Reading BMP header" << endl;

   //   SetSizeX(header.biWidth);
   //   SetSizeY(header.biHeight);
   //   SetBitsPixel(header.biBitCount);
   
   width_=header.biWidth;
   height_=header.biHeight;
   bits_per_component_ = header.biBitCount;
   components_ = 1;
   
   // Determine the number of colors and set color map if necessary
   int ccount=0;

   if (header.biClrUsed != 0)
       ccount = header.biClrUsed;
   else
       if (header.biBitCount != 24)
           ccount = 1 << header.biBitCount;
       else 
          {

            // SetImageClass('C'); // color image
            // StringArray sa(3);
            // sa.SetEntry(0, "BLUE");
            // sa.SetEntry(1, "GREEN");
            // sa.SetEntry(2, "RED");
            // SetBandOrder(&sa);
            // SetColorMap(nil);
         }

    
   if (ccount != 0)
     {
       
       vcl_vector<uchar> cmap; // use vector<> to avoid coreleak
       if (header.biSize == sizeof(xBITMAPCOREHEADER))
         cmap.insert(cmap.begin(), (unsigned)ccount*3, (int)0);
       else
         cmap.insert(cmap.begin(), (unsigned)ccount*4, (int)0);
       
       if (is_->read(cmap.begin(),1024) != 1024)
	 {
	   cout << "Error reading image palette" << endl;
	   return false;
	 }
       
       // SetColorNum(ccount);
       // int ncolors = get_color_num();
       int ncolors = ccount; // good guess
       if (ncolors != 0) 
	 {
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
    cout << "Writing BMP header" << endl;
    cout << header.biWidth << ' ' << header.biHeight << ' '
	 << header.biBitCount << endl;
  }

  if (header.biSize == sizeof(xBITMAPINFOHEADER))
      rgb_length = 4;
   else if (header.biSize == sizeof(xBITMAPCOREHEADER))
            rgb_length = 3;
        else
           {
             cout << "Invalid header size"<<endl;
             return false;
           }

  fbmp.bfOffBits = 14+header.biSize+used_color*rgb_length;
  fbmp.bfSize    = fbmp.bfOffBits + get_area()*pixsize;
  

  // SetSizeX(header.biWidth);
  // SetSizeY(header.biHeight);
  // SetBitsPixel(header.biBitCount);


  if (is_->write(&fbmp.bfType, sizeof(fbmp.bfType)) != sizeof(fbmp.bfType))
  {
      cout << "Error while writing BMP file header"<< endl;
      return false;
  }
  if (is_->write(&fbmp.bfSize, sizeof(fbmp.bfSize)) != sizeof(fbmp.bfSize))
  {
      cout << "Error while writing BMP file header"<< endl;
      return false;
  }
  if (is_->write(&fbmp.bfReserved1, sizeof(fbmp.bfReserved1)) != sizeof(fbmp.bfReserved1))
  {
      cout << "Error while writing BMP file header"<< endl;
      return false;
  }
  if (is_->write(&fbmp.bfReserved2, sizeof(fbmp.bfReserved2)) != sizeof(fbmp.bfReserved2))
  {
      cout << "Error while writing BMP file header"<< endl;
      return false;
  }
  if (is_->write(&fbmp.bfOffBits, sizeof(fbmp.bfOffBits)) != sizeof(fbmp.bfOffBits))
  {
      cout << "Error while writing BMP file header"<< endl;
      return false;
  }


  if (header.biSize == sizeof(xBITMAPINFOHEADER))
  {
     rgb_length = 4;
     if (is_->write(&header, sizeof(xBITMAPINFOHEADER)) != sizeof(xBITMAPINFOHEADER))
     {
         cout << "Error while writing header"<< endl;
         return false;
     }

  }
  else if (header.biSize == sizeof(xBITMAPCOREHEADER))
       {
          rgb_length = 3;
          if (is_->write(&header, sizeof(xBITMAPINFOHEADER)) != sizeof(xBITMAPINFOHEADER))
          {
             cout << "Error while writing header"<< endl;
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




  if (bits_per_component_ != 8) {
    cerr << "vil_bmp_generic_image: cannot load " << bits_per_component_ << " bit x " << components_ << " image\n";
    return false;
  }


  vil_rekt r(x0, y0, xs, ys);		// construct rectangle from params
  
  vil_rekt ir, x1, x2, y1, y2;
  
  subtract(r, vil_rekt(get_size_x(),get_size_y()), ir, x1, x2, y1, y2);
  
  int rowbytes=0;
  int bytes_to_read;
  
  rowbytes  = (get_size_x() * pixsize + 3) / 4;
  rowbytes *= 4;
  
  if (ir.get_orig_x() + ir.get_size_x() == get_size_x())
    bytes_to_read = ir.get_size_x() * pixsize + rowbytes - get_size_x() * pixsize;
  else
    bytes_to_read  = ir.get_size_x() * pixsize;
  
  if (!ib)
    {
      ib = new byte [r.get_size_y()*pixsize + bytes_to_read];
      // if (!ib) { SetStatusBad(); return nil; }
    }
      
  
  uchar* dp	= (uchar*)ib;
  uchar* dp2;
  dp2 = new byte [ir.get_size_y() * pixsize + bytes_to_read];
  
  int skip;
  if (xs >= ys) // if horizontal bytes are requested
    {
      skip = int(offset)+(( (rowbytes * get_size_y()) - ( (ir.get_orig_y() * rowbytes) + (rowbytes-ir.get_orig_x()) * (ir.get_size_y())))) ;
      // file->SkipToByte(skip);
      is_->seek(skip);
    }
  else
    {
      skip = int(offset)+( (rowbytes * get_size_y()) - (rowbytes*ir.get_orig_y() + rowbytes - ir.get_orig_x()*pixsize ))  ;
      //file->SkipToByte(skip);
      is_->seek(skip);
    }
  
  int y1c,y2c;
  
  if (xs >= ys)
    {
      y1c	= y1.get_size_x()*pixsize;
      y2c	= y2.get_size_x()*pixsize;
    }
  else
    {
      y1c	= x1.get_size_y()*pixsize;
      y2c	= x2.get_size_y()*pixsize;
    }
  
   ushort def	= 0;                                    // XXX
   
   register int xc, yc;					// counters
   
   int tc,tr;
   if (xs >= ys)
     {
       tc = ir.get_size_y();
       tr = ir.get_size_x();
     }
   else
     {
       tc = ir.get_size_x(); 
       tr = ir.get_size_y();
     }
   
   if (xs >= ys)
     {
       
       for (xc = x1.get_area()*pixsize; xc; xc--) *dp++ = def;// fill x1 with default
       
       for (yc = ir.get_size_y(); yc; yc--)
         {
           for (xc = y1c; xc; xc--) *dp++ = def;		// fill a row of y1
           
           //   file->ReadBytes(dp2, bytes_to_read);	// read a row of ir
           is_->read(dp2, bytes_to_read);	// read a row of ir
           for (int i=0; i<bytes_to_read; i++)
             dp[i] = (uchar)dp2[i];
           
           skip = offset + ((get_size_x() * get_size_y()) - ((ir.get_orig_y() * get_size_x() + (get_size_x()-ir.get_orig_x()) * (ir.get_size_y())))) * pixsize;
           
           is_->seek(skip);
           dp += ir.get_size_x()*pixsize;
           
           for (xc = y2c; xc; xc--) *dp++ = def;		// fill a row of y2
         }
       
       for (xc = x2.get_area()*pixsize; xc; xc--) *dp++ = def;	// fill x2
     }
   else
     {
       //for (xc = y1.get_area()*pixsize; xc; xc--) *dp++ = def; // fill x1 with default
       
       int s = 0;
       for (yc = tr; yc; yc--)
         {
           //for (xc = y1c; xc; xc--) *dp++ = def;             // fill a row of y1
           
           is_->read(&dp[s], tc*pixsize); // read a row of ir
           s+=tc*pixsize;
           
           
           // file->SkipBytes(-(rowbytes+tc*pixsize));
           is_->seek(is_->tell()-(rowbytes+tc*pixsize));
         }
       
       //for (xc = x2.get_area()*pixsize; xc; xc--) *dp++ = def;       // fill x2
       
     }
   return ib != 0;
}






bool vil_bmp_generic_image::put_section(void const *ib, int x0, int y0, int xs, int ys)
{
  
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
    
   if (!ib)
   {
      fprintf(stderr, "PutSection: nil buffer pointer\n");
      return false;
   }

   // if (get_access() == 'r')
   // {
   //   fprintf(stderr, "PutSection: image is read only\n");
   //   return -1;
   // }

   //(void) bytes_to_write;

   int skip;

   skip = ( int(offset) + (( (rowbytes * get_size_y()) - ( (ir.get_orig_y() * rowbytes) + (rowbytes - ir.get_orig_x()) * (ir.get_size_y())))));

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

   uchar* sp    = (uchar*) buf + (r.get_size_x() * roX + roY);

#endif
   int spinc	= r.get_size_x();
       skip	= (rowbytes - ir.get_size_x()) * pixsize;
   int nshorts	= ir.get_size_x();
   int nbytes	= nshorts * pixsize;

   for (int yc = ir.get_size_y(); yc; yc--)
   {
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

