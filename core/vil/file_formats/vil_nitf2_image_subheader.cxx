// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_image_subheader.h"

#include "vil_nitf2_data_mask_table.h"

#include "vil_nitf2_field_functor.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_typed_field_formatter.h"
#include "vil_nitf2_index_vector.h"
// for TREs
#include "vil_nitf2_tagged_record_definition.h"

#include <vcl_sstream.h>
#include <vcl_cassert.h>

vil_nitf2_field_definitions* vil_nitf2_image_subheader::s_field_definitions_21 = 0;
vil_nitf2_field_definitions* vil_nitf2_image_subheader::s_field_definitions_20 = 0;

vil_nitf2_image_subheader::vil_nitf2_image_subheader(vil_nitf2_classification::file_version version)
  : m_field_sequence(version == vil_nitf2_classification::V_NITF_20 ?
                     *get_field_definitions_20() :
                     *get_field_definitions_21()),
    m_data_mask_table(0),
    m_version(version)
{ add_rpc_definitions(); }

vil_nitf2_image_subheader::~vil_nitf2_image_subheader()
{
  if (m_data_mask_table) delete m_data_mask_table;
  vil_nitf2_tagged_record_definition::undefine("RPC00A");
  vil_nitf2_tagged_record_definition::undefine("RPC00B");
}

bool vil_nitf2_image_subheader::read(vil_stream* stream)
{
  bool success = m_field_sequence.read(*stream);

  if (success) {
    	
    // If this image has a data mask, we need to parse that too
    vcl_string compression_code;
    get_property("IC", compression_code);
    if (compression_code.find_first_of("M") != vcl_string::npos) {
      vcl_string imode;
      get_property("IMODE", imode);
      m_data_mask_table = new vil_nitf2_data_mask_table(
                                                        get_num_blocks_x(), get_num_blocks_y(), nplanes(), imode);
      if (!m_data_mask_table->parse(stream)) return false;
    }
  }
  return success;
}

const vil_nitf2_field_definitions* vil_nitf2_image_subheader::get_field_definitions_21()
{
  if (!s_field_definitions_21) {
    s_field_definitions_21 = new vil_nitf2_field_definitions();
    add_shared_field_defs_1(s_field_definitions_21);
    vil_nitf2_classification::add_field_defs(s_field_definitions_21, vil_nitf2_classification::V_NITF_21, "I", "Image");
    add_shared_field_defs_2(s_field_definitions_21);
    add_geo_field_defs(s_field_definitions_21, vil_nitf2_classification::V_NITF_21);
    add_shared_field_defs_3(s_field_definitions_21);
  }
  return s_field_definitions_21;
}

const vil_nitf2_field_definitions* vil_nitf2_image_subheader::get_field_definitions_20()
{
  if (!s_field_definitions_20) {
    s_field_definitions_20 = new vil_nitf2_field_definitions();
    add_shared_field_defs_1(s_field_definitions_20);
    vil_nitf2_classification::add_field_defs(s_field_definitions_20, vil_nitf2_classification::V_NITF_20, "I", "Image");
    add_shared_field_defs_2(s_field_definitions_20);
    add_geo_field_defs(s_field_definitions_20, vil_nitf2_classification::V_NITF_20);
    add_shared_field_defs_3(s_field_definitions_20);
  }
  return s_field_definitions_20;
}

