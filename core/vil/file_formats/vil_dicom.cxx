// This is core/vil/file_formats/vil_dicom.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <vil/vil_config.h>
#if HAS_DCMTK

#include "vil_dicom.h"

#include <vcl_cassert.h>
#include <vcl_compiler.h>

#include <vxl_config.h> // for vxl_byte and such

#include <vil/vil_property.h>
#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_exception.h>

#include <dcfilefo.h>
#include <dcmetinf.h>
#include <dcdatset.h>
#include <dctagkey.h>
#include <dcdeftag.h>
#include <dcstack.h>
#include <diinpxt.h>

#include "vil_dicom_stream.h"
//
//Believe it or not some dicom images have a mixed endian encoding.
// e.g. mixed (7fe0,0010) OW 30f8 vs unmixed (7fe0,0010) OW f830
//The header is little endian and the data is big endian!! There doesn't
//seem to be a way of telling that this is the case. The DCM library
//reports that the  TransferSyntax is "LittleEndianImplicit", which isn't helpful.
// This is a hack to be able to read such files
//#define MIXED_ENDIAN
// Another hack to remove the offset of -1024 needed to calibrate HU. Otherwise
// this reader converts the image to floating point, which may not be desired.
//#define NO_OFFSET

char const* vil_dicom_format_tag = "dicom";


vil_image_resource_sptr vil_dicom_file_format::make_input_image(vil_stream* vs)
{
  bool is_dicom = false;

  char magic[ DCM_MagicLen ];
  vs->seek( DCM_PreambleLen );
  if ( vs->read( magic, DCM_MagicLen ) == DCM_MagicLen ) {
    if ( std::strncmp( magic, DCM_Magic, DCM_MagicLen ) == 0 ) {
      is_dicom = true;
    }
  }

  if ( is_dicom )
    return new vil_dicom_image( vs );
  else
    return 0;
}

vil_image_resource_sptr vil_dicom_file_format::make_output_image(vil_stream* /*vs*/,
                                                                 unsigned /*ni*/,
                                                                 unsigned /*nj*/,
                                                                 unsigned /*nplanes*/,
                                                                 vil_pixel_format)
{
  std::cerr << "ERROR: vil_dicom_file doesn't support output yet\n";
  return 0;
#if 0

  if (nplanes != 1 || format != VIL_PIXEL_FORMAT_INT_32)
  {
    std::cerr << "ERROR: vil_dicom_file_format::make_output_image\n"
             << "       Can only create DICOM images with a single plane\n"
             << "       and 32-bit integer pixels\n";
    return 0;
  }
  return new vil_dicom_image(vs, ni, nj, nplanes, format);
#endif
}

char const* vil_dicom_file_format::tag() const
{
  return vil_dicom_format_tag;
}


// ===========================================================================
// helper routines defined at end

static
void
read_header( DcmObject* dataset, vil_dicom_header_info& i );

static
void
read_pixels_into_buffer(DcmPixelData* pixels,
                        unsigned num_samples,
                        Uint16 alloc,
                        Uint16 stored,
                        Uint16 high,
                        Uint16 rep,
                        Float64 slope,
                        Float64 intercept,
                        vil_memory_chunk_sptr& out_buf,
                        vil_pixel_format& out_format);


vil_dicom_image::vil_dicom_image(vil_stream* vs)
  : pixels_( 0 )
{
  vil_dicom_header_info_clear( header_ );

  vil_dicom_stream_input dcis( vs );

  DcmFileFormat ffmt;
  ffmt.transferInit();
  OFCondition cond = ffmt.read( dcis );
  ffmt.transferEnd();

  if ( cond != EC_Normal ) {
    std::cerr << "vil_dicom ERROR: could not read file (" << cond.text() << ")\n"
             << "And the error code is: " << cond.code() << std::endl;
    //if (cond.code() != 4)
      return;
  }

  DcmDataset& dset = *ffmt.getDataset();
  read_header( &dset, header_ );

  //correct known manufacturers' drop-offs in header data!
  correct_manufacturer_discrepancies();

  // I don't know (yet) how to deal with look up tables. (Without the
  // tables, the pixel values represent actual measurements.)
  //
  if ( dset.tagExists( DCM_ModalityLUTSequence ) ) {
    std::cerr << "vil_dicom ERROR: don't know (yet) how to handle modality LUTs\n";
    return;
  }

  // If no ModalityLUT is present, I think the slope and intercept are
  // required. However, it seems that some DICOM images don't, so
  // assume the identity relationship.
  //
  Float64 slope, intercept;
  if ( dset.findAndGetFloat64( DCM_RescaleSlope, slope ) != EC_Normal )
    slope = 1;
  if ( dset.findAndGetFloat64( DCM_RescaleIntercept, intercept ) != EC_Normal )
    intercept = 0;

  // Gather the storage format info

#define Stringify( v ) #v
#define MustRead( func, key, var )                                                     \
  do {                                                                                 \
    if ( dset. func( key, var ) != EC_Normal ) {                                       \
      std::cerr << "vil_dicom ERROR: couldn't read " Stringify(key) "; can't handle\n"; \
      return;                                                                          \
  }} while (false)

  Uint16 bits_alloc, bits_stored, high_bit, pixel_rep;

  MustRead( findAndGetUint16, DCM_BitsAllocated, bits_alloc );
  MustRead( findAndGetUint16, DCM_BitsStored, bits_stored );
  MustRead( findAndGetUint16, DCM_HighBit, high_bit );
  MustRead( findAndGetUint16, DCM_PixelRepresentation, pixel_rep );

#undef MustRead
#undef Stringify

  // The pixels buffer will eventually be assigned to pixel_buf, and
  // the pixel format written into pixel_format.
  //
  vil_memory_chunk_sptr pixel_buf;
  vil_pixel_format pixel_format = VIL_PIXEL_FORMAT_UNKNOWN;

  // First, read in the bytes and interpret them appropriately
  //
  {
    DcmPixelData* pixels = 0;
    DcmStack stack;
    if ( dset.search( DCM_PixelData, stack, ESM_fromHere, true ) == EC_Normal )
    {
      if ( stack.card() == 0 ) {
        std::cerr << "vil_dicom ERROR: no pixel data found\n";
        return;
      }
      else {
        assert( stack.top()->ident() == EVR_PixelData );
        pixels = static_cast<DcmPixelData*>(stack.top());
      }
    }
    unsigned num_samples = ni() * nj() * nplanes();
    read_pixels_into_buffer(pixels, num_samples,
                            bits_alloc, bits_stored, high_bit, pixel_rep,
                            slope, intercept,
                            pixel_buf, pixel_format);
  }

  // Create an image resource to manage the pixel buffer
  //
#define DOCASE( fmt )                                                \
      case fmt: {                                                    \
        typedef vil_pixel_format_type_of<fmt>::component_type T;     \
        pixels_ = vil_new_image_resource_of_view(                    \
                    vil_image_view<T>(pixel_buf,                     \
                                      (T*)pixel_buf->data(),         \
                                      ni(), nj(), nplanes(),         \
                                      nplanes(), ni()*nplanes(), 1));\
      } break

  switch ( pixel_format ) {
    DOCASE( VIL_PIXEL_FORMAT_UINT_16 );
    DOCASE( VIL_PIXEL_FORMAT_INT_16 );
    DOCASE( VIL_PIXEL_FORMAT_BYTE );
    DOCASE( VIL_PIXEL_FORMAT_SBYTE );
    DOCASE( VIL_PIXEL_FORMAT_FLOAT );
    default: std::cerr << "vil_dicom ERROR: unexpected pixel format\n";
  }
#undef DOCASE
}


