// This is ./vxl/vul/vul_psfile.cxx

//:
// \file

#include "vul_psfile.h"
#include <vcl_iomanip.h>
#include <vcl_cmath.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>

#include <vul/vul_printf.h>

#define PIX2INCH 72.0
#define RANGE(a,b,c) { if (a < b) a = b;  if (a > c) a = c; }
#define in_range(a) (a < (1 << 8))
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
vul_psfile::vul_psfile(char const* f, bool dbg)
  : output_filestream(f)
{
  debug = dbg;
  set_paper_orientation(vul_psfile::PORTRAIT);
  set_paper_type(vul_psfile::US_NORMAL);
  set_paper_layout(vul_psfile::CENTER);
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

  if (debug) vcl_cerr << "vul_psfile::vul_psfile" << vcl_endl;
}

//-----------------------------------------------------------------------------
//: Destructor
//-----------------------------------------------------------------------------
vul_psfile::~vul_psfile()
{
  if (debug) vcl_cerr << "vul_psfile::~vul_psfile" << vcl_endl;
  reset_bounding_box();
  if (!doneps)
    done();
}


//-----------------------------------------------------------------------------
//: Rewrite output bounding box parameters.
//-----------------------------------------------------------------------------
void vul_psfile::reset_bounding_box()
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
void vul_psfile::compute_bounding_box()
{
  box_width  = max_x - min_x;
  box_height = max_y - min_y;
  double hsx, hsy;
  double sz_inx, sz_iny;   // image size, in inches

  if (printer_paper_orientation == vul_psfile::LANDSCAPE)
  {
    psizex = paper_size[printer_paper_type][1];
    psizey = paper_size[printer_paper_type][0];
  }
  else
  {
    psizex = paper_size[printer_paper_type][0];
    psizey = paper_size[printer_paper_type][1];
  }

  if (printer_paper_layout == vul_psfile::CENTER)
  {
    sz_inx = (double) box_width  / PIX2INCH * (scale_x / 100.0);
    sz_iny = (double) box_height / PIX2INCH * (scale_y / 100.0);

    // round to integer .001ths of an inch
    sz_inx = vcl_floor(sz_inx * 1000.0 + 0.5) / 1000.0;
    sz_iny = vcl_floor(sz_iny * 1000.0 + 0.5) / 1000.0;
    // from xv xvps.c subroutine: centerimage
    pos_inx = psizex/2 - sz_inx/2;
    pos_iny = psizey/2 - sz_iny/2;

    // make sure 'center' of image is still on page
    hsx = sz_inx/2;  hsy = sz_iny/2;
    RANGE(pos_inx, -hsx, psizex-hsx);
    RANGE(pos_iny, -hsy, psizey-hsy);

    // round to integer .001ths of an inch
    pos_inx = vcl_floor(pos_inx * 1000.0 + 0.5) / 1000.0;
    pos_iny = vcl_floor(pos_iny * 1000.0 + 0.5) / 1000.0;
  }

  if (printer_paper_layout == vul_psfile::MAX)
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
    sz_inx = vcl_floor(sz_inx * 1000.0 + 0.5) / 1000.0;
    sz_iny = vcl_floor(sz_iny * 1000.0 + 0.5) / 1000.0;
    pos_inx = psizex/2 - sz_inx/2;
    pos_iny = psizey/2 - sz_iny/2;

    // round to integer .001ths of an inch
    pos_inx = vcl_floor(pos_inx * 1000.0 + 0.5) / 1000.0;
    pos_iny = vcl_floor(pos_iny * 1000.0 + 0.5) / 1000.0;
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

  if (debug) vcl_cerr << "vul_psfile::compute_bounding_box, box_width = " << box_width
                      << ", box_height = " << box_height << vcl_endl;
}

//-----------------------------------------------------------------------------
//: Set Bounding Box Min and Max x, y.
//-----------------------------------------------------------------------------
void vul_psfile::set_min_max_xy(float xx, float yy)
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
void vul_psfile::set_min_max_xy(int x, int y)
{
  if (x < min_x)   min_x = x;
  if (y < min_y)   min_y = y;
  if (x > max_x)   max_x = x;
  if (y > max_y)   max_y = y;
}

