#include "vbl_psfile.h"
#include <vcl_iomanip.h>
#include <vcl_cmath.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>

#include <vbl/vbl_printf.h>

#define PIX2INCH 72.0 
#define RANGE(a,b,c) { if (a < b) a = b;  if (a > c) a = c; }
#define in_range(a) (a < (1 << 8) ? true : false)
#define Bit4ToChar(a) ((char)((a<=9) ? (a+'0'): (a - 10 + 'a')))

static bool debug = true;

// sizes of pages in inches
static double paper_size[8][2] ={ { 8.500, 11.000},   // US NORMAL
				  { 8.268, 11.693},   // A4 210mm x 297mm
				  { 7.205, 10.118},   // B5 183mm x 257mm
				  {11.693, 16.535},   // A3 297mm x 420mm
				  { 8.500, 14.000},   // US LEGAL
				  {11.000, 17.000},   // B-size
				  { 3.875,  4.875},   // 4 by 5
				  { 0.945,  1.417}};  // 35mm (24x36)

// size of l+r margin and t+b margin.  image is centered
static double margins[8 ][2] = { { 1.000, 1.000},   // US NORMAL
				{ 1.000, 1.000},   // A4
				{ 1.000, 1.000},   // B5
				{ 1.000, 1.000},   // A3
				{ 1.000, 1.000},   // US LEGAL
				{ 1.000, 1.000},   // B-size
				{ 0.275, 0.275},   // 4 by 5
				{ 0.078, 0.078}};  // 35mm (24x36)

// min and max value for PostScript paper size
static int ps_minimum = 10;
static int ps_maximum = 800;

static const vcl_streampos HEADER_START(-1);

//-----------------------------------------------------------------------------
//: Default constructor.
//-----------------------------------------------------------------------------
vbl_psfile::vbl_psfile(char const* f) 
  : output_filestream(f) 
{
  set_paper_orientation(vbl_psfile::PORTRAIT);
  set_paper_type(vbl_psfile::US_NORMAL);
  set_paper_layout(vbl_psfile::CENTER);
  header_pos = HEADER_START;
  reduction_factor = 1;
  filename = f;
  graphics_prolog_exists = false;
  scale_x = 100;
  scale_y = 100;
  doneps = false;
  ox = 0;
  oy = 0;
  min_x =  1000;
  min_y =  1000;
  max_x = -1000;
  max_y = -1000;
  exist_image = false;
  exist_objs  = false;
  fg_r = 0; fg_g = 0; fg_b = 0;
  bg_r = 1; bg_g = 1; bg_b = 1;
  line_width = 1;

  if (debug) vcl_cerr << "vbl_psfile::vbl_psfile" << vcl_endl;
}

//-----------------------------------------------------------------------------
//: Destructor 
//-----------------------------------------------------------------------------
vbl_psfile::~vbl_psfile()
{
  if (debug) vcl_cerr << "vbl_psfile::~vbl_psfile" << vcl_endl;
  reset_bounding_box();
  if (!doneps)
    done();
}


//-----------------------------------------------------------------------------
//: Rewrite output bounding box parameters.
//-----------------------------------------------------------------------------
void vbl_psfile::reset_bounding_box()
{
  vcl_streampos temp_pos;
  temp_pos = output_filestream.tellp();
  
  if (exist_image)
  {
    // for image part
    output_filestream.seekp(translate_pos);
    image_translate_and_scale();
  }
  if (exist_objs)  
  {
    // For Object part.
    output_filestream.seekp(sobj_t_pos);
    object_translate_and_scale();
  }
  
  // reset Bounding Box parameters (the fourth line).
  output_filestream.seekp(header_pos);
  reset_postscript_header();
  output_filestream.seekp(temp_pos);
}

//-----------------------------------------------------------------------------
//: Recalculate bounding box and scale x and y (if necessary).
//-----------------------------------------------------------------------------
void vbl_psfile::compute_bounding_box()
{
  box_width  = max_x - min_x;
  box_height = max_y - min_y;
  double hsx, hsy;
  double sz_inx, sz_iny;   // image size, in inches

  if (printer_paper_orientation == vbl_psfile::LANDSCAPE)
  {
    psizex = paper_size[printer_paper_type][1];
    psizey = paper_size[printer_paper_type][0];
  }
  else
  {
    psizex = paper_size[printer_paper_type][0];
    psizey = paper_size[printer_paper_type][1];
  }

  if (printer_paper_layout == vbl_psfile::CENTER)
  {
    sz_inx = (double) box_width  / PIX2INCH * (scale_x / 100.0);  
    sz_iny = (double) box_height / PIX2INCH * (scale_y / 100.0);  
  
    // round to integer .001ths of an inch
    sz_inx = floor(sz_inx * 1000.0 + 0.5) / 1000.0;
    sz_iny = floor(sz_iny * 1000.0 + 0.5) / 1000.0;
    // from xv xvps.c subroutine: centerimage
    pos_inx = psizex/2 - sz_inx/2;
    pos_iny = psizey/2 - sz_iny/2;

    // make sure 'center' of image is still on page
    hsx = sz_inx/2;  hsy = sz_iny/2;
    RANGE(pos_inx, -hsx, psizex-hsx);
    RANGE(pos_iny, -hsy, psizey-hsy);

    // round to integer .001ths of an inch
    pos_inx = floor(pos_inx * 1000.0 + 0.5) / 1000.0;
    pos_iny = floor(pos_iny * 1000.0 + 0.5) / 1000.0;
  }

  if (printer_paper_layout == vbl_psfile::MAX)
  {
    double scx, scy;
    sz_inx = psizex - margins[printer_paper_type][0];
    sz_iny = psizey - margins[printer_paper_type][1];
 
    // choose the smaller scaling factor
    scx = sz_inx / box_width;
    scy = sz_iny / box_height;
    if (scx < scy) 
    { 
      sz_iny = box_height * scx; 
      scale_y = (int) ((100 * (sz_iny * PIX2INCH) / box_width) + .5);
      scale_x = scale_y;
    } 
    else 
    { 
      sz_inx = box_width * scy; 
      scale_x = (int) ((100 * (sz_inx * PIX2INCH) / box_width) + .5);
      scale_y = scale_x;
    }
      
    RANGE(scale_x,ps_minimum,ps_maximum);
    RANGE(scale_y,ps_minimum,ps_maximum);
    sz_inx = (double) box_width / PIX2INCH * (scale_x / 100.0);  
    sz_iny = (double) box_height / PIX2INCH * (scale_y / 100.0);  
      
    // round to integer .001ths of an inch
    sz_inx = floor(sz_inx * 1000.0 + 0.5) / 1000.0;
    sz_iny = floor(sz_iny * 1000.0 + 0.5) / 1000.0;
    pos_inx = psizex/2 - sz_inx/2;
    pos_iny = psizey/2 - sz_iny/2;

    // round to integer .001ths of an inch
    pos_inx = floor(pos_inx * 1000.0 + 0.5) / 1000.0;
    pos_iny = floor(pos_iny * 1000.0 + 0.5) / 1000.0;
  } 

  // printed image will have size iw,ih (in picas)
  if (exist_image && !exist_objs)
  {
    iw = (int) (width  * scale_x/100.0 + 0.5);
    ih = (int) (height * scale_y/100.0 + 0.5);   
    iwf = width  * scale_x/100.0;
    ihf = height * scale_y/100.0;
  }
  if (exist_objs)
  {
    iw = (int) (box_width  * scale_x/100.0 + 0.5);
    ih = (int) (box_height * scale_y/100.0 + 0.5);   
  }
  
  // compute offset to bottom-left of image (in picas)
  ox = (int) (pos_inx * PIX2INCH + 0.5);
  oy = (int) (pos_iny * PIX2INCH + 0.5);

  if (debug) vcl_cerr << "vbl_psfile::compute_bounding_box, box_width = " << box_width
    << ", box_height = " << box_height << vcl_endl;
}