bool vil_dicom_image::get_property(char const* tag, void* value) const
{
  if (std::strcmp(vil_property_quantisation_depth, tag)==0)
  {
    if (value)
      *static_cast<unsigned int*>(value) = header_.stored_bits_;
    return true;
  }

  if (std::strcmp(vil_property_pixel_size, tag)==0 && value!=0)
  {
    float *pixel_size = static_cast<float*>(value);
    pixel_size[0] = header_.spacing_x_ / 1000.0f;
    pixel_size[1] = header_.spacing_y_ / 1000.0f;
    return true;
  }

  // Need to write lots of access code for the dicom header.
  return false;
}

char const* vil_dicom_image::file_format() const
{
  return vil_dicom_format_tag;
}

vil_dicom_image::vil_dicom_image(vil_stream* /*vs*/, unsigned ni, unsigned nj,
                                 unsigned nplanes, vil_pixel_format format)
{
  assert(!"vil_dicom_image doesn't yet support output");

  assert(nplanes == 1 && format == VIL_PIXEL_FORMAT_INT_32);
  header_.size_x_ = ni;
  header_.size_y_ = nj;
  header_.size_z_ = 1;
}

vil_dicom_image::~vil_dicom_image()
{
}

unsigned vil_dicom_image::nplanes() const
{
  return header_.pix_samps_;
}

unsigned vil_dicom_image::ni() const
{
  return header_.size_x_;
}

unsigned vil_dicom_image::nj() const
{
  return header_.size_y_;
}

enum vil_pixel_format vil_dicom_image::pixel_format() const
{
  return pixels_ ? pixels_->pixel_format() : VIL_PIXEL_FORMAT_UNKNOWN;
}

vil_image_view_base_sptr vil_dicom_image::get_copy_view(
  unsigned x0, unsigned nx, unsigned y0, unsigned ny) const
{
  return pixels_ ? pixels_->get_copy_view( x0, nx, y0, ny ) : 0;
}

vil_image_view_base_sptr vil_dicom_image::get_view(
  unsigned x0, unsigned nx, unsigned y0, unsigned ny) const
{
  return pixels_ ? pixels_->get_view( x0, nx, y0, ny ) : 0;
}

bool vil_dicom_image::put_view(const vil_image_view_base& view,
                               unsigned x0, unsigned y0)
{
  assert(!"vil_dicom_image doesn't yet support output yet");

  if (!view_fits(view, x0, y0))
  {
    vil_exception_warning(vil_exception_out_of_bounds("vil_dicom_image::put_view"));
    return false;
  }

  return false;
}

// ===========================================================================
//MANUFACTURER SPECIFIC TWEAKS
//--- Add any further manufacturer specific tweaks in this section,
// but if it all gets out of hand, then we will need some derived classes which should attempt to read first
// ===========================================================================