void vil_nitf2_image_subheader::
add_shared_field_defs_3(vil_nitf2_field_definitions* defs)
{
  (*defs)
    .field("NICOM", "Number of Image Comments", NITF_INT(1), false, 0, 0)

    //TODO: does it make any sense for a vcl_vector to have blank entries????  For now, I'm saying no (false parameter)
    .repeat("NICOM", vil_nitf2_field_definitions()

            .field("ICOMn", "Image Comment n", NITF_STR_ECSA(80), false, 0, 0))

    .field("IC", "Image Compression",
           NITF_ENUM(2, vil_nitf2_enum_values()
                     .value("NC", "Not compressed")
                     .value("NM", "Uncompressed - contains block mask and/or pad pixel mask")
                     .value("C1", "Bi-level")
                     .value("C3", "JPEG")
                     .value("C4", "Vector Quantization")
                     .value("C5", "Lossless JPEG")
                     .value("C6", "Reserved - future correlated multicomponent compression")
                     .value("C7", "Reserved - future SAR compression")
                     .value("C8", "JPEG2000")
                     .value("I1", "Downsampled JPEG")
                     .value("M1", "Compressed - contains block mask and/or pad pixel mask")
                     .value("M3", "Compressed - contains block mask and/or pad pixel mask")
                     .value("M4", "Compressed - contains block mask and/or pad pixel mask")
                     .value("M5", "Compressed - contains block mask and/or pad pixel mask")
                     .value("M6", "Reserved - future correlated multicomponent compression")
                     .value("M7", "Reserved - future SAR compression")
                     .value("M8", "JPEG2000 - contains block mask and/or pad pixel mask")),
           false, 0, 0);

  vcl_vector<vcl_string> comp_ic_values;
  comp_ic_values.push_back("C1");
  comp_ic_values.push_back("C3");
  comp_ic_values.push_back("C4");
  comp_ic_values.push_back("C5");
  comp_ic_values.push_back("C8");
  comp_ic_values.push_back("M1");
  comp_ic_values.push_back("M3");
  comp_ic_values.push_back("M4");
  comp_ic_values.push_back("M5");
  comp_ic_values.push_back("M8");
  comp_ic_values.push_back("I1");

  // Using string because the valid enum values are different based on the content of IC
  (*defs)
    .field("COMRAT", "Compression Rate Code",         NITF_STR_BCSA(4),true, 0,
           new vil_nitf2_field_value_one_of<vcl_string>("IC", comp_ic_values))

    // The value of IREP determines which values are acceptable here
    // (e.g., if IREP=MONO, then this must equal 1)
    .field("NBANDS", "Number of Bands",               NITF_INT(1), false, 0, 0)

    // Acceptable range [00010-99999]... only used if NBANDS=0
    .field("XBANDS", "Number of multispectral bands", NITF_INT(5), true, 0,
           new vil_nitf2_field_value_one_of<int>("NBANDS",0))

    .repeat(new vil_nitf2_choose_field_value<int>(
                                                  "NBANDS", "XBANDS", new vil_nitf2_field_value_greater_than<int>("NBANDS", 0)),
            vil_nitf2_field_definitions()

            .field("IREPBAND", "nth Band Representation",             NITF_STR_BCSA(2), true, 0, 0)
            .field("ISUBCAT",  "nth Band Subcategory",                NITF_STR_BCSA(6), true, 0, 0)
            .field("IFC",      "nth Image Filter Condition",          NITF_STR_BCSA(1), false, 0, 0)
            .field("IMFLT",    "nth Band Standard Image Filter Code", NITF_STR_BCSA(3), true, 0, 0)
            .field("NLUTS",    "Number of LUTS for the nth Image Band",        NITF_INT(1, false),  false, 0, 0)
            .field("NELUT",    "Number of LUT Entries for the nth Image Band", NITF_INT(5, false),  false, 0,
                   new vil_nitf2_field_value_greater_than<int>("NLUTS", 0) /*condition*/ )

            .repeat(new vil_nitf2_field_value<int>("NLUTS"), vil_nitf2_field_definitions()

                    .field("LUTDnm", "nth Image Band, mth LUT",             NITF_BIN(1), false,
                           new vil_nitf2_field_value<int>("NELUT"), 0))
            )

    .field("ISYNC", "Image Sync Code", NITF_INT(1),  false, 0, 0)
    .field("IMODE", "Image Mode",
           NITF_ENUM(1, vil_nitf2_enum_values()
                     .value("B", "Band interleaved by block")
                     .value("P", "Band interleaved by pixel")
                     .value("R", "Band interleaved by row")
                     .value("S", "Band sequential")),
           false, 0, 0)

    .field("NBPR",  "Number of Blocks per Row",              NITF_INT(4), false, 0, 0)
    .field("NBPC",  "Number of Blocks per Column",           NITF_INT(4), false, 0, 0)
    .field("NPPBH", "Number of Pixels per Block Horizontal", NITF_INT(4), false, 0, 0) //[0000-8192]
    .field("NPPBV", "Number of Pixels per Block Vertical",   NITF_INT(4), false, 0, 0) //[0000-8192]
    .field("NBPP",  "Number of Bits per Pixel per Band",     NITF_INT(2), false, 0, 0) //[01-96]
    .field("IDLVL", "Image Display Level",                   NITF_INT(3), false, 0, 0)
    .field("IALVL", "Attachment Level",                      NITF_INT(3), false, 0, 0) //[000-998]

    // TODO: Enter these two values as ints with optional sign (once supported)
    // Actually, since they are both in one field, maybe we'd want to make it one
    // compound field (in the new tree format)
    .field("ILOC_ROW", "Image Location Row",            NITF_STR_BCSA(5), false, 0, 0)
    .field("ILOC_COL", "Image Location Column",         NITF_STR_BCSA(5), false, 0, 0)

    // TODO: THis should be a decimal field that supports non-negative power of two
    // fractions (eg '/2  ' means 1/2, '/16 'means 1/16
    .field("IMAG",  "Image Magnification",              NITF_STR_BCSA(4), false, 0, 0)
    .field("UDIDL", "User Defined Image Data Length",        NITF_INT(5), false, 0, 0) //[00000,00003-99999]

    .field("UDOFL", "User Defined Overflow",                 NITF_INT(3), false, 0,
           new vil_nitf2_field_value_greater_than<int>("UDIDL", 0))
 
    .field("UDID", "User Defined Image Data",                NITF_TRES(), false,
           new vil_nitf2_max_field_value_plus_offset_and_threshold("UDIDL", -3), 0)
 
    .field("IXSHDL", "Image Extended Subheader Data Length", NITF_INT(5), false, 0, 0)

    .field("IXSOFL", "Image Extended Subheader Overflow",    NITF_INT(3), false, 0,
           new vil_nitf2_field_value_greater_than<int>("IXSHDL", 0))
  
    .field("IXSHD", "Image Extended Subheader Data",         NITF_TRES(), false,
           new vil_nitf2_max_field_value_plus_offset_and_threshold("IXSHDL", -3), 0);
}