//-----------------------------------------------------------------------------
//: Set Bounding Box Min and Max x, y.
//-----------------------------------------------------------------------------
void vbl_psfile::set_min_max_xy(float xx, float yy)
{
  int x = (int) (xx + 0.5);
  int y = (int) (yy + 0.5);
  if (x < min_x)   min_x = x;
  if (y < min_y)   min_y = y;
  if (x > max_x)   max_x = x;
  if (y > max_y)   max_y = y;
}

//-----------------------------------------------------------------------------
//: Set Bounding Box Min and Max x, y.
//-----------------------------------------------------------------------------
void vbl_psfile::set_min_max_xy(int x, int y)
{
  if (x < min_x)   min_x = x;
  if (y < min_y)   min_y = y;
  if (x > max_x)   max_x = x;
  if (y > max_y)   max_y = y;
}

//-----------------------------------------------------------------------------
//: Write 8 bit grey scale image.
//-----------------------------------------------------------------------------
void vbl_psfile::print_greyscale_image(byte* buffer, int sizex, int sizey)
{
  if (debug) vcl_cerr << "vbl_psfile::print_greyscale_image, width = " << sizex <<
    ", height = " << sizey  << ", reduction_factor = " << reduction_factor << vcl_endl;

  exist_image = true;
  width = sizex;
  height = sizey;
  compute_bounding_box();

  int new_width = (int)(width/reduction_factor);
  int new_height= (int)(height/reduction_factor);

  output_filestream << "\n%%Page: 1 1\n\n"
	  << "% remember original state\n"
	  << "/origstate save def\n"
	  << "\n% build a temporary dictionary\n" 
	  << "20 dict begin\n\n"
	  << "% define string to hold a scanline's worth of data\n"
	  << "/pix " << new_width << " string def\n";
  if (printer_paper_orientation == vbl_psfile::LANDSCAPE)
  {
    output_filestream << "% print in landscapey mode\n";
    output_filestream << "90 rotate 0 " << (int) (-psizey*PIX2INCH) << " translate\n\n";
  }
  output_filestream << "% lower left corner\n";
  translate_pos = output_filestream.tellp();

  image_translate_and_scale();
  
  output_filestream << new_width << " " 
	  << new_height << " " 
	  << "8                      % dimensions of data\n"
	  << "[" << new_width << " 0 0 -"
	  << new_height << " 0 " 
	  << new_height << "]           % mapping matrix\n"
	  << "{currentfile pix readhexstring pop}\n"
	  << "image\n\n";
  int linesize = 72;
  int index;
  int height_left;
  int width_left;
  int number_of_pixels_sampled;
  // write image data to output PostScript file 
  for (int j=0; j<new_height; j++)
  {
    int countrow = 0;
    for (int i = 0; i < new_width; i++)
    {
      // Reduce resolution of image if necessary
      if(reduction_factor != 1)
      {
        int pixel_number= (width * j + i) * reduction_factor;
        index=0;
        width_left = new_width - i;
        number_of_pixels_sampled=0;
        if( width_left >= 1)
        {  
          for (int m=0; m < reduction_factor;m++)
          {
            height_left = new_height - j;
            if( height_left >= 1)
            {
              for (int n=0; n < reduction_factor;n++)
              {
	        index += (int) (*(buffer + (pixel_number+m+n*width)));
	        number_of_pixels_sampled += 1;
              }
            }
	    else 
	    {
	      for (int n=0; n <= height % reduction_factor;n++)
              {
	        index += (int) (*(buffer + (pixel_number+m+n*width)));
	        number_of_pixels_sampled += 1;
              }
            }
          }
        }
        else
        {
          for (int m=0; m <= width % reduction_factor ;m++)
          {
            height_left = new_height - j;
            if( height_left >= 1)
            {
              for (int n=0; n < reduction_factor;n++)
              {
	        index += (int) (*(buffer + (pixel_number+m+n*width)));
	        number_of_pixels_sampled += 1;
	      }
            }
	    else 
	    {
	      for (int n=0; n <= height % reduction_factor;n++)
              {
	        index += (int) (*(buffer + (pixel_number+m+n*width)));
	        number_of_pixels_sampled += 1;
	      }
	    }
	  }
        }  
        index=index/number_of_pixels_sampled; // Average the pixel intensity value.
      }
      else
        index=(int)(*(buffer + width * j + i));
      countrow+=2;
      if (in_range(index))
      {
        char pixel[3];
        unsigned char low4 = (unsigned char)  (index & 0x000f);
        unsigned char high4 = (unsigned char) ((index & 0x00f0) >> 4);
        pixel[0] = Bit4ToChar(high4);
        pixel[1] = Bit4ToChar(low4);
        pixel[2] = '\0';
        output_filestream << pixel;
      }
      else
        vcl_cout << " index out of range: " << index << vcl_endl;

      if (countrow >= linesize)
      {
        countrow = 0;
        output_filestream << '\n';
      }
    }
    output_filestream << '\n';
  }
  output_filestream << "% stop using temporary dictionary\n"
    << "end\n\n"
    << "% restore original state\n"
    << "origstate restore\n\n";
}