//:correct known manufacturers drop-offs in header data!
//For example Hologic encode pixel-size in the imageComment!
//NB if this section starts bloating, use derived classes which override correct_manufacturer_discrepancies
void vil_dicom_image::correct_manufacturer_discrepancies()
{
    //If manufacturer is Hologic and it's a QDR or Discovery model DXA scanner...
  if ( ( (header_.manufacturer_ == "HOLOGIC") || (header_.manufacturer_ == "Hologic") ) &&
       ( (header_.model_name_.find("QDR") != header_.model_name_.npos ) ||
         (header_.model_name_.find("Discovery") != header_.model_name_.npos )         ) )
  {
    //Hologic QDR Bone Densitometry source - set (default) pixel spacing from private format image comments
    float xPixelSize=1.0;
    float yPixelSize=1.0;
    if (interpret_hologic_header(xPixelSize,yPixelSize))
    {
      header_.spacing_x_ = xPixelSize;
      header_.spacing_y_ = yPixelSize;
    }
  }

  //
  // GE Lunar iDXA scanner produces files with no indication of the pixel size or scaling, other than the
  // exposed area.
  //
  if ( ( (header_.manufacturer_ == "GE Healthcare") ) &&
       ( (header_.model_name_.find("Lunar iDXA") != header_.model_name_.npos )         ) )
  {
    //GE Lunar iDXA scanner: set pixel size from exposed area and number of rows/columns

    if (header_.exposedarea_x_!=0 && header_.exposedarea_y_!=0 && header_.size_x_!=0 && header_.size_y_!=0)
    {
      header_.spacing_x_ = header_.exposedarea_x_;
      header_.spacing_y_ = header_.exposedarea_y_;
      header_.spacing_x_ /= header_.size_x_;
      header_.spacing_y_ /= header_.size_y_;
    }
  }

  // By default, set the pixel spacing to 1 if it is still 0 after correcting for any discrepancies.
  // This is, for example, the case if the pixel spacing has been explicitely set to 0 in the file header
  // without any further manufacturer-specific information.
  if ((header_.spacing_x_ == 0) && (header_.spacing_y_ == 0))
  {
    header_.spacing_x_ = 1;
    header_.spacing_y_ = 1;
  }
}

//:try and interpret the Hologic comments section to extract pixel size
bool vil_dicom_image::interpret_hologic_header(float& xpixSize, float& ypixSize)
{
  //The magic internal Hologic tags used within their vast "Image Comments"
  //which is used as a cop-out from sensible DICOM conformance
  static const std::string HOLOGIC_PixelXSizeMM = "<PixelXSizeMM>";
  static const std::string HOLOGIC_PixelXSizeMM_END = "</PixelXSizeMM>";
  static const std::string HOLOGIC_PixelYSizeMM = "<PixelYSizeMM>";
  static const std::string HOLOGIC_PixelYSizeMM_END = "</PixelYSizeMM>";

  std::string src = header_.image_comments_;
  //Find start of x pixel size sub-text
  std::size_t ipxStart = src.find(HOLOGIC_PixelXSizeMM);
  if (ipxStart==src.npos) return false;

  //Find end of x pixel size sub-text
  std::size_t ipxEnd = src.find(HOLOGIC_PixelXSizeMM_END,ipxStart);
  if (ipxEnd==src.npos) return false;

  //Extract just the numerical part of the text
  std::string strPixelXSizeMM="";
  ipxStart+= HOLOGIC_PixelXSizeMM.size();
  strPixelXSizeMM.append(src,ipxStart, ipxEnd-ipxStart);

  if (strPixelXSizeMM.size()>0)
  {
    //Translate string to number
    std::stringstream translate_is(strPixelXSizeMM,std::stringstream::in);
    translate_is>>xpixSize;
    if (!translate_is) return false;
    if (xpixSize<=0.0 || xpixSize>=1.0E6)
      return false; //Don't believe crazy values
  }
  else
  {
    return false; //No x pixel value present between the tags
  }

  //Find start of y pixel size sub-text
  std::size_t ipyStart = src.find(HOLOGIC_PixelYSizeMM);
  if (ipyStart==src.npos) return false;

  //Find end of y pixel size sub-text
  std::size_t ipyEnd = src.find(HOLOGIC_PixelYSizeMM_END,ipyStart);
  if (ipyEnd==src.npos) return false;

  //Extract just the numerical part of the text
  std::string strPixelYSizeMM="";
  ipyStart+= HOLOGIC_PixelYSizeMM.size();
  strPixelYSizeMM.append(src,ipyStart, ipyEnd-ipyStart);

  if (strPixelYSizeMM.size()>0)
  {
    //Translate string to number
    std::stringstream translate_is(strPixelYSizeMM,std::stringstream::in);
    translate_is>>ypixSize;
    if (!translate_is) return false;
    if (ypixSize<=0.0 || ypixSize>=1.0E6)
      return false; //Don't believe crazy values
  }
  else
  {
    return false; //No y pixel value present between the tags
  }

  return true;    //It all worked
}


// ===========================================================================
//                                                             HELPER ROUTINES
// ===========================================================================


static
DcmElement*
find_element( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element )
{
  DcmElement* result = 0;
  DcmTagKey key( group, element );
  DcmStack stack;
  if ( dset->search( key, stack, ESM_fromHere, true ) == EC_Normal )
  {
    if ( stack.card() == 0 ) {
      std::cerr << "vil_dicom ERROR: no results on stack\n";
    }
    else {
      result = static_cast<DcmElement*>(stack.top());
      if ( result->getVM() == 0 ) {
        // header present, but has no value == header not present
        result = 0;
      }
    }
  }

  return result;
}

// anonymous namespace for helper routines
namespace
{
  // Specializations of this template contains code to convert from the
  // given VR type (template parameter) to the given C++ type (4th
  // parameter of the function "proc").
  //
  template <int T /*vil_dicom_header_vr_type*/>
  struct try_set;

