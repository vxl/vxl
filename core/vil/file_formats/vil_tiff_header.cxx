#include <iostream>
#include <cstdio>
#include <ctime>
#include "vil_tiff_header.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#if HAS_GEOTIFF
#include <vil/file_formats/vil_geotiff_header.h>
#endif

static std::string date_and_time()
{
  std::time_t clock;
  struct std::tm *t_m;
  clock = std::time(nullptr);
  t_m = std::localtime(&clock);
  char tmp[20];
  char datetime[20];
  std::strftime(tmp,sizeof(datetime),"%Y-%m-%d %H:%M:%S",t_m);
  // changed from "%c", to make it generic, size=19, and avoid compiler warning
  std::sprintf(datetime,"%19s",tmp);
  return std::string(datetime);
}

static void read_string(TIFF* tif, ttag_t tag, std::string& stag, std::string const& deflt = "not_defined")
{
  char* adr = nullptr;
  TIFFGetField(tif, tag, &adr);
  if (adr)
    stag = std::string(adr);
  else
    stag = deflt;
}

static void read_short_tag(TIFF* tif, ttag_t tag, ushort_tag& utag, vxl_uint_16 deflt =0)
{
  utag.valid = TIFFGetField(tif, tag, &(utag.val))>0;
  if (!utag.valid)
    utag.val = deflt;
}

static void read_long_tag(TIFF* tif, ttag_t tag, ulong_tag& utag, vxl_uint_32 deflt = 0)
{
  utag.valid = TIFFGetField(tif, tag, &(utag.val))>0;
  if (!utag.valid)
    utag.val = deflt;
}

static void read_float_tag(TIFF* tif, ttag_t tag, float& val, bool& valid, float deflt = 0)
{
  valid = TIFFGetField(tif, tag, &(val))>0;
  if (!valid)
    val = deflt;
}

#if 0 // unused static function
//assumes array is resized properly
static bool read_long_array(TIFF* tif, ttag_t tag,
                            std::vector<vxl_uint_32>& array)
{
  vxl_uint_32 * a;
  if (TIFFGetField(tif, tag, &a))
  {
    for (vxl_uint_32 i=0; i<array.size(); ++i) { array[i]=a[i]; }
    return true;
  }
  else return false;
}
#endif // unused static function

static void write_short_tag(TIFF* tif, ttag_t tag, ushort_tag const& ustag)
{
  if (ustag.valid)
    TIFFSetField(tif, tag, ustag.val);
}

static void write_long_tag(TIFF* tif, ttag_t tag, ulong_tag const& ultag)
{
  if (ultag.valid)
    TIFFSetField(tif, tag, ultag.val);
}

#if 0 // not currently used...
static void write_float_tag(TIFF* tif, ttag_t tag, float const val, bool const valid)
{
  if (valid)
    TIFFSetField(tif, tag, val);
}
#endif

static void write_string(TIFF* tif, ttag_t tag, std::string const& stag)
{
  TIFFSetField(tif, tag, stag.c_str());
}


