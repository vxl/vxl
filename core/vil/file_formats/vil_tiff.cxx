// This is core/vil/file_formats/vil_tiff.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// See vil_tiff.h for a description of this file.
//
// \author  awf@robots.ox.ac.uk
//
// \verbatim
// Modifications:
//   09-NOV-2001  K.Y.McGaul  Use default value for orientation when it can't be read.
// \endverbatim

#include "vil_tiff.h"

#include <vcl_cassert.h>
#if 0 // commented out
#include <vcl_cstdio.h> // sprintf
#endif
#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vil/vil_stream.h>
#include <vil/vil_property.h>
#include <vil/vil_image_view.h>

#include <tiffio.h>

// Constants
char const* vil_tiff_format_tag = "tiff";

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

bool vil_tiff_file_format_probe(vil_stream* is)
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

vil_image_resource_sptr vil_tiff_file_format::make_input_image(vil_stream* is)
{
  if (!vil_tiff_file_format_probe(is))
    return 0;

  return new vil_tiff_image(is);
}

vil_image_resource_sptr
  vil_tiff_file_format::make_output_image(vil_stream* vs,
                                          unsigned nx,
                                          unsigned ny,
                                          unsigned nplanes,
                                          enum vil_pixel_format format)
{
  if (nplanes==1 && vil_pixel_format_sizeof_components(format)>1)
  {
    vcl_cerr << "ERROR with vil_tiff_file_format::make_output_image():\n"
             << "Can't deal with greyscale images with pixel widths other than 8 bits\n";
    return 0;
  }
  return new vil_tiff_image(vs, nx, ny, nplanes, format);
}