  // A general proc implementation that returns a string representation
  // of any VR.
  //
  struct try_set_to_string
  {
    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, std::string& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e )
      {
        OFString str;
        if ( e->getOFString( str, 0 ) != EC_Normal ) {
          std::cerr << "vil_dicom Warning: value of ("<<group<<','<<element<<") is not string\n";
        }
        else {
          value = str.c_str();
        }
      }
    }
  };

  template <>
  struct try_set< vil_dicom_header_AE >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_AS >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_AT >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_CS >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_DA >
  {
    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, long& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e )
      {
        OFString str;
        if ( e->getOFString( str, 0 ) != EC_Normal ) {
          std::cerr << "Warning: value of ("<<group<<','<<element<<") is not string\n";
        }
        else {
          value = std::atol( str.c_str() );
        }
      }
    }
  };

  template <>
  struct try_set< vil_dicom_header_DS >
  {
    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, float& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e )
      {
        OFString str;
        if ( e->getOFString( str, 0 ) != EC_Normal ) {
          std::cerr << "vil_dicom Warning: value of ("<<group<<','<<element<<") is not string\n";
        }
        else {
          value = static_cast<float>( std::atof( str.c_str() ) );
        }
      }
    }

    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, std::vector<float>& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e ) {
        for ( unsigned pos = 0; pos < e->getVM(); ++pos )
        {
          OFString str;
          if ( e->getOFString( str, pos ) != EC_Normal ) {
            std::cerr << "Warning: value of ("<<group<<','<<element<<") at " << pos << " is not string\n";
          }
          else {
            value.push_back( static_cast<float>( std::atof( str.c_str() ) ) );
          }
        }
      }
    }
  };

  template <>
  struct try_set< vil_dicom_header_FD >
  {
    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_ieee_64& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e ) {
        if ( e->getFloat64( value ) != EC_Normal ) {
          std::cerr << "Warning: value of ("<<group<<','<<element<<") is not Float64\n";
        }
      }
    }
  };

  template <>
  struct try_set< vil_dicom_header_FL >
  {
    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_ieee_32& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e ) {
        if ( e->getFloat32( value ) != EC_Normal ) {
          std::cerr << "Warning: value of ("<<group<<','<<element<<") is not Float32\n";
        }
      }
    }
  };

  template <>
  struct try_set< vil_dicom_header_IS >
  {
    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, long& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e )
      {
        OFString str;
        if ( e->getOFString( str, 0 ) != EC_Normal ) {
          std::cerr << "vil_dicom Warning: value of ("<<group<<','<<element<<") is not string\n";
        }
        else {
          value = std::atol( str.c_str() );
        }
      }
    }
  };

  template <>
  struct try_set< vil_dicom_header_LO >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_LT >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_OB >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_OW >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_PN >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_SH >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_SL >
  {
    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_sint_32& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e ) {
        if ( e->getSint32( reinterpret_cast<Sint32&>(value) ) != EC_Normal ) {
          std::cerr << "vil_dicom Warning: value of ("<<group<<','<<element<<") is not Sint32\n";
        }
      }
    }
  };

  template <>
  struct try_set< vil_dicom_header_SQ >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_SS >
  {
    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_sint_16& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e ) {
        if ( e->getSint16( reinterpret_cast<Sint16&>(value) ) != EC_Normal ) {
          std::cerr << "vil_dicom Warning: value of ("<<group<<','<<element<<") is not Sint16\n";
        }
      }
    }
  };

  template <>
  struct try_set< vil_dicom_header_ST >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_TM >
  {
    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, float& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e )
      {
        OFString str;
        if ( e->getOFString( str, 0 ) != EC_Normal ) {
          std::cerr << "vil_dicom Warning: value of ("<<group<<','<<element<<") is not string\n";
        }
        else {
          value = static_cast<float>( std::atof( str.c_str() ) );
        }
      }
    }
  };

  template <>
  struct try_set< vil_dicom_header_UI >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_UL >
  {
    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_uint_32& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e ) {
        if ( e->getUint32( reinterpret_cast<Uint32&>(value) ) != EC_Normal ) {
          std::cerr << "vil_dicom Warning: value of ("<<group<<','<<element<<") is not Uint32\n";
        }
      }
    }
  };

  template <>
  struct try_set< vil_dicom_header_UN >
    : public try_set_to_string
  {
  };

  template <>
  struct try_set< vil_dicom_header_US >
  {
    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_uint_16& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e ) {
        if ( e->getUint16( value ) != EC_Normal ) {
          std::cerr << "vil_dicom Warning: value of ("<<group<<','<<element<<") is not Uint16\n";
        }
      }
    }

    static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, std::vector<vxl_uint_16>& value ) {
      DcmElement* e = find_element( dset, group, element );
      if ( e ) {
        vxl_uint_16 value_at_pos;
        for ( unsigned long pos = 0; pos < e->getVM(); ++pos )
        {
          if ( e->getUint16( value_at_pos, pos ) != EC_Normal ) {
            std::cerr << "vil_dicom Warning: value of ("<<group<<','<<element<<") at " << pos << " is not Uint16\n";
          }
          else {
              value.push_back(value_at_pos);
          }
        }
      }
    }
  };

  template <>
  struct try_set< vil_dicom_header_UT >
    : public try_set_to_string
  {
  };
} // end anonymous namespace

