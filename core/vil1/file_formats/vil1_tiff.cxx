// This is core/vil1/file_formats/vil1_tiff.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief See vil1_tiff.h for a description of this file.
// \author  awf@robots.ox.ac.uk
//
// \verbatim
//  Modifications
//   09-NOV-2001  K.Y.McGaul  Use default value for orientation when it can't be read.
// \endverbatim

#include "vil1_tiff.h"

#include <vcl_cassert.h>
#if 0 // commented out
#include <vcl_cstdio.h> // sprintf
#endif
#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vil1/vil1_stream.h>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_property.h>

#include <tiffio.h>

// Constants
char const* vil1_tiff_format_tag = "tiff";

// Functions
static bool xxproblem(char const* linefile, char const* msg)
{
  vcl_cerr << linefile << "[PROBLEM " <<msg << ']';
  return false;
}
#define xproblem(x, l) xxproblem(__FILE__ ":" #l ":", x)
#define yxproblem(x, l) xproblem(x, l)
#define problem(x) yxproblem(x, __LINE__)

#define trace if (true) { } else vcl_cerr

bool vil1_tiff_file_format_probe(vil1_stream* is)
{
  // The byte ordering in a TIFF image (usually) depends on the byte-order
  // of the writing host. The header is always 4 bytes.
#if 0
  short int hdr[2];
  int read = is->read(hdr, sizeof hdr);
  if (read < 2)
    return 0;
  // First word specifies the file byte-order (0x4D4D=big, 0x4949=little)
  if (hdr[0]!=0x4D4D && hdr[0]!=0x4949)
    return 0;
  // Second word specifies the TIFF version (we expect v42 for some reason?)
  if (hdr[1]!=0x002A && hdr[1]!=0x2A00)
    return 0;
#else
  char hdr[4];
  unsigned int read = is->read(hdr, sizeof hdr);
  if (read < sizeof hdr)
    return false;

  // First two bytes specify the file byte-order (0x4D4D=big, 0x4949=little).
  // Second two bytes specify the TIFF version (we expect 0x2A for some reason?).
  // So,
  //   0x4D 0x4D 0x2A 0x00
  // and
  //   0x49 0x49 0x00 0x2A
  // are invalid TIFF headers.
  if      (hdr[0]==0x4D && hdr[1]==0x4D &&
           hdr[2]==0x00 && hdr[3]==0x2A)
    return true;

  else if (hdr[0]==0x49 && hdr[1]==0x49 &&
           hdr[2]==0x2A && hdr[3]==0x00)
    return true;

  else if ( ((hdr[0]==0x4D && hdr[1]==0x4D) || (hdr[1]==0x49 && hdr[1]==0x49)) &&
            ((hdr[2]==0x00 && hdr[3]==0x2A) || (hdr[2]==0x2A && hdr[3]==0x00)) ) {
    vcl_cerr << __FILE__ ": suspicious TIFF header\n";
    return true; // allow it.
  }

  else
    return false;
#endif
}

vil1_image_impl* vil1_tiff_file_format::make_input_image(vil1_stream* is)
{
  if (!vil1_tiff_file_format_probe(is))
    return 0;

  return new vil1_tiff_generic_image(is);
}

vil1_image_impl* vil1_tiff_file_format::make_output_image(vil1_stream* is, int planes,
                                                          int width,
                                                          int height,
                                                          int components,
                                                          int bits_per_component,
                                                          vil1_component_format format)
{
  return new vil1_tiff_generic_image(is, planes, width, height, components, bits_per_component, format);
}