//-----------------------------------------------------------------------------
//: Write 24 bit colour image.
//-----------------------------------------------------------------------------
void vbl_psfile::print_color_image(byte* data, int width_arg, int height_arg)
{
  int bytes_per_pixel = 3;
  int new_height;
  int new_width;
  exist_image = true;
  if (debug) vcl_cerr << "Reduction factor is " << reduction_factor << "\n";
  new_width = (int)(width_arg/reduction_factor);
  new_height= (int)(height_arg/reduction_factor);

  // This part uses xv outfile as a reference:
  output_filestream << "\n%%Page: 1 1\n\n"
    << "% remember original state\n"
    << "/origstate save def\n"
    << "\n% build a temporary dictionary\n"
    << "20 dict begin\n\n"
    << "% define string to hold a scanline's worth of data\n"
    << "/pix " << 3 * width_arg << " string def\n"
    << "\n% define space for color conversions\n"
    << "/grays " << width_arg
    << " string def  % space for gray scale line\n"
    << "/npixls 0 def\n"
    << "/rgbindx 0 def\n\n";
  if (printer_paper_orientation == vbl_psfile::LANDSCAPE)
  {
    output_filestream << "% print in landscape mode\n";
    output_filestream << "90 rotate 0 " << (int) (-psizey*PIX2INCH) << " translate\n\n";
  }
  output_filestream << "% lower left corner\n";
  translate_pos = output_filestream.tellp();
  image_translate_and_scale();
 
  output_filestream << '\n'
    << "% define 'colorimage' if it isn't defined\n"
    << "%   ('colortogray' and 'mergeprocs' come from xwd2ps\n"
    << "%     via xgrab)\n"
    << "/colorimage where   % do we know about 'colorimage'?\n"
    << "  { pop }           % yes: pop off the 'dict' returned\n"
    << "  {                 % no:  define one\n"
    << "    /colortogray {  % define an RGB->I function\n"
    << "      /rgbdata exch store    % call input 'rgbdata'\n"
    << "      rgbdata length 3 idiv\n"
    << "      /npixls exch store\n"
    << "      /rgbindx 0 store\n"
    << "      0 1 npixls 1 sub {\n"
    << "        grays exch\n"
    << "        rgbdata rgbindx       get 20 mul    % Red\n"
    << "        rgbdata rgbindx 1 add get 32 mul    % Green\n"
    << "        rgbdata rgbindx 2 add get 12 mul    % Blue\n"
    << "        add add 64 idiv      % I = .5G + .31R + .18B\n"
    << "        put\n"
    << "        /rgbindx rgbindx 3 add store\n"
    << "      } for\n"
    << "      grays 0 npixls getinterval\n"
    << "    } bind def\n\n"
    << "    % Utility procedure for colorimage operator.\n"
    << "    % This procedure takes two procedures off the\n"
    << "    % stack and merges them into a single procedure.\n\n"
    << "    /mergeprocs { % def\n"
    << "      dup length\n"
    << "      3 -1 roll\n"
    << "      dup\n"
    << "      length\n"
    << "      dup\n"
    << "      5 1 roll\n"
    << "      3 -1 roll\n"
    << "      add\n"
    << "      array cvx\n"
    << "      dup\n"
    << "      3 -1 roll\n"
    << "      0 exch\n"
    << "      putinterval\n"
    << "      dup\n"
    << "      4 2 roll\n"
    << "      putinterval\n"
    << "    } bind def\n\n"
    << "    /colorimage { % def\n"
    << "      pop pop     % remove 'false 3' operands\n"
    << "      {colortogray} mergeprocs\n"
    << "      image\n"
    << "    } bind def\n"
    << "  } ifelse          % end of 'false' case\n\n\n"
    << "\n"
    << width_arg << " "
    << height_arg << " 8                      % dimensions of data\n"
    << "[" << width_arg << " 0 0 -"
    << height_arg << " 0 "
    << height_arg << "]            % mapping matrix\n"
    << "{currentfile pix readhexstring pop}\n"
    << "false 3 colorimage\n\n";

  // write image data into PostScript file.
  int index;
  int height_left;
  int width_left;
  int number_of_pixels_sampled;

  // extract RGB data from pixel value and write it to output file
  int linesize = 72;
  for (int j = 0; j < new_height;j++)
  {
    int countrow = 0;
    for (int i = 0; i < new_width; i++)
    {
      // get RGB hex index.
      index = (int) (*(data + width_arg * bytes_per_pixel * j +i));
      countrow+=6 *bytes_per_pixel;
      // Reduce Image if necessary
      if(reduction_factor != 1)
      {
        int pixel_number= (width_arg * bytes_per_pixel * j + i) * reduction_factor;
        index=0;
        width_left = new_width - i;
        number_of_pixels_sampled=0;
        if(width_left >= 1)
        {
          for (int m=0; m < reduction_factor;m++)
          {
            height_left = new_height - j;
            if( height_left >= 1)
            {
              for (int n=0; n < reduction_factor;n++)
              {
                index += (int) (*(data + (pixel_number+m+n * bytes_per_pixel * width_arg)));
                number_of_pixels_sampled += 1;
              }
            }
            else // height_left < 1
            {
              for (int n=0; n <= height_arg % reduction_factor;n++)
              {
               index += (int) (*(data + (pixel_number+m+n * bytes_per_pixel*width_arg)));
               number_of_pixels_sampled += 1;
              }
            }
          }
        }
        else // width_left < 1
        {
          for (int m=0; m <= width_arg % reduction_factor ;m++)
          {
            height_left = new_height - j;
            if( height_left >= 1)
            {
              for (int n=0; n < reduction_factor;n++)
              {
                index += (int) (*(data + (pixel_number+m+n * bytes_per_pixel*width_arg)));
                number_of_pixels_sampled += 1;
              }
            }
            else
            {
              for (int n=0; n <= height_arg % reduction_factor;n++)
              {
                index += (int) (*(data + (pixel_number+m+n * bytes_per_pixel*width_arg)));
                number_of_pixels_sampled += 1;
              }
            }
          }
        }
        index=index/number_of_pixels_sampled;  // average the pixel intensity
      }
      else // reduction_factor == 1
      {
        index=(int)(*(data + width_arg * bytes_per_pixel * j + i));
      }
      countrow+=6*bytes_per_pixel;
 
       // write RGC hex.
       //output_filestream << (const char*)color_lookup[index];  //FIXME

   /*     char pixel[3];
        unsigned char low4 = (unsigned char)  (index & 0x000f);
        unsigned char high4 = (unsigned char) ((index & 0x00f0) >> 4);
        pixel[0] = Bit4ToChar(high4);
        pixel[1] = Bit4ToChar(low4);
        pixel[2] = '\0';
        output_filestream << pixel; */

       if (countrow >= linesize)
       {
         countrow = 0;
         output_filestream << '\n';
       }
     }
     output_filestream << "\n";
  }
 
  output_filestream << "% stop using temporary dictionary\n"
    << "end\n\n"
    << "% restore original state\n"
    << "origstate restore\n\n";
}

