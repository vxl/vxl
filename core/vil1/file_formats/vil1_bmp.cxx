#ifdef __GNUC__
#pragma implementation "vil_bmp.h"
#endif

#include "vil_bmp.h"

#include <assert.h>
#include <stdio.h> // for sprintf

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cstring.h>

#include <vil/vil_stream.h>
#include <vil/vil_generic_image.h>

typedef unsigned char uchar; 
typedef unsigned char byte;
typedef unsigned short ushort;

char const* vil_bmp_format_tag = "bmp";

vil_generic_image* vil_bmp_file_format::make_input_image(vil_stream* is)
{
  // Attempt to read header

   xBITMAPFILEHEADER head;
   
   is->read((void*)(&head),sizeof(head));
   
   if(head.bfType != BMP_SIGNATURE_WORD)
     {
       return 0;
     }
   
   return new vil_bmp_generic_image(is);
}

vil_generic_image* vil_bmp_file_format::make_output_image(vil_stream* is, vil_generic_image const* prototype)
{
  return new vil_bmp_generic_image(is, prototype);
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

vil_bmp_generic_image::vil_bmp_generic_image(vil_stream* is, vil_generic_image const* prototype):
  is_(is)
{
  local_color_map_=0;

  width_ = prototype->width();  // TODO what is this suppposed to do?
  height_ = prototype->height();
  components_ = prototype->components();
  bits_per_component_ = prototype->bits_per_component();

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

      uchar *cmap;
      if (header.biSize == sizeof(xBITMAPCOREHEADER))
         cmap = new uchar[ccount * 3];
      else
         cmap = new uchar[ccount * 4];

      if (is_->read(cmap,1024) != 1024)
      {
         cout << "Error reading image palette" << endl;
         return false;
      }

      // SetColorNum(ccount);
      // int ncolors = GetColorNum();
      int ncolors = ccount; // good guess
      if (ncolors != 0) 
      {
         int **color_map = new int*[3];
         uchar* cmapp    = cmap;
         for (int i=0; i<3; ++i) {
             color_map[i] = new int[ncolors];
             for (int j=0; j<ncolors; j++) 
                 color_map[i][j] = (int) cmapp[2-i+4*j];
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
  fbmp.bfSize    = fbmp.bfOffBits + GetArea()*pixsize;
  

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

    // int** cm = GetColorMap();
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

bool vil_bmp_generic_image::do_get_section(void* ib, int x0, int y0, int xs, int ys) const
{




  if (bits_per_component_ != 8) {
    cerr << "vil_bmp_generic_image: cannot load " << bits_per_component_ << " bit x " << components_ << " image\n";
    return false;
  }


  vil_rekt r(x0, y0, xs, ys);		// construct rectangle from params
  
  vil_rekt ir, x1, x2, y1, y2;
  
  subtract(r, vil_rekt(GetSizeX(),GetSizeY()), ir, x1, x2, y1, y2);
  
  int rowbytes=0;
  int bytes_to_read;
  
  rowbytes  = (GetSizeX() * pixsize + 3) / 4;
  rowbytes *= 4;
  
  if (ir.GetOrigX() + ir.GetSizeX() == GetSizeX())
    bytes_to_read = ir.GetSizeX() * pixsize + rowbytes - GetSizeX() * pixsize;
  else
    bytes_to_read  = ir.GetSizeX() * pixsize;
  
  if (!ib)
    {
      ib = new byte [r.GetSizeY()*pixsize + bytes_to_read];
      // if (!ib) { SetStatusBad(); return nil; }
    }
      
  
  uchar* dp	= (uchar*)ib;
  uchar* dp2;
  dp2 = new byte [ir.GetSizeY() * pixsize + bytes_to_read];
  
  int skip;
  if (xs >= ys) // if horizontal bytes are requested
    {
      skip = int(offset)+(( (rowbytes * GetSizeY()) - ( (ir.GetOrigY() * rowbytes) + (rowbytes-ir.GetOrigX()) * (ir.GetSizeY())))) ;
      // file->SkipToByte(skip);
      is_->seek(skip);
    }
  else
    {
      skip = int(offset)+( (rowbytes * GetSizeY()) - (rowbytes*ir.GetOrigY() + rowbytes - ir.GetOrigX()*pixsize ))  ;
      //file->SkipToByte(skip);
      is_->seek(skip);
    }
  
  int y1c,y2c;
  
  if (xs >= ys)
    {
      y1c	= y1.GetSizeX()*pixsize;
      y2c	= y2.GetSizeX()*pixsize;
    }
  else
    {
      y1c	= x1.GetSizeY()*pixsize;
      y2c	= x2.GetSizeY()*pixsize;
    }
  
   ushort def	= 0;                                    // XXX
   
   register int xc, yc;					// counters
   
   int tc,tr;
   if (xs >= ys)
     {
       tc = ir.GetSizeY();
       tr = ir.GetSizeX();
     }
   else
     {
       tc = ir.GetSizeX(); 
       tr = ir.GetSizeY();
     }
   
   if (xs >= ys)
     {
       
       for (xc = x1.GetArea()*pixsize; xc; xc--) *dp++ = def;// fill x1 with default
       
       for (yc = ir.GetSizeY(); yc; yc--)
         {
           for (xc = y1c; xc; xc--) *dp++ = def;		// fill a row of y1
           
           //   file->ReadBytes(dp2, bytes_to_read);	// read a row of ir
           is_->read(dp2, bytes_to_read);	// read a row of ir
           for (int i=0; i<bytes_to_read; i++)
             dp[i] = (uchar)dp2[i];
           
           skip = offset + ((GetSizeX() * GetSizeY()) - ((ir.GetOrigY() * GetSizeX() + (GetSizeX()-ir.GetOrigX()) * (ir.GetSizeY())))) * pixsize;
           
           is_->seek(skip);
           dp += ir.GetSizeX()*pixsize;
           
           for (xc = y2c; xc; xc--) *dp++ = def;		// fill a row of y2
         }
       
       for (xc = x2.GetArea()*pixsize; xc; xc--) *dp++ = def;	// fill x2
     }
   else
     {
       //for (xc = y1.GetArea()*pixsize; xc; xc--) *dp++ = def; // fill x1 with default
       
       int s = 0;
       for (yc = tr; yc; yc--)
         {
           //for (xc = y1c; xc; xc--) *dp++ = def;             // fill a row of y1
           
           is_->read(&dp[s], tc*pixsize); // read a row of ir
           s+=tc*pixsize;
           
           
           // file->SkipBytes(-(rowbytes+tc*pixsize));
           is_->seek(is_->tell()-(rowbytes+tc*pixsize));
         }
       
       //for (xc = x2.GetArea()*pixsize; xc; xc--) *dp++ = def;       // fill x2
       
     }
   return ib;
}






bool vil_bmp_generic_image::do_put_section(void const *ib, int x0, int y0, int xs, int ys)
{
  
  if (bits_per_component_ != 8) {
    cerr << "vil_bmp_generic_image: cannot save " << bits_per_component_ << " bit x " << components_ << " image\n";
    return false;
  }
  



   vil_rekt r(x0, y0, xs, ys);		// construct rectangle from params

   vil_rekt ir = r * vil_rekt(GetSizeX(),GetSizeY());	// intersect with image extent

   int rowbytes = (GetSizeX() * pixsize + 3) / 4;
   rowbytes *= 4;
   
// int bytes_to_write;
// if (ir.GetOrigX() + ir.GetSizeX() == GetSizeX())
//   bytes_to_write = ir.GetSizeX() * pixsize + rowbytes - GetSizeX() * pixsize;
// else
//    bytes_to_write  = ir.GetSizeX() * pixsize; 
    
   if (!ib)
   {
      fprintf(stderr, "PutSection: nil buffer pointer\n");
      return -1;
   }

   // if (GetAccess() == 'r')
   // {
   //   fprintf(stderr, "PutSection: image is read only\n");
   //   return -1;
   // }


   int skip;

   skip = ( int(offset) + (( (rowbytes * GetSizeY()) - ( (ir.GetOrigY() * rowbytes) + (rowbytes - ir.GetOrigX()) * (ir.GetSizeY())))));

   // file->SkipToByte(skip);
   is_->seek(skip);

#ifndef hpux
   //   intPoint2D ro = ir.GetOrig() - r.GetOrig();  // origin of ir relative to r
   //   uchar* sp    = (uchar*) ib + (r.GetSizeX() * ro.X() + ro.Y());
   uchar* sp    = (uchar*) ib + (r.GetSizeX() * (ir.GetOrigX() -r.GetOrigX()) 
                                 + (ir.GetOrigY() -r.GetOrigY()) );


#else
   //   int roX = ir.GetOrig().X() - r.GetOrig().X();
   //   int roY = ir.GetOrig().Y() - r.GetOrig().Y();
   //   uchar* sp    = (uchar*) ib + (r.GetSizeX() * roX + roY);

   int roX = ir.GetOrigX() - r.GetOrigX();
   int roY = ir.GetOrigY() - r.GetOrigY();

   uchar* sp    = (uchar*) buf + (r.GetSizeX() * roX + roY);

#endif
   int spinc	= r.GetSizeX();
       skip	= (rowbytes - ir.GetSizeX()) * pixsize;
   int nshorts	= ir.GetSizeX();
   int nbytes	= nshorts * pixsize;

   for (int yc = ir.GetSizeY(); yc; yc--)
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
  //   int x0 = max(r1.xo, r2.xo);			// upper-left of intersection
  // int y0 = max(r1.yo, r2.yo);
  // int x1 = min(r1.xo + r1.xs, r2.xo + r2.xs);	// lower-right of intersection
  // int y1 = min(r1.yo + r1.ys, r2.yo + r2.ys);

  int x0 = r1.xo;
  if(r2.xo>r1.xo){
    x0 = r2.xo;
  }
  
  int y0 = r1.yo;
  if(r2.yo>r1.yo){
    y0 = r2.yo;
  }
  
  int x1 = r1.xo + r1.xs;
  if(r2.xo + r2.xs <x1){
    x1 = r2.xo + r2.xs;
  }

  int y1 = r1.yo + r1.ys;
  if(r2.yo + r2.ys <y1){
    y1 = r2.yo + r2.ys;
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
   if(!ir.GetArea())
   {
      x1 = r1;
      x2 = y1 = y2 = vil_rekt(0, 0, 0, 0);
   }
   else
   {
      x1.xo = r1.xo;
      x1.yo = r1.yo;
      x1.xs = r1.xs;
      x1.ys = ir.yo - r1.yo;

      y1.xo = r1.xo;
      y1.yo = r1.yo + x1.ys;
      y1.xs = ir.xo - r1.xo;
      y1.ys = ir.ys;

      int xsum = y1.xs + ir.xs;
      int ysum = x1.ys + ir.ys;

      x2.xo = r1.xo;
      x2.yo = r1.yo + ysum;
      x2.xs = r1.xs;
      x2.ys = r1.ys - ysum;

      y2.xo = r1.xo + xsum;
      y2.yo = y1.yo;
      y2.xs = r1.xs - xsum;
      y2.ys = ir.ys;
   }
}