void vil_nitf2_image_subheader::add_geo_field_defs(vil_nitf2_field_definitions* defs,
                                                   const vil_nitf2_classification::file_version& version)
{
  switch (version) {
  case vil_nitf2_classification::V_NITF_20:
    {
      (*defs)
        .field("ICORDS", "Image Coordinate Representation",

               NITF_ENUM(1, vil_nitf2_enum_values()
                         .value("U", "UTM")
                         .value("G", "Geodetic/Geographic")
                         .value("N", "None")
                         .value("C", "Geocentric")),

               false, 0, 0);

      vcl_vector<vcl_string> igeolo_icords;
      igeolo_icords.push_back("U");
      igeolo_icords.push_back("G");
      igeolo_icords.push_back("C");

      (*defs)
        .field("IGEOLO", "Image Geographic Location", NITF_STR_BCSA(60), false, 0,
               new vil_nitf2_field_value_one_of<vcl_string>("ICORDS", igeolo_icords));
      break;
    }
  case vil_nitf2_classification::V_NITF_21:
    {
      (*defs)
        .field("ICORDS", "Image Coordinate Representation",

               NITF_ENUM(1, vil_nitf2_enum_values()
                         .value("U", "UTM expressed in MGRS form")
                         .value("G", "Geographic")
                         .value("N", "UTM/UPS (Northern hemisphere)") // actually means None for Nitf 2.0
                         .value("S", "UTM/UPS (Southern hemisphere)") // NITF 2.1 only
                         .value("D", "Decimal degrees")),             // NITF 2.1 only

               true, 0, 0)

        .field("IGEOLO", "Image Geographic Location", NITF_STR_BCSA(60), false, 0,
               new vil_nitf2_field_specified("ICORDS"));
    }
    break;
  default:
    assert(0);
  } // end switch
}

