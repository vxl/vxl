// This is core/vil/file_formats/vil_dicom.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil_dicom.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_vector.h>

#include <vxl_config.h> // for vxl_byte and such

#include <vil/vil_property.h>
#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>

#include <dcfilefo.h>
#include <dcmetinf.h>
#include <dcdatset.h>
#include <dctagkey.h>
#include <dcdeftag.h>
#include <dcstack.h>

#include "vil_dicom_stream.h"

char const* vil_dicom_format_tag = "dicom";


vil_image_resource_sptr vil_dicom_file_format::make_input_image(vil_stream* vs)
{
  bool is_dicom = false;

  char magic[ DCM_MagicLen ];
  vs->seek( DCM_PreambleLen );
  if ( vs->read( magic, DCM_MagicLen ) == DCM_MagicLen ) {
    if ( vcl_strncmp( magic, DCM_Magic, DCM_MagicLen ) == 0 ) {
      is_dicom = true;
    }
  }

  if ( is_dicom ) {
    return new vil_dicom_image( vs );
  } else {
    return 0;
  }
}

vil_image_resource_sptr vil_dicom_file_format::make_output_image(vil_stream* /*vs*/,
                                                                 unsigned /*ni*/,
                                                                 unsigned /*nj*/,
                                                                 unsigned /*nplanes*/,
                                                                 vil_pixel_format )
{
  vcl_cerr << "ERROR: vil_dicom_file doesn't support output yet\n";
  return 0;
#if 0

  if (nplanes != 1 || format != VIL_PIXEL_FORMAT_INT_32)
  {
    vcl_cerr << "ERROR: vil_dicom_file_format::make_output_image\n"
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
read_pixels_into_buffer( DcmElement* pixels,
                         unsigned num_samples,
                         Uint16 alloc,
                         Uint16 stored,
                         Uint16 high,
                         Uint16 rep,
                         Float64 slope,
                         Float64 intercept,
                         vil_memory_chunk_sptr& out_buf,
                         vil_pixel_format& out_format );



vil_dicom_image::vil_dicom_image(vil_stream* vs)
  : pixels_( 0 )
{
  vil_dicom_header_info_clear( header_ );

  vil_dicom_stream_input dcis( vs );

  DcmFileFormat ffmt;
  ffmt.transferInit();
  OFCondition cond = ffmt.read( dcis );
  ffmt.transferEnd();

  if( cond != EC_Normal ) {
    vcl_cerr << "vil_dicom ERROR: could not read file (" << cond.text() << ")\n";
    return;
  }

  DcmDataset& dset = *ffmt.getDataset();

  read_header( &dset, header_ );

  // I don't know (yet) how to deal with look up tables. (Without the
  // tables, the pixel values represent actual measurements.)
  //
  if( dset.tagExists( DCM_ModalityLUTSequence ) ) {
    vcl_cerr << "vil_dicom ERROR: don't know (yet) how to handle modality LUTs\n";
    return;
  }

  // If no ModalityLUT is present, I think the slope and intercept are
  // required. However, it seems that some DICOM images don't, so
  // assume the identity relationship.
  //
  Float64 slope, intercept;
  if( dset.findAndGetFloat64( DCM_RescaleSlope, slope ) != EC_Normal )
    slope = 1;
  if( dset.findAndGetFloat64( DCM_RescaleIntercept, intercept ) != EC_Normal )
    intercept = 0;


  // Gather the storage format info

#define Stringify( v ) #v
#define MustRead( func, key, var )                                                      \
    do{                                                                                 \
    if( dset. func( key, var ) != EC_Normal ) {                                         \
      vcl_cerr << "vil_dicom ERROR: couldn't read " Stringify(key) "; can't handle\n";  \
      return;                                                                           \
    }}while(0)

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
  vil_pixel_format pixel_format;

  // First, read in the bytes and interpret them appropriately
  //
  {
    DcmElement* pixels = 0;
    DcmStack stack;
    if ( dset.search( DCM_PixelData, stack, ESM_fromHere, true ) == EC_Normal ) {
      if ( stack.card() == 0 ) {
        vcl_cerr << "vil_dicom ERROR: no pixel data found\n";
        return;
      } else {
        pixels = static_cast<DcmElement*>(stack.top());
        if ( pixels->getVM() != 1 ) {
          vcl_cerr << "vil_dicom ERROR: no pixel data has VM != 1. Can't handle\n";
          return;
        }
      }
    }
    unsigned num_samples = ni() * nj() * nplanes();
    read_pixels_into_buffer( pixels, num_samples,
                             bits_alloc, bits_stored, high_bit, pixel_rep,
                             slope, intercept,
                             pixel_buf, pixel_format );
  }

  // Determine the plane order
  //
  Uint16 planar_config;
  vcl_ptrdiff_t plane_step, i_step, j_step;
  if( dset.findAndGetUint16( DCM_PlanarConfiguration, planar_config ) == EC_Normal &&
      planar_config == 1 ) {
    i_step = 1;
    j_step = ni();
    plane_step = ni() * nj();
  } else {
    i_step = nplanes();
    j_step = ni() * nplanes();
    plane_step = 1;
  }

  // Create an image resource to manage the pixel buffer
  //
#define DOCASE( fmt )                                                   \
      case fmt: {                                                       \
        typedef vil_pixel_format_type_of<fmt>::component_type T;        \
        pixels_ = vil_new_image_resource_of_view(                       \
               vil_image_view<T>( pixel_buf,                            \
                                  (T*)pixel_buf->data(),                \
                                  ni(), nj(), nplanes(),                \
                                  nplanes(), ni()*nplanes(), 1 ) );     \
      }break

  switch( pixel_format ) {
    DOCASE( VIL_PIXEL_FORMAT_UINT_16 );
    DOCASE( VIL_PIXEL_FORMAT_INT_16 );
    DOCASE( VIL_PIXEL_FORMAT_BYTE );
    DOCASE( VIL_PIXEL_FORMAT_SBYTE );
    DOCASE( VIL_PIXEL_FORMAT_FLOAT );
    default:
      vcl_cerr << "vil_dicom ERROR: unexpected pixel format\n";
  }
#undef DOCASE
}


bool vil_dicom_image::get_property(char const* tag, void* value) const
{
  if (vcl_strcmp(vil_property_quantisation_depth, tag)==0)
  {
    if (value)
      *static_cast<unsigned int*>(value) = header_.stored_bits_;
    return true;
  }

  if (vcl_strcmp(vil_property_pixel_size, tag)==0 && value!=0)
  {
    float *pixel_size = static_cast<float*>(value);
    pixel_size[0] = header_.spacing_x_ / 1000.0f;
    pixel_size[0] = header_.spacing_y_ / 1000.0f;
    return true;
  }

  // Need to write lots of access code for the dicom header.
  return false;
}

char const* vil_dicom_image::file_format() const
{
  return vil_dicom_format_tag;
}

vil_dicom_image::vil_dicom_image(vil_stream* vs, unsigned ni, unsigned nj,
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
  return pixels_->pixel_format();
}


vil_image_view_base_sptr vil_dicom_image::get_copy_view(
  unsigned x0, unsigned nx, unsigned y0, unsigned ny) const
{
  return pixels_->get_copy_view( x0, nx, y0, ny );
}


vil_image_view_base_sptr vil_dicom_image::get_view(
  unsigned x0, unsigned nx, unsigned y0, unsigned ny) const
{
  return pixels_->get_view( x0, nx, y0, ny );
}


bool vil_dicom_image::put_view(const vil_image_view_base& view,
                                unsigned x0, unsigned y0)
{
  assert(!"vil_dicom_image doesn't yet support output yet");

  if (!view_fits(view, x0, y0))
  {
    vcl_cerr << "ERROR: " << __FILE__ << ": view does not fit\n";
    return false;
  }
  return false;
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
  if ( dset->search( key, stack, ESM_fromHere, true ) == EC_Normal ) {
    if ( stack.card() == 0 ) {
      vcl_cerr << "vil_dicom ERROR: no results on stack\n";
    } else {
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
namespace {


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
  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vcl_string& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      OFString str;
      if ( e->getOFString( str, 0 ) != EC_Normal ) {
        vcl_cerr << "vil_dicom ERROR: value of ("<<group<<','<<element<<") is not string\n";
      } else {
        value = str.c_str();
      }
    }
  }
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_AE >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_AS >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_AT >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_CS >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_DA >
{
  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, long& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      OFString str;
      if ( e->getOFString( str, 0 ) != EC_Normal ) {
        vcl_cerr << "ERROR: value of ("<<group<<','<<element<<") is not string\n";
      } else {
        value = vcl_atol( str.c_str() );
      }
    }
  }
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_DS >
{
  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, float& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      OFString str;
      if ( e->getOFString( str, 0 ) != EC_Normal ) {
        vcl_cerr << "vil_dicom ERROR: value of ("<<group<<','<<element<<") is not string\n";
      } else {
        value = static_cast<float>( vcl_atof( str.c_str() ) );
      }
    }
  }

  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vcl_vector<float>& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      for ( unsigned pos = 0; pos < e->getVM(); ++pos ) {
        OFString str;
        if ( e->getOFString( str, pos ) != EC_Normal ) {
          vcl_cerr << "ERROR: value of ("<<group<<','<<element<<") at " << pos << " is not string\n";
        } else {
          value.push_back( static_cast<float>( vcl_atof( str.c_str() ) ) );
        }
      }
    }
  }
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_FD >
{
  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_ieee_64& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      if ( e->getFloat64( value ) != EC_Normal ) {
        vcl_cerr << "ERROR: value of ("<<group<<','<<element<<") is not Float64\n";
      }
    }
  }
};

VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_FL >
{
  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_ieee_32& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      if ( e->getFloat32( value ) != EC_Normal ) {
        vcl_cerr << "ERROR: value of ("<<group<<','<<element<<") is not Float32\n";
      }
    }
  }
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_IS >
{
  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, long& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      OFString str;
      if ( e->getOFString( str, 0 ) != EC_Normal ) {
        vcl_cerr << "vil_dicom ERROR: value of ("<<group<<','<<element<<") is not string\n";
      } else {
        value = vcl_atol( str.c_str() );
      }
    }
  }
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_LO >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_LT >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_OB >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_OW >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_PN >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_SH >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_SL >
{
  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_sint_32& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      if ( e->getSint32( reinterpret_cast<Sint32&>(value) ) != EC_Normal ) {
        vcl_cerr << "vil_dicom ERROR: value of ("<<group<<','<<element<<") is not Sint32\n";
      }
    }
  }
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_SQ >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_SS >
{
  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_sint_16& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      if ( e->getSint16( reinterpret_cast<Sint16&>(value) ) != EC_Normal ) {
        vcl_cerr << "vil_dicom ERROR: value of ("<<group<<','<<element<<") is not Sint16\n";
      }
    }
  }
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_ST >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_TM >
{
  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, float& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      OFString str;
      if ( e->getOFString( str, 0 ) != EC_Normal ) {
        vcl_cerr << "vil_dicom ERROR: value of ("<<group<<','<<element<<") is not string\n";
      } else {
        value = static_cast<float>( vcl_atof( str.c_str() ) );
      }
    }
  }
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_UI >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_UL >
{
  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_uint_32& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      if ( e->getUint32( reinterpret_cast<Uint32&>(value) ) != EC_Normal ) {
        vcl_cerr << "vil_dicom ERROR: value of ("<<group<<','<<element<<") is not Uint32\n";
      }
    }
  }
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_UN >
  : public try_set_to_string
{
};