bool vil_tiff_header::read_header()
{
  //DEBUG
#ifdef DEBUG
  std::cout << date_and_time() << '\n';
#endif
  //====
  //Determine the endian state of the file and machine
  file_is_big_endian_ = TIFFIsByteSwapped(tif_)>0;

  //also need machine endian
#if VXL_BIG_ENDIAN
    machine_is_big_endian_ = true;
#else
    machine_is_big_endian_ = false;
#endif
  //the following group must be read first since they
  //dictate subsequent tag structures
  ///-----------------------------------------------///
  read_short_tag(tif_,TIFFTAG_PHOTOMETRIC, photometric);
  read_short_tag(tif_,TIFFTAG_PLANARCONFIG, planar_config, 1);
  read_short_tag(tif_,TIFFTAG_SAMPLESPERPIXEL, samples_per_pixel, 1);
  read_short_tag(tif_,TIFFTAG_BITSPERSAMPLE, bits_per_sample, 8);
  is_tiled_flag = TIFFIsTiled(tif_)>0;
  ///-----------------------------------------------///
  read_string(tif_,TIFFTAG_ARTIST , artist);
  read_short_tag(tif_,TIFFTAG_CELLLENGTH, cell_length);
  read_short_tag(tif_,TIFFTAG_CELLWIDTH, cell_width);
  color_map_valid = false;
  if (bits_per_sample.valid &&
      photometric.valid &&
      photometric.val == PHOTOMETRIC_PALETTE)
  {
    vxl_uint_16* cm[3];
    TIFFGetField(tif_,TIFFTAG_COLORMAP, &cm[0], &cm[1], &cm[2]);
    unsigned size = 1<<bits_per_sample.val;
    color_map.resize(size);
    for (unsigned i = 0; i<size; ++i)
    {
      std::vector<vxl_uint_16> rgb(3);
      rgb[0]=cm[0][i];  rgb[1]=cm[1][i];  rgb[2]=cm[2][i];
      color_map[i] = rgb;
#ifdef DEBUG
      std::cout << "RGB[" << i << "]=(" << rgb[0] << ' ' << rgb[1] << ' ' << rgb[2] << ")\n";
#endif
    }
    color_map_valid = true;
  }
  read_short_tag(tif_,TIFFTAG_COMPRESSION, compression);
  read_string(tif_,TIFFTAG_COPYRIGHT, copyright);
  read_string(tif_,TIFFTAG_DATETIME,date_time);

  // EXTRASAMPLES tag requires two input arguments, which is different
  // from other 16bit values.
  vxl_uint_16* sample_info=nullptr;
  extra_samples.val=0;
  extra_samples.valid = false;
  int const ret_extrasamples = TIFFGetField(tif_, TIFFTAG_EXTRASAMPLES, &extra_samples.val, &sample_info);
  if (ret_extrasamples && extra_samples.val > 0)
    extra_samples.valid = true;

  read_short_tag(tif_,TIFFTAG_FILLORDER, fill_order);
  vxl_uint_16* gc=nullptr;
  TIFFGetField(tif_,TIFFTAG_GRAYRESPONSECURVE, &gc);
  read_short_tag(tif_,TIFFTAG_GRAYRESPONSEUNIT, gray_response_unit);
  read_string(tif_,TIFFTAG_HOSTCOMPUTER, host_computer);
  read_string(tif_,TIFFTAG_IMAGEDESCRIPTION, image_description);
  read_long_tag(tif_,TIFFTAG_IMAGELENGTH, image_length);
  read_long_tag(tif_,TIFFTAG_IMAGEWIDTH, image_width);
  read_string(tif_,TIFFTAG_MAKE, make);
  read_short_tag(tif_,TIFFTAG_MAXSAMPLEVALUE, max_sample_value, 255);
  read_short_tag(tif_,TIFFTAG_MINSAMPLEVALUE, min_sample_value, 0);
  read_string(tif_,TIFFTAG_MODEL, model);
  read_short_tag(tif_,TIFFTAG_SUBFILETYPE, subfile_type);
  read_short_tag(tif_,TIFFTAG_ORIENTATION, orientation, 1);
  read_short_tag(tif_,TIFFTAG_RESOLUTIONUNIT, resolution_unit);
  read_long_tag(tif_,TIFFTAG_ROWSPERSTRIP, rows_per_strip);
  read_string(tif_,TIFFTAG_SOFTWARE, software);
  read_short_tag(tif_,TIFFTAG_SAMPLEFORMAT, sample_format, 1);
  strip_byte_counts_valid = false;
  if (rows_per_strip.valid)
  {
    strip_byte_counts_valid =
      TIFFGetField(tif_,TIFFTAG_STRIPBYTECOUNTS , &strip_byte_counts)>0;
#ifdef DEBUG
    //      vxl_uint_32 size = strips_per_image()*samples_per_pixel.val;
    vxl_uint_32 size = strips_per_image();
    for (vxl_uint_32 i = 0; i<size; ++i)
      std::cout << "SBC[" << i << "]=" << strip_byte_counts[i] << '\n';
#endif
  }

  strip_offsets_valid = false;
#ifdef DEBUG
  if (rows_per_strip.valid)
  {
    strip_offsets_valid =
      TIFFGetField(tif_, TIFFTAG_STRIPOFFSETS, &strip_offsets)>0;
    //      vxl_uint_32 size = strips_per_image()*samples_per_pixel.val;
    vxl_uint_32 size = strips_per_image();
      for (vxl_uint_32 i = 0; i<size; ++i)
      std::cout << "SOFF[" << i << "]=" << strip_offsets[i] << '\n';
  }
#endif
  read_short_tag(tif_,TIFFTAG_THRESHHOLDING, thresholding);

  read_float_tag(tif_, TIFFTAG_XRESOLUTION, x_resolution, x_resolution_valid);
  read_float_tag(tif_, TIFFTAG_YRESOLUTION, y_resolution, y_resolution_valid);

  read_long_tag(tif_, TIFFTAG_TILEWIDTH, tile_width, 0);
  read_long_tag(tif_, TIFFTAG_TILELENGTH, tile_length, 0);

  tile_offsets_valid = false;

#ifdef DEBUG
  if (tile_width.valid&&tile_length.valid)
  {
    tile_offsets_valid =
      TIFFGetField(tif_, TIFFTAG_TILEOFFSETS, &tile_offsets)>0;
    //      vxl_uint_32 size = tiles_per_image()*samples_per_pixel.val;
    vxl_uint_32 size = tiles_per_image();
    for (vxl_uint_32 i = 0; i<size; ++i)
      std::cout << "TOFF[" << i << "]=" << tile_offsets[i] << '\n';
  }
#endif

  tile_byte_counts_valid = false;
#ifdef DEBUG
  if (tile_width.valid&&tile_length.valid)
  {
    tile_byte_counts_valid =
     TIFFGetField(tif_, TIFFTAG_TILEBYTECOUNTS, &tile_byte_counts)>0;
    //      vxl_uint_32 size = tiles_per_image()*samples_per_pixel.val;
    vxl_uint_32 size = tiles_per_image();
    for (vxl_uint_32 i = 0; i<size; ++i)
      std::cout << "TBC[" << i << "]=" << tile_byte_counts[i] << '\n';
  }
#endif
  return this->compute_pixel_format();
  // int success = TIFFReadDirectory(tif_);
}