void vil_nitf2_image_subheader::add_shared_field_defs_2(vil_nitf2_field_definitions* defs)
{
  (*defs)
    .field("ENCRYP", "Encryption",
           NITF_ENUM(1, vil_nitf2_enum_values()
                     // Only one valid value, until the spec gets updated
                     .value("0", "Not Encrypted")),
           false, 0, 0)
    .field("ISORCE", "Image Source", NITF_STR_ECSA(42),  true, 0, 0)
    //TODO: only allow range [00000001-99999999]
    .field("NROWS", "Number of Significant Rows in Image", NITF_INT(8, false), false, 0, 0)
    //TODO: only allow range [00000001-99999999]
    .field("NCOLS", "Number of Significant Columns in Image", NITF_INT(8, false), false, 0, 0)
    .field("PVTYPE", "Pixel Value Type",
           NITF_ENUM(3, vil_nitf2_enum_values()
                     .value("INT", "Integer")
                     .value("B",   "Bi-level")
                     .value("SI",  "2's complement signed integer")
                     .value("R",   "Real")
                     .value("C",   "Complex")),
           false, 0, 0)
    .field("IREP", "Image Representation",
           NITF_ENUM(8, vil_nitf2_enum_values()
                     .value("MONO",     "Monochrome")
                     .value("RGB",      "Red, green, blue true color")
                     .value("RGB/LUT",  "Red, green, blue mapped Color")
                     .value("MULTI",    "Multiband imagery")
                     .value("NODISPLY", "Not intended for display")
                     .value("NVECTOR",  "Cartesian coordinates")
                     .value("POLAR",    "Polar coordinates")
                     .value("VPH",      "SAR video phase history")
                     .value("YCbCr601", "BT.601-5 color space")),
           false, 0, 0)
#ifdef UNCLASS_ONLY
    .field("ICAT", "Image Category",
           NITF_ENUM(8, vil_nitf2_enum_values()
                     .value("VIS",    "Visible imagery")
                     .value("SL",     "Side-looking radar")
                     .value("TI",     "Thermal infrared")
                     .value("FL",     "Forward-looking radar")
                     .value("RD",     "Radar")
                     .value("EO",     "Electro-optical")
                     .value("OP",     "Optical")
                     .value("HR",     "High-resolution radar")
                     .value("HS",     "Hyperspectral")
                     .value("CP",     "Color frame photography")
                     .value("BP",     "Black/white frame photography")
                     .value("SARIQ",  "SAR radio hologram")
                     .value("SAR",    "Synthetic aperture radar")
                     .value("IR",     "Infrared")
                     .value("MS",     "Multispectral")
                     .value("FP",     "Fingerprints")
                     .value("MRI",    "Magnetic resonance imagery")
                     .value("XRAY",   "X-ray")
                     .value("CAT",    "CAT scan")
                     .value("VD",     "Video")
                     .value("BARO",   "Barometric pressure")
                     .value("CURRENT","Water current")
                     .value("DEPTH",  "Water depth")
                     .value("MAP",    "Raster map")
                     .value("PAT",    "Color patch")
                     .value("LEG",    "Legends")
                     .value("DTEM",   "Elevation models")
                     .value("MATR",   "Matrix data (other types)")
                     .value("LOCG",   "Location Grids")),
           false, 0, 0)
#else
    .field("ICAT", "Image Category", NITF_STR_ECSA(8), false, 0, 0)
#endif //UNCLASS_ONLY

    .field("ABPP", "Actual Bits Per Pixel per Band", NITF_INT(2), false, 0, 0) //[1-96]
    .field("PJUST", "Pixel Justification",
           NITF_ENUM(1, vil_nitf2_enum_values()
                     .value("L", "Left-justified")
                     .value("R", "Right-justified")),
           false, 0, 0);
}

void vil_nitf2_image_subheader::add_shared_field_defs_1(vil_nitf2_field_definitions* defs)
{
  (*defs)
    .field("IM", "File Part Type",
           NITF_ENUM(2, vil_nitf2_enum_values()
                     .value("IM", "Image Header")))
    .field("IID1", "Image Identifier 1", NITF_STR_BCSA(10))

    // order of data/time depends on NITF2.1 or NITF2.0, so just read in as string for now
    //.field("IDATIM", "Image Date and Time", NITF_DAT(14)));
    .field("IDATIM", "Image Date and Time", NITF_STR_BCSA(14))
    .field("TGTID",  "Target Identifier",   NITF_STR_BCSA(17), true)
    .field("IID2",   "Image Identifier 2",  NITF_STR_ECSA(80), true);
}

