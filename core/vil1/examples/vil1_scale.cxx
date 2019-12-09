// This is core/vil1/examples/vil1_scale.cxx
// Example: scaling.

#include <iostream>
#include <cmath>
#include <cstdlib>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vil1/vil1_new.h"
#include "vil1/vil1_load.h"
#include "vil1/vil1_rgb.h"

void
vil1_scale(const vil1_image & in, int newxsize, int newysize, const vil1_image & out);

int
main(int argc, char ** argv)
{
  if (argc != 6)
  {
    std::cerr << "usage: vil1_scale w h in out format\n";
    return -1;
  }
  int          w = std::stoi(argv[1]);
  int          h = std::stoi(argv[2]);
  char const * input_filename = argv[3];
  char const * output_filename = argv[4];
  char const * output_format = argv[5];

  vil1_image in = vil1_load(input_filename);
  if (!in)
    return -1;

  vil1_image out = vil1_new(output_filename, w, h, in, output_format);

  vil1_scale(in, w, h, out);
  return 0;
}


constexpr int SCALE = 4096;
constexpr int HALFSCALE = 2048;

struct pnmscale
{
  int   rows, cols, format, newformat, newrows, newcols, newpixels;
  int   argn, specxscale, specyscale, specxsize, specysize, specxysize;
  float xscale, yscale;
  long  sxscale, syscale;

  pnmscale() { specxscale = specyscale = specxsize = specysize = specxysize = newpixels = 0; }

  void
  pm_error(char const * msg)
  {
    std::cerr << "vil1_scale: ERROR: " << msg << std::endl;
    std::abort();
  }

  void
  set_xscale(double xscale)
  {
    if (specxscale)
      pm_error("already specified an x scale");
    if (specxsize)
      pm_error("only one of -xsize/-width and -xscale may be specified");
    if (xscale <= 0.0)
      pm_error("x scale must be greater than 0");
    specxscale = 1;
    this->xscale = float(xscale);
  }

  void
  set_yscale(double yscale)
  {
    if (specyscale)
      pm_error("already specified a y scale");
    if (specysize)
      pm_error("only one of -ysize/-height and -yscale may be specified");
    if (yscale <= 0.0)
      pm_error("y scale must be greater than 0");
    specyscale = 1;
    this->yscale = float(yscale);
  }

  void
  set_width(int newcols)
  {
    set_xsize(newcols);
  }
  void
  set_xsize(int newcols)
  {
    if (specxsize)
      pm_error("already specified a width");
    if (specxscale)
      pm_error("only one of -xscale and -xsize/-width may be specified");
    if (newcols <= 0)
      pm_error("new width must be greater than 0");
    specxsize = 1;
    this->newcols = newcols;
  }

  void
  set_height(int newrows)
  {
    set_ysize(newrows);
  }
  void
  set_ysize(int newrows)
  {
    if (specysize)
      pm_error("already specified a height");
    if (specyscale)
      pm_error("only one of -yscale and -ysize/-height may be specified");

    if (newrows <= 0)
      pm_error("new height must be greater than 0");
    specysize = 1;
    this->newrows = newrows;
  }

  void
  set_xysize(int newcols, int newrows)
  {
    if (specxsize || specysize || specxscale || specyscale || newpixels)
      pm_error("can't use -xysize with any other specifiers");

    if (newcols <= 0 || newrows <= 0)
      pm_error("new width and height must be greater than 0");
    specxsize = 1;
    specysize = 1;
    specxysize = 1;
    this->newcols = newcols;
    this->newrows = newrows;
  }

  void
  set_pixels(int newpixels)
  {
    if (specxsize || specysize || specxscale || specyscale)
      pm_error("can't use -pixels with any other specifiers");
    if (newpixels <= 0)
      pm_error("number of pixels must be greater than 0");
    this->newpixels = newpixels;
  }

  void
  set_scale(double scale)
  {
    yscale = xscale = float(scale);
    specxscale = specyscale = 1;
    if (scale <= 0.0)
      pm_error("scale must be greater than 0");
  }

  int        current_inrow;
  vil1_image in;
  int        current_outrow;
  vil1_image out;

  void
  init(const vil1_image & in, const vil1_image & out)
  {
    this->in = in;
    this->out = out;
    current_inrow = 0;
    current_outrow = 0;

    cols = in.width();
    rows = in.height();

    /* Compute all sizes and scales. */
    if (newpixels)
    {
      if (rows * cols <= newpixels)
      {
        newrows = rows;
        newcols = cols;
        xscale = yscale = 1.0;
      }
      else
      {
        xscale = yscale = std::sqrt((float)newpixels / (float)cols / (float)rows);
        specxscale = specyscale = 1;
      }
    }

    if (specxysize)
    {
      if ((float)newcols / (float)newrows > (float)cols / (float)rows)
        specxsize = 0;
      else
        specysize = 0;
    }

    if (specxsize)
      xscale = (float)newcols / (float)cols;
    else if (specxscale)
      newcols = int(cols * xscale + 0.999);

    if (specysize)
      yscale = (float)newrows / (float)rows;
    else if (specyscale)
      newrows = int(rows * yscale + 0.999);
    else
    {
      if (specxsize)
      {
        yscale = xscale;
        newrows = int(rows * yscale + 0.999);
      }
      else
      {
        yscale = 1.0;
        newrows = rows;
      }
    }

    if (!(specxsize || specxscale))
    {
      if (specysize)
      {
        xscale = yscale;
        newcols = int(cols * xscale + 0.999);
      }
      else
      {
        xscale = 1.0;
        newcols = cols;
      }
    }

    sxscale = int(xscale * SCALE);
    syscale = int(yscale * SCALE);
  }
};