char const* vil_tiff_file_format::tag() const
{
  return vil_tiff_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

struct vil_tiff_structures {
  vil_tiff_structures(vil_stream *vs_):
    vs(vs_),
    filesize(0),
    buf(0)
  { if (vs) vs->ref(); }

 ~vil_tiff_structures()
  {
    delete [] buf;
    if (vs) vs->unref();
  }

  TIFF* tif;
  vil_stream* vs;
  int filesize;

  unsigned long tilewidth;
  unsigned long tileheight;
  unsigned short compression;
  unsigned long rows_per_strip;
  unsigned short planar_config;
  unsigned short photometric;

  unsigned long stripsize;
  long scanlinesize;
  unsigned long numberofstrips;

  bool tiled;
  bool compressed;
  bool jumbo_strips;

  vxl_byte* buf;
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

static tsize_t vil_tiff_readproc(thandle_t h, tdata_t buf, tsize_t n)
{
  vil_tiff_structures* p = (vil_tiff_structures*)h;
  if (n > p->filesize) p->filesize= n;
  tsize_t ret = p->vs->read(buf, n);
  trace << "readproc, n = " << n << ", ret = " << ret << '\n';
  return ret;
}

static tsize_t vil_tiff_writeproc(thandle_t h, tdata_t buf, tsize_t n)
{
  vil_tiff_structures* p = (vil_tiff_structures*)h;
  tsize_t ret = p->vs->write(buf, n);
  vil_streampos s = p->vs->tell();
  if (s > p->filesize)
    p->filesize = s;
  trace << "writeproc: ret=" << ret << '/' << n << " , filesize = " << p->filesize << "   " << s << vcl_endl;
  return ret;
}

static toff_t vil_tiff_seekproc(thandle_t h, toff_t offset, int whence)
{
  trace << "seek " << offset << " w = " << whence << vcl_endl;
  vil_tiff_structures* p = (vil_tiff_structures*)h;
  if      (whence == SEEK_SET) { p->vs->seek(offset); }
  else if (whence == SEEK_CUR) { p->vs->seek(p->vs->tell() + offset); }
  else if (whence == SEEK_END) { p->vs->seek(p->filesize + offset); }
  vil_streampos s = p->vs->tell();
  if (s > p->filesize)
    p->filesize = s;
  return s;
}

static int vil_tiff_closeproc(thandle_t h)
{
  trace << "vil_tiff_closeproc\n";
  vil_tiff_structures* p = (vil_tiff_structures*)h;
  //delete p->vs;
  if (p->vs) {
    p->vs->unref();
    p->vs = 0;
  }
  return 0;
}

static toff_t vil_tiff_sizeproc(thandle_t)
{
  trace << "vil_tiff_sizeproc\n";
  // TODO
  return (toff_t)(-1); // could be unsigned - avoid compiler warning
}

static int vil_tiff_mapfileproc(thandle_t, tdata_t*, toff_t*)
{
  // TODO: Add mmap support to vil_tiff_mapfileproc
  return 0;
}

static void vil_tiff_unmapfileproc(thandle_t, tdata_t, toff_t)
{
}

/////////////////////////////////////////////////////////////////////////////

vil_tiff_image::vil_tiff_image(vil_stream* is):
  p(new vil_tiff_structures(is))
{
  read_header();
}

bool vil_tiff_image::get_property(char const * tag, void * value) const
{
  if (vcl_strcmp(vil_property_quantisation_depth, tag)==0)
  {
    if (value)
      *static_cast<unsigned int*>(value) = bits_per_component_;
    return true;
  }

  return false;
}


vil_tiff_image::vil_tiff_image(vil_stream* is,
                               unsigned ni,
                               unsigned nj,
                               unsigned nplanes,
                               vil_pixel_format format):
  p(new vil_tiff_structures(is))
{
  width_ = ni;
  height_ = nj;
  components_ = nplanes;
  bits_per_component_ = format==VIL_PIXEL_FORMAT_BOOL ? 1 : vil_pixel_format_sizeof_components(format)*8;

  write_header();
}

vil_pixel_format vil_tiff_image::pixel_format() const
{
  if (bits_per_component_ <= 1)
    return VIL_PIXEL_FORMAT_BOOL;
  else if (bits_per_component_ <= 8)
    return VIL_PIXEL_FORMAT_BYTE;
  else if (bits_per_component_ <= 16)
    return VIL_PIXEL_FORMAT_UINT_16;
  else if (bits_per_component_ <= 32)
    return VIL_PIXEL_FORMAT_UINT_32;
  else
    return VIL_PIXEL_FORMAT_UNKNOWN;
}

vil_tiff_image::~vil_tiff_image()
{
  if (p->tif)
    TIFFClose(p->tif);
  delete p;
}

char const* vil_tiff_image::file_format() const
{
  return vil_tiff_format_tag;
}

bool vil_tiff_image::read_header()
{
  p->vs->seek(0L);
  p->tif = TIFFClientOpen("unknown filename",
                          "rC", // read, enable strip chopping
                          (thandle_t)p,
                          vil_tiff_readproc, vil_tiff_writeproc,
                          vil_tiff_seekproc, vil_tiff_closeproc,
                          vil_tiff_sizeproc,
                          vil_tiff_mapfileproc, vil_tiff_unmapfileproc);

  if (!p->tif) {
    return problem("TIFFClientOpen");
  }

#ifdef DEBUG
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
    // vcl_cerr << "vil_tiff: Saw " << samplesperpixel << " samples @ " << bitspersample << '\n';
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
  switch (p->photometric) {
  case PHOTOMETRIC_RGB:
    {
      if (!TIFFIsTiled(p->tif)) {
        // section_tiff_image = new ForeignImage(GetDescription(), 'r', GetSizeX(), GetSizeY(), GetBitsPixel(), 8);
#ifdef DEBUG
        vcl_cerr << "vil_tiff: Treating Tiff image as uncompressed ForeignImage\n";
#endif
      }
    }
    break;
  case PHOTOMETRIC_MINISBLACK:
    {
      if (!TIFFIsTiled(p->tif))
        {
          // section_tiff_image = new ForeignImage(GetDescription(), 'r', GetSizeX(), GetSizeY(), GetBitsPixel(), 8);
#ifdef DEBUG
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
    }
    break;
  case PHOTOMETRIC_MINISWHITE:
    {
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
    }
    break;
  case PHOTOMETRIC_PALETTE:
    {
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
    }
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


#ifdef DEBUG
  vcl_cerr << "vil_tiff: size " << ni() << 'x' << nj() << 'x' << nplanes()
           << " of " << bits_per_component_ << " bits, tiled " << p->tiled
           << ", compressed " << p->compressed
           << ", rows per strip " << p->rows_per_strip
           << ", photometric code " << p->photometric
           << ", stripsize " << p->stripsize
           << ", scanlinesize " << p->scanlinesize << '\n';
#endif

  static const int MB = 1024 * 1024;
  p->jumbo_strips = (!p->compressed && p->stripsize > 2 * MB);

  // Allocate tmp buf
  delete [] p->buf;
  if (p->jumbo_strips)
    // only ever use a scan line
    p->buf = new vxl_byte[(7 + width_ * components_ * bits_per_component_) / 8];
  else
    p->buf = new vxl_byte[p->stripsize];

  return true;
}

bool vil_tiff_image::write_header()
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
            !(GetBlockSizeX() % 8) && !(GetBlockSizeY() % 8)){
                // Tiles
                TIFFSetField(p->tif, TIFFTAG_TILEWIDTH, GetBlockSizeX());
                TIFFSetField(p->tif, TIFFTAG_TILELENGTH, GetBlockSizeY());
        }
        else{
                // Strips
                // Think about setting the strip length
                TIFFSetField(p->tif, TIFFTAG_ROWSPERSTRIP, GetSizeY());
        }
  vcl_cerr << "Warning I no longer know how to create a tiled TIFF Image\n"
           << "but thats ok because I could never do it right anyway...\n";
#endif


  p->tif = TIFFClientOpen("file_formats/vil_tiff.cxx:374:unknown_filename",
                          "w",
                          (thandle_t)p,
                          vil_tiff_readproc, vil_tiff_writeproc,
                          vil_tiff_seekproc, vil_tiff_closeproc,
                          vil_tiff_sizeproc,
                          vil_tiff_mapfileproc, vil_tiff_unmapfileproc);

  TIFFSetField(p->tif, TIFFTAG_IMAGEWIDTH, width_);
  TIFFSetField(p->tif, TIFFTAG_IMAGELENGTH, height_);
  TIFFSetField(p->tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

  // hmmm.  single strip image.  good for compression?
  //  p->rows_per_strip = height_;
  p->rows_per_strip = 1;
  TIFFSetField(p->tif, TIFFTAG_ROWSPERSTRIP, p->rows_per_strip);

  unsigned int samplesperpixel = components_;
  TIFFSetField(p->tif, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);

  unsigned int bitspersample = bits_per_component_;
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
  TIFFSetField(p->tif, TIFFTAG_SOFTWARE, "vxl vil_tiff");

  p->numberofstrips = TIFFNumberOfStrips(p->tif);
  p->scanlinesize = (width_ * bitspersample * samplesperpixel + 7) / 8;
  if (p->scanlinesize != TIFFScanlineSize(p->tif))
    vcl_cerr << "WARNING: vil_tiff_image::write_header() -\n"
             << "\tscan line size is incorrect: "
             << p->scanlinesize << " != " << TIFFScanlineSize(p->tif) << '\n';
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
  p->buf = new vxl_byte[p->stripsize];

#ifdef DEBUG
  vcl_cerr << "vil_tiff: size " << ni() << 'x' << nj() << 'x' << nplanes()
           << " of " << bits_per_component_ << " bits, tiled " << p->tiled
           << ", compressed " << p->compressed
           << ", rows per strip " << p->rows_per_strip
           << ", photometric code " << p->photometric
           << ", stripsize " << p->stripsize
           << ", scanlinesize " << p->scanlinesize << '\n';
#endif

  return true;
}

void vil_tiff_image::get_resolution(float& x_res, float& y_res, unsigned short& units) const
{
  TIFFGetField(p->tif, TIFFTAG_XRESOLUTION, &x_res);
  TIFFGetField(p->tif, TIFFTAG_YRESOLUTION, &y_res);
  TIFFGetField(p->tif, TIFFTAG_RESOLUTIONUNIT, &units);
}

void vil_tiff_image::set_resolution(float x_res, float y_res, unsigned short units)
{
  TIFFSetField(p->tif, TIFFTAG_XRESOLUTION, x_res);
  TIFFSetField(p->tif, TIFFTAG_YRESOLUTION, y_res);
  TIFFSetField(p->tif, TIFFTAG_RESOLUTIONUNIT, units);
}

vil_image_view_base_sptr vil_tiff_image::get_copy_view(unsigned i0,
                                                       unsigned ni,
                                                       unsigned j0,
                                                       unsigned nj) const
{
  if (!p->jumbo_strips)
  {
    if (p->tiled)
      vcl_cerr << "vil_tiff_image: TILED TIFF: may be wrongly read?\n";

    if (components_ > 1 && bits_per_component_%8 != 0)
    {
      vcl_cerr << "vil_tiff_image: Can't deal with bits per component that is not a multiple of 8\n";
      return 0;
    }

    // Random access only to strips.
    // Get the nearby strips...
    int j1 = (j0 + nj - 1);
    unsigned strip_min = j0 / p->rows_per_strip;
    unsigned strip_max = j1 / p->rows_per_strip;
    assert(strip_max <= p->numberofstrips);
    // Get each strip
    unsigned int pixel_bit_size = components_ * bits_per_component_;
    unsigned int pixel_byte_size = components_ * ((bits_per_component_+7)/8);
    vil_memory_chunk_sptr buf = new vil_memory_chunk(pixel_byte_size*nj*ni, pixel_format());
    {
      for (unsigned long strip_id = strip_min; strip_id <= strip_max; ++strip_id) {
        TIFFReadEncodedStrip(p->tif, strip_id, p->buf, (tsize_t) -1);
        // Strip contains some rows...
        unsigned long strip_min_row = strip_id * p->rows_per_strip;
        unsigned long strip_max_row = strip_min_row + p->rows_per_strip - 1;

        unsigned long ymin = strip_min_row;
        if (ymin < j0) ymin = j0;
        unsigned long ymax = strip_max_row;
        if (ymax > (unsigned)j1) ymax = j1;

#ifdef DEBUG
        vcl_cerr << "reading strip " << strip_id << ", y  = " << ymin << " .. " << ymax << '\n';
#endif
        for (unsigned long y = ymin; y <= ymax; ++y)
        {
          vxl_byte* in_row = p->buf + (y - strip_min_row) * p->scanlinesize;
          if (bits_per_component_%8 == 0) {
            vxl_byte* out_row = reinterpret_cast<vxl_byte*>(buf->data()) + ((y - j0) * ni * pixel_bit_size + 7) / 8;
            vcl_memcpy(out_row, in_row + (i0 * pixel_bit_size + 7) / 8, (ni * pixel_bit_size + 7) / 8);
          }
          else
          {
            vxl_byte* out_row = reinterpret_cast<vxl_byte*>(buf->data()) + (y - j0) * ni * pixel_byte_size;
            for (unsigned int x=0, bit=0; x < ni; ++x, bit+=pixel_bit_size)
            {
              // read the correct number of bits from the TIFF file buffer:
              unsigned int in_byte = bit/8;
              // Assuming most-significant-bit-first fill order (which is the default)
              unsigned long out_data = in_row[in_byte] & vxl_byte(0xff >> (bit%8));
              for (unsigned int s=in_byte+1; s<=(bit+pixel_bit_size-1)/8; ++s)
              {
                out_data <<= 8;
                out_data |= in_row[s];
              }
              out_data >>= 7-((bit+pixel_bit_size-1)%8);
              if (bits_per_component_ == 1)
                // assuming min-is-black in case of 1-bit pixels
                *reinterpret_cast<bool*>(out_row+x) = out_data==0;
              else
#if VXL_BIG_ENDIAN
                vcl_memcpy(out_row+x*pixel_byte_size, &out_data, pixel_byte_size);
#else
                vcl_memcpy(out_row+x*pixel_byte_size, &out_data+sizeof(long)-pixel_byte_size, pixel_byte_size);
#endif
            }
          }
        }
      }
    }
    if (bits_per_component_ <= 1) {
      return new vil_image_view<bool>(buf, reinterpret_cast<bool*>(buf->data()), ni, nj,
                                      components_, components_, components_*ni, 1);
    }
    else if (bits_per_component_ <= 8)
      return new vil_image_view<vxl_byte>(buf, reinterpret_cast<vxl_byte*>(buf->data()), ni, nj,
                                          components_, components_, components_*ni, 1);
    else if (bits_per_component_ <= 16)
      return new vil_image_view<vxl_uint_16>(buf, reinterpret_cast<vxl_uint_16*>(buf->data()), ni, nj,
                                             components_, components_, components_*ni, 1);
    else if (bits_per_component_ <= 32)
      return new vil_image_view<vxl_uint_32>(buf, reinterpret_cast<vxl_uint_32*>(buf->data()), ni, nj,
                                             components_, components_, components_*ni, 1);
    else {
      // not compressed, and jumbo strips. dig it out manually in case ReadScanLine pulls in all the image
      problem("Can't deal with this pixel depth.");
      return 0;
    }

  } else {
    // not compressed, and jumbo strips. dig it out manually in case ReadScanLine pulls in all the image
    problem("Jumbo strips, and strip chopping appears to be disabled...");
    return 0;
  }
}

bool vil_tiff_image::put_view(const vil_image_view_base &im,
                              unsigned i0, unsigned j0)
{
  if (im.pixel_format() != pixel_format())
  {
    vcl_cerr << "WARNING: vil_tiff_image::put_view\n"
             << "Failed because input is wrong pixel_type.\n"
             << "Input is " << im.pixel_format()
             << ", tiff_image is " << pixel_format() << vcl_endl;
    return false;
  }
  unsigned int depth = pixel_format()==VIL_PIXEL_FORMAT_BOOL ? 1 : vil_pixel_format_sizeof_components(pixel_format())*8;
  if (bits_per_component_ != depth)
  {
    vcl_cerr << "WARNING: vil_tiff_image::put_view() failed - TIFF image has incorrect component size:\n"
             << "pixel format = " << pixel_format() << ", #bits = " << bits_per_component_ << '\n';
    return false;
  }
  if (im.nplanes() != components_)
  {
    vcl_cerr << "WARNING: vil_tiff_image::put_view\n"
             << "Failed because TIFF image has incorrect component size.\n";
    return false;
  }

  // Random access only to strips.
  // Put the nearby strips...
  int jend = (j0 + nj() - 1);
  unsigned strip_min = j0 / p->rows_per_strip;
  unsigned strip_max = jend / p->rows_per_strip;
  assert(strip_max <= p->numberofstrips);
  // Put each strip
  unsigned int pixel_byte_size = (7 + components_ * bits_per_component_ ) / 8;
  for (unsigned long strip_id = strip_min; strip_id <= strip_max; ++strip_id)
  {
    // Strip contains some rows...
    unsigned long strip_min_row = strip_id * p->rows_per_strip;
    unsigned long strip_max_row = strip_min_row + p->rows_per_strip - 1;

    unsigned long ymin = strip_min_row;
    if (ymin < j0) ymin = j0;
    unsigned long ymax = strip_max_row;
    if (ymax > (unsigned)jend) ymax = jend;

#ifdef DEBUG
    vcl_cerr << "writing strip " << strip_id << ", y  = " << ymin << " .. " << ymax << '\n';
#endif
    for (unsigned long y = ymin; y <= ymax; ++y) {
      vxl_byte* file_row = p->buf + (y - strip_min_row) * p->scanlinesize;

      if (im.pixel_format() == VIL_PIXEL_FORMAT_BOOL && im.nplanes() == 1)
      {
        // fill one raster with data.
        for (unsigned i=0; i<im.ni(); ++i)
          // Assuming most-significant-bit-first fill order (which is the default)
          // and assuming min-is-black pixels
          if (static_cast<const vil_image_view<bool>&>(im)(i,y,0))
            file_row[(i+i0)/8] &= vxl_byte(~(1<<(7-((i+i0)%8))));
          else
            file_row[(i+i0)/8] |= vxl_byte(1<<(7-((i+i0)%8)));
      }
      else if (im.pixel_format() == VIL_PIXEL_FORMAT_BYTE)
      {
        // fill one raster with data, component-wise.
        for (unsigned i=0; i<im.ni(); ++i)
          for (unsigned plane=0; plane<im.nplanes(); ++plane)
            file_row[i*pixel_byte_size + plane] =
              static_cast<const vil_image_view<vxl_byte>&>(im)(i,y,plane);
      }
      else if (im.pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
      {
        // fill one raster with data, component-wise.
        for (unsigned i=0; i<im.ni(); ++i)
          for (unsigned plane=0; plane<im.nplanes(); ++plane)
            *reinterpret_cast<vxl_uint_16*>(file_row + i*pixel_byte_size + plane*2) =
              static_cast<const vil_image_view<vxl_uint_16>&>(im)(i,y,plane);
      }
      else if (im.pixel_format() == VIL_PIXEL_FORMAT_UINT_32)
      {
        // fill one raster with data, component-wise.
        for (unsigned i=0; i<im.ni(); ++i)
          for (unsigned plane=0; plane<im.nplanes(); ++plane)
            *reinterpret_cast<vxl_uint_32*>(file_row + i*pixel_byte_size + plane*4) =
              static_cast<const vil_image_view<vxl_uint_32>&>(im)(i,y,plane);
      }
      else
      {
        vcl_cerr << "WARNING: vil_tiff_image::put_view\n"
                 << "Can't deal with this pixel depth.\n";
        return false;
      }
    }
    TIFFWriteEncodedStrip(p->tif, strip_id, p->buf, (long)(ymax - ymin + 1) * p->scanlinesize);
  }
  return true;
}