//-----------------------------------------------------------------------------
//: Set graphic coordiate (translate and rotate to local coordinate).
//-----------------------------------------------------------------------------
void vbl_psfile::graphic_header()
{
  if (printer_paper_orientation == vbl_psfile::LANDSCAPE)
  {
    output_filestream << "% print in landscape mode\n";
    output_filestream << "90 rotate 0 " << (int) (-psizey*PIX2INCH) << " translate\n\n";
  }

  output_filestream.flush();
  // save streampos so we can come back and modify it.
  sobj_t_pos = output_filestream.tellp();
  // move relative coordinate to local origin and set up scale factor.
  object_translate_and_scale();
}

//-----------------------------------------------------------------------------
//: Set Image translate and scale.
//-----------------------------------------------------------------------------
void vbl_psfile::image_translate_and_scale()
{
  int scale_height = (int) ((float) height* scale_y/100.0);
  int scale_min_x  = (int) ((float) min_x * scale_x/100.0);
  int scale_max_y  = (int) ((float) max_y * scale_y/100.0);

  if (debug) vcl_cerr << "vbl_psfile::image_translate_and_scale, scale_height= " << 
    scale_height << ", scale_min_x = " << scale_min_x << ", scale_max_y = " <<
    scale_max_y << vcl_endl;

  vbl_printf(output_filestream, "%d %d translate\n", ox - scale_min_x, oy + scale_max_y - scale_height);
  vbl_printf(output_filestream, "\n%% size of image (on paper, in 1/72inch coords)\n");
  vbl_printf(output_filestream, "%g %g scale\n\n", iwf, ihf);
}

//-----------------------------------------------------------------------------
//: Set object translate and scale.
//-----------------------------------------------------------------------------
void vbl_psfile::object_translate_and_scale()
{
  int scale_height = (int) ((float) box_height * scale_y/100.0);
  int scale_min_x  = (int) ((float) min_x * scale_x/100.0);
  int scale_min_y  = (int) ((float) min_y * scale_y/100.0);

  // move origin
  vbl_printf(output_filestream, "%d %d translate\n", ox - scale_min_x, oy + scale_height + scale_min_y);
  vbl_printf(output_filestream, "%g %g scale\n\n", scale_x/100.0, -scale_y/100.0);
  vbl_printf(output_filestream, "/originalCTM matrix currentmatrix def\n");
}

//-----------------------------------------------------------------------------
//: Set ox, oy , iw, ih, iwf, ihf parameters for PostScript file use.
//-----------------------------------------------------------------------------
bool vbl_psfile::set_parameters(int sizex,int sizey)
{
  width = sizex;
  height = sizey;

  set_min_max_xy(0,0);
  set_min_max_xy(width,height);
  compute_bounding_box();
  
  double hsx, hsy;
  double sz_inx, sz_iny;   // image size, in inches

  if (printer_paper_orientation == vbl_psfile::LANDSCAPE)
  {
    psizex = paper_size[printer_paper_type][1];
    psizey = paper_size[printer_paper_type][0];
  }
  else
  {
    psizex = paper_size[printer_paper_type][0];
    psizey = paper_size[printer_paper_type][1];
  }

  if (printer_paper_layout == vbl_psfile::CENTER)
  {
    sz_inx = (double) width  / PIX2INCH * (scale_x / 100.0);  
    sz_iny = (double) height / PIX2INCH * (scale_y / 100.0);  
  
    // round to integer .001ths of an inch
    sz_inx = floor(sz_inx * 1000.0 + 0.5) / 1000.0;
    sz_iny = floor(sz_iny * 1000.0 + 0.5) / 1000.0;
    // from xv xvps.c subroutine: centerimage
    pos_inx = psizex/2 - sz_inx/2;
    pos_iny = psizey/2 - sz_iny/2;

    // make sure 'center' of image is still on page
    hsx = sz_inx/2;  hsy = sz_iny/2;
    RANGE(pos_inx, -hsx, psizex-hsx);
    RANGE(pos_iny, -hsy, psizey-hsy);

    // round to integer .001ths of an inch
    pos_inx = floor(pos_inx * 1000.0 + 0.5) / 1000.0;
    pos_iny = floor(pos_iny * 1000.0 + 0.5) / 1000.0;
  }

  else if (printer_paper_layout == vbl_psfile::MAX)
  {
    double scx, scy;
    sz_inx = psizex - margins[printer_paper_type][0];
    sz_iny = psizey - margins[printer_paper_type][1];
 
    // choose the smaller scaling factor
    scx = sz_inx / width;
    scy = sz_iny / height;
    if (scx < scy) { sz_iny = height * scx; } else { sz_inx = width * scy; }
    scale_x = (int) ((100 * (sz_inx * PIX2INCH) / width) + .5);
    scale_y = (int) ((100 * (sz_inx * PIX2INCH) / width) + .5);
    RANGE(scale_x,ps_minimum,ps_maximum);
    RANGE(scale_y,ps_minimum,ps_maximum);
    sz_inx = (double) width / PIX2INCH * (scale_x / 100.0);  
    sz_iny = (double) height / PIX2INCH * (scale_y / 100.0);  
    
    // round to integer .001ths of an inch
    sz_inx = floor(sz_inx * 1000.0 + 0.5) / 1000.0;
    sz_iny = floor(sz_iny * 1000.0 + 0.5) / 1000.0;
    pos_inx = psizex/2 - sz_inx/2;
    pos_iny = psizey/2 - sz_iny/2;

    // round to integer .001ths of an inch
    pos_inx = floor(pos_inx * 1000.0 + 0.5) / 1000.0;
    pos_iny = floor(pos_iny * 1000.0 + 0.5) / 1000.0;
  }
  else 
  {
    sz_iny =1;
    sz_inx = 1;
  }

  // printed image will have size iw,ih (in picas)
  iw = (int) (sz_inx * PIX2INCH + 0.5);
  ih = (int) (sz_iny * PIX2INCH + 0.5);   
  iwf = sz_inx * PIX2INCH;
  ihf = sz_iny * PIX2INCH;

  // compute offset to bottom-left of image (in picas)
  ox = (int) (pos_inx * PIX2INCH + 0.5);
  oy = (int) (pos_iny * PIX2INCH + 0.5);

  return true;
}