// the number of separate sample planes defined by the tiff file
// for example a multi-band image.
unsigned vil_tiff_header::n_separate_image_planes() const
{
  if (planar_config.valid)
  {
    if (planar_config.val == PLANARCONFIG_SEPARATE)
      return samples_per_pixel.val;
    else if (planar_config.val == PLANARCONFIG_CONTIG)
      return 1;
    else
      return 0;
  }
  else
    return 0;
}

bool vil_tiff_header::is_tiled() const
{
  return is_tiled_flag ||
    (tile_width.valid&&tile_length.valid&&tile_width.val>0&&tile_length.val>0);
}

bool vil_tiff_header::is_striped() const
{
  // Assume if rows_per_strip is not provided, assume it is infinity,
  // and that the image is striped.
  return (rows_per_strip.valid && rows_per_strip.val > 0) || !rows_per_strip.valid;
}

#if HAS_GEOTIFF
bool vil_tiff_header::is_GEOTIFF() const
{
  short *data;
  short count;
  return TIFFGetField(tif_, 34735 /*TIFFTAG_GEOKEYDIRECTORY*/, &count, &data)!=0;
}
#endif

unsigned vil_tiff_header::encoded_bytes_per_block() const
{
  if (is_tiled())
    return static_cast<unsigned>(bytes_per_tile());
  else if (is_striped())
    return static_cast<unsigned>(bytes_per_strip());
  else
    return 0;
}

// the number of samples in a scan line
unsigned vil_tiff_header::samples_per_line() const
{
  if (image_width.valid)
    return samples_per_pixel.val*image_width.val;
  return 0;
}

vxl_uint_32 vil_tiff_header::bytes_per_line() const
{
  unsigned nsamp = this->samples_per_line();
  unsigned bits_per_line = bits_per_sample.val*nsamp;
  return (bits_per_line + 7)/8;
}

vxl_uint_32 vil_tiff_header::actual_bytes_per_line() const
{
    return TIFFScanlineSize(tif_);
}

vxl_uint_32 vil_tiff_header::rows_in_strip() const
{
  if (rows_per_strip.valid&&image_length.valid)
  {
    vxl_uint_32 rps = rows_per_strip.val;
    if (rps>image_length.val)
      return image_length.val;
    return rps;
  }
  else if (image_length.valid)
  {
    return image_length.val;
  }
  return 0;
}

//this value can vary from one strip to the next
vxl_uint_32 vil_tiff_header::
actual_bytes_per_strip(const vxl_uint_32 strip_index) const
{
  if (strip_byte_counts_valid)
  return strip_byte_counts[strip_index];
  return 0;
}

//the theoretical amount needed
vxl_uint_32 vil_tiff_header::bytes_per_strip() const
{
  return rows_in_strip()*bytes_per_line();
}

vxl_uint_32 vil_tiff_header::bytes_per_tile() const
{
  return TIFFTileSize(tif_);
}

//The number of images in the tiff file
vxl_uint_16  vil_tiff_header::n_images()
{
  return TIFFNumberOfDirectories(tif_);
}