unsigned vil_nitf2_image_subheader::nplanes() const
{
  //the number of image bands is stored in NBANDS unless NBANDS=0.
  //in that case, then it's stored under XBANDS.  That's why we have
  //to potentially try both
  int numBands;
  if (get_property("NBANDS", numBands))
    {
      if (numBands > 0 ||
          get_property("XBANDS", numBands))
        {
          return numBands;
        }
    }
  return 0;
}

unsigned int vil_nitf2_image_subheader::get_pixels_per_block_x() const
{
  int pixels_per_block;
  if (get_property("NPPBH", pixels_per_block)) {
    return pixels_per_block;
  }
  return 0;
}

unsigned int vil_nitf2_image_subheader::get_pixels_per_block_y() const
{
  int pixels_per_block;
  if (get_property("NPPBV", pixels_per_block)) {
    return pixels_per_block;
  }
  return 0;
}

unsigned int vil_nitf2_image_subheader::get_num_blocks_x() const
{
  int blocks_per_row;
  if (get_property("NBPR", blocks_per_row)) {
    return blocks_per_row;
  }
  return 0;
}

unsigned int vil_nitf2_image_subheader::get_num_blocks_y() const
{
  int blocks_per_col;
  if (get_property("NBPC", blocks_per_col)) {
    return blocks_per_col;
  }
  return 0;
}

bool vil_nitf2_image_subheader::get_lut_info(unsigned int i,
                                             int& n_luts, int& ne_lut,
                                             vcl_vector< vcl_vector< unsigned char > >& lut_d) const
{
  // TO DO: test this!
  if (!m_field_sequence.get_value("NLUTS", vil_nitf2_index_vector(i), n_luts) ||
      !m_field_sequence.get_value("NELUT", vil_nitf2_index_vector(i), ne_lut)) {
    return false;
  }
  lut_d.clear();
  lut_d.resize(n_luts);
  for (int lut_index = 0 ; lut_index < n_luts ; lut_index++) {
    lut_d[lut_index].resize(ne_lut);
    for (int el_index = 0 ; el_index < ne_lut ; el_index++) {
      void* currValue;
      vil_nitf2_index_vector index(2);
      index[0]=i;
      index[1]=lut_index*ne_lut+el_index;
      if (m_field_sequence.get_value("LUTDnm", index, currValue)) {
        lut_d[lut_index][el_index] = *((char*)currValue);
      } else {
        return false; 
      }
    }
  }
  return true;
}

vil_nitf2_field::field_tree* vil_nitf2_image_subheader::get_tree( int i ) const
{
  vil_nitf2_field::field_tree* t = new vil_nitf2_field::field_tree;
  vcl_stringstream name_stream;
  name_stream << "Image Subheader";
  if( i > 0 ) name_stream << " #" << i;
  t->columns.push_back( name_stream.str() );
  m_field_sequence.get_tree( t );
  return t;
}