VCL_DEFINE_SPECIALIZATION
struct try_set< vil_dicom_header_US >
{
  static void proc( DcmObject* dset, vxl_uint_16 group, vxl_uint_16 element, vxl_uint_16& value ) {
    DcmElement* e = find_element( dset, group, element );
    if ( e ) {
      if ( e->getUint16( value ) != EC_Normal ) {
        vcl_cerr << "vil_dicom ERROR: value of ("<<group<<','<<element<<") is not Uint16\n";
      }
    }
  }
};


VCL_DEFINE_SPECIALIZATION
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
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQECHOTRAINLENGTH),       i.echo_train_length_ ); // It's the echo train length
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQPIXELBANDWIDTH),        i.pixel_bandwidth_ ); // It's the pixel bandwidth
  try_set< ap_type(LO) >::proc( f, group, ap_el(AQSOFTWAREVERSION),       i.software_vers_ ); // It's the scanner software version
  try_set< ap_type(LO) >::proc( f, group, ap_el(AQPROTOCOLNAME),          i.protocol_name_ ); // It's the protocol name
  try_set< ap_type(IS) >::proc( f, group, ap_el(AQHEARTRATE),             i.heart_rate_ ); // It's the heart rate
  try_set< ap_type(IS) >::proc( f, group, ap_el(AQCARDIACNUMBEROFIMAGES), i.card_num_images_ ); // It's the cardiac number of images
  try_set< ap_type(IS) >::proc( f, group, ap_el(AQTRIGGERWINDOW),         i.trigger_window_ ); // It's the trigger window
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQRECONTRUCTIONDIAMETER), i.reconst_diameter_ ); // It's the reconstruction diameter
  try_set< ap_type(SH) >::proc( f, group, ap_el(AQRECEIVINGCOIL),         i.receiving_coil_ ); // It's the receiving coil
  try_set< ap_type(CS) >::proc( f, group, ap_el(AQPHASEENCODINGDIRECTION),i.phase_enc_dir_ ); // It's the phase encoding direction
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQFLIPANGLE),             i.flip_angle_ ); // It's the flip angle
  try_set< ap_type(DS) >::proc( f, group, ap_el(AQSAR),                   i.sar_ ); // It's the sar
  try_set< ap_type(CS) >::proc( f, group, ap_el(AQPATIENTPOSITION),       i.patient_pos_ ); // It's the patient position

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

  typedef vil_dicom_header_type_of<vil_dicom_header_DS>::type DS_type;
  vcl_vector<DS_type> ps;
  try_set< ap_type(DS) >::proc( f, group, ap_el(IMPIXELSPACING), ps );
  if ( ps.size() > 0 )
    i.spacing_x_ = ps[0];
  else
    i.spacing_x_ = 0;
  if ( ps.size() > 1 )
    i.spacing_y_ = ps[1];
  else
    i.spacing_y_ = i.spacing_x_;