static
void
read_header( DcmObject* f, vil_dicom_header_info& i )
{
  vxl_uint_16 group;

#define ap_join(A,B) A ## B
#define ap_type(X) ap_join( vil_dicom_header_, X )
#define ap_el(X) ap_join( VIL_DICOM_HEADER_, X )

  group = VIL_DICOM_HEADER_IDENTIFYINGGROUP;
  try_set< ap_type(CS) >::proc( f, group, ap_el(IDIMAGETYPE),         i.image_id_type_ ); // It's the image type
  try_set< ap_type(UI) >::proc( f, group, ap_el(IDSOPCLASSID),        i.sop_cl_uid_ ); // It's the SOP class ID
  try_set< ap_type(UI) >::proc( f, group, ap_el(IDSOPINSTANCEID),     i.sop_in_uid_ ); // It's the SOP instance ID
  try_set< ap_type(DA) >::proc( f, group, ap_el(IDSTUDYDATE),         i.study_date_ ); // It's the study date
  try_set< ap_type(DA) >::proc( f, group, ap_el(IDSERIESDATE),        i.series_date_ ); // It's the series date
  try_set< ap_type(DA) >::proc( f, group, ap_el(IDACQUISITIONDATE),   i.acquisition_date_ ); // It's the acquisition date
  try_set< ap_type(DA) >::proc( f, group, ap_el(IDIMAGEDATE),         i.image_date_ ); // It's the image date
  try_set< ap_type(TM) >::proc( f, group, ap_el(IDSTUDYTIME),         i.study_time_ ); // It's the study time
  try_set< ap_type(TM) >::proc( f, group, ap_el(IDSERIESTIME),        i.series_time_ ); // It's the series time
  try_set< ap_type(TM) >::proc( f, group, ap_el(IDACQUISITIONTIME),   i.acquisition_time_ ); // It's the acquisition time
  try_set< ap_type(TM) >::proc( f, group, ap_el(IDIMAGETIME),         i.image_time_ ); // It's the image time
  try_set< ap_type(SH) >::proc( f, group, ap_el(IDACCESSIONNUMBER),   i.accession_number_ ); // It's the accession number
  try_set< ap_type(CS) >::proc( f, group, ap_el(IDMODALITY),          i.modality_ ); // It's the imaging modality
  try_set< ap_type(LO) >::proc( f, group, ap_el(IDMANUFACTURER),      i.manufacturer_ ); // It's the manufacturer name
  try_set< ap_type(LO) >::proc( f, group, ap_el(IDINSTITUTIONNAME),   i.institution_name_ ); // It's the institution name
  try_set< ap_type(ST) >::proc( f, group, ap_el(IDINSTITUTIONADDRESS),i.institution_addr_ ); // It's the institution address
  try_set< ap_type(PN) >::proc( f, group, ap_el(IDREFERRINGPHYSICIAN),i.ref_phys_name_ ); // It's the referring physician's name
  try_set< ap_type(SH) >::proc( f, group, ap_el(IDSTATIONNAME),       i.station_name_ ); // It's the imaging station name
  try_set< ap_type(LO) >::proc( f, group, ap_el(IDSTUDYDESCRIPTION),  i.study_desc_ ); // It's the study description
  try_set< ap_type(LO) >::proc( f, group, ap_el(IDSERIESDESCRIPTION), i.series_desc_ ); // It's the series description
  try_set< ap_type(PN) >::proc( f, group, ap_el(IDATTENDINGPHYSICIAN),i.att_phys_name_); // It's the name of the attending physician
  try_set< ap_type(PN) >::proc( f, group, ap_el(IDOPERATORNAME),      i.operator_name_ ); // It's the name of the scanner operator
  try_set< ap_type(LO) >::proc( f, group, ap_el(IDMANUFACTURERMODEL), i.model_name_ ); // It's the scanner model

  group = VIL_DICOM_HEADER_PATIENTINFOGROUP;
  try_set< ap_type(PN) >::proc( f, group, ap_el(PIPATIENTNAME),     i.patient_name_ ); // It's the patient's name
  try_set< ap_type(LO) >::proc( f, group, ap_el(PIPATIENTID),       i.patient_id_ ); // It's the patient's id
  try_set< ap_type(DA) >::proc( f, group, ap_el(PIPATIENTBIRTHDATE),i.patient_dob_ ); // It's the patient's date of birth
  try_set< ap_type(CS) >::proc( f, group, ap_el(PIPATIENTSEX),      i.patient_sex_ ); // It's the patient's sex
  try_set< ap_type(AS) >::proc( f, group, ap_el(PIPATIENTAGE),      i.patient_age_ ); // It's the patient's age
  try_set< ap_type(DS) >::proc( f, group, ap_el(PIPATIENTWEIGHT),   i.patient_weight_ ); // It's the patient's weight
  try_set< ap_type(LT) >::proc( f, group, ap_el(PIPATIENTHISTORY),  i.patient_hist_ ); // It's the patient's history

  group = VIL_DICOM_HEADER_ACQUISITIONGROUP;
  try_set< ap_type(CS) >::proc( f, group, ap_el(AQSCANNINGSEQUENCE),      i.scanning_seq_ ); // It's the scanning sequence
  try_set< ap_type(CS) >::proc( f, group, ap_el(AQSEQUENCEVARIANT),       i.sequence_var_ ); // It's the sequence variant
  try_set< ap_type(CS) >::proc( f, group, ap_el(AQSCANOPTIONS),           i.scan_options_ ); // It's the scan options
  try_set< ap_type(CS) >::proc( f, group, ap_el(AQMRACQUISITIONTYPE),     i.mr_acq_type_ ); // It's the MR acquisition type
  try_set< ap_type(SH) >::proc( f, group, ap_el(AQSEQUENCENAME),          i.sequence_name_ ); // It's the sequence name
  try_set< ap_type(CS) >::proc( f, group, ap_el(AQANGIOFLAG),             i.angio_flag_ ); // It's the angio flag
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQSLICETHICKNESS),        i.slice_thickness_ ); // It's the slice thickness
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQREPETITIONTIME),        i.repetition_time_ ); // It's the repetition time
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQECHOTIME),              i.echo_time_ ); // It's the echo time
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQINVERSIONTIME),         i.inversion_time_ ); // It's the inversion time
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQNUMBEROFAVERAGES),      i.number_of_averages_ ); // It's the number of averages
  try_set< ap_type(IS) >::proc( f, group, ap_el(AQECHONUMBERS),           i.echo_numbers_ ); // It's the echo numbers
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQMAGNETICFIELDSTRENGTH), i.mag_field_strength_);// It's the magnetic field strength
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQSLICESPACING),          i.spacing_slice_ ); // It's the slice spacing
  try_set< ap_type(IS) >::proc( f, group, ap_el(AQECHOTRAINLENGTH),       i.echo_train_length_ ); // It's the echo train length
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQPIXELBANDWIDTH),        i.pixel_bandwidth_ ); // It's the pixel bandwidth
  try_set< ap_type(LO) >::proc( f, group, ap_el(AQSOFTWAREVERSION),       i.software_vers_ ); // It's the scanner software version
  try_set< ap_type(LO) >::proc( f, group, ap_el(AQPROTOCOLNAME),          i.protocol_name_ ); // It's the protocol name
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQTRIGGERTIME),           i.trigger_time_ ); // It's the trigger time
  try_set< ap_type(IS) >::proc( f, group, ap_el(AQHEARTRATE),             i.heart_rate_ ); // It's the heart rate
  try_set< ap_type(IS) >::proc( f, group, ap_el(AQCARDIACNUMBEROFIMAGES), i.card_num_images_ ); // It's the cardiac number of images
  try_set< ap_type(IS) >::proc( f, group, ap_el(AQTRIGGERWINDOW),         i.trigger_window_ ); // It's the trigger window
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQRECONTRUCTIONDIAMETER), i.reconst_diameter_ ); // It's the reconstruction diameter
  try_set< ap_type(SH) >::proc( f, group, ap_el(AQRECEIVINGCOIL),         i.receiving_coil_ ); // It's the receiving coil
  try_set< ap_type(CS) >::proc( f, group, ap_el(AQPHASEENCODINGDIRECTION),i.phase_enc_dir_ ); // It's the phase encoding direction
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQFLIPANGLE),             i.flip_angle_ ); // It's the flip angle
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQSAR),                   i.sar_ ); // It's the sar
  try_set< ap_type(CS) >::proc( f, group, ap_el(AQPATIENTPOSITION),       i.patient_pos_ ); // It's the patient position

  typedef vil_dicom_header_type_of<vil_dicom_header_DS>::type DS_type;
  std::vector<DS_type> ps_ips;
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQIMAGERPIXELSPACING), ps_ips );
  if ( ps_ips.size() > 0 )
    i.imager_spacing_x_ = ps_ips[0];
  else
    i.imager_spacing_x_ = 0;
  if ( ps_ips.size() > 1 )
    i.imager_spacing_y_ = ps_ips[1];
  else
    i.imager_spacing_y_ = i.imager_spacing_x_;

  group = VIL_DICOM_HEADER_RELATIONSHIPGROUP;
  try_set< ap_type(UI) >::proc( f, group, ap_el(RSSTUDYINSTANCEUID),   i.stud_ins_uid_ ); // It's the study instance id
  try_set< ap_type(UI) >::proc( f, group, ap_el(RSSERIESINSTANCEUID),  i.ser_ins_uid_ ); // It's the series instance id
  try_set< ap_type(SH) >::proc( f, group, ap_el(RSSTUDYID),            i.study_id_ ); // It's the study id
  try_set< ap_type(IS) >::proc( f, group, ap_el(RSSERIESNUMBER),       i.series_number_ ); // It's the series number
  try_set< ap_type(IS) >::proc( f, group, ap_el(RSAQUISITIONNUMBER),   i.acquisition_number_ ); // It's the acqusition number
  try_set< ap_type(IS) >::proc( f, group, ap_el(RSIMAGENUMBER),        i.image_number_ ); // It's the image number
  try_set< ap_type(CS) >::proc( f, group, ap_el(RSPATIENTORIENTATION), i.pat_orient_ ); // It's the patient orientation
  try_set< ap_type(DS) >::proc( f, group, ap_el(RSIMAGEPOSITION),      i.image_pos_ ); // It's the image position
  try_set< ap_type(DS) >::proc( f, group, ap_el(RSIMAGEORIENTATION),   i.image_orient_ ); // It's the image orientation
  try_set< ap_type(UI) >::proc( f, group, ap_el(RSFRAMEOFREFERENCEUID),i.frame_of_ref_ ); // It's the frame of reference uid
  try_set< ap_type(IS) >::proc( f, group, ap_el(RSIMAGESINACQUISITION),i.images_in_acq_); // the number of images in the acquisition
  try_set< ap_type(LO) >::proc( f, group, ap_el(RSPOSITIONREFERENCE),  i.pos_ref_ind_ ); // It's the position reference
  try_set< ap_type(LT) >::proc( f, group, ap_el(RSIMAGECOMMENTS),      i.image_comments_ ); // It's the image comments

  group = VIL_DICOM_HEADER_IMAGEGROUP;
  try_set< ap_type(US) >::proc( f, group, ap_el(IMSAMPLESPERPIXEL),    i.pix_samps_ ); // It's the samples per pixel
  try_set< ap_type(CS) >::proc( f, group, ap_el(IMPHOTOMETRICINTERP),  i.photo_interp_ ); // It's the photometric interpretation
  try_set< ap_type(US) >::proc( f, group, ap_el(IMROWS),               i.size_y_ ); // It's the rows
  try_set< ap_type(US) >::proc( f, group, ap_el(IMCOLUMNS),            i.size_x_ ); // It's the columns
  try_set< ap_type(US) >::proc( f, group, ap_el(IMPLANES),             i.size_z_ ); // It's the planes
  try_set< ap_type(US) >::proc( f, group, ap_el(IMBITSALLOCATED),      i.allocated_bits_ ); // It's the allocated bits
  try_set< ap_type(US) >::proc( f, group, ap_el(IMBITSSTORED),         i.stored_bits_ ); // It's the stored bits info
  try_set< ap_type(US) >::proc( f, group, ap_el(IMHIGHBIT),            i.high_bit_ ); // It's the high bit
  try_set< ap_type(US) >::proc( f, group, ap_el(IMPIXELREPRESENTATION),i.pix_rep_ ); // It's the pixel representation
  try_set< ap_type(US) >::proc( f, group, ap_el(IMSMALLIMPIXELVALUE),  i.small_im_pix_val_ ); // It's the smallest image pixel value
  try_set< ap_type(US) >::proc( f, group, ap_el(IMLARGEIMPIXELVALUE),  i.large_im_pix_val_ ); // It's the largest image pixel value
  try_set< ap_type(US) >::proc( f, group, ap_el(IMPIXELPADDINGVALUE),  i.pixel_padding_val_ ); // It's the pixel padding value
  try_set< ap_type(DS) >::proc( f, group, ap_el(IMWINDOWCENTER),       i.window_centre_ ); // It's the window centre
  try_set< ap_type(DS) >::proc( f, group, ap_el(IMWINDOWWIDTH),        i.window_width_ ); // It's the window width
  try_set< ap_type(DS) >::proc( f, group, ap_el(IMRESCALEINTERCEPT),   i.res_intercept_ ); // It's the rescale intercept
  try_set< ap_type(DS) >::proc( f, group, ap_el(IMRESCALESLOPE),       i.res_slope_ ); // It's the rescale slope

  std::vector<DS_type> ps;
  try_set< ap_type(DS) >::proc( f, group, ap_el(IMPIXELSPACING), ps );
  if ( ps.size() > 0 )
    i.spacing_x_ = ps[0];
  else
    i.spacing_x_ = 0;
  if ( ps.size() > 1 )
    i.spacing_y_ = ps[1];
  else
    i.spacing_y_ = i.spacing_x_;
  // check whether pixelspacing is available if not set to imagerpixelspacing
  // Note: I removed the messages here since they may not be accurate; correct_manufacturer_discrepancies
  // obtains accurate pixel spacings for some images.

  if (ps.size() <= 0)
  {
    if (ps_ips.size() <= 0)
    {
      i.spacing_x_ = 1;
      i.spacing_y_ = 1;
      //std::cout<<"DICOM tags PixelSpacing and ImagerPixelSpacing missing: use PixelSpacing = 1.0."<<std::endl;
    }
    else if (ps_ips.size() > 1)
    {
      i.spacing_x_ = i.imager_spacing_x_;
      i.spacing_y_ = i.imager_spacing_y_;
      //std::cout<<"DICOM tag PixelSpacing missing: use PixelSpacing = ImagerPixelSpacing."<<std::endl;
    }
    else if (ps_ips.size() > 0)
    {
      i.spacing_x_ = i.imager_spacing_x_;
      i.spacing_y_ = i.imager_spacing_x_;
      //std::cout<<"DICOM tag x.PixelSpacing missing: use x.PixelSpacing = x.ImagerPixelSpacing."<<std::endl;
      //std::cout<<"DICOM tags y.PixelSpacing and y.ImagerPixelSpacing missing: use y.PixelSpacing = x.ImagerPixelSpacing."<<std::endl;
    }
  }

  group = VIL_DICOM_HEADER_PROCEDUREGROUP;
  try_set< ap_type(FD) >::proc( f, group, ap_el(PRREALWORLDVALUEINTERCEPT),i.real_world_value_intercept_ ); // It's the real world intercept value
  try_set< ap_type(FD) >::proc( f, group, ap_el(PRREALWORLDVALUESLOPE),    i.real_world_value_slope_ ); // It's the real world slope value

  typedef vil_dicom_header_type_of<vil_dicom_header_US>::type US_type;
  std::vector<US_type> psb;
  try_set< ap_type(US) >::proc( f, group, ap_el(EXPOSEDAREA), psb );
  if ( psb.size() > 0 )
    i.exposedarea_x_ = psb[0];
  else
    i.exposedarea_x_ = 0;
  if ( psb.size() > 1 )
    i.exposedarea_y_ = psb[1];
  else
    i.exposedarea_y_ = i.exposedarea_x_;

  group = VIL_DICOM_HEADER_NSPHILIPSGROUP;
  try_set< ap_type(DS) >::proc( f, group, ap_el(NSPHILIPSPRIVATEINTERCEPT),i.philips_private_intercept_); // It's the Philips private intercept value
  try_set< ap_type(DS) >::proc( f, group, ap_el(NSPHILIPSPRIVATESLOPE),    i.philips_private_slope_); // It's the Philips private slope value

  i.header_valid_ = true;