template <class T, class longT>
struct pnmscaleT : public pnmscale
{
  void
  go();
  void
  pnm_readpnmrow(T * buf, int cols)
  {
    in.get_section(buf, 0, current_inrow++, cols, 1);
  }
  void
  pnm_writepnmrow(T * tempxelrow, int newcols)
  {
    out.put_section(tempxelrow, 0, current_outrow++, newcols, 1);
  }
};

template <class T, class longT>
void
pnmscaleT<T, longT>::go()
{
  T *  xelrow;
  T *  tempxelrow;
  T *  newxelrow;
  T *  xP;
  T *  nxP;
  int  row, col, needtoreadrow;
  long fracrowtofill, fracrowleft;

  xelrow = new T[cols];
  if (newrows == rows) /* shortcut Y scaling if possible */
    tempxelrow = xelrow;
  else
    tempxelrow = new T[cols];

  auto * gs = new longT[cols];
  int    rowsread = 0;
  fracrowleft = syscale;
  needtoreadrow = 1;
  for (col = 0; col < cols; ++col)
    gs[col] = longT(HALFSCALE);
  fracrowtofill = SCALE;

  newxelrow = new T[newcols];

  for (row = 0; row < newrows; ++row)
  {
    // First scale Y from xelrow into tempxelrow.
    if (newrows == rows) // shortcut Y scaling if possible
    {
      pnm_readpnmrow(xelrow, cols);
    }
    else
    {
      while (fracrowleft < fracrowtofill)
      {
        if (needtoreadrow)
          if (rowsread < rows)
          {
            pnm_readpnmrow(xelrow, cols);
            ++rowsread;
            // needtoreadrow = 0;
          }
        for (col = 0, xP = xelrow; col < cols; ++col, ++xP)
          gs[col] += longT(fracrowleft * (*xP));
        fracrowtofill -= fracrowleft;
        fracrowleft = syscale;
        needtoreadrow = 1;
      }
      // Now fracrowleft is >= fracrowtofill, so we can produce a row.
      if (needtoreadrow)
        if (rowsread < rows)
        {
          pnm_readpnmrow(xelrow, cols);
          ++rowsread;
          needtoreadrow = 0;
        }
      for (col = 0, xP = xelrow, nxP = tempxelrow; col < cols; ++col, ++xP, ++nxP)
      {
        longT g = gs[col] + fracrowtofill * *xP;
        g /= SCALE;

        *nxP = (T)g;
        gs[col] = longT(HALFSCALE);
      }
      fracrowleft -= fracrowtofill;
      if (fracrowleft == 0)
      {
        fracrowleft = syscale;
        needtoreadrow = 1;
      }
      fracrowtofill = SCALE;
    }

    /* Now scale X from tempxelrow into newxelrow and write it out. */
    if (newcols == cols) /* shortcut X scaling if possible */
      pnm_writepnmrow(tempxelrow, newcols);
    else
    {
      longT g;
      long  fraccoltofill, fraccolleft;
      int   needcol;

      nxP = newxelrow;
      fraccoltofill = SCALE;
      g = longT(HALFSCALE);
      needcol = 0;
      for (col = 0, xP = tempxelrow; col < cols; ++col, ++xP)
      {
        fraccolleft = sxscale;
        while (fraccolleft >= fraccoltofill)
        {
          if (needcol)
          {
            ++nxP;
            g = longT(HALFSCALE);
          }
          g += fraccoltofill * *xP;
          g /= SCALE;
          *nxP = (T)g;

          fraccolleft -= fraccoltofill;
          fraccoltofill = SCALE;
          needcol = 1;
        }
        if (fraccolleft > 0)
        {
          if (needcol)
          {
            ++nxP;
            g = longT(HALFSCALE);
            needcol = 0;
          }
          g += fraccolleft * *xP;
          fraccoltofill -= fraccolleft;
        }
      }
      if (fraccoltofill > 0)
      {
        --xP;
        g += fraccoltofill * *xP;
      }
      if (!needcol)
      {
        g /= SCALE;
        *nxP = (T)g;
      }
      pnm_writepnmrow(newxelrow, newcols);
    }
  }
}

template struct pnmscaleT<unsigned char, long>;

template struct pnmscaleT<vil1_rgb<unsigned char>, vil1_rgb<long>>;

void
vil1_scale(const vil1_image & in, int newxsize, int newysize, const vil1_image & out)
{
  pnmscaleT<vil1_rgb<unsigned char>, vil1_rgb<long>> p;
  p.set_xsize(newxsize);
  p.set_ysize(newysize);
  p.init(in, out);
  p.go();
}