//-----------------------------------------------------------------------------
//: Write 8 bit grey scale image.
//-----------------------------------------------------------------------------
void vul_psfile::print_greyscale_image(byte* buffer, int sizex, int sizey)
{
  if (debug)
    vcl_cerr << "vul_psfile::print_greyscale_image, width = " << sizex
             << ", height = " << sizey  << ", reduction_factor = "
             << reduction_factor << vcl_endl;

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

  if (printer_paper_orientation == vul_psfile::LANDSCAPE)
  {
    output_filestream << "% print in landscapey mode\n"
                      << "90 rotate 0 " << (int) (-psizey*PIX2INCH) << " translate\n\n";
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
          for (int m=0; m <= width % reduction_factor; m++)
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
        output_filestream << vcl_endl;
      }
    }
    output_filestream << vcl_endl;
  }
  output_filestream << "% stop using temporary dictionary\n"
                    << "end\n\n"
                    << "% restore original state\n"
                    << "origstate restore\n\n";
}

//-----------------------------------------------------------------------------
//: Write 24 bit colour image.
//-----------------------------------------------------------------------------
void vul_psfile::print_color_image(byte* data, int width_arg, int height_arg)
{
  int bytes_per_pixel = 3;
  int new_height;
  int new_width;
  exist_image = true;
  if (debug) vcl_cerr << "Reduction factor is " << reduction_factor << vcl_endl;
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
  if (printer_paper_orientation == vul_psfile::LANDSCAPE)
  {
    output_filestream << "% print in landscape mode\n"
                      << "90 rotate 0 " << (int) (-psizey*PIX2INCH) << " translate\n\n";
  }
  output_filestream << "% lower left corner\n";
  translate_pos = output_filestream.tellp();
  image_translate_and_scale();

  output_filestream
    << vcl_endl
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
    << "  } ifelse          % end of 'false' case\n\n\n\n"
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
          for (int m=0; m <= width_arg % reduction_factor; m++)
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
      countrow+=2*bytes_per_pixel;

      // write RGC hex.
//    output_filestream << (const char*)color_lookup[index];  //FIXME

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
         output_filestream << vcl_endl;
       }
     }
     output_filestream << vcl_endl;
  }

  output_filestream << "% stop using temporary dictionary\n"
                    << "end\n\n"
                    << "% restore original state\n"
                    << "origstate restore\n\n";
}