#undef ap_join
#undef ap_type
#undef ap_el
}


// start anonymous namespace for helpers
namespace
{
  template<class InT>
  void
  convert_src_type( InT const*,
                    DcmPixelData* pixels,
                    unsigned num_samples,
                    Uint16 alloc,
                    Uint16 stored,
                    Uint16 high,
                    Uint16 rep,
                    DiInputPixel*& pixel_data,
                    vil_pixel_format& act_format )
  {
    if ( rep == 0 && stored <= 8 ) {
      act_format = VIL_PIXEL_FORMAT_BYTE;
      pixel_data = new DiInputPixelTemplate<InT,Uint8>( pixels, alloc, stored, high, 0, num_samples );
    }
    else if ( rep == 0 && stored <= 16 ) {
      act_format = VIL_PIXEL_FORMAT_UINT_16;
      pixel_data = new DiInputPixelTemplate<InT,Uint16>( pixels, alloc, stored, high, 0, num_samples );
    }
    else if ( rep == 1 && stored <= 8 ) {
      act_format = VIL_PIXEL_FORMAT_SBYTE;
      pixel_data = new DiInputPixelTemplate<InT,Sint8>( pixels, alloc, stored, high, 0, num_samples );
    }
    else if ( rep == 1 && stored <= 16 ) {
      act_format = VIL_PIXEL_FORMAT_INT_16;
      pixel_data = new DiInputPixelTemplate<InT,Sint16>( pixels, alloc, stored, high, 0, num_samples );
    }
  }