//-----------------------------------------------------------------------------
//: PostScript file header.
//-----------------------------------------------------------------------------
void vbl_psfile::postscript_header()
{
  if (header_pos != HEADER_START) 
  {
    vcl_cerr << "vbl_psfile: Header already set to " << header_pos << vcl_endl;
    return;
  }

  output_filestream << "%!PS-Adobe-2.0 EPSF-2.0\n"
	  << "%%Title: " << filename.c_str() << '\n'
	  << "%%Creator: GE Targetjr - by Alan S. Liu\n"
	  << "%%compute_bounding_box: " ;

  header_pos = output_filestream.tellp();
  reset_postscript_header();
}


//-----------------------------------------------------------------------------
//: Reset PostScript header file
//-----------------------------------------------------------------------------
void vbl_psfile::reset_postscript_header()
{
  if (printer_paper_orientation == vbl_psfile::LANDSCAPE)
  {
    vbl_printf(output_filestream, "%d %d %d %d\n",
        (int) (pos_iny * PIX2INCH + 0.5),
        (int) (pos_inx * PIX2INCH + 0.5),
        (int) (pos_iny * PIX2INCH + 0.5) + ih,
        (int) (pos_inx * PIX2INCH + 0.5) + iw);
  }
  else
  {
    vbl_printf(output_filestream, "%d %d %d %d\n", ox,oy,ox+iw,oy+ih);
  }
  output_filestream << "%%Pages: 1\n"
    << "%%DocumentFonts:\n"
    << "%%EndComments\n";
}

//-----------------------------------------------------------------------------
// sobj_rgb_params() -- 
//-----------------------------------------------------------------------------
void vbl_psfile::sobj_rgb_params(char const* obj_str, bool filled)
{
  print_graphics_prolog();
  output_filestream << "\nBegin %I " << obj_str << "\n";
  output_filestream << "2 0 0 [] 0 SetB\n";
  output_filestream << fg_r << " " << fg_g << " " << fg_b << " SetCFg\n";
  output_filestream << bg_r << " " << bg_g << " " << bg_b << " SetCBg\n";
  output_filestream << line_width << " setlinewidth\n";
  output_filestream << (filled ? "0": "none") << " SetP %I p n\n";
}

//-----------------------------------------------------------------------------
//:  Add a line between the given points to the Postscript file.
//-----------------------------------------------------------------------------
void vbl_psfile::line(float x1, float y1, float x2, float y2)
{
  // set up bounding box.
  set_min_max_xy(x1,y1);
  set_min_max_xy(x2,y2);
  compute_bounding_box();
  
  print_graphics_prolog();
  sobj_rgb_params("Line", false);
  
  output_filestream << (int) x1 << " " << (int) y1 << " "
	             << (int) x2 << " " << (int) y2 << " Line\n";
  output_filestream << "End\n";
}

//-----------------------------------------------------------------------------
//: Add a point at the given coordinates to the Postscript file.
//-----------------------------------------------------------------------------
void vbl_psfile::point(float x, float y, float point_size)
{
  print_graphics_prolog();
  set_min_max_xy(x,y);
  compute_bounding_box();
  
  this->sobj_rgb_params("Point", true);

  point_size /= 2;
  output_filestream << x << " " << y << " " << point_size << " " << point_size << " Elli\nEnd\n";
}

//-----------------------------------------------------------------------------
//: Add an ellipse to the Postscript file.
//-----------------------------------------------------------------------------
void vbl_psfile::ellipse(float x, float y, float a_axis, float b_axis, int angle)
{
  #ifndef PI // should already be defined in math.h - PVR
  #define PI 3.141592654
  #endif

  set_min_max_xy((int) (x+a_axis*cos(((float)angle)*PI/180.0) + 0.5),
	        (int) (y+a_axis*sin(((float)angle)*PI/180.0) + 0.5) );
  set_min_max_xy((int) (x-a_axis*cos(((float)angle)*PI/180.0) + 0.5),
	        (int) (y-a_axis*sin(((float)angle)*PI/180.0) + 0.5) );
  compute_bounding_box();

  print_graphics_prolog();
  sobj_rgb_params("Ellipse", false);
  if (angle)
  {
    output_filestream << (int) x << " " << (int) y << " translate \n";
    output_filestream << -angle << " rotate\n";
    output_filestream << "0 0 " << (int) a_axis << " " 
      << (int) b_axis << " Elli\nEnd\n";
  }
  else
  {
    output_filestream << (int) x << " " 
      << (int) y << " "
      << (int) a_axis << " "
      << (int) b_axis << " Elli\nEnd\n";
  }
}

//-----------------------------------------------------------------------------
//: Add a circle with the given centre point and radius to the Postscript file.
//-----------------------------------------------------------------------------
void vbl_psfile::circle(float x, float y, float radius)
{
  // set up bounding box
  set_min_max_xy(x+radius,y);
  set_min_max_xy(x-radius,y);
  set_min_max_xy(x,y+radius);
  set_min_max_xy(x,y-radius);
  compute_bounding_box();

  print_graphics_prolog();
  sobj_rgb_params("Circle", false);
  ellipse(x,y,radius,radius);
  output_filestream << "End\n";
}