//-----------------------------------------------------------------------------
//: Set graphic coordiate (translate and rotate to local coordinate).
//-----------------------------------------------------------------------------
void vul_psfile::graphic_header()
{
  if (printer_paper_orientation == vul_psfile::LANDSCAPE)
  {
    output_filestream << "% print in landscape mode\n"
                      << "90 rotate 0 " << (int) (-psizey*PIX2INCH) << " translate\n\n";
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
void vul_psfile::image_translate_and_scale()
{
  int scale_height = (int) ((float) height* scale_y/100.0);
  int scale_min_x  = (int) ((float) min_x * scale_x/100.0);
  int scale_max_y  = (int) ((float) max_y * scale_y/100.0);

  if (debug)
    vcl_cerr << "vul_psfile::image_translate_and_scale, scale_height= "
             << scale_height << ", scale_min_x = " << scale_min_x
             << ", scale_max_y = " << scale_max_y << vcl_endl;

  vul_printf(output_filestream, "%d %d translate\n", ox - scale_min_x, oy + scale_max_y - scale_height);
  vul_printf(output_filestream, "\n%% size of image (on paper, in 1/72inch coords)\n");
  vul_printf(output_filestream, "%g %g scale\n\n", iwf, ihf);
}

//-----------------------------------------------------------------------------
//: Set object translate and scale.
//-----------------------------------------------------------------------------
void vul_psfile::object_translate_and_scale()
{
  int scale_height = (int) ((float) box_height * scale_y/100.0);
  int scale_min_x  = (int) ((float) min_x * scale_x/100.0);
  int scale_min_y  = (int) ((float) min_y * scale_y/100.0);

  // move origin
  vul_printf(output_filestream, "%d %d translate\n", ox - scale_min_x, oy + scale_height + scale_min_y);
  vul_printf(output_filestream, "%g %g scale\n\n", scale_x/100.0, -scale_y/100.0);
  vul_printf(output_filestream, "/originalCTM matrix currentmatrix def\n");
}

//-----------------------------------------------------------------------------
//: Set ox, oy , iw, ih, iwf, ihf parameters for PostScript file use.
//-----------------------------------------------------------------------------
bool vul_psfile::set_parameters(int sizex,int sizey)
{
  width = sizex;
  height = sizey;

  set_min_max_xy(0,0);
  set_min_max_xy(width,height);
  compute_bounding_box();

  double hsx, hsy;
  double sz_inx, sz_iny;   // image size, in inches

  if (printer_paper_orientation == vul_psfile::LANDSCAPE)
  {
    psizex = paper_size[printer_paper_type][1];
    psizey = paper_size[printer_paper_type][0];
  }
  else
  {
    psizex = paper_size[printer_paper_type][0];
    psizey = paper_size[printer_paper_type][1];
  }

  if (printer_paper_layout == vul_psfile::CENTER)
  {
    sz_inx = (double) width  / PIX2INCH * (scale_x / 100.0);
    sz_iny = (double) height / PIX2INCH * (scale_y / 100.0);

    // round to integer .001ths of an inch
    sz_inx = vcl_floor(sz_inx * 1000.0 + 0.5) / 1000.0;
    sz_iny = vcl_floor(sz_iny * 1000.0 + 0.5) / 1000.0;
    // from xv xvps.c subroutine: centerimage
    pos_inx = psizex/2 - sz_inx/2;
    pos_iny = psizey/2 - sz_iny/2;

    // make sure 'center' of image is still on page
    hsx = sz_inx/2;  hsy = sz_iny/2;
    RANGE(pos_inx, -hsx, psizex-hsx);
    RANGE(pos_iny, -hsy, psizey-hsy);

    // round to integer .001ths of an inch
    pos_inx = vcl_floor(pos_inx * 1000.0 + 0.5) / 1000.0;
    pos_iny = vcl_floor(pos_iny * 1000.0 + 0.5) / 1000.0;
  }

  else if (printer_paper_layout == vul_psfile::MAX)
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
    sz_inx = vcl_floor(sz_inx * 1000.0 + 0.5) / 1000.0;
    sz_iny = vcl_floor(sz_iny * 1000.0 + 0.5) / 1000.0;
    pos_inx = psizex/2 - sz_inx/2;
    pos_iny = psizey/2 - sz_iny/2;

    // round to integer .001ths of an inch
    pos_inx = vcl_floor(pos_inx * 1000.0 + 0.5) / 1000.0;
    pos_iny = vcl_floor(pos_iny * 1000.0 + 0.5) / 1000.0;
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
void vul_psfile::postscript_header()
{
  if (header_pos != HEADER_START)
  {
    vcl_cerr << "vul_psfile: Header already set to " << header_pos << vcl_endl;
    return;
  }

  output_filestream << "%!PS-Adobe-2.0 EPSF-2.0\n"
                    << "%%Title: " << filename.c_str() << vcl_endl
                    << "%%Creator: vul_psfile - by Alan S. Liu\n"
                    << "%%compute_bounding_box: ";

  header_pos = output_filestream.tellp();
  reset_postscript_header();
}


//-----------------------------------------------------------------------------
//: Reset PostScript header file
//-----------------------------------------------------------------------------
void vul_psfile::reset_postscript_header()
{
  if (printer_paper_orientation == vul_psfile::LANDSCAPE)
  {
    vul_printf(output_filestream, "%4d %4d %4d %4d\n",
        (int) (pos_iny * PIX2INCH + 0.5),
        (int) (pos_inx * PIX2INCH + 0.5),
        (int) (pos_iny * PIX2INCH + 0.5) + ih,
        (int) (pos_inx * PIX2INCH + 0.5) + iw);
  }
  else
  {
    vul_printf(output_filestream, "%4d %4d %4d %4d\n", ox,oy,ox+iw,oy+ih);
  }
  output_filestream << "%%Pages: 1\n"
                    << "%%DocumentFonts:\n"
                    << "%%EndComments\n";
}

//-----------------------------------------------------------------------------
// sobj_rgb_params() --
//-----------------------------------------------------------------------------
void vul_psfile::sobj_rgb_params(char const* obj_str, bool filled)
{
  print_graphics_prolog();
  output_filestream << "\nBegin %I " << obj_str << vcl_endl
                    << "2 0 0 [] 0 SetB\n"
                    << fg_r << " " << fg_g << " " << fg_b << " SetCFg\n"
                    << bg_r << " " << bg_g << " " << bg_b << " SetCBg\n"
                    << line_width << " setlinewidth\n"
                    << (filled ? "0": "none") << " SetP %I p n\n";
}

//-----------------------------------------------------------------------------
//:  Add a line between the given points to the Postscript file.
//-----------------------------------------------------------------------------
void vul_psfile::line(float x1, float y1, float x2, float y2)
{
  // set up bounding box.
  set_min_max_xy(x1,y1);
  set_min_max_xy(x2,y2);
  compute_bounding_box();

  print_graphics_prolog();
  sobj_rgb_params("Line", false);

  output_filestream << (int) x1 << " " << (int) y1 << " "
                    << (int) x2 << " " << (int) y2 << " Line\n"
                    << "End\n";
}

//-----------------------------------------------------------------------------
//: Add a point at the given coordinates to the Postscript file.
//-----------------------------------------------------------------------------
void vul_psfile::point(float x, float y, float point_size)
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
void vul_psfile::ellipse(float x, float y, float a_axis, float b_axis, int angle)
{
  #ifndef PI // should already be defined in math.h - PVR
  #define PI 3.14159265358979323846
  #endif

  set_min_max_xy((int) (x+a_axis*vcl_cos(((float)angle)*PI/180.0) + 0.5),
                (int) (y+a_axis*vcl_sin(((float)angle)*PI/180.0) + 0.5) );
  set_min_max_xy((int) (x-a_axis*vcl_cos(((float)angle)*PI/180.0) + 0.5),
                (int) (y-a_axis*vcl_sin(((float)angle)*PI/180.0) + 0.5) );
  compute_bounding_box();

  print_graphics_prolog();
  sobj_rgb_params("Ellipse", false);
  if (angle)
  {
    output_filestream << (int) x << " " << (int) y << " translate \n"
                      << -angle << " rotate\n"
                      << "0 0 " << (int) a_axis << " "
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
void vul_psfile::circle(float x, float y, float radius)
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
void vul_psfile::print_graphics_prolog()
{
  if (graphics_prolog_exists)
    return;
  exist_objs = true;
  output_filestream
    << "\n%%BeginTargetjrPrologue\n"
    << "/arrowhead {\n"
    << "0 begin\n"
    << "transform originalCTM itransform\n"
    << "/taily exch def\n"
    << "/tailx exch def\n"
    << "transform originalCTM itransform\n"
    << "/tipy exch def\n"
    << "/tipx exch def\n"
    << "/dy tipy taily sub def\n"
    << "/dx tipx tailx sub def\n"
    << "/angle dx 0 ne dy 0 ne or { dy dx atan } { 90 } ifelse def\n"
    << "gsave\n"
    << "originalCTM setmatrix\n"
    << "tipx tipy translate\n"
    << "angle rotate\n"
    << "newpath\n"
    << "arrowHeight neg arrowWidth 2 div moveto\n"
    << "0 0 lineto\n"
    << "arrowHeight neg arrowWidth 2 div neg lineto\n"
    << "patternNone not {\n"
    << "originalCTM setmatrix\n"
    << "/padtip arrowHeight 2 exp 0.25 arrowWidth 2 exp mul add sqrt brushWidth mul\n"
    << "arrowWidth div def\n"
    << "/padtail brushWidth 2 div def\n"
    << "tipx tipy translate\n"
    << "angle rotate\n"
    << "padtip 0 translate\n"
    << "arrowHeight padtip add padtail add arrowHeight div dup scale\n"
    << "arrowheadpath\n"
    << "ifill\n"
    << "} if\n"
    << "brushNone not {\n"
    << "originalCTM setmatrix\n"
    << "tipx tipy translate\n"
    << "angle rotate\n"
    << "arrowheadpath\n"
    << "istroke\n"
    << "} if\n"
    << "grestore\n"
    << "end\n"
    << "} dup 0 9 dict put def\n\n"
    << "/arrowheadpath {\n"
    << "newpath\n"
    << "arrowHeight neg arrowWidth 2 div moveto\n"
    << "0 0 lineto\n"
    << "arrowHeight neg arrowWidth 2 div neg lineto\n"
    << "} def\n\n"
    << "/leftarrow {\n"
    << "0 begin\n"
    << "y exch get /taily exch def\n"
    << "x exch get /tailx exch def\n"
    << "y exch get /tipy exch def\n"
    << "x exch get /tipx exch def\n"
    << "brushLeftArrow { tipx tipy tailx taily arrowhead } if\n"
    << "end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/rightarrow {\n"
    << "0 begin\n"
    << "y exch get /tipy exch def\n"
    << "x exch get /tipx exch def\n"
    << "y exch get /taily exch def\n"
    << "x exch get /tailx exch def\n"
    << "brushRightArrow { tipx tipy tailx taily arrowhead } if\n"
    << "end\n"
    << "} dup 0 4 dict put def\n\n"
    << "%%EndTargetjrPrologue\n\n"
    << "/arrowHeight 10 def\n"
    << "/arrowWidth 5 def\n\n"
    << "/TargetjrDict 50 dict def\n"
    << "TargetjrDict begin\n\n"
    << "/none null def\n"
    << "/numGraphicParameters 17 def\n"
    << "/stringLimit 65535 def\n\n"
    << "/Begin {\n"
    << "save\n"
    << "numGraphicParameters dict begin\n"
    << "} def\n\n"
    << "/End {\n"
    << "end\n"
    << "restore\n"
    << "} def\n\n"
    << "/SetB { % width leftarrow rightarrow DashArray DashOffset SetB\n"
    << "dup type /nulltype eq {\n"
    << "pop\n"
    << "false /brushRightArrow idef\n"
    << "false /brushLeftArrow idef\n"
    << "true /brushNone idef\n"
    << "} {\n"
    << "/brushDashOffset idef\n"
    << "/brushDashArray idef\n"
    << "0 ne /brushRightArrow idef\n"
    << "0 ne /brushLeftArrow idef\n"
    << "/brushWidth idef\n"
    << "false /brushNone idef\n"
    << "} ifelse\n"
    << "} def\n\n"
    << "/SetCFg {\n"
    << "/fgblue idef\n"
    << "/fggreen idef\n"
    << "/fgred idef\n"
    << "} def\n\n"
    << "/SetCBg {\n"
    << "/bgblue idef\n"
    << "/bggreen idef\n"
    << "/bgred idef\n"
    << "} def\n\n"
    << "/SetF {\n"
    << "/printSize idef\n"
    << "/printFont idef\n"
    << "} def\n\n"
    << "/SetP {  % string -1 SetP  OR gray SetP\n"
    << "dup type /nulltype eq {\n"
    << "pop true /patternNone idef\n"
    << "} {\n"
    << "dup -1 eq {\n"
    << "/patternGrayLevel idef\n"
    << "/patternString idef\n"
    << "} {\n"
    << "/patternGrayLevel idef\n"
    << "} ifelse\n"
    << "false /patternNone idef\n"
    << "} ifelse\n"
    << "} def\n\n"
    << "/BSpl {\n"
    << "0 begin\n"
    << "storexyn\n"
    << "newpath\n"
    << "n 1 gt {\n"
    << "0 0 0 0 0 0 1 1 true subspline\n"
    << "n 2 gt {\n"
    << "0 0 0 0 1 1 2 2 false subspline\n"
    << "1 1 n 3 sub {\n"
    << "/i exch def\n"
    << "i 1 sub dup i dup i 1 add dup i 2 add dup false subspline\n"
    << "} for\n"
    << "n 3 sub dup n 2 sub dup n 1 sub dup 2 copy false subspline\n"
    << "} if\n"
    << "n 2 sub dup n 1 sub dup 2 copy 2 copy false subspline\n"
    << "patternNone not brushLeftArrow not brushRightArrow not and and { ifill } if\n"
    << "brushNone not { istroke } if\n"
    << "0 0 1 1 leftarrow\n"
    << "n 2 sub dup n 1 sub dup rightarrow\n"
    << "} if\n"
    << "end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/Circ {\n"
    << "newpath\n"
    << "0 360 arc\n"
    << "patternNone not { ifill } if\n"
    << "brushNone not { istroke } if\n"
    << "} def\n\n"
    << "/CBSpl {\n"
    << "0 begin\n"
    << "dup 2 gt {\n"
    << "storexyn\n"
    << "newpath\n"
    << "n 1 sub dup 0 0 1 1 2 2 true subspline\n"
    << "1 1 n 3 sub {\n"
    << "/i exch def\n"
    << "i 1 sub dup i dup i 1 add dup i 2 add dup false subspline\n"
    << "} for\n"
    << "n 3 sub dup n 2 sub dup n 1 sub dup 0 0 false subspline\n"
    << "n 2 sub dup n 1 sub dup 0 0 1 1 false subspline\n"
    << "patternNone not { ifill } if\n"
    << "brushNone not { istroke } if\n"
    << "} {\n"
    << "Poly\n"
    << "} ifelse\n"
    << "end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/Elli {\n"
    << "0 begin\n"
    << "newpath\n"
    << "4 2 roll\n"
    << "translate\n"
    << "scale\n"
    << "0 0 1 0 360 arc\n"
    << "patternNone not { ifill } if\n"
    << "brushNone not { istroke } if\n"
    << "end\n"
    << "} dup 0 1 dict put def\n\n"
    << "/Line {\n"
    << "0 begin\n"
    << "2 storexyn\n"
    << "newpath\n"
    << "x 0 get y 0 get moveto\n"
    << "x 1 get y 1 get lineto\n"
    << "brushNone not { istroke } if\n"
    << "0 0 1 1 leftarrow\n"
    << "0 0 1 1 rightarrow\n"
    << "end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/MLine {\n"
    << "0 begin\n"
    << "storexyn\n"
    << "newpath\n"
    << "n 1 gt {\n"
    << "x 0 get y 0 get moveto\n"
    << "1 1 n 1 sub {\n"
    << "/i exch def\n"
    << "x i get y i get lineto\n"
    << "} for\n"
    << "patternNone not brushLeftArrow not brushRightArrow not and and { ifill } if\n"
    << "brushNone not { istroke } if\n"
    << "0 0 1 1 leftarrow\n"
    << "n 2 sub dup n 1 sub dup rightarrow\n"
    << "} if\n"
    << "end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/Poly {\n"
    << "3 1 roll\n"
    << "newpath\n"
    << "moveto\n"
    << "-1 add\n"
    << "{ lineto } repeat\n"
    << "closepath\n"
    << "patternNone not { ifill } if\n"
    << "brushNone not { istroke } if\n"
    << "} def\n\n"
    << "/Rect {\n"
    << "0 begin\n"
    << "/t exch def\n"
    << "/r exch def\n"
    << "/b exch def\n"
    << "/l exch def\n"
    << "newpath\n"
    << "l b moveto\n"
    << "l t lineto\n"
    << "r t lineto\n"
    << "r b lineto\n"
    << "closepath\n"
    << "patternNone not { ifill } if\n"
    << "brushNone not { istroke } if\n"
    << "end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/Text {\n"
    << "ishow\n"
    << "} def\n\n"
    << "/idef {\n"
    << "dup where { pop pop pop } { exch def } ifelse\n"
    << "} def\n\n"
    << "/ifill {\n"
    << "0 begin\n"
    << "gsave\n"
    << "patternGrayLevel -1 ne {\n"
    << "fgred bgred fgred sub patternGrayLevel mul add\n"
    << "fggreen bggreen fggreen sub patternGrayLevel mul add\n"
    << "fgblue bgblue fgblue sub patternGrayLevel mul add setrgbcolor\n"
    << "eofill\n"
    << "} {\n"
    << "eoclip\n"
    << "originalCTM setmatrix\n"
    << "pathbbox /t exch def /r exch def /b exch def /l exch def\n"
    << "/w r l sub ceiling cvi def\n"
    << "/h t b sub ceiling cvi def\n"
    << "/imageByteWidth w 8 div ceiling cvi def\n"
    << "/imageHeight h def\n"
    << "bgred bggreen bgblue setrgbcolor\n"
    << "eofill\n"
    << "fgred fggreen fgblue setrgbcolor\n"
    << "w 0 gt h 0 gt and {\n"
    << "l w add b translate w neg h scale\n"
    << "w h true [w 0 0 h neg 0 h] { patternproc } imagemask\n"
    << "} if\n"
    << "} ifelse\n"
    << "grestore\n"
    << "end\n"
    << "} dup 0 8 dict put def\n\n"
    << "/istroke {\n"
    << "gsave\n"
    << "brushDashOffset -1 eq {\n"
    << "[] 0 setdash\n"
    << "1 setgray\n"
    << "} {\n"
    << "brushDashArray brushDashOffset setdash\n"
    << "fgred fggreen fgblue setrgbcolor\n"
    << "} ifelse\n"
//  << "brushWidth setlinewidth\n"
    << "originalCTM setmatrix\n"
    << "stroke\n"
    << "grestore\n"
    << "} def\n\n"
    << "/ishow {\n"
    << "0 begin\n"
    << "gsave\n"
    << "fgred fggreen fgblue setrgbcolor\n"
    << "/fontDict printFont printSize scalefont dup setfont def\n"
    << "/descender fontDict begin 0 [FontBBox] 1 get FontMatrix end\n"
    << "transform exch pop def\n"
    << "/vertoffset 1 printSize sub descender sub def {\n"
    << "0 vertoffset moveto show\n"
    << "/vertoffset vertoffset printSize sub def\n"
    << "} forall\n"
    << "grestore\n"
    << "end\n"
    << "} dup 0 3 dict put def\n"
    << "/patternproc {\n"
    << "0 begin\n"
    << "/patternByteLength patternString length def\n"
    << "/patternHeight patternByteLength 8 mul sqrt cvi def\n"
    << "/patternWidth patternHeight def\n"
    << "/patternByteWidth patternWidth 8 idiv def\n"
    << "/imageByteMaxLength imageByteWidth imageHeight mul\n"
    << "stringLimit patternByteWidth sub min def\n"
    << "/imageMaxHeight imageByteMaxLength imageByteWidth idiv patternHeight idiv\n"
    << "patternHeight mul patternHeight max def\n"
    << "/imageHeight imageHeight imageMaxHeight sub store\n"
    << "/imageString imageByteWidth imageMaxHeight mul patternByteWidth add string def\n"
    << "0 1 imageMaxHeight 1 sub {\n"
    << "/y exch def\n"
    << "/patternRow y patternByteWidth mul patternByteLength mod def\n"
    << "/patternRowString patternString patternRow patternByteWidth getinterval def\n"
    << "/imageRow y imageByteWidth mul def\n"
    << "0 patternByteWidth imageByteWidth 1 sub {\n"
    << "/x exch def\n"
    << "imageString imageRow x add patternRowString putinterval\n"
    << "} for\n"
    << "} for\n"
    << "imageString\n"
    << "end\n"
    << "} dup 0 12 dict put def\n\n"
    << "/min {\n"
    << "dup 3 2 roll dup 4 3 roll lt { exch } if pop\n"
    << "} def\n\n"
    << "/max {\n"
    << "dup 3 2 roll dup 4 3 roll gt { exch } if pop\n"
    << "} def\n\n"
    << "/midpoint {\n"
    << "0 begin\n"
    << "/y1 exch def\n"
    << "/x1 exch def\n"
    << "/y0 exch def\n"
    << "/x0 exch def\n"
    << "x0 x1 add 2 div\n"
    << "y0 y1 add 2 div\n"
    << "end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/thirdpoint {\n"
    << "0 begin\n"
    << "/y1 exch def\n"
    << "/x1 exch def\n"
    << "/y0 exch def\n"
    << "/x0 exch def\n"
    << "x0 2 mul x1 add 3 div\n"
    << "y0 2 mul y1 add 3 div\n"
    << "end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/subspline {\n"
    << "0 begin\n"
    << "/movetoNeeded exch def\n"
    << "y exch get /y3 exch def\n"
    << "x exch get /x3 exch def\n"
    << "y exch get /y2 exch def\n"
    << "x exch get /x2 exch def\n"
    << "y exch get /y1 exch def\n"
    << "x exch get /x1 exch def\n"
    << "y exch get /y0 exch def\n"
    << "x exch get /x0 exch def\n"
    << "x1 y1 x2 y2 thirdpoint\n"
    << "/p1y exch def\n"
    << "/p1x exch def\n"
    << "x2 y2 x1 y1 thirdpoint\n"
    << "/p2y exch def\n"
    << "/p2x exch def\n"
    << "x1 y1 x0 y0 thirdpoint\n"
    << "p1x p1y midpoint\n"
    << "/p0y exch def\n"
    << "/p0x exch def\n"
    << "x2 y2 x3 y3 thirdpoint\n"
    << "p2x p2y midpoint\n"
    << "/p3y exch def\n"
    << "/p3x exch def\n"
    << "movetoNeeded { p0x p0y moveto } if\n"
    << "p1x p1y p2x p2y p3x p3y curveto\n"
    << "end\n"
    << "} dup 0 17 dict put def\n\n"
    << "/storexyn {\n"
    << "/n exch def\n"
    << "/y n array def\n"
    << "/x n array def\n"
    << "n 1 sub -1 0 {\n"
    << "/i exch def\n"
    << "y i 3 2 roll put\n"
    << "x i 3 2 roll put\n"
    << "} for\n"
    << "} def\n\n"
    << "/SSten {\n"
    << "fgred fggreen fgblue setrgbcolor\n"
    << "dup true exch 1 0 0 -1 0 6 -1 roll matrix astore\n"
    << "} def\n\n"
    << "/FSten {\n"
    << "dup 3 -1 roll dup 4 1 roll exch\n"
    << "newpath\n"
    << "0 0 moveto\n"
    << "dup 0 exch lineto\n"
    << "exch dup 3 1 roll exch lineto\n"
    << "0 lineto\n"
    << "closepath\n"
    << "bgred bggreen bgblue setrgbcolor\n"
    << "eofill\n"
    << "SSten\n"
    << "} def\n\n"
    << "/Rast {\n"
    << "exch dup 3 1 roll 1 0 0 -1 0 6 -1 roll matrix astore\n"
    << "} def\n";

  // For scale and translate ..
  graphic_header();

  graphics_prolog_exists = true;
}

void vul_psfile::done()
{
  if (debug) vcl_cerr << "vul_psfile::done" << vcl_endl;
  doneps = true;
  if (graphics_prolog_exists)
  {
    output_filestream << "end % TargetjrDict" << vcl_endl;
  }

  output_filestream << "showpage\n%%Trailer" << vcl_endl;
}