  template<class IntType, class OutType>
  void
  rescale_values( IntType const* int_begin,
                  unsigned num_samples,
                  OutType* float_begin,
                  Float64 slope, Float64 intercept )
  {
    IntType const* const int_end = int_begin + num_samples;
    for ( ; int_begin != int_end; ++int_begin, ++float_begin ) {
      *float_begin = static_cast<OutType>( *int_begin * slope + intercept );
    }
  }
} // anonymous namespace
#ifdef MIXED_ENDIAN
static  unsigned short swap_short(unsigned short v)
{
  return (v << 8)
       | (v >> 8);
}

static void swap_shorts(unsigned short *ip, unsigned short *op, int count)
{
  while (count)
  {
    *op++ = swap_short(*ip++);
    count--;
  }
}
#endif //MIXED_ENDIAN
static
void
read_pixels_into_buffer(DcmPixelData* pixels,
                        unsigned num_samples,
                        Uint16 alloc,
                        Uint16 stored,
                        Uint16 high,
                        Uint16 rep,
                        Float64 slope,
                        Float64 intercept,
                        vil_memory_chunk_sptr& out_buf,
                        vil_pixel_format& out_format)
{
  // This will be the "true" pixel buffer type after the overlay
  // planes are removed and the pixel bits shifted to the lowest bits
  // of the bytes.
  //
  vil_pixel_format act_format = VIL_PIXEL_FORMAT_UNKNOWN;

  // First convert from the stored src pixels to the actual
  // pixels. This is an integral type to integral type conversion.
  // Make sure pixel_data is deleted before this function exits!
  //
  DiInputPixel* pixel_data = 0;
  if ( pixels->getVR() == EVR_OW ) {
    convert_src_type( (Uint16*)0, pixels, num_samples, alloc, stored, high, rep, pixel_data, act_format );
  }
  else {
    convert_src_type( (Uint8*)0, pixels, num_samples, alloc, stored, high, rep, pixel_data, act_format );
  }
#ifdef MIXED_ENDIAN
#ifdef NO_OFFSET
  slope = 1; intercept = 0;
  if (act_format == VIL_PIXEL_FORMAT_SBYTE)
    act_format = VIL_PIXEL_FORMAT_BYTE;
  if (act_format == VIL_PIXEL_FORMAT_INT_16)
    act_format = VIL_PIXEL_FORMAT_UINT_16;
#endif //NO_OFFSET
  bool swap_data = false;
  unsigned short* temp1 = new unsigned short[num_samples];
  unsigned short* temp2 =
    reinterpret_cast<unsigned short*>(pixel_data->getData());
  swap_shorts(temp2, temp1, num_samples);
  vxl_byte* temp3 = reinterpret_cast<vxl_byte*>(temp1);
#endif //MIXED_ENDIAN
  // On error, return without doing anything
  if ( pixel_data == 0 ) {
    return;
  }

  // The data has been copied and converted. Release the source.
  pixels->clear();

  // Now, the actual buffer is good, or else we need to rescale
  //
  if ( slope == 1 && intercept == 0 ) {
    out_format = act_format;

    // There isn't an easy way to wrap a vil_memory_chunk around the
    // converted pixel data, since we don't know the array type. If we
    // could pass a void* to vil_memory_chunk, it will call the delete
    // the buffer assuming a char[] array, which it may not be. This
    // causes undefined behaviour. For the moment, we take the hit and
    // do a memcpy.
    //
    out_buf = new vil_memory_chunk( num_samples * ((stored+7)/8), VIL_PIXEL_FORMAT_BYTE );
#ifdef MIXED_ENDIAN
    std::memcpy( out_buf->data(), temp3, out_buf->size() );
#else
    std::memcpy( out_buf->data(), pixel_data->getData(), out_buf->size() );
#endif //MIXED_ENDIAN
  }
  else {
    out_buf = new vil_memory_chunk( num_samples * sizeof(float), VIL_PIXEL_FORMAT_FLOAT );
    out_format = VIL_PIXEL_FORMAT_FLOAT;
#ifdef MIXED_ENDIAN
    void* in_begin = reinterpret_cast<void*>(temp1);
#else
    void* in_begin = const_cast<void*>(pixel_data->getData());
#endif //MIXED_ENDIAN
    float* out_begin = static_cast<float*>( out_buf->data() );

    switch ( act_format )
    {
     case VIL_PIXEL_FORMAT_BYTE:
      rescale_values( (vxl_byte*)in_begin, num_samples, out_begin, slope, intercept );
      break;
     case VIL_PIXEL_FORMAT_SBYTE:
      rescale_values( (vxl_sbyte*)in_begin, num_samples, out_begin, slope, intercept );
      break;
     case VIL_PIXEL_FORMAT_UINT_16:
      rescale_values( (vxl_uint_16*)in_begin, num_samples, out_begin, slope, intercept );
      break;
     case VIL_PIXEL_FORMAT_INT_16:
      rescale_values( (vxl_sint_16*)in_begin, num_samples, out_begin, slope, intercept );
      break;
     default:
      std::cerr << "vil_dicom ERROR: unexpected internal pixel format\n";
    }
  }

#ifdef MIXED_ENDIAN
    delete [] temp1;
#endif //MIXED_ENDIAN

  delete pixel_data;
}

#endif // HAS_DCMTK
