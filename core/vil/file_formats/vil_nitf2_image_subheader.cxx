// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_image_subheader.h"

#include "vil_nitf2_data_mask_table.h"

#include "vil_nitf2_field_functor.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_typed_field_formatter.h"
#include "vil_nitf2_index_vector.h"

vil_nitf2_field_definitions* vil_nitf2_image_subheader::s_field_definitions_21 = 0;
vil_nitf2_field_definitions* vil_nitf2_image_subheader::s_field_definitions_20 = 0;

vil_nitf2_image_subheader::vil_nitf2_image_subheader(vil_nitf2_classification::file_version version)
  : m_field_sequence(version == vil_nitf2_classification::V_NITF_20 ? 
                     *get_field_definitions_20() : 
                     *get_field_definitions_21()),
    m_data_mask_table(0),
    m_version(version)
{ }

vil_nitf2_image_subheader::~vil_nitf2_image_subheader()
{
  if (m_data_mask_table) delete m_data_mask_table;
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
  .field("ILOC_ROW", "Image Location Row",           NITF_STR_BCSA(5), false, 0, 0)
  .field("ILOC_COL", "Image Location Column",        NITF_STR_BCSA(5), false, 0, 0)

  // TODO: THis should be a decimal field that supports non-negative power of two 
  // fractions (eg '/2  ' means 1/2, '/16 'means 1/16
  .field("IMAG", "Image Magnification",        NITF_STR_BCSA(4),  false, 0, 0)
  .field("UDIDL", "User Defined Image Data Length",        NITF_INT(5), false, 0, 0) //[00000,00003-99999]
  .field("UDOFL", "User Defined Overflow",                 NITF_INT(3), false, 0, 
         new vil_nitf2_field_value_greater_than<int>("UDIDL", 0))

  // TODO: UDID is a variable lengh TRE ('UDIDL-3' bytes). Right now, we're just reading it as 
  // a binary data. We should do something better than that (perhaps recursively read in a new TRE)
  .field("UDID", "User Defined Image Data",              NITF_BIN(1), false, 
         new vil_nitf2_max_field_value_plus_offset_and_threshold("UDIDL", -3), 0)

  .field("IXSHDL", "Image Extended Subheader Data Length", NITF_INT(5), false, 0, 0)
  .field("IXSOFL", "Image Extended Subheader Overflow",    NITF_INT(3), false, 0, 
         new vil_nitf2_field_value_greater_than<int>("IXSHDL", 0))

  // TODO: IXSHD is a variable lengh TRE ('IXSHDL-3' bytes).  Right now, we're just reading it in as a 
  // character vector. We should do something better than that
  .field("IXSHD", "Image Extended Subheader Data", NITF_BIN(1), false, 
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
           .value("SI",  "2's Complement Signed Integer")
           .value("R",   "Real")
           .value("C",   "Complex")),                                                            
          false, 0, 0)
  .field("IREP", "Image Representation", 
         NITF_ENUM(8, vil_nitf2_enum_values()
           .value("MONO",     "Monocrhome")
           .value("RGB",      "Red, Green, Blue True color")
           .value("RGB/LUT",  "Red, Green, Blue mapped color")
           .value("MULTI",    "Multiband imagery")
           .value("NODISPLY", "Not intended for display")
           .value("NVECTOR",  "Cartesion Coordinates")
           .value("POLAR",    "Polar Coordinates")
           .value("VPH",      "SAR video phase history")
           .value("YCbCr601", "BT.601-5 color space")),
          false, 0, 0)
#ifdef UNCLASS_ONLY
  .field("ICAT", "Image Category", 
         NITF_ENUM(8, vil_nitf2_enum_values()
           .value("VIS",    "Visible imagery")
           .value("SL",     "Side looking radar")
           .value("TI",     "Thermal infrared")
           .value("FL",     "Forward looking radar")
           .value("RD",     "Radar")
           .value("EO",     "Electro-optical")
           .value("OP",     "Optical")
           .value("HR",     "High resolution radar")
           .value("HS",     "Hyperspectral")
           .value("CP",     "Color frame photography")
           .value("BP",     "Black/white frame photography")
           .value("SARIQ",  "SAR radio hologram")
           .value("SAR",    "Synthetic aperture radar")
           .value("IR",     "Infrared")
           .value("MS",     "Multispectral")
           .value("FP",     "Fingerprints")
           .value("MRI",    "Magnetic resonance imagery")
           .value("XRAY",   "X-Ray")
           .value("CAT",    "CAT scan")
           .value("VD",     "Video")
           .value("BARO",   "Barometric pressure")
           .value("CURRENT","Water current")
           .value("DEPTH",  "Water depth")
           .value("MAP",    "Raster map")
           .value("PAT",    "Color Patch")
           .value("LEG",    "Legends")
           .value("DTEM",   "Elevation models")
           .value("MATR",   "Matrix data (other types)")
           .value("LOCG",   "Location Grids")),
          false, 0, 0)
#else
  .field("ICAT", "Image Category", NITF_STR_ECSA(8), false, 0, 0)
#endif //UNCLASS_ONLY

  .field("ABPP", "Actual Bits-Per_pixel Per Band", NITF_INT(2), false, 0, 0) //[1-96]
  .field("PJUST", "Pixel Justification", 
         NITF_ENUM(1, vil_nitf2_enum_values()
           .value("L", "Left justified")
           .value("R", "Right justified")),
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