//assemble the information to define the vil_pixel_format
//return false if the format cannot be handled
bool vil_tiff_header::compute_pixel_format()
{
  //also need sample_format.valid but use default (1) for images that don't have it
  // -GY- use default (1) for images that do not have SamplesPerPixel tag
  if (!(bits_per_sample.valid) || /*!(samples_per_pixel.valid) ||*/
      !(planar_config.valid) || !photometric.valid           )
  {
    pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
    return false;
  }

  vxl_uint_16 const b = bits_per_sample.val;
  vxl_uint_16 const bbs = bytes_per_sample();
  nplanes = 1;
  //Let's do the easy case first -- scalar pixels but various types
  if (samples_per_pixel.val==1)
  {
    //handle sample formats (unsigned, signed, float, double)
    switch (sample_format.val)
    {
      case 2: //2s complement signed
        switch (b)
        {
          case 8:
            pix_fmt = VIL_PIXEL_FORMAT_SBYTE;
            return true;
          case 16:
            pix_fmt = VIL_PIXEL_FORMAT_INT_16;
            return true;
          case 32:
            pix_fmt = VIL_PIXEL_FORMAT_INT_32;
            return true;
          default: //other bit sizes don't make sense
            pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
            return false;
        }
      case 3: // floating point
        switch (bbs)
        {
          case (sizeof(float)):
            pix_fmt = VIL_PIXEL_FORMAT_FLOAT;
            return true;
          case (sizeof(double)):
            pix_fmt = VIL_PIXEL_FORMAT_DOUBLE;
            return true;
          default: //other bit sizes don't make sense
            pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
            return false;
        }
      case 4: //undefined format
      case 1: //unsigned values
      default:
        // One special case is palette images
        // vil doesn't currently support color maps so need to convert to
        // regular three component RGB image (LATER)
        if (photometric.val==PHOTOMETRIC_RGB /*&& samples_per_pixel.val==1 &&
            sample_format.val == 1*/) //only support unsigned
          switch (bbs)
          {
            case 1:
              pix_fmt = VIL_PIXEL_FORMAT_BYTE;
              nplanes = 3;
              return false;
            case 2:
              pix_fmt = VIL_PIXEL_FORMAT_UINT_16;
              nplanes = 4;
              return false;
            default://other palette dynamic ranges don't make sense
              pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
              return false;
          }

        if (b==1){
          pix_fmt = VIL_PIXEL_FORMAT_BOOL;
          return true;}
        else
          switch (bbs)
          {
            case 1:
              pix_fmt = VIL_PIXEL_FORMAT_BYTE;
              return true;
            case 2:
              pix_fmt = VIL_PIXEL_FORMAT_UINT_16;
              return true;
            case 3:
            case 4:
              pix_fmt = VIL_PIXEL_FORMAT_UINT_32;
              return true;
            default:
              pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
              return false;
          }
    }
  }

  // Two-channel case --- greyscale+alpha
  else if (samples_per_pixel.val==2)
  {
    nplanes = 2;
    switch (sample_format.val)
    {
      case 1: //unsigned values
        switch (b)
        {
          case 8:
            pix_fmt = VIL_PIXEL_FORMAT_BYTE;
            return true;
          case 16:
            pix_fmt = VIL_PIXEL_FORMAT_UINT_16;
            return true;
          case 32:
            pix_fmt = VIL_PIXEL_FORMAT_UINT_32;
            return true;
          default: //other dynamic ranges, e.g. 12 bits/sample
            pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
            return false;
        }
      default:
        // Need to handle other signed values??
        return false;
    }
  }
  // Now for regular color images
  // handle sample formats (unsigned, signed, float, double)
  // vil normally doesn't directly express these interleaved formats but
  // pretends the samples are in different planes.
  // The current implementation can't handle planar_config ==2, which is
  // separate color bands.
  else if (/*samples_per_pixel.val>1 &&*/ photometric.val==PHOTOMETRIC_RGB && planar_config.val == 1 )
  {
    vxl_uint_16 const s = samples_per_pixel.val;
    switch (sample_format.val)
    {
      case 1: //unsigned values
        switch (b)
        {
          case 8:
            pix_fmt = VIL_PIXEL_FORMAT_BYTE;
            switch (s)
            {
              case 3:
                nplanes = 3;
                return true;
              case 4:{
                               nplanes = 4;
                                  if(extra_samples.valid && extra_samples.val==1)
                                        pix_fmt = VIL_PIXEL_FORMAT_RGBA_BYTE;
                return true;
                                         }
              default:
                pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
                return false;
            }
          case 16:
            pix_fmt = VIL_PIXEL_FORMAT_UINT_16;
            switch (s)
            {
              case 3:
                nplanes = 3;
                return true;
              case 4:{
                nplanes = 4;
                                // can be RBGA if extra samples and
                                // so shouldn't be treated as a simple multiband image
                                if(extra_samples.valid && extra_samples.val==1)
                                        pix_fmt = VIL_PIXEL_FORMAT_RGBA_UINT_16;
                return true;
                                }
              default:
                pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
                return false;
            }
          case 32:
            pix_fmt = VIL_PIXEL_FORMAT_UINT_32;
            switch (s)
            {
              case 3:
                nplanes = 3;
                return true;
              case 4:
                nplanes = 4;
                return false;
              default:
                pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
                return false;
            }
          default: //other dynamic ranges, e.g. 12 bits/sample
            switch (bbs)
            {
              case 1:
                pix_fmt = VIL_PIXEL_FORMAT_RGB_BYTE;
                switch (s)
                {
                  case 3:
                    nplanes = 3;
                    return true;
                  case 4:
                    nplanes = 4;
                    return false;
                  default:
                    pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
                    return false;
                }
              case 2:
                pix_fmt = VIL_PIXEL_FORMAT_UINT_16;
                switch (s)
                {
                  case 3:
                    nplanes = 3;
                    return true;
                  case 4:
                    nplanes = 4;
                    return true;
                  default:
                    pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
                    return false;
                }
              case 4:
                pix_fmt = VIL_PIXEL_FORMAT_UINT_32;
                switch (s)
                {
                  case 3:
                    nplanes = 3;
                    return true;
                  case 4:
                    nplanes = 4;
                    return false;
                  default:
                    pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
                    return false;
                }
              default:
                pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
                return false;
            }
        }
      case 2: //do signed color images make sense?
        pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
        return false;

      case 3: // do floating point color images make sense?
        pix_fmt = VIL_PIXEL_FORMAT_FLOAT; // DEC changed from vil_pixel_format_unknown
        nplanes = s; // DEC
        return true; // DEC changed from false
      case 4: //undefined format
      default:
        pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
        return false;
    }
  }else if(planar_config.val == 2){//TIFF multiband images
	// only handle unsigned pixel types for planar config == 2
	if(sample_format.val != 1){
		pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
        return false;
	}
	vxl_uint_16 const s = samples_per_pixel.val;
	  switch (b){
	   case 16://only handle 2 byte pixels for now
            pix_fmt = VIL_PIXEL_FORMAT_UINT_16;
            switch (s)
            {
              case 3:
                nplanes = 3;
                return true;
              case 4:{
                nplanes = 4;
                return true;
				}
			  case 8:{
                nplanes = 8;
                return true;
				}
              default:
                pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
                return false;
            }
		}
	}
  //Separate TIFF transparency mask - not handled
  if (photometric.val==PHOTOMETRIC_MASK)
  {
    pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
    return false;
  }

  //TIFF color separations - not handled
  if (photometric.val==PHOTOMETRIC_SEPARATED)
  {
    pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
    return false;
  }

  pix_fmt = VIL_PIXEL_FORMAT_UNKNOWN;
  return false;
}