//-----------------------------------------------------------------------------
//: the defined procedured for PostScript script use. 
//-----------------------------------------------------------------------------
void vbl_psfile::print_graphics_prolog()
{
  if (graphics_prolog_exists)
    return;
  exist_objs = true;
  output_filestream << "\n%%BeginTargetjrPrologue\n";
  output_filestream << "/arrowhead {\n";
  output_filestream << "0 begin\n";
  output_filestream << "transform originalCTM itransform\n";
  output_filestream << "/taily exch def\n";
  output_filestream << "/tailx exch def\n";
  output_filestream << "transform originalCTM itransform\n";
  output_filestream << "/tipy exch def\n";
  output_filestream << "/tipx exch def\n";
  output_filestream << "/dy tipy taily sub def\n";
  output_filestream << "/dx tipx tailx sub def\n";
  output_filestream << "/angle dx 0 ne dy 0 ne or { dy dx atan } { 90 } ifelse def\n";
  output_filestream << "gsave\n";
  output_filestream << "originalCTM setmatrix\n";
  output_filestream << "tipx tipy translate\n";
  output_filestream << "angle rotate\n";
  output_filestream << "newpath\n";
  output_filestream << "arrowHeight neg arrowWidth 2 div moveto\n";
  output_filestream << "0 0 lineto\n";
  output_filestream << "arrowHeight neg arrowWidth 2 div neg lineto\n";
  output_filestream << "patternNone not {\n";
  output_filestream << "originalCTM setmatrix\n";
  output_filestream << "/padtip arrowHeight 2 exp 0.25 arrowWidth 2 exp mul add sqrt brushWidth mul\n";
  output_filestream << "arrowWidth div def\n";
  output_filestream << "/padtail brushWidth 2 div def\n";
  output_filestream << "tipx tipy translate\n";
  output_filestream << "angle rotate\n";
  output_filestream << "padtip 0 translate\n";
  output_filestream << "arrowHeight padtip add padtail add arrowHeight div dup scale\n";
  output_filestream << "arrowheadpath\n";
  output_filestream << "ifill\n";
  output_filestream << "} if\n";
  output_filestream << "brushNone not {\n";
  output_filestream << "originalCTM setmatrix\n";
  output_filestream << "tipx tipy translate\n";
  output_filestream << "angle rotate\n";
  output_filestream << "arrowheadpath\n";
  output_filestream << "istroke\n";
  output_filestream << "} if\n";
  output_filestream << "grestore\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 9 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/arrowheadpath {\n";
  output_filestream << "newpath\n";
  output_filestream << "arrowHeight neg arrowWidth 2 div moveto\n";
  output_filestream << "0 0 lineto\n";
  output_filestream << "arrowHeight neg arrowWidth 2 div neg lineto\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/leftarrow {\n";
  output_filestream << "0 begin\n";
  output_filestream << "y exch get /taily exch def\n";
  output_filestream << "x exch get /tailx exch def\n";
  output_filestream << "y exch get /tipy exch def\n";
  output_filestream << "x exch get /tipx exch def\n";
  output_filestream << "brushLeftArrow { tipx tipy tailx taily arrowhead } if\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 4 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/rightarrow {\n";
  output_filestream << "0 begin\n";
  output_filestream << "y exch get /tipy exch def\n";
  output_filestream << "x exch get /tipx exch def\n";
  output_filestream << "y exch get /taily exch def\n";
  output_filestream << "x exch get /tailx exch def\n";
  output_filestream << "brushRightArrow { tipx tipy tailx taily arrowhead } if\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 4 dict put def\n";
  output_filestream << "\n";
  output_filestream << "%%EndTargetjrPrologue\n";
  output_filestream << "\n";
  output_filestream << "/arrowHeight 10 def\n";
  output_filestream << "/arrowWidth 5 def\n";
  output_filestream << "\n";
  output_filestream << "/TargetjrDict 50 dict def\n";
  output_filestream << "TargetjrDict begin\n";
  output_filestream << "\n";
  output_filestream << "/none null def\n";
  output_filestream << "/numGraphicParameters 17 def\n";
  output_filestream << "/stringLimit 65535 def\n";
  output_filestream << "\n";
  output_filestream << "/Begin {\n";
  output_filestream << "save\n";
  output_filestream << "numGraphicParameters dict begin\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/End {\n";
  output_filestream << "end\n";
  output_filestream << "restore\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/SetB { % width leftarrow rightarrow DashArray DashOffset SetB\n";
  output_filestream << "dup type /nulltype eq {\n";
  output_filestream << "pop\n";
  output_filestream << "false /brushRightArrow idef\n";
  output_filestream << "false /brushLeftArrow idef\n";
  output_filestream << "true /brushNone idef\n";
  output_filestream << "} {\n";
  output_filestream << "/brushDashOffset idef\n";
  output_filestream << "/brushDashArray idef\n";
  output_filestream << "0 ne /brushRightArrow idef\n";
  output_filestream << "0 ne /brushLeftArrow idef\n";
  output_filestream << "/brushWidth idef\n";
  output_filestream << "false /brushNone idef\n";
  output_filestream << "} ifelse\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/SetCFg {\n";
  output_filestream << "/fgblue idef\n";
  output_filestream << "/fggreen idef\n";
  output_filestream << "/fgred idef\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/SetCBg {\n";
  output_filestream << "/bgblue idef\n";
  output_filestream << "/bggreen idef\n";
  output_filestream << "/bgred idef\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/SetF {\n";
  output_filestream << "/printSize idef\n";
  output_filestream << "/printFont idef\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/SetP {  % string -1 SetP  OR gray SetP\n";
  output_filestream << "dup type /nulltype eq {\n";
  output_filestream << "pop true /patternNone idef\n";
  output_filestream << "} {\n";
  output_filestream << "dup -1 eq {\n";
  output_filestream << "/patternGrayLevel idef\n";
  output_filestream << "/patternString idef\n";
  output_filestream << "} {\n";
  output_filestream << "/patternGrayLevel idef\n";
  output_filestream << "} ifelse\n";
  output_filestream << "false /patternNone idef\n";
  output_filestream << "} ifelse\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/BSpl {\n";
  output_filestream << "0 begin\n";
  output_filestream << "storexyn\n";
  output_filestream << "newpath\n";
  output_filestream << "n 1 gt {\n";
  output_filestream << "0 0 0 0 0 0 1 1 true subspline\n";
  output_filestream << "n 2 gt {\n";
  output_filestream << "0 0 0 0 1 1 2 2 false subspline\n";
  output_filestream << "1 1 n 3 sub {\n";
  output_filestream << "/i exch def\n";
  output_filestream << "i 1 sub dup i dup i 1 add dup i 2 add dup false subspline\n";
  output_filestream << "} for\n";
  output_filestream << "n 3 sub dup n 2 sub dup n 1 sub dup 2 copy false subspline\n";
  output_filestream << "} if\n";
  output_filestream << "n 2 sub dup n 1 sub dup 2 copy 2 copy false subspline\n";
  output_filestream << "patternNone not brushLeftArrow not brushRightArrow not and and { ifill } if\n";
  output_filestream << "brushNone not { istroke } if\n";
  output_filestream << "0 0 1 1 leftarrow\n";
  output_filestream << "n 2 sub dup n 1 sub dup rightarrow\n";
  output_filestream << "} if\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 4 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/Circ {\n";
  output_filestream << "newpath\n";
  output_filestream << "0 360 arc\n";
  output_filestream << "patternNone not { ifill } if\n";
  output_filestream << "brushNone not { istroke } if\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/CBSpl {\n";
  output_filestream << "0 begin\n";
  output_filestream << "dup 2 gt {\n";
  output_filestream << "storexyn\n";
  output_filestream << "newpath\n";
  output_filestream << "n 1 sub dup 0 0 1 1 2 2 true subspline\n";
  output_filestream << "1 1 n 3 sub {\n";
  output_filestream << "/i exch def\n";
  output_filestream << "i 1 sub dup i dup i 1 add dup i 2 add dup false subspline\n";
  output_filestream << "} for\n";
  output_filestream << "n 3 sub dup n 2 sub dup n 1 sub dup 0 0 false subspline\n";
  output_filestream << "n 2 sub dup n 1 sub dup 0 0 1 1 false subspline\n";
  output_filestream << "patternNone not { ifill } if\n";
  output_filestream << "brushNone not { istroke } if\n";
  output_filestream << "} {\n";
  output_filestream << "Poly\n";
  output_filestream << "} ifelse\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 4 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/Elli {\n";
  output_filestream << "0 begin\n";
  output_filestream << "newpath\n";
  output_filestream << "4 2 roll\n";
  output_filestream << "translate\n";
  output_filestream << "scale\n";
  output_filestream << "0 0 1 0 360 arc\n";
  output_filestream << "patternNone not { ifill } if\n";
  output_filestream << "brushNone not { istroke } if\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 1 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/Line {\n";
  output_filestream << "0 begin\n";
  output_filestream << "2 storexyn\n";
  output_filestream << "newpath\n";
  output_filestream << "x 0 get y 0 get moveto\n";
  output_filestream << "x 1 get y 1 get lineto\n";
  output_filestream << "brushNone not { istroke } if\n";
  output_filestream << "0 0 1 1 leftarrow\n";
  output_filestream << "0 0 1 1 rightarrow\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 4 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/MLine {\n";
  output_filestream << "0 begin\n";
  output_filestream << "storexyn\n";
  output_filestream << "newpath\n";
  output_filestream << "n 1 gt {\n";
  output_filestream << "x 0 get y 0 get moveto\n";
  output_filestream << "1 1 n 1 sub {\n";
  output_filestream << "/i exch def\n";
  output_filestream << "x i get y i get lineto\n";
  output_filestream << "} for\n";
  output_filestream << "patternNone not brushLeftArrow not brushRightArrow not and and { ifill } if\n";
  output_filestream << "brushNone not { istroke } if\n";
  output_filestream << "0 0 1 1 leftarrow\n";
  output_filestream << "n 2 sub dup n 1 sub dup rightarrow\n";
  output_filestream << "} if\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 4 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/Poly {\n";
  output_filestream << "3 1 roll\n";
  output_filestream << "newpath\n";
  output_filestream << "moveto\n";
  output_filestream << "-1 add\n";
  output_filestream << "{ lineto } repeat\n";
  output_filestream << "closepath\n";
  output_filestream << "patternNone not { ifill } if\n";
  output_filestream << "brushNone not { istroke } if\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/Rect {\n";
  output_filestream << "0 begin\n";
  output_filestream << "/t exch def\n";
  output_filestream << "/r exch def\n";
  output_filestream << "/b exch def\n";
  output_filestream << "/l exch def\n";
  output_filestream << "newpath\n";
  output_filestream << "l b moveto\n";
  output_filestream << "l t lineto\n";
  output_filestream << "r t lineto\n";
  output_filestream << "r b lineto\n";
  output_filestream << "closepath\n";
  output_filestream << "patternNone not { ifill } if\n";
  output_filestream << "brushNone not { istroke } if\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 4 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/Text {\n";
  output_filestream << "ishow\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/idef {\n";
  output_filestream << "dup where { pop pop pop } { exch def } ifelse\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/ifill {\n";
  output_filestream << "0 begin\n";
  output_filestream << "gsave\n";
  output_filestream << "patternGrayLevel -1 ne {\n";
  output_filestream << "fgred bgred fgred sub patternGrayLevel mul add\n";
  output_filestream << "fggreen bggreen fggreen sub patternGrayLevel mul add\n";
  output_filestream << "fgblue bgblue fgblue sub patternGrayLevel mul add setrgbcolor\n";
  output_filestream << "eofill\n";
  output_filestream << "} {\n";
  output_filestream << "eoclip\n";
  output_filestream << "originalCTM setmatrix\n";
  output_filestream << "pathbbox /t exch def /r exch def /b exch def /l exch def\n";
  output_filestream << "/w r l sub ceiling cvi def\n";
  output_filestream << "/h t b sub ceiling cvi def\n";
  output_filestream << "/imageByteWidth w 8 div ceiling cvi def\n";
  output_filestream << "/imageHeight h def\n";
  output_filestream << "bgred bggreen bgblue setrgbcolor\n";
  output_filestream << "eofill\n";
  output_filestream << "fgred fggreen fgblue setrgbcolor\n";
  output_filestream << "w 0 gt h 0 gt and {\n";
  output_filestream << "l w add b translate w neg h scale\n";
  output_filestream << "w h true [w 0 0 h neg 0 h] { patternproc } imagemask\n";
  output_filestream << "} if\n";
  output_filestream << "} ifelse\n";
  output_filestream << "grestore\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 8 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/istroke {\n";
  output_filestream << "gsave\n";
  output_filestream << "brushDashOffset -1 eq {\n";
  output_filestream << "[] 0 setdash\n";
  output_filestream << "1 setgray\n";
  output_filestream << "} {\n";
  output_filestream << "brushDashArray brushDashOffset setdash\n";
  output_filestream << "fgred fggreen fgblue setrgbcolor\n";
  output_filestream << "} ifelse\n";
//  output_filestream << "brushWidth setlinewidth\n";
  output_filestream << "originalCTM setmatrix\n";
  output_filestream << "stroke\n";
  output_filestream << "grestore\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/ishow {\n";
  output_filestream << "0 begin\n";
  output_filestream << "gsave\n";
  output_filestream << "fgred fggreen fgblue setrgbcolor\n";
  output_filestream << "/fontDict printFont printSize scalefont dup setfont def\n";
  output_filestream << "/descender fontDict begin 0 [FontBBox] 1 get FontMatrix end\n";
  output_filestream << "transform exch pop def\n";
  output_filestream << "/vertoffset 1 printSize sub descender sub def {\n";
  output_filestream << "0 vertoffset moveto show\n";
  output_filestream << "/vertoffset vertoffset printSize sub def\n";
  output_filestream << "} forall\n";
  output_filestream << "grestore\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 3 dict put def\n";
  output_filestream << "/patternproc {\n";
  output_filestream << "0 begin\n";
  output_filestream << "/patternByteLength patternString length def\n";
  output_filestream << "/patternHeight patternByteLength 8 mul sqrt cvi def\n";
  output_filestream << "/patternWidth patternHeight def\n";
  output_filestream << "/patternByteWidth patternWidth 8 idiv def\n";
  output_filestream << "/imageByteMaxLength imageByteWidth imageHeight mul\n";
  output_filestream << "stringLimit patternByteWidth sub min def\n";
  output_filestream << "/imageMaxHeight imageByteMaxLength imageByteWidth idiv patternHeight idiv\n";
  output_filestream << "patternHeight mul patternHeight max def\n";
  output_filestream << "/imageHeight imageHeight imageMaxHeight sub store\n";
  output_filestream << "/imageString imageByteWidth imageMaxHeight mul patternByteWidth add string def\n";
  output_filestream << "0 1 imageMaxHeight 1 sub {\n";
  output_filestream << "/y exch def\n";
  output_filestream << "/patternRow y patternByteWidth mul patternByteLength mod def\n";
  output_filestream << "/patternRowString patternString patternRow patternByteWidth getinterval def\n";
  output_filestream << "/imageRow y imageByteWidth mul def\n";
  output_filestream << "0 patternByteWidth imageByteWidth 1 sub {\n";
  output_filestream << "/x exch def\n";
  output_filestream << "imageString imageRow x add patternRowString putinterval\n";
  output_filestream << "} for\n";
  output_filestream << "} for\n";
  output_filestream << "imageString\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 12 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/min {\n";
  output_filestream << "dup 3 2 roll dup 4 3 roll lt { exch } if pop\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/max {\n";
  output_filestream << "dup 3 2 roll dup 4 3 roll gt { exch } if pop\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/midpoint {\n";
  output_filestream << "0 begin\n";
  output_filestream << "/y1 exch def\n";
  output_filestream << "/x1 exch def\n";
  output_filestream << "/y0 exch def\n";
  output_filestream << "/x0 exch def\n";
  output_filestream << "x0 x1 add 2 div\n";
  output_filestream << "y0 y1 add 2 div\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 4 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/thirdpoint {\n";
  output_filestream << "0 begin\n";
  output_filestream << "/y1 exch def\n";
  output_filestream << "/x1 exch def\n";
  output_filestream << "/y0 exch def\n";
  output_filestream << "/x0 exch def\n";
  output_filestream << "x0 2 mul x1 add 3 div\n";
  output_filestream << "y0 2 mul y1 add 3 div\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 4 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/subspline {\n";
  output_filestream << "0 begin\n";
  output_filestream << "/movetoNeeded exch def\n";
  output_filestream << "y exch get /y3 exch def\n";
  output_filestream << "x exch get /x3 exch def\n";
  output_filestream << "y exch get /y2 exch def\n";
  output_filestream << "x exch get /x2 exch def\n";
  output_filestream << "y exch get /y1 exch def\n";
  output_filestream << "x exch get /x1 exch def\n";
  output_filestream << "y exch get /y0 exch def\n";
  output_filestream << "x exch get /x0 exch def\n";
  output_filestream << "x1 y1 x2 y2 thirdpoint\n";
  output_filestream << "/p1y exch def\n";
  output_filestream << "/p1x exch def\n";
  output_filestream << "x2 y2 x1 y1 thirdpoint\n";
  output_filestream << "/p2y exch def\n";
  output_filestream << "/p2x exch def\n";
  output_filestream << "x1 y1 x0 y0 thirdpoint\n";
  output_filestream << "p1x p1y midpoint\n";
  output_filestream << "/p0y exch def\n";
  output_filestream << "/p0x exch def\n";
  output_filestream << "x2 y2 x3 y3 thirdpoint\n";
  output_filestream << "p2x p2y midpoint\n";
  output_filestream << "/p3y exch def\n";
  output_filestream << "/p3x exch def\n";
  output_filestream << "movetoNeeded { p0x p0y moveto } if\n";
  output_filestream << "p1x p1y p2x p2y p3x p3y curveto\n";
  output_filestream << "end\n";
  output_filestream << "} dup 0 17 dict put def\n";
  output_filestream << "\n";
  output_filestream << "/storexyn {\n";
  output_filestream << "/n exch def\n";
  output_filestream << "/y n array def\n";
  output_filestream << "/x n array def\n";
  output_filestream << "n 1 sub -1 0 {\n";
  output_filestream << "/i exch def\n";
  output_filestream << "y i 3 2 roll put\n";
  output_filestream << "x i 3 2 roll put\n";
  output_filestream << "} for\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/SSten {\n";
  output_filestream << "fgred fggreen fgblue setrgbcolor\n";
  output_filestream << "dup true exch 1 0 0 -1 0 6 -1 roll matrix astore\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/FSten {\n";
  output_filestream << "dup 3 -1 roll dup 4 1 roll exch\n";
  output_filestream << "newpath\n";
  output_filestream << "0 0 moveto\n";
  output_filestream << "dup 0 exch lineto\n";
  output_filestream << "exch dup 3 1 roll exch lineto\n";
  output_filestream << "0 lineto\n";
  output_filestream << "closepath\n";
  output_filestream << "bgred bggreen bgblue setrgbcolor\n";
  output_filestream << "eofill\n";
  output_filestream << "SSten\n";
  output_filestream << "} def\n";
  output_filestream << "\n";
  output_filestream << "/Rast {\n";
  output_filestream << "exch dup 3 1 roll 1 0 0 -1 0 6 -1 roll matrix astore\n";
  output_filestream << "} def\n";
  
  // For scale and translate ..
  graphic_header();
  
  graphics_prolog_exists = true;
}

void vbl_psfile::done()
{
  if (debug) vcl_cerr << "vbl_psfile::done" << vcl_endl;
  doneps = true;
  if (graphics_prolog_exists) 
  {
    output_filestream << "end % TargetjrDict" << vcl_endl;
  }

  output_filestream << "showpage\n%%Trailer" << vcl_endl;  
}
