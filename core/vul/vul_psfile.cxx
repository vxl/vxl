// This is core/vul/vul_psfile.cxx
#include "vul_psfile.h"
//:
// \file

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h> // for setw()
#include <vcl_algorithm.h> // for vcl_min()
#include <vcl_cassert.h>

#define RANGE(a,b,c) { if (a < b) a = b;  if (a > c) a = c; }
#define in_range(a) (a < 0x100)
#define Hex4bit(a) ((char)((a<=9) ? (a+'0') : (a - 10 + 'a')))

static const float PIX2INCH = 72.0f;
static bool debug = true;

// sizes of pages in inches
static double paper_size[8][2] = {
  { 8.500, 11.000}, // US NORMAL
  { 8.268, 11.693}, // A4 210mm x 297mm
  { 7.205, 10.118}, // B5 183mm x 257mm
  {11.693, 16.535}, // A3 297mm x 420mm
  { 8.500, 14.000}, // US LEGAL
  {11.000, 17.000}, // B-size
  { 3.875,  4.875}, // 4 by 5
  { 0.945,  1.417}  // 35mm (24x36)
};

// size of l+r margin and t+b margin.  image is centered
static double margins[8 ][2] = {
  { 1.000, 1.000}, // US NORMAL
  { 1.000, 1.000}, // A4
  { 1.000, 1.000}, // B5
  { 1.000, 1.000}, // A3
  { 1.000, 1.000}, // US LEGAL
  { 1.000, 1.000}, // B-size
  { 0.275, 0.275}, // 4 by 5
  { 0.078, 0.078}  // 35mm (24x36)
};

// min and max value for PostScript paper size
static float ps_minimum = 0.1f;
static float ps_maximum = 8.f;

static const vcl_streampos HEADER_START(-1);

//-----------------------------------------------------------------------------
//: Default constructor.
//-----------------------------------------------------------------------------
vul_psfile::vul_psfile(char const* f, bool dbg)
  : output_filestream(f),
    fg_r(0), fg_g(0), fg_b(0),
    bg_r(1), bg_g(1), bg_b(1),
    line_width_(1),
    scale_x(1.f), scale_y(1.f),
    ox(0), oy(0), iw(0), ih(0),
    iwf(1.0), ihf(1.0),
    psizex(8.5), psizey(11),
    pos_inx(4.25), pos_iny(5.5),
    width(0), height(0),
    filename(f),
    printer_paper_type(vul_psfile::US_NORMAL),
    printer_paper_orientation(vul_psfile::PORTRAIT),
    printer_paper_layout(vul_psfile::CENTER),
    reduction_factor(1),
    doneps(false),
    min_x(1000), min_y(1000),
    max_x(-1000), max_y(-1000),
    box_width(0), box_height(0),
    translate_pos(-1L),
    sobj_t_pos(-1L),
    header_pos(HEADER_START),
    graphics_prolog_exists(false),
    exist_image(false),
    exist_objs (false)
{
  debug = dbg;
  if (debug) vcl_cout << "vul_psfile::vul_psfile\n";
  postscript_header();
}

//-----------------------------------------------------------------------------
//: Destructor
//-----------------------------------------------------------------------------
vul_psfile::~vul_psfile()
{
  if (debug) vcl_cout << "vul_psfile::~vul_psfile\n";
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
    double hsx = box_width  / PIX2INCH * scale_x * .5;
    double hsy = box_height / PIX2INCH * scale_y * .5;

    // from xv xvps.c subroutine: centerimage
    pos_inx = psizex*.5 - hsx;
    pos_iny = psizey*.5 - hsy;

    // make sure 'center' of image is still on page
    RANGE(pos_inx, -hsx, psizex-hsx);
    RANGE(pos_iny, -hsy, psizey-hsy);

    // round to integer .001ths of an inch
    pos_inx = vcl_floor(pos_inx * 1000.0 + 0.5) * .001;
    pos_iny = vcl_floor(pos_iny * 1000.0 + 0.5) * .001;
  }
  else if (printer_paper_layout == vul_psfile::MAX)
  {
    double hsx = psizex - margins[printer_paper_type][0];
    double hsy = psizey - margins[printer_paper_type][1];

    // avoid division by 0:
    if (box_width == 0) box_width = 1;
    if (box_height == 0) box_height = 1;

    // choose the smaller scaling factor
    scale_x = scale_y = (float)vcl_min(hsx/box_width, hsy/box_height) * PIX2INCH;

    RANGE(scale_x,ps_minimum,ps_maximum);
    RANGE(scale_y,ps_minimum,ps_maximum);

    pos_inx = psizex*.5 - box_width / PIX2INCH * scale_x *.5;
    pos_iny = psizey*.5 - box_height/ PIX2INCH * scale_y *.5;

    // round to integer .001ths of an inch
    pos_inx = vcl_floor(pos_inx * 1000.0 + 0.5) * .001;
    pos_iny = vcl_floor(pos_iny * 1000.0 + 0.5) * .001;
  }

  // printed image will have size iw,ih (in picas)
  if (exist_image)
  {
    iwf = width  * scale_x; iw = int(iwf + 0.5);
    ihf = height * scale_y; ih = int(ihf + 0.5);
  }
  if (exist_objs)
  {
    iw = int(box_width  * scale_x + 0.5);
    ih = int(box_height * scale_y + 0.5);
  }

  // compute offset to bottom-left of image (in picas)
  ox = int(pos_inx*PIX2INCH+0.5);
  oy = int(pos_iny*PIX2INCH+0.5);

  if (debug) vcl_cout << "vul_psfile::compute_bounding_box, box_width = "
                      << box_width << ", box_height = " << box_height << '\n';
}