#undef ap_join
#undef ap_type
#undef ap_el

}


// start anonymous namespace for helpers
namespace {

template<class InType, class OutType>
void
convert_unsigned_int(
      InType const* in_buf_begin,
      InType const* in_buf_end,
      OutType* out_buf,
      Uint16 alloc, Uint16 stored, Uint16 high,
      Uint16 rep )
{
  assert( stored <= sizeof(OutType)*8 );
  assert( alloc <= sizeof(InType)*8 );
  assert( rep == 0 );
  
  Uint16 right_shift = high + 1 - stored;
  InType mask = 0xFFFF >> ( alloc - stored );

  InType const* inp = in_buf_begin;
  OutType* outp = out_buf;
  for( ; inp != in_buf_end; ++inp, ++outp ) {
      *outp = ( *inp >> right_shift ) & mask;
  }
}


template<class InType, class OutType>
void
convert_signed_int(
      InType const* in_buf_begin,
      InType const* in_buf_end,
      OutType* out_buf,
      Uint16 alloc, Uint16 stored, Uint16 high,
      Uint16 rep )
{
  assert( stored <= sizeof(OutType)*8 );
  assert( alloc <= sizeof(InType)*8 );
  assert( rep == 1 );
  
  Uint16 right_shift = high + 1 - stored;
  InType mask = 0xFFFF >> ( alloc - stored );
  InType sign_bit = 1 << (stored-1);
  InType value_mask = mask >> 1; // 

  InType const* inp = in_buf_begin;
  OutType* outp = out_buf;
  for( ; inp != in_buf_end; ++inp, ++outp ) {
    InType v = *inp >> right_shift;
    if( (v & sign_bit) == 0 )
      *outp = v & mask;
    else
      // DICOM signed values are stored as 2s complement
      *outp = - ( ( ~v & mask ) + 1 );
  }
}


template<class SrcType>
void
convert_src_type( SrcType const* src_buf_begin,
                  unsigned num_samples,
                  Uint16 alloc,
                  Uint16 stored,
                  Uint16 high,
                  Uint16 rep,
                  vil_memory_chunk_sptr& act_buf,
                  vil_pixel_format& act_format )
{
  SrcType const* src_buf_end   = src_buf_begin + num_samples;

  unsigned act_bytes_per_samp = ( stored+7 ) / 8;
  act_buf = new vil_memory_chunk( num_samples * act_bytes_per_samp, VIL_PIXEL_FORMAT_BYTE );

  if( rep == 0 ) {
    if( act_bytes_per_samp == 1 ) {
      convert_unsigned_int( src_buf_begin, src_buf_end,
                            static_cast<vxl_byte*>( act_buf->data() ),
                            alloc, stored, high, rep );
      act_format = VIL_PIXEL_FORMAT_BYTE;
    } else {
      assert( act_bytes_per_samp == 2 );
      convert_unsigned_int( src_buf_begin, src_buf_end,
                            static_cast<vxl_uint_16*>( act_buf->data() ),
                            alloc, stored, high, rep );
      act_format = VIL_PIXEL_FORMAT_UINT_16;
    }
  } else {
    if( act_bytes_per_samp == 1 ) {
      convert_signed_int( src_buf_begin, src_buf_end,
                          static_cast<vxl_sbyte*>( act_buf->data() ),
                          alloc, stored, high, rep );
      act_format = VIL_PIXEL_FORMAT_SBYTE;
    } else {
      assert( act_bytes_per_samp == 2 );
      convert_signed_int( src_buf_begin, src_buf_end,
                          static_cast<vxl_sint_16*>( act_buf->data() ),
                          alloc, stored, high, rep );
      act_format = VIL_PIXEL_FORMAT_INT_16;
    }
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
  for( ; int_begin != int_end; ++int_begin, ++float_begin ) {
    *float_begin = static_cast<OutType>( *int_begin * slope + intercept );
  } 
}

} // anonymous namespace



static
void
read_pixels_into_buffer( DcmElement* pixels,
                         unsigned num_samples,
                         Uint16 alloc,
                         Uint16 stored,
                         Uint16 high,
                         Uint16 rep,
                         Float64 slope,
                         Float64 intercept,
                         vil_memory_chunk_sptr& out_buf,
                         vil_pixel_format& out_format )
{
  // This will be the "true" pixel buffer after the overlay planes are
  // removed and the pixel bits shifted to the lowest bits of the
  // bytes.
  //
  vil_pixel_format act_format = VIL_PIXEL_FORMAT_UNKNOWN;
  vil_memory_chunk_sptr act_buf = 0;

  // First convert from the stored src pixels to the actual
  // pixels. This is an integral type to integral type conversion.
  //
  if( pixels->getVR() == EVR_OW ) {
    Uint16* src_buf;
    if( pixels->getUint16Array( src_buf ) != EC_Normal )
      return;
    convert_src_type( src_buf, num_samples, alloc, stored, high, rep, act_buf, act_format );
  } else {
    // assume OB
    Uint8* src_buf;
    if( pixels->getUint8Array( src_buf ) != EC_Normal )
      return;
    convert_src_type( src_buf, num_samples, alloc, stored, high, rep, act_buf, act_format );
  }

  // We've copied and converted the data. Release the source.
  pixels->clear();

  // Now, the actual buffer is good, or else we need to rescale
  //
  if( slope == 1 && intercept == 0 ) {
    out_format = act_format;
    out_buf = act_buf;
  } else {
    if( act_buf->size() == sizeof(float) * num_samples )
      out_buf = act_buf;
    else
      out_buf = new vil_memory_chunk( num_samples * sizeof(float), VIL_PIXEL_FORMAT_FLOAT );
    out_format = VIL_PIXEL_FORMAT_FLOAT;

    char* in_begin = static_cast<char*>( act_buf->data() );
    float* out_begin = static_cast<float*>( out_buf->data() );

    switch( act_format ) {
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
        vcl_cerr << "vil_dicom ERROR: unexpected internal pixel format\n";
        return;
    }
  }
}
