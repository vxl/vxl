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
#define in_range(a) (a < (1 << 8))
#define Bit4ToChar(a) ((char)((a<=9) ? (a+'0'): (a - 10 + 'a')))

static const double PIX2INCH = 72.0;
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
    scale_x = scale_y = vcl_min(hsx/box_width, hsy/box_height) * PIX2INCH;

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
    << "[" << new_width << " 0 0 -" << new_height << " 0 " << new_height
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
        pixel[0] = Bit4ToChar(high4);
        pixel[1] = Bit4ToChar(low4);
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
    << "\n%%Page: 1 1\n\n% remember original state\n/origstate save def\n\n% build a temporary dictionary\n20 dict begin\n\n% defin"
    << "e string to hold a scanline's worth of data\n/pix " << 3 * new_width << " string def\n\n% define space for color conversion"
    << "s\n/grays " << new_width << " string def  % space for gray scale line\n/npixls 0 def\n/rgbindx 0 def\n\n";

  if (printer_paper_orientation == vul_psfile::LANDSCAPE)
    output_filestream
      << "% print in landscape mode\n90 rotate 0 " << int(-psizey*PIX2INCH) << " translate\n\n";
  output_filestream << "% lower left corner\n";
  translate_pos = output_filestream.tellp();
  image_translate_and_scale();

  output_filestream
    << "\n% define 'colorimage' if it isn't defined\n%   ('colortogray' and 'mergeprocs' come from xwd2ps via xgrab)\n/colorimage w"
    << "here   % do we know about 'colorimage'?\n  { pop }           % yes: pop off the 'dict' returned\n  {                 % no: "
    << " define one\n    /colortogray {  % define an RGB->I function\n      /rgbdata exch store    % call input 'rgbdata'\n      rg"
    << "bdata length 3 idiv\n      /npixls exch store\n      /rgbindx 0 store\n      0 1 npixls 1 sub {\n        grays exch\n      "
    << "  rgbdata rgbindx       get 20 mul    % Red\n        rgbdata rgbindx 1 add get 32 mul    % Green\n        rgbdata rgbindx 2"
    << " add get 12 mul    % Blue\n        add add 64 idiv      % I = .5G + .31R + .18B\n        put\n        /rgbindx rgbindx 3 ad"
    << "d store\n      } for\n      grays 0 npixls getinterval\n    } bind def\n\n    % Utility procedure for colorimage operator."
    << "\n    % This procedure takes two procedures off the\n    % stack and merges them into a single procedure.\n\n    /mergeproc"
    << "s { % def\n      dup length\n      3 -1 roll\n      dup\n      length\n      dup\n      5 1 roll\n      3 -1 roll\n      ad"
    << "d\n      array cvx\n      dup\n      3 -1 roll\n      0 exch\n      putinterval\n      dup\n      4 2 roll\n      putinterv"
    << "al\n    } bind def\n\n    /colorimage { % def\n      pop pop     % remove 'false 3' operands\n      {colortogray} mergeproc"
    << "s\n      image\n    } bind def\n  } ifelse          % end of 'false' case\n\n\n\n" << new_width << ' ' << new_height << " 8"
    << "             % dimensions of data\n[" << new_width << " 0 0 -" << new_height << " 0 " << new_height << "]   % mapping matri"
    << "x\n{currentfile pix readhexstring pop}\nfalse 3 colorimage\n\n";

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
          pixel[0] = Bit4ToChar(high4);
          pixel[1] = Bit4ToChar(low4);
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
//: Set graphic coordiate (translate and rotate to local coordinate).
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
  scale_x = vcl_floor(scale_x * 100.0 + 0.5) * .01;
  scale_y = vcl_floor(scale_y * 100.0 + 0.5) * .01;

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
    vcl_cerr << "vul_psfile: Header already set to " << header_pos << '\n';
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
  output_filestream << "%%Pages: 1\n%%DocumentFonts:\n%%EndComments      \n";
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

  set_min_max_xy(int(x+a_axis*vcl_cos(angle*PI/180.0) + 0.5),
                 int(y+a_axis*vcl_sin(angle*PI/180.0) + 0.5) );
  set_min_max_xy(int(x-a_axis*vcl_cos(angle*PI/180.0) + 0.5),
                 int(y-a_axis*vcl_sin(angle*PI/180.0) + 0.5) );
  compute_bounding_box();

  print_graphics_prolog();
  sobj_rgb_params("Ellipse", false);
  if (angle)
    output_filestream << (int)x << ' ' << (int)y << " translate \n"
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
    << "\n\n%%BeginTargetjrPrologue\n/arrowhead {\n0 begin\ntransform originalCTM itransform\n/taily exch def\n/tailx exch def\ntra"
    << "nsform originalCTM itransform\n/tipy exch def\n/tipx exch def\n/dy tipy taily sub def\n/dx tipx tailx sub def\n/angle dx 0 "
    << "ne dy 0 ne or { dy dx atan } { 90 } ifelse def\ngsave\noriginalCTM setmatrix\ntipx tipy translate\nangle rotate\nnewpath\na"
    << "rrowHeight neg arrowWidth 2 div moveto\n0 0 lineto\narrowHeight neg arrowWidth 2 div neg lineto\npatternNone not {\norigina"
    << "lCTM setmatrix\n/padtip arrowHeight 2 exp 0.25 arrowWidth 2 exp mul add sqrt brushWidth mul\narrowWidth div def\n/padtail b"
    << "rushWidth 2 div def\ntipx tipy translate\nangle rotate\npadtip 0 translate\narrowHeight padtip add padtail add arrowHeight "
    << "div dup scale\narrowheadpath\nifill\n} if\nbrushNone not {\noriginalCTM setmatrix\ntipx tipy translate\nangle rotate\narrow"
    << "headpath\nistroke\n} if\ngrestore\nend\n} dup 0 9 dict put def\n\n/arrowheadpath {\nnewpath\narrowHeight neg arrowWidth 2 d"
    << "iv moveto\n0 0 lineto\narrowHeight neg arrowWidth 2 div neg lineto\n} def\n\n/leftarrow {\n0 begin\ny exch get /taily exch "
    << "def\nx exch get /tailx exch def\ny exch get /tipy exch def\nx exch get /tipx exch def\nbrushLeftArrow { tipx tipy tailx tai"
    << "ly arrowhead } if\nend\n} dup 0 4 dict put def\n\n/rightarrow {\n0 begin\ny exch get /tipy exch def\nx exch get /tipx exch "
    << "def\ny exch get /taily exch def\nx exch get /tailx exch def\nbrushRightArrow { tipx tipy tailx taily arrowhead } if\nend\n}"
    << " dup 0 4 dict put def\n\n%%EndTargetjrPrologue\n\n/arrowHeight 10 def\n/arrowWidth 5 def\n\n/TargetjrDict 50 dict def\nTarg"
    << "etjrDict begin\n\n/none null def\n/numGraphicParameters 17 def\n/stringLimit 65535 def\n\n/Begin {\nsave\nnumGraphicParamet"
    << "ers dict begin\n} def\n\n/End {\nend\nrestore\n} def\n\n/SetB { % width leftarrow rightarrow DashArray DashOffset SetB\ndup"
    << " type /nulltype eq {\npop\nfalse /brushRightArrow idef\nfalse /brushLeftArrow idef\ntrue /brushNone idef\n} {\n/brushDashOf"
    << "fset idef\n/brushDashArray idef\n0 ne /brushRightArrow idef\n0 ne /brushLeftArrow idef\n/brushWidth idef\nfalse /brushNone "
    << "idef\n} ifelse\n} def\n\n/SetCFg {\n/fgblue idef\n/fggreen idef\n/fgred idef\n} def\n\n/SetCBg {\n/bgblue idef\n/bggreen id"
    << "ef\n/bgred idef\n} def\n\n/SetF {\n/printSize idef\n/printFont idef\n} def\n\n/SetP {  % string -1 SetP  OR gray SetP\ndup "
    << "type /nulltype eq {\npop true /patternNone idef\n} {\ndup -1 eq {\n/patternGrayLevel idef\n/patternString idef\n} {\n/patte"
    << "rnGrayLevel idef\n} ifelse\nfalse /patternNone idef\n} ifelse\n} def\n\n/BSpl {\n0 begin\nstorexyn\nnewpath\nn 1 gt {\n0 0 "
    << "0 0 0 0 1 1 true subspline\nn 2 gt {\n0 0 0 0 1 1 2 2 false subspline\n1 1 n 3 sub {\n/i exch def\ni 1 sub dup i dup i 1 ad"
    << "d dup i 2 add dup false subspline\n} for\nn 3 sub dup n 2 sub dup n 1 sub dup 2 copy false subspline\n} if\nn 2 sub dup n 1"
    << " sub dup 2 copy 2 copy false subspline\npatternNone not brushLeftArrow not brushRightArrow not and and { ifill } if\nbrushN"
    << "one not { istroke } if\n0 0 1 1 leftarrow\nn 2 sub dup n 1 sub dup rightarrow\n} if\nend\n} dup 0 4 dict put def\n\n/Circ {"
    << "\nnewpath\n0 360 arc\npatternNone not { ifill } if\nbrushNone not { istroke } if\n} def\n\n/CBSpl {\n0 begin\ndup 2 gt {\ns"
    << "torexyn\nnewpath\nn 1 sub dup 0 0 1 1 2 2 true subspline\n1 1 n 3 sub {\n/i exch def\ni 1 sub dup i dup i 1 add dup i 2 add"
    << " dup false subspline\n} for\nn 3 sub dup n 2 sub dup n 1 sub dup 0 0 false subspline\nn 2 sub dup n 1 sub dup 0 0 1 1 false"
    << " subspline\npatternNone not { ifill } if\nbrushNone not { istroke } if\n} {\nPoly\n} ifelse\nend\n} dup 0 4 dict put def\n/"
    << "Elli {\n0 begin\nnewpath\n4 2 roll\ntranslate\nscale\n0 0 1 0 360 arc\npatternNone not { ifill } if\nbrushNone not { istrok"
    << "e } if\nend\n} dup 0 1 dict put def\n\n/Line {\n0 begin\n2 storexyn\nnewpath\nx 0 get y 0 get moveto\nx 1 get y 1 get linet"
    << "o\nbrushNone not { istroke } if\n0 0 1 1 leftarrow\n0 0 1 1 rightarrow\nend\n} dup 0 4 dict put def\n\n/MLine {\n0 begin\n"
    << "storexyn\nnewpath\nn 1 gt {\nx 0 get y 0 get moveto\n1 1 n 1 sub {\n/i exch def\nx i get y i get lineto\n} for\npatternNon"
    << "e not brushLeftArrow not brushRightArrow not and and { ifill } if\nbrushNone not { istroke } if\n0 0 1 1 leftarrow\nn 2 sub"
    << " dup n 1 sub dup rightarrow\n} if\nend\n} dup 0 4 dict put def\n\n/Poly {\n3 1 roll\nnewpath\nmoveto\n-1 add\n{ lineto } re"
    << "peat\nclosepath\npatternNone not { ifill } if\nbrushNone not { istroke } if\n} def\n\n/Rect {\n0 begin\n/t exch def\n/r exc"
    << "h def\n/b exch def\n/l exch def\nnewpath\nl b moveto\nl t lineto\nr t lineto\nr b lineto\nclosepath\npatternNone not { ifil"
    << "l } if\nbrushNone not { istroke } if\nend\n} dup 0 4 dict put def\n\n/Text {\nishow\n} def\n\n/idef {\ndup where { pop pop "
    << "pop } { exch def } ifelse\n} def\n\n/ifill {\n0 begin\ngsave\npatternGrayLevel -1 ne {\nfgred bgred fgred sub patternGrayLe"
    << "vel mul add\nfggreen bggreen fggreen sub patternGrayLevel mul add\nfgblue bgblue fgblue sub patternGrayLevel mul add setrgb"
    << "color\neofill\n} {\neoclip\noriginalCTM setmatrix\npathbbox /t exch def /r exch def /b exch def /l exch def\n/w r l sub cei"
    << "ling cvi def\n/h t b sub ceiling cvi def\n/imageByteWidth w 8 div ceiling cvi def\n/imageHeight h def\nbgred bggreen bgblue"
    << " setrgbcolor\neofill\nfgred fggreen fgblue setrgbcolor\nw 0 gt h 0 gt and {\nl w add b translate w neg h scale\nw h true [w"
    << " 0 0 h neg 0 h] { patternproc } imagemask\n} if\n} ifelse\ngrestore\nend\n} dup 0 8 dict put def\n\n/istroke {\ngsave\nbrus"
    << "hDashOffset -1 eq {\n[] 0 setdash\n1 setgray\n} {\nbrushDashArray brushDashOffset setdash\nfgred fggreen fgblue setrgbcolor"
    << "\n} ifelse\noriginalCTM setmatrix\nstroke\ngrestore\n} def\n\n/ishow {\n0 begin\ngsave\nfgred fggreen fgblue setrgbcolor\n/"
    << "fontDict printFont printSize scalefont dup setfont def\n/descender fontDict begin 0 [FontBBox] 1 get FontMatrix end\ntransf"
    << "orm exch pop def\n/vertoffset 1 printSize sub descender sub def {\n0 vertoffset moveto show\n/vertoffset vertoffset printSi"
    << "ze sub def\n} forall\ngrestore\nend\n} dup 0 3 dict put def\n/patternproc {\n0 begin\n/patternByteLength patternString leng"
    << "th def\n/patternHeight patternByteLength 8 mul sqrt cvi def\n/patternWidth patternHeight def\n/patternByteWidth patternWidt"
    << "h 8 idiv def\n/imageByteMaxLength imageByteWidth imageHeight mul\nstringLimit patternByteWidth sub min def\n/imageMaxHeight"
    << " imageByteMaxLength imageByteWidth idiv patternHeight idiv\npatternHeight mul patternHeight max def\n/imageHeight imageHeig"
    << "ht imageMaxHeight sub store\n/imageString imageByteWidth imageMaxHeight mul patternByteWidth add string def\n0 1 imageMaxHe"
    << "ight 1 sub {\n/y exch def\n/patternRow y patternByteWidth mul patternByteLength mod def\n/patternRowString patternString pa"
    << "tternRow patternByteWidth getinterval def\n/imageRow y imageByteWidth mul def\n0 patternByteWidth imageByteWidth 1 sub {\n/"
    << "x exch def\nimageString imageRow x add patternRowString putinterval\n} for\n} for\nimageString\nend\n} dup 0 12 dict put de"
    << "f\n\n/min {\ndup 3 2 roll dup 4 3 roll lt { exch } if pop\n} def\n\n/max {\ndup 3 2 roll dup 4 3 roll gt { exch } if pop\n}"
    << " def\n\n/midpoint {\n0 begin\n/y1 exch def\n/x1 exch def\n/y0 exch def\n/x0 exch def\nx0 x1 add 2 div\ny0 y1 add 2 div\nend"
    << "\n} dup 0 4 dict put def\n\n/thirdpoint {\n0 begin\n/y1 exch def\n/x1 exch def\n/y0 exch def\n/x0 exch def\nx0 2 mul x1 add"
    << " 3 div\ny0 2 mul y1 add 3 div\nend\n} dup 0 4 dict put def\n\n/subspline {\n0 begin\n/movetoNeeded exch def\ny exch get /y3"
    << " exch def\nx exch get /x3 exch def\ny exch get /y2 exch def\nx exch get /x2 exch def\ny exch get /y1 exch def\nx exch get /"
    << "x1 exch def\ny exch get /y0 exch def\nx exch get /x0 exch def\nx1 y1 x2 y2 thirdpoint\n/p1y exch def\n/p1x exch def\nx2 y2 "
    << "x1 y1 thirdpoint\n/p2y exch def\n/p2x exch def\nx1 y1 x0 y0 thirdpoint\np1x p1y midpoint\n/p0y exch def\n/p0x exch def\nx2 "
    << "y2 x3 y3 thirdpoint\np2x p2y midpoint\n/p3y exch def\n/p3x exch def\nmovetoNeeded { p0x p0y moveto } if\np1x p1y p2x p2y p3"
    << "x p3y curveto\nend\n} dup 0 17 dict put def\n\n/storexyn {\n/n exch def\n/y n array def\n/x n array def\nn 1 sub -1 0 {\n/i"
    << " exch def\ny i 3 2 roll put\nx i 3 2 roll put\n} for\n} def\n\n/SSten {\nfgred fggreen fgblue setrgbcolor\ndup true exch 1 "
    << "0 0 -1 0 6 -1 roll matrix astore\n} def\n\n/FSten {\ndup 3 -1 roll dup 4 1 roll exch\nnewpath\n0 0 moveto\ndup 0 exch linet"
    << "o\nexch dup 3 1 roll exch lineto\n0 lineto\nclosepath\nbgred bggreen bgblue setrgbcolor\neofill\nSSten\n} def\n\n/Rast {\ne"
    << "xch dup 3 1 roll 1 0 0 -1 0 6 -1 roll matrix astore\n} def\n";

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