//-----------------------------------------------------------------------------
//: Set Bounding Box Min and Max x, y.
//-----------------------------------------------------------------------------
void vul_psfile::set_min_max_xy(float xx, float yy)
{
  int x = int(xx + 0.5);
  int y = int(yy + 0.5);
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
void vul_psfile::print_greyscale_image(unsigned char* buffer, int sizex, int sizey)
{
  if (debug)
    vcl_cout << "vul_psfile::print_greyscale_image, width = " << sizex
             << ", height = " << sizey  << ", reduction_factor = "
             << reduction_factor << '\n';

  exist_image = true;
  width = sizex;
  height = sizey;
  set_parameters(sizex,sizey);
  compute_bounding_box();

  int new_width = (int)vcl_ceil(sizex/(double)reduction_factor); // round up
  int new_height= (int)vcl_ceil(sizey/(double)reduction_factor);

  output_filestream
    << "\n%%Page: 1 1\n\n% remember original state\n/origstate save def\n"
    << "\n% build a temporary dictionary\n20 dict begin\n\n"
    << "% define string to hold a scanline's worth of data\n"
    << "/pix " << new_width << " string def\n";

  if (printer_paper_orientation == vul_psfile::LANDSCAPE)
    output_filestream
      << "% print in landscape mode\n90 rotate 0 " << int(-psizey*PIX2INCH) << " translate\n\n";
  output_filestream << "% lower left corner\n";
  translate_pos = output_filestream.tellp();
  image_translate_and_scale();

  output_filestream
    << new_width << ' ' << new_height << " 8             % dimensions of data\n"
    << '[' << new_width << " 0 0 -" << new_height << " 0 " << new_height
    << "]  % mapping matrix\n{currentfile pix readhexstring pop}\nimage\n\n";
  const int linesize = 72;

  // write image data to output PostScript file
  for (int j=0; j<new_height; j++)
  {
    int countrow = 0;
    for (int i = 0; i < new_width; i++)
    {
      int index;

      if (reduction_factor == 1)
        index = int(*(buffer + width * j + i));
      else // Reduce resolution of image if necessary
      {
        int pixel_number= (width * j + i) * reduction_factor;
        index=0;
        int number_of_pixels_sampled=0;
        for (int m=0; m < reduction_factor;m++)
          for (int n=0; n < reduction_factor;n++)
            if (i*reduction_factor+m < width && j*reduction_factor+n < height)
            {
              index += int(*(buffer + (pixel_number+m+n*width)));
              ++number_of_pixels_sampled;
            }
        index/=number_of_pixels_sampled; // Average the pixel intensity value.
      }

      // write hex pixel value
      if (in_range(index))
      {
        char pixel[3];
        unsigned char low4 = (unsigned char)  (index & 0x000f);
        unsigned char high4 = (unsigned char) ((index & 0x00f0) >> 4);
        pixel[0] = Hex4bit(high4);
        pixel[1] = Hex4bit(low4);
        pixel[2] = '\0';
        output_filestream << pixel;
      }
      else
        vcl_cout << " index out of range: " << index << '\n';

      countrow+=2;
      if (countrow >= linesize)
      {
        countrow = 0;
        output_filestream << '\n';
      }
    }
    output_filestream << '\n';
  }
  output_filestream << "% stop using temporary dictionary\nend\n\n"
                    << "% restore original state\norigstate restore\n\n";
}

//-----------------------------------------------------------------------------
//: Write 24 bit colour image.
//-----------------------------------------------------------------------------
void vul_psfile::print_color_image(unsigned char* data, int sizex, int sizey)
{
  if (debug)
    vcl_cout << "vul_psfile::print_color_image, width = " << sizex
             << ", height = " << sizey  << ", reduction_factor = "
             << reduction_factor << '\n';

  const int bytes_per_pixel = 3;
  exist_image = true;
  width = sizex;
  height = sizey;
  set_parameters(sizex,sizey);
  compute_bounding_box();

  int new_width = (int)vcl_ceil(sizex/(double)reduction_factor); // round up
  int new_height= (int)vcl_ceil(sizey/(double)reduction_factor);

  // This part uses xv outfile as a reference:
  output_filestream
    << "\n%%Page: 1 1\n\n"
    << "% remember original state\n"
    << "/origstate save def\n\n"
    << "% build a temporary dictionary\n"
    << "20 dict begin\n\n"
    << "% define string to hold a scanline's worth of data\n"
    << "/pix " << 3 * new_width << " string def\n\n"
    << "% define space for color conversions\n"
    << "/grays " << new_width << " string def  % space for gray scale line\n"
    << "/npixls 0 def\n"
    << "/rgbindx 0 def\n\n";

  if (printer_paper_orientation == vul_psfile::LANDSCAPE)
    output_filestream
      << "% print in landscape mode\n90 rotate 0 " << int(-psizey*PIX2INCH) << " translate\n\n";
  output_filestream << "% lower left corner\n";
  translate_pos = output_filestream.tellp();
  image_translate_and_scale();

  output_filestream
    << "\n% define 'colorimage' if it isn't defined\n"
    << "% ('colortogray' and 'mergeprocs' come from xwd2ps via xgrab)\n"
    << "/colorimage where  % do we know about 'colorimage'?\n"
    << "  { pop }          % yes: pop off the 'dict' returned\n"
    << "  {                % no:  define one\n"
    << "    /colortogray { % define an RGB->I function\n"
    << "      /rgbdata exch store  % call input 'rgbdata'\n"
    << "      rgbdata length 3 idiv\n"
    << "      /npixls exch store\n"
    << "      /rgbindx 0 store\n"
    << "      0 1 npixls 1 sub {\n"
    << "        grays exch\n"
    << "        rgbdata rgbindx       get 20 mul % Red\n"
    << "        rgbdata rgbindx 1 add get 32 mul % Green\n"
    << "        rgbdata rgbindx 2 add get 12 mul % Blue\n"
    << "        add add 64 idiv  % I = .3125 R + .5 G + .1875 B\n"
    << "        put\n"
    << "        /rgbindx rgbindx 3 add store\n"
    << "      } for\n"
    << "      grays 0 npixls getinterval\n"
    << "    } bind def\n\n"
    << "    % Utility procedure for colorimage operator.\n"
    << "    % This procedure takes two procedures off the stack and merges them into a single procedure.\n\n"
    << "    /mergeprocs {\n"
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
    << "    /colorimage {\n"
    << "      pop pop    % remove 'false 3' operands\n"
    << "      {colortogray} mergeprocs\n"
    << "      image\n"
    << "    } bind def\n"
    << "  } ifelse    % end of 'false' case\n\n"
    << new_width << ' ' << new_height << " 8  % dimensions of data\n"
    << '[' << new_width << " 0 0 -" << new_height << " 0 " << new_height << "]  % mapping matrix\n"
    << "{currentfile pix readhexstring pop}\n"
    << "false 3 colorimage\n\n";

  // write image data into PostScript file.
  const int linesize = 72;

  // extract RGB data from pixel value and write it to output file
  for (int j = 0; j < new_height;j++)
  {
    int countrow = 0;
    for (int i = 0; i < new_width; i++)
    {
      for (int c = 0; c < bytes_per_pixel; ++c)
      {
        // get RGB hex index.
        int index;

        if (reduction_factor == 1)
          index = int(*(data + (sizex*j+i) * bytes_per_pixel + c));
        else // Reduce image if necessary
        {
          int pixel_number= (sizex*j+i) * bytes_per_pixel * reduction_factor + c;
          index=0;
          int number_of_pixels_sampled=0;
          for (int m=0; m < reduction_factor;m++)
            for (int n=0; n < reduction_factor;n++)
              if (i*reduction_factor+m < sizex && j*reduction_factor+n < sizey)
              {
                index += int(*(data+(pixel_number+(m+n*sizex)*bytes_per_pixel)));
                ++number_of_pixels_sampled;
              }
          index/=number_of_pixels_sampled;  // average the pixel intensity
        }

        // write RGC hex.
        if (in_range(index))
        {
          char pixel[3];
          unsigned char low4 = (unsigned char)  (index & 0x000f);
          unsigned char high4 = (unsigned char) ((index & 0x00f0) >> 4);
          pixel[0] = Hex4bit(high4);
          pixel[1] = Hex4bit(low4);
          pixel[2] = '\0';
          output_filestream << pixel;
        }
        else
          vcl_cout << " index out of range: " << index << '\n';

        countrow+=2;
        if (countrow >= linesize)
        {
          countrow = 0;
          output_filestream << '\n';
        }
      }
    }
    output_filestream << '\n';
  }

  output_filestream << "% stop using temporary dictionary\nend\n\n"
                    << "% restore original state\norigstate restore\n\n";
}

//-----------------------------------------------------------------------------
//: Set graphic coordinate (translate and rotate to local coordinate).
//-----------------------------------------------------------------------------
void vul_psfile::graphic_header()
{
  if (printer_paper_orientation == vul_psfile::LANDSCAPE)
    output_filestream << "% print in landscape mode\n90 rotate 0 "
                      << int(-psizey*PIX2INCH) << " translate\n\n";

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
  int scale_height = int(height* scale_y);
  int scale_min_x  = int(min_x * scale_x);
  int scale_max_y  = int(max_y * scale_y);

  if (debug)
    vcl_cout << "vul_psfile::image_translate_and_scale, scale_height= "
             << scale_height << ", scale_min_x = " << scale_min_x
             << ", scale_max_y = " << scale_max_y << '\n';

  output_filestream << vcl_setw(6) << ox - scale_min_x << ' '
                    << vcl_setw(6) << oy + scale_max_y - scale_height << " translate\n"
                    << "\n% size of image (on paper, in 1/72inch coordinates)\n"
                    << vcl_setw(9) << iwf << ' '
                    << vcl_setw(9) << ihf << " scale\n\n";
}

//-----------------------------------------------------------------------------
//: Set object translate and scale.
//-----------------------------------------------------------------------------
void vul_psfile::object_translate_and_scale()
{
  int scale_height = int(box_height * scale_y);
  int scale_min_x  = int(min_x * scale_x);
  int scale_min_y  = int(min_y * scale_y);
  // round to integer .01ths
  scale_x = vcl_floor(scale_x * 100.0f + 0.5f) * .01f;
  scale_y = vcl_floor(scale_y * 100.0f + 0.5f) * .01f;

  // move origin
  output_filestream << vcl_setw(6) << ox - scale_min_x << ' '
                    << vcl_setw(6) << oy + scale_height + scale_min_y << " translate\n"
                    << vcl_setw(9) << scale_x << ' ' << vcl_setw(9) << -scale_y << " scale\n\n"
                    << "/originalCTM matrix currentmatrix def\n";
}

//-----------------------------------------------------------------------------
//: Set ox, oy , iw, ih, iwf, ihf parameters for PostScript file use.
//-----------------------------------------------------------------------------
bool vul_psfile::set_parameters(int sizex,int sizey)
{
  width = sizex;
  height = sizey;
  // avoid division by 0 or other fancy things later on:
  assert (width > 0 && height > 0);

  set_min_max_xy(0,0);
  set_min_max_xy(width,height);
  compute_bounding_box();

  return true;
}

//-----------------------------------------------------------------------------
//: PostScript file header.
//-----------------------------------------------------------------------------
void vul_psfile::postscript_header()
{
  if (header_pos != HEADER_START)
  {
    vcl_cerr << "vul_psfile: Header already set to " << long(header_pos) << '\n';
    return;
  }

  output_filestream
    << "%!PS-Adobe-2.0 EPSF-2.0\n%%Title: " << filename.c_str()
    << "\n%%Creator: vul_psfile\n%%BoundingBox: ";

  header_pos = output_filestream.tellp();
  reset_postscript_header();
}


//-----------------------------------------------------------------------------
//: Reset PostScript header file
//-----------------------------------------------------------------------------
void vul_psfile::reset_postscript_header()
{
  if (printer_paper_orientation == vul_psfile::LANDSCAPE)
    output_filestream
       << vcl_setw(6) << int(pos_iny*PIX2INCH+0.5) << ' '
       << vcl_setw(6) << int(pos_inx*PIX2INCH+0.5) << ' '
       << vcl_setw(6) << int(pos_iny*PIX2INCH+0.5)+ih << ' '
       << vcl_setw(6) << int(pos_inx*PIX2INCH+0.5)+iw << '\n';
  else
    output_filestream
       << vcl_setw(6) << ox << ' '
       << vcl_setw(6) << oy << ' '
       << vcl_setw(6) << ox+iw << ' '
       << vcl_setw(6) << oy+ih << '\n';
  output_filestream << "%%Pages: 1\n%%DocumentFonts:\n%%EndComments\n";
}

//-----------------------------------------------------------------------------
//: Utility program used in point(), line(), ellipse() and circle()
//-----------------------------------------------------------------------------
void vul_psfile::sobj_rgb_params(char const* obj_str, bool filled)
{
  print_graphics_prolog();
  output_filestream
    << "\nBegin %I " << obj_str << "\n2 0 0 [] 0 SetB\n"
    << fg_r << ' ' << fg_g << ' ' << fg_b << " SetCFg\n"
    << bg_r << ' ' << bg_g << ' ' << bg_b << " SetCBg\n"
    << line_width_ << " setlinewidth\n"
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

  output_filestream << int(x1) << ' ' << int(y1) << ' '
                    << int(x2) << ' ' << int(y2) << " Line\nEnd\n";
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
  output_filestream << x << ' ' << y << ' ' << point_size << ' ' << point_size << " Elli\nEnd\n";
}

//-----------------------------------------------------------------------------
//: Add an ellipse to the Postscript file.
//-----------------------------------------------------------------------------
void vul_psfile::ellipse(float x, float y, float a_axis, float b_axis, int angle)
{
  #ifndef PI // should already be defined in math.h - PVR
  #define PI 3.14159265358979323846
  #endif
  const double radsperdeg = PI/180.0;

  set_min_max_xy(int(x+a_axis*vcl_cos(angle*radsperdeg) + 0.5),
                 int(y+a_axis*vcl_sin(angle*radsperdeg) + 0.5) );
  set_min_max_xy(int(x-a_axis*vcl_cos(angle*radsperdeg) + 0.5),
                 int(y-a_axis*vcl_sin(angle*radsperdeg) + 0.5) );
  compute_bounding_box();

  print_graphics_prolog();
  sobj_rgb_params("Ellipse", false);
  if (angle)
    output_filestream << (int)x << ' ' << (int)y << " translate\n"
                      << -angle << " rotate\n0 0 " << (int)a_axis << ' '
                      << (int)b_axis << " Elli\nEnd\n";
  else
    output_filestream << (int)x << ' ' << (int)y << ' '
                      << (int)a_axis << ' ' << (int)b_axis << " Elli\nEnd\n";
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
//: the defined procedure for PostScript script use.
//-----------------------------------------------------------------------------
void vul_psfile::print_graphics_prolog()
{
  if (graphics_prolog_exists)
    return;
  exist_objs = true;
  output_filestream
    << "\n\n%%BeginTargetjrPrologue\n"
    << "/arrowhead {\n"
    << "  0 begin\n"
    << "  transform originalCTM itransform\n"
    << "  /taily exch def\n"
    << "  /tailx exch def\n"
    << "  transform originalCTM itransform\n"
    << "  /tipy exch def\n"
    << "  /tipx exch def\n"
    << "  /dy tipy taily sub def\n"
    << "  /dx tipx tailx sub def\n"
    << "  /angle dx 0 ne dy 0 ne or { dy dx atan } { 90 } ifelse def\n"
    << "  gsave\n"
    << "  originalCTM setmatrix\n"
    << "  tipx tipy translate\n"
    << "  angle rotate\n"
    << "  newpath\n"
    << "  arrowHeight neg arrowWidth 2 div moveto\n"
    << "  0 0 lineto\n"
    << "  arrowHeight neg arrowWidth 2 div neg lineto\n"
    << "  patternNone not {\n"
    << "    originalCTM setmatrix\n"
    << "    /padtip arrowHeight 2 exp 0.25 arrowWidth 2 exp mul add sqrt brushWidth mul\n"
    << "    arrowWidth div def\n"
    << "    /padtail brushWidth 2 div def\n"
    << "    tipx tipy translate\n"
    << "    angle rotate\n"
    << "    padtip 0 translate\n"
    << "    arrowHeight padtip add padtail add arrowHeight div dup scale\n"
    << "    arrowheadpath\n"
    << "    ifill\n"
    << "  } if\n"
    << "  brushNone not {\n"
    << "    originalCTM setmatrix\n"
    << "    tipx tipy translate\n"
    << "    angle rotate\n"
    << "    arrowheadpath\n"
    << "    istroke\n"
    << "  } if\n"
    << "  grestore\n"
    << "  end\n"
    << "} dup 0 9 dict put def\n\n"
    << "/arrowheadpath {\n"
    << "  newpath\n"
    << "  arrowHeight neg arrowWidth 2 div moveto\n"
    << "  0 0 lineto\n"
    << "  arrowHeight neg arrowWidth 2 div neg lineto\n"
    << "} def\n\n"
    << "/leftarrow {\n"
    << "  0 begin\n"
    << "  y exch get /taily exch def\n"
    << "  x exch get /tailx exch def\n"
    << "  y exch get /tipy exch def\n"
    << "  x exch get /tipx exch def\n"
    << "  brushLeftArrow { tipx tipy tailx taily arrowhead } if\n"
    << "  end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/rightarrow {\n"
    << "  0 begin\n"
    << "  y exch get /tipy exch def\n"
    << "  x exch get /tipx exch def\n"
    << "  y exch get /taily exch def\n"
    << "  x exch get /tailx exch def\n"
    << "  brushRightArrow { tipx tipy tailx taily arrowhead } if\n"
    << "  end\n"
    << "} dup 0 4 dict put def\n\n"
    << "%%EndTargetjrPrologue\n\n"
    << "/arrowHeight 10 def\n"
    << "/arrowWidth 5 def\n\n"
    << "/TargetjrDict 50 dict def\n"
    << "TargetjrDict begin\n\n"
    << "/none null def\n"
    << "/numGraphicParameters 17 def\n"
    << "/stringLimit 65535 def\n\n"
    << "/Begin { save numGraphicParameters dict begin } def\n\n"
    << "/End { end restore } def\n\n"
    << "/SetB { % width leftarrow rightarrow DashArray DashOffset SetB\n"
    << "  dup type /nulltype eq {\n"
    << "    pop\n"
    << "    false /brushRightArrow idef\n"
    << "    false /brushLeftArrow idef\n"
    << "    true /brushNone idef\n"
    << "  } {\n"
    << "    /brushDashOffset idef\n"
    << "    /brushDashArray idef\n"
    << "    0 ne /brushRightArrow idef\n"
    << "    0 ne /brushLeftArrow idef\n"
    << "    /brushWidth idef\n"
    << "    false /brushNone idef\n"
    << "  } ifelse\n"
    << "} def\n\n"
    << "/SetCFg { /fgblue idef /fggreen idef /fgred idef } def\n\n"
    << "/SetCBg { /bgblue idef /bggreen idef /bgred idef } def\n\n"
    << "/SetF { /printSize idef /printFont idef } def\n\n"
    << "/SetP {  % string -1 SetP  OR gray SetP\n"
    << "  dup type /nulltype eq { pop true /patternNone idef }\n"
    << "  {\n"
    << "    dup -1 eq { /patternGrayLevel idef /patternString idef }\n"
    << "    { /patternGrayLevel idef } ifelse\n"
    << "    false /patternNone idef\n"
    << "  } ifelse\n"
    << "} def\n\n"
    << "/BSpl {\n"
    << "  0 begin\n"
    << "  storexyn\n"
    << "  newpath\n"
    << "  n 1 gt {\n"
    << "    0 0 0 0 0 0 1 1 true subspline\n"
    << "    n 2 gt {\n"
    << "      0 0 0 0 1 1 2 2 false subspline\n"
    << "      1 1 n 3 sub { /i exch def i 1 sub dup i dup i 1 add dup i 2 add dup false subspline } for\n"
    << "      n 3 sub dup n 2 sub dup n 1 sub dup 2 copy false subspline\n"
    << "    } if\n"
    << "    n 2 sub dup n 1 sub dup 2 copy 2 copy false subspline\n"
    << "    patternNone not brushLeftArrow not brushRightArrow not and and { ifill } if\n"
    << "    brushNone not { istroke } if\n"
    << "    0 0 1 1 leftarrow\n"
    << "    n 2 sub dup n 1 sub dup rightarrow\n"
    << "  } if\n"
    << "  end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/Circ { newpath 0 360 arc patternNone not { ifill } if brushNone not { istroke } if } def\n\n"
    << "/CBSpl {\n"
    << "  0 begin\n"
    << "  dup 2 gt {\n"
    << "    storexyn\n"
    << "    newpath\n"
    << "    n 1 sub dup 0 0 1 1 2 2 true subspline\n"
    << "    1 1 n 3 sub { /i exch def i 1 sub dup i dup i 1 add dup i 2 add dup false subspline } for\n"
    << "    n 3 sub dup n 2 sub dup n 1 sub dup 0 0 false subspline\n"
    << "    n 2 sub dup n 1 sub dup 0 0 1 1 false subspline\n"
    << "    patternNone not { ifill } if\n"
    << "    brushNone not { istroke } if\n"
    << "  } { Poly } ifelse\n"
    << "  end\n"
    << "} dup 0 4 dict put def\n"
    << "/Elli {\n"
    << "  0 begin\n"
    << "  newpath 4 2 roll translate scale\n"
    << "  0 0 1 0 360 arc\n"
    << "  patternNone not { ifill } if\n"
    << "  brushNone not { istroke } if\n"
    << "  end\n"
    << "} dup 0 1 dict put def\n\n"
    << "/Line {\n"
    << "  0 begin\n"
    << "  2 storexyn\n"
    << "  newpath\n"
    << "  x 0 get y 0 get moveto\n"
    << "  x 1 get y 1 get lineto\n"
    << "  brushNone not { istroke } if\n"
    << "  0 0 1 1 leftarrow\n"
    << "  0 0 1 1 rightarrow\n"
    << "  end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/MLine {\n"
    << "  0 begin\n"
    << "  storexyn\n"
    << "  newpath\n"
    << "  n 1 gt {\n"
    << "    x 0 get y 0 get moveto\n"
    << "    1 1 n 1 sub { /i exch def x i get y i get lineto } for\n"
    << "    patternNone not brushLeftArrow not brushRightArrow not and and { ifill } if\n"
    << "    brushNone not { istroke } if\n"
    << "    0 0 1 1 leftarrow\n"
    << "    n 2 sub dup n 1 sub dup rightarrow\n"
    << "  } if\n"
    << "  end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/Poly {\n"
    << "  3 1 roll\n"
    << "  newpath moveto -1 add { lineto } repeat closepath\n"
    << "  patternNone not { ifill } if\n"
    << "  brushNone not { istroke } if\n"
    << "} def\n\n"
    << "/Rect {\n"
    << "  0 begin\n"
    << "  /t exch def\n"
    << "  /r exch def\n"
    << "  /b exch def\n"
    << "  /l exch def\n"
    << "  newpath\n"
    << "  l b moveto\n"
    << "  l t lineto\n"
    << "  r t lineto\n"
    << "  r b lineto\n"
    << "  closepath\n"
    << "  patternNone not { ifill } if\n"
    << "  brushNone not { istroke } if\n"
    << "  end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/Text { ishow } def\n\n"
    << "/idef { dup where { pop pop pop } { exch def } ifelse } def\n\n"
    << "/ifill {\n"
    << "  0 begin\n"
    << "  gsave\n"
    << "  patternGrayLevel -1 ne {\n"
    << "    fgred bgred fgred sub patternGrayLevel mul add\n"
    << "    fggreen bggreen fggreen sub patternGrayLevel mul add\n"
    << "    fgblue bgblue fgblue sub patternGrayLevel mul add setrgbcolor\n"
    << "    eofill\n"
    << "  } {\n"
    << "    eoclip\n"
    << "    originalCTM setmatrix\n"
    << "    pathbbox /t exch def /r exch def /b exch def /l exch def\n"
    << "    /w r l sub ceiling cvi def\n"
    << "    /h t b sub ceiling cvi def\n"
    << "    /imageByteWidth w 8 div ceiling cvi def\n"
    << "    /imageHeight h def\n"
    << "    bgred bggreen bgblue setrgbcolor\n"
    << "    eofill\n"
    << "    fgred fggreen fgblue setrgbcolor\n"
    << "    w 0 gt h 0 gt and { l w add b translate w neg h scale w h true [w 0 0 h neg 0 h] { patternproc } imagemask } if\n"
    << "  } ifelse\n"
    << "  grestore\n"
    << "  end\n"
    << "} dup 0 8 dict put def\n\n"
    << "/istroke {\n"
    << "  gsave\n"
    << "  brushDashOffset -1 eq { [] 0 setdash 1 setgray }\n"
    << "  { brushDashArray brushDashOffset setdash fgred fggreen fgblue setrgbcolor } ifelse\n"
    << "  originalCTM setmatrix\n"
    << "  stroke\n"
    << "  grestore\n"
    << "} def\n\n"
    << "/ishow {\n"
    << "  0 begin\n"
    << "  gsave\n"
    << "  fgred fggreen fgblue setrgbcolor\n"
    << "  /fontDict printFont printSize scalefont dup setfont def\n"
    << "  /descender fontDict begin 0 [FontBBox] 1 get FontMatrix end\n"
    << "  transform exch pop def\n"
    << "  /vertoffset 1 printSize sub descender sub def\n"
    << "  { 0 vertoffset moveto show /vertoffset vertoffset printSize sub def } forall\n"
    << "  grestore\n"
    << "  end\n"
    << "} dup 0 3 dict put def\n"
    << "/patternproc {\n"
    << "  0 begin\n"
    << "  /patternByteLength patternString length def\n"
    << "  /patternHeight patternByteLength 8 mul sqrt cvi def\n"
    << "  /patternWidth patternHeight def\n"
    << "  /patternByteWidth patternWidth 8 idiv def\n"
    << "  /imageByteMaxLength imageByteWidth imageHeight mul\n"
    << "  stringLimit patternByteWidth sub min def\n"
    << "  /imageMaxHeight imageByteMaxLength imageByteWidth idiv patternHeight idiv\n"
    << "  patternHeight mul patternHeight max def\n"
    << "  /imageHeight imageHeight imageMaxHeight sub store\n"
    << "  /imageString imageByteWidth imageMaxHeight mul patternByteWidth add string def\n"
    << "  0 1 imageMaxHeight 1 sub {\n"
    << "    /y exch def\n"
    << "    /patternRow y patternByteWidth mul patternByteLength mod def\n"
    << "    /patternRowString patternString patternRow patternByteWidth getinterval def\n"
    << "    /imageRow y imageByteWidth mul def\n"
    << "    0 patternByteWidth imageByteWidth 1 sub { /x exch def imageString imageRow x add patternRowString putinterval } for\n"
    << "  } for\n"
    << "  imageString\n"
    << "  end\n"
    << "} dup 0 12 dict put def\n\n"
    << "/min { dup 3 2 roll dup 4 3 roll lt { exch } if pop } def\n\n"
    << "/max { dup 3 2 roll dup 4 3 roll gt { exch } if pop } def\n\n"
    << "/midpoint {\n"
    << "  0 begin\n"
    << "  /y1 exch def\n"
    << "  /x1 exch def\n"
    << "  /y0 exch def\n"
    << "  /x0 exch def\n"
    << "  x0 x1 add 2 div\n"
    << "  y0 y1 add 2 div\n"
    << "  end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/thirdpoint {\n"
    << "  0 begin\n"
    << "  /y1 exch def\n"
    << "  /x1 exch def\n"
    << "  /y0 exch def\n"
    << "  /x0 exch def\n"
    << "  x0 2 mul x1 add 3 div\n"
    << "  y0 2 mul y1 add 3 div\n"
    << "  end\n"
    << "} dup 0 4 dict put def\n\n"
    << "/subspline {\n"
    << "  0 begin\n"
    << "  /movetoNeeded exch def\n"
    << "  y exch get /y3 exch def\n"
    << "  x exch get /x3 exch def\n"
    << "  y exch get /y2 exch def\n"
    << "  x exch get /x2 exch def\n"
    << "  y exch get /y1 exch def\n"
    << "  x exch get /x1 exch def\n"
    << "  y exch get /y0 exch def\n"
    << "  x exch get /x0 exch def\n"
    << "  x1 y1 x2 y2 thirdpoint\n"
    << "  /p1y exch def\n"
    << "  /p1x exch def\n"
    << "  x2 y2 x1 y1 thirdpoint\n"
    << "  /p2y exch def\n"
    << "  /p2x exch def\n"
    << "  x1 y1 x0 y0 thirdpoint\n"
    << "  p1x p1y midpoint\n"
    << "  /p0y exch def\n"
    << "  /p0x exch def\n"
    << "  x2 y2 x3 y3 thirdpoint\n"
    << "  p2x p2y midpoint\n"
    << "  /p3y exch def\n"
    << "  /p3x exch def\n"
    << "  movetoNeeded { p0x p0y moveto } if\n"
    << "  p1x p1y p2x p2y p3x p3y curveto\n"
    << "  end\n"
    << "} dup 0 17 dict put def\n\n"
    << "/storexyn {\n"
    << "  /n exch def\n"
    << "  /y n array def\n"
    << "  /x n array def\n"
    << "  n 1 sub -1 0 { /i exch def y i 3 2 roll put x i 3 2 roll put } for\n"
    << "} def\n\n"
    << "/SSten { fgred fggreen fgblue setrgbcolor dup true exch 1 0 0 -1 0 6 -1 roll matrix astore } def\n\n"
    << "/FSten {\n"
    << "  dup 3 -1 roll dup 4 1 roll exch\n"
    << "  newpath\n"
    << "  0 0 moveto\n"
    << "  dup 0 exch lineto\n"
    << "  exch dup 3 1 roll exch lineto\n"
    << "  0 lineto\n"
    << "  closepath\n"
    << "  bgred bggreen bgblue setrgbcolor\n"
    << "  eofill\n"
    << "  SSten\n"
    << "} def\n\n"
    << "/Rast { exch dup 3 1 roll 1 0 0 -1 0 6 -1 roll matrix astore } def\n\n";

  // For scale and translate ..
  graphic_header();

  graphics_prolog_exists = true;
}

void vul_psfile::done()
{
  if (debug) vcl_cout << "vul_psfile::done\n";
  doneps = true;
  if (graphics_prolog_exists)
    output_filestream << "end % TargetjrDict\n";

  output_filestream << "showpage\n%%Trailer\n";
}