//Returns false if the pixel format cannot be written
bool vil_tiff_header::parse_pixel_format(vil_pixel_format const& fmt)
{
  //Check for supported types
  sample_format.val = 1; sample_format.valid = true;
  switch ( fmt )
  {
    case VIL_PIXEL_FORMAT_BOOL:
      bits_per_sample.val = 1; bits_per_sample.valid = true;
      return true;
    case VIL_PIXEL_FORMAT_BYTE:
      bits_per_sample.val = 8; bits_per_sample.valid = true;
      return true;
    case VIL_PIXEL_FORMAT_UINT_16:
      bits_per_sample.val = 16; bits_per_sample.valid = true;
      return true;
    case VIL_PIXEL_FORMAT_UINT_32:
      bits_per_sample.val = 32; bits_per_sample.valid = true;
      return true;
    case VIL_PIXEL_FORMAT_FLOAT:
      bits_per_sample.val = 8*sizeof(float); bits_per_sample.valid = true;
      sample_format.val = 3;
      return true;
    case VIL_PIXEL_FORMAT_DOUBLE:
      bits_per_sample.val = 8*sizeof(double); bits_per_sample.valid = true;
      sample_format.val = 3;
      return true;
    default:
      break;
  }
  return false;
}

//Setup the required header information in preparation for writing to
//the tiff file header
bool vil_tiff_header::set_header(unsigned ni, unsigned nj, unsigned nplns,
                                 vil_pixel_format const& fmt,
                                 const unsigned size_block_i,
                                 const unsigned size_block_j)
{
  //also need machine endian
#if VXL_BIG_ENDIAN
    machine_is_big_endian_ = true;
#else
    machine_is_big_endian_ = false;
#endif
    //write file with same endian state as the machine
    file_is_big_endian_ = machine_is_big_endian_;
  pix_fmt = fmt;
  if (!this->parse_pixel_format(fmt))
    return false;
  nplanes = nplns;
  //check for color type
  photometric.valid = true;
  switch ( nplanes )
  {
    case 1:
    case 2:
      photometric.val = 1;
      break;
    case 3:
    case 4:
      photometric.val = 2;
      break;
    default:
      return false;
  }
  image_length.val = nj; image_length.valid = true;
  image_width.val = ni; image_width.valid = true;
  if (size_block_i>0&&size_block_j>0)
  {
    is_tiled_flag = true;
    tile_width.val = size_block_i; tile_width.valid = true;
    tile_length.val = size_block_j; tile_length.valid = true;
  }
  else
  {
    is_tiled_flag = false;
    //Check for default -- one scanline per row
    unsigned n_rows = size_block_j;
    if (n_rows == 0)
      n_rows = 1u;
    rows_per_strip.val = n_rows; rows_per_strip.valid = true;
  }
  samples_per_pixel.val = nplanes; samples_per_pixel.valid=true;
  // Can't handle separate color planes
  planar_config.val = 1; planar_config.valid = true;
  // The sensible way ..
  orientation.val = ORIENTATION_TOPLEFT; orientation.valid = true;
  software = "https://vxl.github.io/  vil image library";
  return true;
}