// Add definitions for rational polynomial coefficients
void vil_nitf2_image_subheader::add_rpc_definitions()
{
  vil_nitf2_tagged_record_definition* tr =
    vil_nitf2_tagged_record_definition::find("RPC00B");
  if(!tr)
    {
      vil_nitf2_tagged_record_definition::define("RPC00B", "Rational Polynomial Coefficients Type B" )
  
        // These are the "offset and scale" fields that precede the polynomial coefficients
        .field("SUCCESS",     "Success parameter",  NITF_INT(1))                 // not used, but must read
        .field("ERR_BIAS",    "ERR_BIAS",           NITF_DBL(7, 2, false), true) // not used, but must read 
        .field("ERR_RAND",    "ERR_RAND",           NITF_DBL(7, 2, false), true) // not used
        .field("LINE_OFF",    "Line Offset",        NITF_INT(6))
        .field("SAMP_OFF",    "Sample Offset",      NITF_INT(5))
        .field("LAT_OFF",     "Latitude Offset",    NITF_DBL(8, 4, true), false)
        .field("LON_OFF",     "Longitude offset",   NITF_DBL(9, 4, true), false)
        .field("HEIGHT_OFF",  "Height Offset",      NITF_INT(5, true))
        .field("LINE_SCALE",  "Line Scale",         NITF_INT(6))
        .field("SAMP_SCALE",  "Sample Scale",       NITF_INT(5))
        .field("LAT_SCALE",   "Latitude Scale",     NITF_DBL(8, 4, true), false)
        .field("LON_SCALE",   "Longitude Scale",    NITF_DBL(9, 4, true), false)
        .field("HEIGHT_SCALE", "Height Scale",      NITF_INT(5, true))

        // Now come the 4 sequential groups of (20 each) polynomial coefficients for line number,
        //   line density, sample number and sample density.
        // As these values are in <+/->n.nnnnnnE<+/->n (exponential) format, let's just read as
        //   strings now and convert into doubles later.
	
        .repeat(20, vil_nitf2_field_definitions()
                .field("LNC",        "Line Number Coefficient",    NITF_EXP(6,1))
                )
        .repeat(20, vil_nitf2_field_definitions()
                .field("LDC",        "Line Density Coefficient",   NITF_EXP(6,1))
                )
        .repeat(20, vil_nitf2_field_definitions()
                .field("SNC",        "Sample Number Coefficient",  NITF_EXP(6,1))
                )
        .repeat(20, vil_nitf2_field_definitions()
                .field("SDC",        "Sample Density Coefficient", NITF_EXP(6,1))
                )
        .end();  // of RPC TRE
    }
  tr =vil_nitf2_tagged_record_definition::find("RPC00A");
  if(!tr)
    {
      vil_nitf2_tagged_record_definition::define("RPC00A", "Rational Polynomial Coefficients Type A" )
  
        // These are the "offset and scale" fields that precede the polynomial coefficients
        .field("SUCCESS",     "Success parameter",  NITF_INT(1))                 // not used, but must read
        .field("ERR_BIAS",    "ERR_BIAS",           NITF_DBL(7, 2, false), true) // not used, but must read 
        .field("ERR_RAND",    "ERR_RAND",           NITF_DBL(7, 2, false), true) // not used
        .field("LINE_OFF",    "Line Offset",        NITF_INT(6))
        .field("SAMP_OFF",    "Sample Offset",      NITF_INT(5))
        .field("LAT_OFF",     "Latitude Offset",    NITF_DBL(8, 4, true), false)
        .field("LON_OFF",     "Longitude offset",   NITF_DBL(9, 4, true), false)
        .field("HEIGHT_OFF",  "Height Offset",      NITF_INT(5, true))
        .field("LINE_SCALE",  "Line Scale",         NITF_INT(6))
        .field("SAMP_SCALE",  "Sample Scale",       NITF_INT(5))
        .field("LAT_SCALE",   "Latitude Scale",     NITF_DBL(8, 4, true), false)
        .field("LON_SCALE",   "Longitude Scale",    NITF_DBL(9, 4, true), false)
        .field("HEIGHT_SCALE", "Height Scale",      NITF_INT(5, true))

        // Now come the 4 sequential groups of (20 each) polynomial coefficients for line number,
        //   line density, sample number and sample density.
        // As these values are in <+/->n.nnnnnnE<+/->n (exponential) format, let's just read as
        //   strings now and convert into doubles later.
	
        .repeat(20, vil_nitf2_field_definitions()
                .field("LNC",        "Line Number Coefficient",    NITF_EXP(6,1))
                )
        .repeat(20, vil_nitf2_field_definitions()
                .field("LDC",        "Line Density Coefficient",   NITF_EXP(6,1))
                )
        .repeat(20, vil_nitf2_field_definitions()
                .field("SNC",        "Sample Number Coefficient",  NITF_EXP(6,1))
                )
        .repeat(20, vil_nitf2_field_definitions()
                .field("SDC",        "Sample Density Coefficient", NITF_EXP(6,1))
                )
        .end();  // of RPCA TRE
    }
}

// Collect the RPC parameters for the current image. Image corners are reported
// as a string of geographic coordinates,one for each image corner.
  bool vil_nitf2_image_subheader::