char const* vil1_tiff_file_format::tag() const
{
  return vil1_tiff_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

struct vil1_tiff_structures {
  vil1_tiff_structures(vil1_stream *vs_):
    vs(vs_),
    filesize(0),
    buf(0)
    { if (vs) vs->ref(); }
  ~vil1_tiff_structures() {
    delete [] buf;
    if (vs) vs->unref();
  }

  TIFF* tif;
  vil1_stream* vs;
  int filesize;

  unsigned long tilewidth;
  unsigned long tileheight;
  unsigned short compression;
  unsigned long rows_per_strip;
  unsigned short planar_config;
  unsigned short photometric;

  unsigned long stripsize;
  unsigned long scanlinesize;
  unsigned long numberofstrips;

  bool tiled;
  bool compressed;
  bool jumbo_strips;

  unsigned char* buf;
};

#if 0 // commented out
typedef tsize_t (*TIFFReadWriteProc)(thandle_t, tdata_t, tsize_t);
typedef toff_t (*TIFFSeekProc)(thandle_t, toff_t, int);
typedef int (*TIFFCloseProc)(thandle_t);
typedef toff_t (*TIFFSizeProc)(thandle_t);
typedef int (*TIFFMapFileProc)(thandle_t, tdata_t*, toff_t*);
typedef void (*TIFFUnmapFileProc)(thandle_t, tdata_t, toff_t);

TIFF* TIFFClientOpen(const char* filename, const char* mode, thandle_t clientdata,
                     TIFFReadWriteProc readproc, TIFFReadWriteProc writeproc, TIFFSeekProc seekproc,
                     TIFFCloseProc closeproc, TIFFSizeProc sizeproc, TIFFMapFileProc mapproc,
                     TIFFUnmapFileProc unmapproc)
#endif

static tsize_t vil1_tiff_readproc(thandle_t h, tdata_t buf, tsize_t n)
{
  vil1_tiff_structures* p = (vil1_tiff_structures*)h;
  if (n > p->filesize) p->filesize= n;
  tsize_t ret = p->vs->read(buf, n);
  trace << "readproc, n = " << n << ", ret = " << ret << '\n';
  return ret;
}

static tsize_t vil1_tiff_writeproc(thandle_t h, tdata_t buf, tsize_t n)
{
  vil1_tiff_structures* p = (vil1_tiff_structures*)h;
  tsize_t ret = p->vs->write(buf, n);
  vil1_streampos s = p->vs->tell();
  if (s > p->filesize)
    p->filesize = s;
  trace << "writeproc: ret=" << ret << '/' << n << " , filesize = " << p->filesize << "   " << s << vcl_endl;
  return ret;
}

static toff_t vil1_tiff_seekproc(thandle_t h, toff_t offset, int whence)
{
  trace << "seek " << offset << " w = " << whence << vcl_endl;
  vil1_tiff_structures* p = (vil1_tiff_structures*)h;
  if (whence == SEEK_SET) {
    p->vs->seek(offset);
  } else if (whence == SEEK_CUR) {
    p->vs->seek(p->vs->tell() + offset);
  } else if (whence == SEEK_END) {
    p->vs->seek(p->filesize + offset);
  }
  vil1_streampos s = p->vs->tell();
  if (s > p->filesize)
    p->filesize = s;
  return s;
}

static int vil1_tiff_closeproc(thandle_t h)
{
  trace << "vil1_tiff_closeproc\n";
  vil1_tiff_structures* p = (vil1_tiff_structures*)h;
  //delete p->vs;
  if (p->vs) {
    p->vs->unref();
    p->vs = 0;
  }
  return 0;
}

static toff_t vil1_tiff_sizeproc(thandle_t)
{
  trace << "vil1_tiff_sizeproc\n";
  // TODO
  return (toff_t)(-1); // could be unsigned - avoid compiler warning
}

static int vil1_tiff_mapfileproc(thandle_t, tdata_t*, toff_t*)
{
  // TODO: Add mmap support to vil1_tiff_mapfileproc
  return 0;
}

static void vil1_tiff_unmapfileproc(thandle_t, tdata_t, toff_t)
{
}

/////////////////////////////////////////////////////////////////////////////

vil1_tiff_generic_image::vil1_tiff_generic_image(vil1_stream* is):
  p(new vil1_tiff_structures(is))
{
  read_header();
}

bool vil1_tiff_generic_image::get_property(char const *tag, void *prop) const
{
  unsigned short orientation;
  int orientation_val_ok = TIFFGetField(p->tif, TIFFTAG_ORIENTATION, &orientation);
  if (orientation_val_ok != 1)
  {
    // kym - apparently most products ignore this orientation tag and use the default
    // value of 1 (eg Adobe Photoshop) so its debatable whether its worth reading at all.
    orientation = 1;
  }

  bool topdown = (orientation==ORIENTATION_TOPLEFT ||
                  orientation==ORIENTATION_TOPRIGHT ||
                  orientation==ORIENTATION_LEFTTOP ||
                  orientation==ORIENTATION_RIGHTTOP);
  bool leftright=(orientation==ORIENTATION_TOPLEFT ||
                  orientation==ORIENTATION_BOTLEFT ||
                  orientation==ORIENTATION_LEFTTOP ||
                  orientation==ORIENTATION_LEFTBOT);

  if (0==vcl_strcmp(tag, vil1_property_top_row_first))
    return prop ? (*(bool*)prop) = topdown, true : true;

  if (0==vcl_strcmp(tag, vil1_property_left_first))
    return prop ? (*(bool*)prop) = leftright, true : true;

  return false;
}

bool vil1_tiff_generic_image::set_property(char const *tag, const void *prop) const
{
  bool topdown; get_property(vil1_property_top_row_first, &topdown);
  bool leftright; get_property(vil1_property_left_first, &leftright);
  const bool newprop = prop ? (*(const bool*)prop) : true; // default is to set the property

  if (0==vcl_strcmp(tag, vil1_property_top_row_first))
  {
    if (topdown == newprop) // no change necessary
      return true;
    unsigned short orientation =
      ( topdown &&  leftright) ? ORIENTATION_TOPLEFT :
      (!topdown &&  leftright) ? ORIENTATION_BOTLEFT :
      ( topdown && !leftright) ? ORIENTATION_TOPRIGHT :
                                 ORIENTATION_BOTRIGHT;
    TIFFSetField(p->tif, TIFFTAG_ORIENTATION, orientation);
    return true;
  }

  else if (0==vcl_strcmp(tag, vil1_property_left_first))
  {
    if (leftright == newprop) // no change necessary
      return true;
    unsigned short orientation =
      ( topdown &&  leftright) ? ORIENTATION_TOPLEFT :
      (!topdown &&  leftright) ? ORIENTATION_BOTLEFT :
      ( topdown && !leftright) ? ORIENTATION_TOPRIGHT :
                                 ORIENTATION_BOTRIGHT;
    TIFFSetField(p->tif, TIFFTAG_ORIENTATION, orientation);
    return true;
  }

  else
    return false;
}

vil1_tiff_generic_image::vil1_tiff_generic_image(vil1_stream* is, int /*planes*/,
                                                 int width,
                                                 int height,
                                                 int components,
                                                 int bits_per_component,
                                                 vil1_component_format /*format*/):
  p(new vil1_tiff_structures(is))
{
  width_ = width;
  height_ = height;
  components_ = components;
  bits_per_component_ = bits_per_component;

  // vcl_cerr << "\n\n ***\n";
  write_header();
}

vil1_tiff_generic_image::~vil1_tiff_generic_image()
{
  if (p->tif)
    TIFFClose(p->tif);
  delete p;
}

char const* vil1_tiff_generic_image::file_format() const
{
  return vil1_tiff_format_tag;
}

bool vil1_tiff_generic_image::read_header()
{
  p->vs->seek(0L);
  p->tif = TIFFClientOpen("unknown filename",
                          "rC", // read, enable strip chopping
                          (thandle_t)p,
                          vil1_tiff_readproc, vil1_tiff_writeproc,
                          vil1_tiff_seekproc, vil1_tiff_closeproc,
                          vil1_tiff_sizeproc,
                          vil1_tiff_mapfileproc, vil1_tiff_unmapfileproc);

  if (!p->tif) {
    return problem("TIFFClientOpen");
  }

#if defined(RIH_DEBUG)
  TIFFPrintDirectory(p->tif, stderr);
#endif

  unsigned short bitspersample;
  TIFFGetField(p->tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);

  unsigned short samplesperpixel;
  // assume one sample per pixel if not in tiff file
  if (!TIFFGetField(p->tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel))
    samplesperpixel = 1;

  switch (samplesperpixel) {
  case 1:
  case 3:
    this->components_ = samplesperpixel;
    this->bits_per_component_ = bitspersample;
    break;
  case 16256:
    this->components_ = 1;
    this->bits_per_component_ = bitspersample;
    break;
  default:
    // vcl_cerr << "vil1_tiff: Saw " << samplesperpixel << " samples @ " << bitspersample << '\n';
    TIFFError("TIFFImageRH: ", "Can only handle 1-channel gray scale or 3-channel color");
    return false;
  }

  unsigned long width;
  TIFFGetField(p->tif, TIFFTAG_IMAGEWIDTH, &width);
  this->width_ = width;

  unsigned long height;
  TIFFGetField(p->tif, TIFFTAG_IMAGELENGTH, &height);
  this->height_ = height;

  if (TIFFIsTiled(p->tif)){
    p->tiled = true;
    TIFFGetField(p->tif, TIFFTAG_TILEWIDTH, &p->tilewidth);
    TIFFGetField(p->tif, TIFFTAG_TILELENGTH, &p->tileheight);
  }
  else {
    p->tiled = false;
    p->tilewidth = 0;
    p->tileheight = 0;
  }

  // int ncolors = (1 << bitspersample);

  TIFFGetField(p->tif, TIFFTAG_PHOTOMETRIC, &p->photometric);
  switch (p->photometric)
  {
   case PHOTOMETRIC_RGB:
    if (!TIFFIsTiled(p->tif)) {
      // section_tiff_image = new ForeignImage(GetDescription(), 'r', GetSizeX(), GetSizeY(), GetBitsPixel(), 8);
#ifdef RIH_DEBUG
      vcl_cerr << "vil1_tiff: Treating Tiff image as uncompressed ForeignImage\n";
#endif
    }
    break;
   case PHOTOMETRIC_MINISBLACK:
    if (!TIFFIsTiled(p->tif))
    {
      // section_tiff_image = new ForeignImage(GetDescription(), 'r', GetSizeX(), GetSizeY(), GetBitsPixel(), 8);
#ifdef RIH_DEBUG
      vcl_cerr << "Treating Tiff image as uncompressed ForeignImage\n";
#endif
    }

#if 0 // commented out
    SetColorNum(0);
    SetBandOrder("IMAGE", 0);
    SetBandOrder("RED",   0);
    SetBandOrder("GREEN", 0);
    SetBandOrder("BLUE",  0);
#endif
    break;
   case PHOTOMETRIC_MINISWHITE:
#if 0 // commented out
     // invert colormap
     int** cm = new int*[3];
     cm[0] = new int[ncolors];
     cm[1] = new int[ncolors];
     cm[2] = new int[ncolors];

     for (int i=0; i < ncolors; i++) {
       cm[0][i] = (ncolors-1)-i;
       cm[1][i] = (ncolors-1)-i;
       cm[2][i] = (ncolors-1)-i;
     }
     SetColorMap(cm);
     SetColorNum(ncolors);

     SetBandOrder("IMAGE", 0);
     SetBandOrder("RED",   0);
     SetBandOrder("GREEN", 0);
     SetBandOrder("BLUE",  0);
#endif
    break;
   case PHOTOMETRIC_PALETTE:
#if 0 // commented out
    int** cm = new int*[3];
    cm[0] = new int[ncolors];
    cm[1] = new int[ncolors];
    cm[2] = new int[ncolors];

    unsigned short *redcolormap,*bluecolormap,*greencolormap;
    TIFFGetField(p->tif, TIFFTAG_COLORMAP,
                 &redcolormap, &greencolormap, &bluecolormap);

    for (int i=0; i < ncolors; i++) {
      cm[0][i] = (int) CVT(redcolormap[i]);
      cm[1][i] = (int) CVT(greencolormap[i]);
      cm[2][i] = (int) CVT(bluecolormap[i]);
    }
    // make sure ncolors is set first or set cm will fail!
    SetColorNum(ncolors);
    SetColorMap(cm);


    SetBandOrder("IMAGE", 0);

    ColorList* cl = GetColorList();
    ArrayMapping* map = cl->GenerateRedMapping();
    SetBandOrder("RED", 0, map);

    cl->GenerateGreenMapping(map);
    SetBandOrder("GREEN", 0, map);

    cl->GenerateBlueMapping(map);
    SetBandOrder("BLUE", 0, map);
    delete map;
#endif
    break;
   default:
    TIFFError("TIFFImageRH: ",
              "Can not handle image with PhotometricInterpretation=%d",
              p->photometric);
    return false;
  }

  TIFFGetField(p->tif, TIFFTAG_ROWSPERSTRIP, &p->rows_per_strip);
  TIFFGetField(p->tif, TIFFTAG_COMPRESSION, &p->compression);
  TIFFGetField(p->tif, TIFFTAG_PLANARCONFIG, &p->planar_config);

  p->compressed = (p->compression != COMPRESSION_NONE);

  p->stripsize = TIFFStripSize(p->tif);
  p->scanlinesize = TIFFScanlineSize(p->tif);
  p->numberofstrips = TIFFNumberOfStrips(p->tif);


#if defined(RIH_DEBUG)
  vcl_printf("vil1_tiff: size %dx%d, components %d of %d bits, tiled %d, compressed %d,"
             " rows per strip %ld, photometric code %d, stripsize %ld, scanlinesize %ld\n",
             this->width(), this->height(), this->components(), this->bits_per_component(),
             p->tiled, p->compressed, p->rows_per_strip, p->photometric, p->stripsize, p->scanlinesize);
#endif

  static const int MB = 1024 * 1024;
  p->jumbo_strips = (!p->compressed && p->stripsize > 2 * MB);

  // Allocate tmp buf
  delete [] p->buf;
  if (p->jumbo_strips)
    // only ever use a scan line
    p->buf = new unsigned char[width_ * components_ * bits_per_component_ / 8];
  else
    p->buf = new unsigned char[p->stripsize];

  return true;
}

bool vil1_tiff_generic_image::write_header()
{
  p->vs->seek(0L);
  p->filesize = 0;

  // TIFF does not support > 8-bit grayscale
  if (bits_per_component_>8 && components_ == 1){
    TIFFError("TIFFImageWH: ", "TIFF6.0 does not support greater than 8-bit grayscale");
    return false;
  }

#if 0 // commented out
        // Strips or Tiles?
        if (GetArea()*GetBytesPixel() > MIN_AREA_FOR_TILING &&
            !(GetBlockSizeX() % 8) && !(GetBlockSizeY() % 8)) {
                // Tiles
                TIFFSetField(p->tif, TIFFTAG_TILEWIDTH, GetBlockSizeX());
                TIFFSetField(p->tif, TIFFTAG_TILELENGTH, GetBlockSizeY());
        } else {
                // Strips
                // Think about setting the strip length
                TIFFSetField(p->tif, TIFFTAG_ROWSPERSTRIP, GetSizeY());
        }
  vcl_cerr << "Warning I no longer know how to create a tiled TIFF Image\n"
           << "but thats ok because I could never do it right anyway...\n";
#endif


  p->tif = TIFFClientOpen("file_formats/vil1_tiff.cxx:374:unknown_filename",
                          "w",
                          (thandle_t)p,
                          vil1_tiff_readproc, vil1_tiff_writeproc,
                          vil1_tiff_seekproc, vil1_tiff_closeproc,
                          vil1_tiff_sizeproc,
                          vil1_tiff_mapfileproc, vil1_tiff_unmapfileproc);

  TIFFSetField(p->tif, TIFFTAG_IMAGEWIDTH, width_);
  TIFFSetField(p->tif, TIFFTAG_IMAGELENGTH, height_);
  TIFFSetField(p->tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

  // hmmm.  single strip image.  good for compression?
  //  p->rows_per_strip = height_;
  p->rows_per_strip = 1;
  TIFFSetField(p->tif, TIFFTAG_ROWSPERSTRIP, p->rows_per_strip);

  int samplesperpixel = components_;
  TIFFSetField(p->tif, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);

  int bitspersample = bits_per_component_;
  TIFFSetField(p->tif, TIFFTAG_BITSPERSAMPLE, bitspersample);

  p->planar_config = PLANARCONFIG_CONTIG;
  TIFFSetField(p->tif, TIFFTAG_PLANARCONFIG, p->planar_config);

#if 0 // commented out
  int ncolors = GetColorNum();
  if (ncolors && samplesperpixel==1)
  {
     int mapsize = 1<<bitspersample;

     unsigned short *cmap = new unsigned short[mapsize*3];
     int** cm = GetColorMap();
     for (int i=0; i<3; i++)
     {
       register int j = 0;
       for (; j<ncolors; j++) *cmap++ = (unsigned short)SCALE(cm[i][j]);
       for (; j<mapsize; j++) *cmap++ = 0;
     }
     cmap -= mapsize*3;
     TIFFSetField(p->tif, TIFFTAG_COLORMAP, cmap, cmap+mapsize, cmap+2*mapsize);
     delete cmap;

     TIFFSetField(p->tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE);
  }
  else
#endif
  {
    if (components_ == 3)
      p->photometric = PHOTOMETRIC_RGB;
    else
      p->photometric = PHOTOMETRIC_MINISBLACK;
  }
  TIFFSetField(p->tif, TIFFTAG_PHOTOMETRIC, p->photometric);

  // TODO: Choice of compression
  //  p->compression = COMPRESSION_LZW;
  p->compression = COMPRESSION_NONE;
  TIFFSetField(p->tif, TIFFTAG_COMPRESSION, p->compression);
  p->compressed = (p->compression != COMPRESSION_NONE);

  // TIFFSetField(p->tif, TIFFTAG_IMAGEDESCRIPTION, GetDescription());
  TIFFSetField(p->tif, TIFFTAG_SOFTWARE, "VXL core/vil1/file_formats/vil1_tiff.cxx");

  p->numberofstrips = TIFFNumberOfStrips(p->tif);
  p->scanlinesize = width_ * bitspersample * samplesperpixel / 8;
  p->scanlinesize = TIFFScanlineSize(p->tif);
  p->stripsize = p->rows_per_strip * p->scanlinesize;
  p->tiled = false;

  // TODO: fix date
#if 0 // commented out
  vcl_time_t clock;
  struct tm *t_m;
  clock = time(NULL);
  t_m = localtime(&clock);
  char tmp[20];
  char datetime[20];
  strftime(tmp,sizeof(datetime),"%c",t_m);
  vcl_sprintf(datetime,"%19s",tmp);
  TIFFSetField(p->tif, TIFFTAG_DATETIME, datetime);
#endif

  // Allocate tmp buf
  delete [] p->buf;
  p->buf = new unsigned char[p->stripsize];

#ifdef RIH_DEBUG
  vcl_printf("vil1_tiff: size %dx%d, components %d of %d bits, tiled %d, compressed %d,"
             " rows per strip %ld, photometric code %d, stripsize %ld, scanlinesize %ld\n",
             this->width(), this->height(), this->components(), this->bits_per_component(),
             p->tiled, p->compressed, p->rows_per_strip, p->photometric, p->stripsize, p->scanlinesize);
#endif

  return true;
}

void vil1_tiff_generic_image::get_resolution(float& x_res, float& y_res, unsigned short& units) const
{
  TIFFGetField(p->tif, TIFFTAG_XRESOLUTION, &x_res);
  TIFFGetField(p->tif, TIFFTAG_YRESOLUTION, &y_res);
  TIFFGetField(p->tif, TIFFTAG_RESOLUTIONUNIT, &units);
}

void vil1_tiff_generic_image::set_resolution(float x_res, float y_res, unsigned short units)
{
  TIFFSetField(p->tif, TIFFTAG_XRESOLUTION, x_res);
  TIFFSetField(p->tif, TIFFTAG_YRESOLUTION, y_res);
  TIFFSetField(p->tif, TIFFTAG_RESOLUTIONUNIT, units);
}

bool vil1_tiff_generic_image::get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  if (!p->jumbo_strips) {
    if (p->tiled)
      vcl_cerr << "vil1_tiff_generic_image: TILED TIFF: may be wrongly read?\n";

    // Random access only to strips.
    // Get the nearby strips...
    int y1 = (y0 + ys - 1);
    unsigned strip_min = y0 / p->rows_per_strip;
    unsigned strip_max = y1 / p->rows_per_strip;
    assert(strip_max <= p->numberofstrips);
    // Get each strip
    int pixel_bit_size = components_ * bits_per_component_;
    {
      for (unsigned long strip_id = strip_min; strip_id <= strip_max; ++strip_id) {
        TIFFReadEncodedStrip(p->tif, strip_id, p->buf, (tsize_t) -1);
        // Strip contains some rows...
        unsigned long strip_min_row = strip_id * p->rows_per_strip;
        unsigned long strip_max_row = strip_min_row + p->rows_per_strip - 1;

        long ymin = (long)strip_min_row;
        if (ymin < y0) ymin = y0;
        long ymax = (long)strip_max_row;
        if (ymax > y1) ymax = y1;

        // printf("reading strip %d, y  = %d .. %d\n", strip_id, ymin, ymax);
        for (long y = ymin; y <= ymax; ++y) {
          unsigned char* in_row = p->buf + (y - strip_min_row) * p->scanlinesize;
          unsigned char* out_row = (unsigned char*)buf + ((y - y0) * xs * pixel_bit_size + 7) / 8;
          vcl_memcpy(out_row, in_row + (x0 * pixel_bit_size + 7) / 8, (xs * pixel_bit_size + 7) / 8);
        }
      }
    }
    return true;
  } else {
    // not compressed, and jumbo strips. dig it out manually in case ReadScanLine pulls in all the image
    return problem("Jumbo strips, and strip chopping appears to be disabled...");
  }
}

bool vil1_tiff_generic_image::put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  // Random access only to strips.
  // Put the nearby strips...
  int y1 = (y0 + ys - 1);
  unsigned strip_min = y0 / p->rows_per_strip;
  unsigned strip_max = y1 / p->rows_per_strip;
  assert(strip_max <= p->numberofstrips);
  // Put each strip
  int pixel_byte_size = components_ * bits_per_component_ / 8;
  for (unsigned long strip_id = strip_min; strip_id <= strip_max; ++strip_id) {
    // Strip contains some rows...
    unsigned long strip_min_row = strip_id * p->rows_per_strip;
    unsigned long strip_max_row = strip_min_row + p->rows_per_strip - 1;

    long ymin = (long)strip_min_row;
    if (ymin < y0) ymin = y0;
    long ymax = (long)strip_max_row;
    if (ymax > y1) ymax = y1;

    // printf("writing strip %d, y  = %d .. %d\n", strip_id, ymin, ymax);
    for (long y = ymin; y <= ymax; ++y) {
      unsigned char* file_row = p->buf + (y - strip_min_row) * p->scanlinesize;
      const unsigned char* mem_row = (const unsigned char*)buf + (y - y0) * xs * pixel_byte_size;
      vcl_memcpy(file_row + x0 * pixel_byte_size, mem_row, xs * pixel_byte_size);
    }

    TIFFWriteEncodedStrip(p->tif, strip_id, p->buf, (long)(ymax - ymin + 1) * p->scanlinesize);
  }
  return true;
}

vil1_image vil1_tiff_generic_image::get_plane(unsigned int plane) const
{
  assert(plane == 0);
  return const_cast<vil1_tiff_generic_image*>(this);
}