// Constructor - writes the necessary header information to the open tiff file
vil_tiff_header::
vil_tiff_header(TIFF* tif, const unsigned ni, const unsigned nj,
                const unsigned nplanes, vil_pixel_format const& fmt,
                const unsigned size_block_i, const unsigned size_block_j)
{
  tif_ = tif;

  format_supported =
    this->set_header(ni, nj, nplanes, fmt, size_block_i, size_block_j);
  if (!format_supported)
    return;
  write_short_tag(tif_,TIFFTAG_PHOTOMETRIC, photometric);
  write_short_tag(tif_,TIFFTAG_PLANARCONFIG, planar_config);
  write_short_tag(tif_,TIFFTAG_SAMPLESPERPIXEL, samples_per_pixel);
  write_short_tag(tif_,TIFFTAG_BITSPERSAMPLE, bits_per_sample);
  write_string(tif_, TIFFTAG_DATETIME, date_and_time());
  write_long_tag(tif_,TIFFTAG_IMAGELENGTH, image_length);
  write_long_tag(tif_,TIFFTAG_IMAGEWIDTH, image_width);
  write_short_tag(tif_,TIFFTAG_ORIENTATION, orientation);
  write_long_tag(tif_,TIFFTAG_ROWSPERSTRIP, rows_per_strip);
  write_string(tif_, TIFFTAG_SOFTWARE, software);
  write_short_tag(tif_,TIFFTAG_SAMPLEFORMAT, sample_format);
  write_long_tag(tif_, TIFFTAG_TILEWIDTH, tile_width);
  write_long_tag(tif_, TIFFTAG_TILELENGTH, tile_length);
#if 1  // Handle Alpha channel, assuming it is the last channel
  if (nplanes == 2 || nplanes == 4)
  {
    //extra_samples.val = 1;
    //extra_samples.valid = true;
    vxl_uint_32 num_extra_samples = 1;
    vxl_uint_16 extra_sample_values[] = { EXTRASAMPLE_ASSOCALPHA/*1*/ };  // Indicate Associated Alpha Data
    TIFFSetField(tif_, TIFFTAG_EXTRASAMPLES, num_extra_samples, extra_sample_values);
  }
#endif
  //initialize other flags to false
  color_map_valid = false;
  grey_response_curve_valid = false;
  strip_byte_counts_valid = false;
  strip_offsets_valid = false;
  tile_offsets_valid = false;
  tile_byte_counts_valid = false;
}