get_rpc_params( vcl_string& rpc_type, vcl_string& image_id, 
                vcl_string& image_corner_geo_locations,
                double* rpc_data )
{
  
  // Get image ID and location from main header values
  vcl_string iid2 = "";
  bool success = this->get_property("IID2", iid2);
  if(!success){
    vcl_cout << "IID2 Property failed in vil_nitf2_image_subheader\n";
    return false;
  }
  image_id = iid2.substr(0,39);// trim length to NN characters to match file ID
  vcl_string igeolo = "";
  success = this->get_property("IGEOLO", igeolo);
  if(!success){
    vcl_cout << "IGEOLO Property failed in vil_nitf2_image_subheader\n";
    return false;
  }
  image_corner_geo_locations = igeolo;

  // Now get the sub-header TRE parameters
  vil_nitf2_tagged_record_sequence isxhd_tres;
  vil_nitf2_tagged_record_sequence::iterator tres_itr;
  this->get_property("IXSHD", isxhd_tres);

  // Check through the TREs to find "RPC"
  for (tres_itr = isxhd_tres.begin(); tres_itr != isxhd_tres.end(); ++tres_itr) 
    {
      vcl_string type = (*tres_itr)->name();
      if ( type == "RPC00B" || type == "RPC00A")// looking for "RPC..."
        {
          // set type in return value
          rpc_type = type;
	  
          // get offsets and scales, print a couple to make sure values are correct
          int line_off;
          success = (*tres_itr)->get_value("LINE_OFF", line_off);
          if(!success){
            vcl_cout << "LINE_OFF Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          rpc_data[80] = line_off;

          int samp_off;
          success = (*tres_itr)->get_value("SAMP_OFF", samp_off);
          if(!success){
            vcl_cout << "SAMP_OFF Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          rpc_data[81]  = samp_off;

          double lat_off;
          success = (*tres_itr)->get_value("LAT_OFF", lat_off);
          if(!success){
            vcl_cout << "LAT_OFF Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          rpc_data[82] = lat_off;

          double lon_off;
          success = (*tres_itr)->get_value("LON_OFF", lon_off);
          if(!success){
            vcl_cout << "LON_OFF Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          rpc_data[83] = lon_off;

          int height_off;
          success = (*tres_itr)->get_value("HEIGHT_OFF", height_off);
          if(!success){
            vcl_cout << "HEIGHT_OFF Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          rpc_data[84] = height_off;

          int line_scale;
          success = (*tres_itr)->get_value("LINE_SCALE", line_scale);
          if(!success){
            vcl_cout << "LINE_SCALE Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          rpc_data[85] = line_scale;

          int samp_scale;
          success = (*tres_itr)->get_value("SAMP_SCALE", samp_scale);
          if(!success){
            vcl_cout << "SAMP_SCALE Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          rpc_data[86] = samp_scale;

          double lat_scale;
          success = (*tres_itr)->get_value("LAT_SCALE", lat_scale);
          if(!success){
            vcl_cout << "LAT_SCALE Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          rpc_data[87] = lat_scale;

          double lon_scale;
          success = (*tres_itr)->get_value("LON_SCALE", lon_scale);
          if(!success){
            vcl_cout << "LON_SCALE Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          rpc_data[88] = lon_scale;

          int height_scale;
          success = (*tres_itr)->get_value("HEIGHT_SCALE", height_scale);
          if(!success){
            vcl_cout << "HEIGHT_SCALE Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          rpc_data[89] = height_scale;

          // finally get the 80 polynomial coefficients  ##################
          vcl_vector<double> LNC;
          success = (*tres_itr)->get_values("LNC", LNC);
          if(!success){
            vcl_cout << "LNC Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          for (int i = 0; i < 20; i++) {
            rpc_data[i] = LNC[i];	  			// copy from vector to regular array.
          }
		
          vcl_vector<double> LDC;
          success = (*tres_itr)->get_values("LDC", LDC);
          if(!success){
            vcl_cout << "LDC Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          for (int i = 0; i < 20; i++) {
            rpc_data[i+20] = LDC[i];	  			// copy from vector to regular array.
          }

          vcl_vector<double> SNC;
          success = (*tres_itr)->get_values("SNC", SNC);
          if(!success){
            vcl_cout << "SNC Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          for (int i = 0; i < 20; i++) {
            rpc_data[i+40] = SNC[i];	  			// copy from vector to regular array.
          }
	  
          vcl_vector<double> SDC;
          success = (*tres_itr)->get_values("SDC", SDC);
          if(!success){
            vcl_cout << "SDC Property failed in vil_nitf2_image_subheader\n";
            return false;
          }
          for (int i = 0; i < 20; i++) {
            rpc_data[i+60] = SDC[i];	  			// copy from vector to regular array.
          }
        }
    }
  return true;
}


