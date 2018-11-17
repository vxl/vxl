// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.
#include <sstream>
#include <cstdlib>
#include "vil_nitf2_image_subheader.h"
//:
// \file
#include "vil_nitf2_data_mask_table.h"
#include "vil_nitf2_field_functor.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_typed_field_formatter.h"
#include "vil_nitf2_index_vector.h"
// for TREs
#include "vil_nitf2_tagged_record_definition.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vil_nitf2_field_definitions* vil_nitf2_image_subheader::s_field_definitions_21 = nullptr;
vil_nitf2_field_definitions* vil_nitf2_image_subheader::s_field_definitions_20 = nullptr;

vil_nitf2_image_subheader::vil_nitf2_image_subheader(vil_nitf2_classification::file_version version)
  : m_field_sequence(version == vil_nitf2_classification::V_NITF_20 ?
                     *get_field_definitions_20() :
                     *get_field_definitions_21()),
    m_data_mask_table(nullptr),
    m_version(version)
{
  add_rpc_definitions();
  add_USE_definitions();
  add_ICHIPB_definitions();
  add_MPD26A_definitions();
  add_STDIDB_definitions();
  add_STDIDC_definitions();
}

vil_nitf2_image_subheader::~vil_nitf2_image_subheader()
{
  if (m_data_mask_table) delete m_data_mask_table;//jlm
  if (vil_nitf2_tagged_record_definition::all_definitions().size()) {
    vil_nitf2_tagged_record_definition::undefine("RPC00A");
    vil_nitf2_tagged_record_definition::undefine("RPC00B");
  }
}

bool vil_nitf2_image_subheader::read(vil_stream* stream)
{
  bool success = m_field_sequence.read(*stream);

  if (success)
  {
    // If this image has a data mask, we need to parse that too
    std::string compression_code;
    get_property("IC", compression_code);
    if (compression_code.find_first_of('M') != std::string::npos) {
      std::string imode;
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
    .field("NICOM", "Number of Image Comments", NITF_INT(1), false, nullptr, nullptr)

    //TODO: does it make any sense for a std::vector to have blank entries????  For now, I'm saying no (false parameter)
    .repeat("NICOM", vil_nitf2_field_definitions()

            .field("ICOMn", "Image Comment n", NITF_STR_ECSA(80), false, nullptr, nullptr))

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
           false, nullptr, nullptr);

  std::vector<std::string> comp_ic_values;
  comp_ic_values.emplace_back("C1");
  comp_ic_values.emplace_back("C3");
  comp_ic_values.emplace_back("C4");
  comp_ic_values.emplace_back("C5");
  comp_ic_values.emplace_back("C8");
  comp_ic_values.emplace_back("M1");
  comp_ic_values.emplace_back("M3");
  comp_ic_values.emplace_back("M4");
  comp_ic_values.emplace_back("M5");
  comp_ic_values.emplace_back("M8");
  comp_ic_values.emplace_back("I1");

  // Using string because the valid enum values are different based on the content of IC
  (*defs)
    .field("COMRAT", "Compression Rate Code",         NITF_STR_BCSA(4),true, nullptr,
           new vil_nitf2_field_value_one_of<std::string>("IC", comp_ic_values))

    // The value of IREP determines which values are acceptable here
    // (e.g., if IREP=MONO, then this must equal 1)
    .field("NBANDS", "Number of Bands",               NITF_INT(1), false, nullptr, nullptr)

    // Acceptable range [00010-99999]... only used if NBANDS=0
    .field("XBANDS", "Number of multispectral bands", NITF_INT(5), true, nullptr,
           new vil_nitf2_field_value_one_of<int>("NBANDS",0))

    .repeat(new vil_nitf2_choose_field_value<int>(
                                                  "NBANDS", "XBANDS", new vil_nitf2_field_value_greater_than<int>("NBANDS", 0)),
            vil_nitf2_field_definitions()

            .field("IREPBAND", "nth Band Representation",             NITF_STR_BCSA(2), true, nullptr, nullptr)
            .field("ISUBCAT",  "nth Band Subcategory",                NITF_STR_BCSA(6), true, nullptr, nullptr)
            .field("IFC",      "nth Image Filter Condition",          NITF_STR_BCSA(1), false, nullptr, nullptr)
            .field("IMFLT",    "nth Band Standard Image Filter Code", NITF_STR_BCSA(3), true, nullptr, nullptr)
            .field("NLUTS",    "Number of LUTS for the nth Image Band",        NITF_INT(1, false),  false, nullptr, nullptr)
            .field("NELUT",    "Number of LUT Entries for the nth Image Band", NITF_INT(5, false),  false, nullptr,
                   new vil_nitf2_field_value_greater_than<int>("NLUTS", 0) /*condition*/ )

            .repeat(new vil_nitf2_field_value<int>("NLUTS"), vil_nitf2_field_definitions()

                    .field("LUTDnm", "nth Image Band, mth LUT",             NITF_BIN(1), false,
                           new vil_nitf2_field_value<int>("NELUT"), nullptr))
            )

    .field("ISYNC", "Image Sync Code", NITF_INT(1),  false, nullptr, nullptr)
    .field("IMODE", "Image Mode",
           NITF_ENUM(1, vil_nitf2_enum_values()
                     .value("B", "Band interleaved by block")
                     .value("P", "Band interleaved by pixel")
                     .value("R", "Band interleaved by row")
                     .value("S", "Band sequential")),
           false, nullptr, nullptr)

    .field("NBPR",  "Number of Blocks per Row",              NITF_INT(4), false, nullptr, nullptr)
    .field("NBPC",  "Number of Blocks per Column",           NITF_INT(4), false, nullptr, nullptr)
    .field("NPPBH", "Number of Pixels per Block Horizontal", NITF_INT(4), false, nullptr, nullptr) //[0000-8192]
    .field("NPPBV", "Number of Pixels per Block Vertical",   NITF_INT(4), false, nullptr, nullptr) //[0000-8192]
    .field("NBPP",  "Number of Bits per Pixel per Band",     NITF_INT(2), false, nullptr, nullptr) //[01-96]
    .field("IDLVL", "Image Display Level",                   NITF_INT(3), false, nullptr, nullptr)
    .field("IALVL", "Attachment Level",                      NITF_INT(3), false, nullptr, nullptr) //[000-998]

    // TODO: Enter these two values as ints with optional sign (once supported)
    // Actually, since they are both in one field, maybe we'd want to make it one
    // compound field (in the new tree format)
    .field("ILOC_ROW", "Image Location Row",            NITF_STR_BCSA(5), false, nullptr, nullptr)
    .field("ILOC_COL", "Image Location Column",         NITF_STR_BCSA(5), false, nullptr, nullptr)

    // TODO: THis should be a decimal field that supports non-negative power of two
    // fractions (eg '/2  ' means 1/2, '/16 'means 1/16
    .field("IMAG",  "Image Magnification",              NITF_STR_BCSA(4), false, nullptr, nullptr)
    .field("UDIDL", "User Defined Image Data Length",        NITF_INT(5), false, nullptr, nullptr) //[00000,00003-99999]

    .field("UDOFL", "User Defined Overflow",                 NITF_INT(3), false, nullptr,
           new vil_nitf2_field_value_greater_than<int>("UDIDL", 0))

    .field("UDID", "User Defined Image Data",                NITF_TRES(), false,
           new vil_nitf2_max_field_value_plus_offset_and_threshold("UDIDL", -3), nullptr)

    .field("IXSHDL", "Image Extended Subheader Data Length", NITF_INT(5), false, nullptr, nullptr)

    .field("IXSOFL", "Image Extended Subheader Overflow",    NITF_INT(3), false, nullptr,
           new vil_nitf2_field_value_greater_than<int>("IXSHDL", 0))

    .field("IXSHD", "Image Extended Subheader Data",         NITF_TRES(), false,
           new vil_nitf2_max_field_value_plus_offset_and_threshold("IXSHDL", -3), nullptr);
}

void vil_nitf2_image_subheader::add_geo_field_defs(vil_nitf2_field_definitions* defs,
                                                   const vil_nitf2_classification::file_version& version)
{
  switch (version)
  {
    case vil_nitf2_classification::V_NITF_20:
    {
      (*defs)
        .field("ICORDS", "Image Coordinate Representation",
               NITF_ENUM(1, vil_nitf2_enum_values()
                         .value("U", "UTM")
                         .value("G", "Geodetic/Geographic")
                         .value("N", "None")
                         .value("C", "Geocentric")),
               false, nullptr, nullptr);

      std::vector<std::string> igeolo_icords;
      igeolo_icords.emplace_back("U");
      igeolo_icords.emplace_back("G");
      igeolo_icords.emplace_back("C");

      (*defs)
        .field("IGEOLO", "Image Geographic Location", NITF_STR_BCSA(60), false, nullptr,
               new vil_nitf2_field_value_one_of<std::string>("ICORDS", igeolo_icords));
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
               true, nullptr, nullptr)

        .field("IGEOLO", "Image Geographic Location", NITF_STR_BCSA(60), false, nullptr,
               new vil_nitf2_field_specified("ICORDS"));
      break;
    }
    default:
      assert(!"unsupported case");
  } // end switch
}

void vil_nitf2_image_subheader::add_shared_field_defs_2(vil_nitf2_field_definitions* defs)
{
  (*defs)
    .field("ENCRYP", "Encryption",
           NITF_ENUM(1, vil_nitf2_enum_values()
                     // Only one valid value, until the spec gets updated
                     .value("0", "Not Encrypted")),
           false, nullptr, nullptr)
    .field("ISORCE", "Image Source", NITF_STR_ECSA(42),  true, nullptr, nullptr)
    //TODO: only allow range [00000001-99999999]
    .field("NROWS", "Number of Significant Rows in Image", NITF_INT(8, false), false, nullptr, nullptr)
    //TODO: only allow range [00000001-99999999]
    .field("NCOLS", "Number of Significant Columns in Image", NITF_INT(8, false), false, nullptr, nullptr)
    .field("PVTYPE", "Pixel Value Type",
           NITF_ENUM(3, vil_nitf2_enum_values()
                     .value("INT", "Integer")
                     .value("B",   "Bi-level")
                     .value("SI",  "2's complement signed integer")
                     .value("R",   "Real")
                     .value("C",   "Complex")),
           false, nullptr, nullptr)
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
           false, nullptr, nullptr)
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
    .field("ICAT", "Image Category", NITF_STR_ECSA(8), false, nullptr, nullptr)
#endif //UNCLASS_ONLY

    .field("ABPP", "Actual Bits Per Pixel per Band", NITF_INT(2), false, nullptr, nullptr) //[1-96]
    .field("PJUST", "Pixel Justification",
           NITF_ENUM(1, vil_nitf2_enum_values()
                     .value("L", "Left-justified")
                     .value("R", "Right-justified")),
           false, nullptr, nullptr);
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

#if 0 // get_date_time() commented out
bool vil_nitf2_image_subheader::
get_date_time(int& year, int& month, int& day, int& hour, int& min)
{
  std::cout  << "In vil_nitf2_image_subheader::get_date_time!\n";
  std::string date_time = "";
  bool success = this->get_property("IDATIM", date_time);
  if (!success) {
    std::cout << "IDATIM Property failed in vil_nitf2_image_subheader\n";
    return false;
  }
  std::cout  << "In vil_nitf2_image_subheader::get_date_time!\n";
  //d==day,h==hour,n==min,ss==sec,Z==zulu,m==month, y==year suffix
  // format is ddhhnnssZmmmyy
  std::string s_day, s_hour, s_min, s_month, s_year_suff;
  s_day       = date_time.substr(0,2);
  s_hour      = date_time.substr(2,2);
  s_min       = date_time.substr(4,2);
  s_month     = date_time.substr(9,3);
  s_year_suff = date_time.substr(12,2);
  std::string months[]={"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG",
                        "SEP", "OCT", "NOV", "DEC"};
  bool found = false;
  std::cout << date_time << '\n'
           << s_day << ' ' << s_hour << ' ' << s_min
           << ' ' << s_month << ' ' << s_year_suff << std::endl;
  for (int i = 0; (i<12)&&(!found); ++i)
    if (s_month==months[i]){
      found = true;
      month = i+1;
    }
  if (!found)
    return false;
  day  = std::atoi(s_day.c_str());
  hour = std::atoi(s_hour.c_str());
  min =  std::atoi(s_min.c_str());
  year = std::atoi(s_year_suff.c_str());
  year += 2000;//good until the next millenium
  return true;
}
#endif // 0

bool vil_nitf2_image_subheader::
get_date_time(int& year, int& month, int& day, int& hour, int& min, int& sec)
{
  std::string date_time = "";
  bool success = this->get_property("IDATIM", date_time);
  if (!success) {
    std::cout << "IDATIM Property failed in vil_nitf2_image_subheader\n";
    return false;
  }
  //d==day,h==hour,n==min,ss==sec,Z==zulu,m==month, y==year
  // format is ddhhnnssZmmmyy OR yyyymmddhhnnss (the NITF 2.1 Commercial format)
  std::string s_day, s_hour, s_min, s_month, s_year, s_sec;
  // try ddhhnnssZmmmyy first
  std::string s_zulu = date_time.substr(8,1);
  if (s_zulu=="Z") {
    s_day   = date_time.substr(0,2);
    s_hour  = date_time.substr(2,2);
    s_min   = date_time.substr(4,2);
    s_sec   = date_time.substr(6,2);
    s_month = date_time.substr(9,3);
    s_year  = date_time.substr(12,2);
    std::string months[]={"JAN", "FEB", "MAR", "APR",
                         "MAY", "JUN", "JUL", "AUG",
                         "SEP", "OCT", "NOV", "DEC"};
    bool found = false;
    for (int i = 0; (i<12)&&(!found); ++i) {
      if (s_month==months[i]){
        found = true;
        month = i+1;
      }
    }
    if (!found) {
      return false;
    }
  }
  else {  // try yyyymmddhhnnss
    s_year  = date_time.substr(0,4);
    s_month = date_time.substr(4,2);
    s_day   = date_time.substr(6,2);
    s_hour  = date_time.substr(8,2);
    s_min   = date_time.substr(10,2);
    s_sec   = date_time.substr(12,2);
    month   = std::atoi(s_month.c_str());
  }
  day  = std::atoi(s_day.c_str());
  hour = std::atoi(s_hour.c_str());
  min =  std::atoi(s_min.c_str());
  sec =  std::atoi(s_sec.c_str());
  year = std::atoi(s_year.c_str());
  if (year < 100)
    year += 2000;//good until the next millenium
  return true;
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

    // Fix added 4/12/17 by tpollard to conform to NITF spec 5.4.2.2d in
    // special case of single block image and width/height > 8192
    if (pixels_per_block == 0) {

      int blocks_per_row = 0;
      get_property("NBPR", blocks_per_row);
      if (blocks_per_row != 1) return 0;

      if (get_property("NCOLS", pixels_per_block))
        return pixels_per_block;
      return 0;

    // Standard case
    } else
      return pixels_per_block;
  }
  return 0;
}

unsigned int vil_nitf2_image_subheader::get_pixels_per_block_y() const
{
  int pixels_per_block;
  if (get_property("NPPBV", pixels_per_block)) {

    // Fix added 4/12/17 by tpollard to conform to NITF spec 5.4.2.2d in
    // special case of single block image and width/height > 8192
    if (pixels_per_block == 0) {

      int blocks_per_col = 0;
      get_property("NBPC", blocks_per_col);
      if (blocks_per_col != 1) return 0;

      if (get_property("NROWS", pixels_per_block))
        return pixels_per_block;
      return 0;

    // Standard case
    } else
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

unsigned int vil_nitf2_image_subheader::get_number_of_bits_per_pixel() const
{
  int bits_per_pixel;
  if (get_property("ABPP", bits_per_pixel)) {
    return bits_per_pixel;
  }
  return 0;
}

std::string vil_nitf2_image_subheader::get_image_source() const
{
  std::string source;
  if (get_property("ISORCE", source)) {
    return source;
  }
  return "";
}

std::string vil_nitf2_image_subheader::get_image_type() const
{
  std::string id;
  if (get_property("IREP", id)) {
    return id;
  }
  return "";
}

bool vil_nitf2_image_subheader::get_lut_info(unsigned int band,
                                             int& n_luts, int& ne_lut,
                                             std::vector< std::vector< unsigned char > >& lut_d) const
{
  if (!m_field_sequence.get_value("NLUTS", vil_nitf2_index_vector(band), n_luts) ||
      !m_field_sequence.get_value("NELUT", vil_nitf2_index_vector(band), ne_lut)) {
    return false;
  }
  lut_d.clear();
  lut_d.resize(n_luts);
  void* raw_lut_data;
  for (int lut_index = 0 ; lut_index < n_luts ; lut_index++) {
    lut_d[lut_index].resize(ne_lut);
    //get the lut_index'th lut for the given image band
    vil_nitf2_index_vector index( band, lut_index );
    if (m_field_sequence.get_value("LUTDnm", index, raw_lut_data )) {
      for ( int el_index = 0 ; el_index < ne_lut ; el_index++ ) {
        lut_d[lut_index][el_index] = static_cast<unsigned char*>(raw_lut_data)[el_index];
      }
    }
    else {
      break;
    }
  }
  return true;
}

vil_nitf2_field::field_tree* vil_nitf2_image_subheader::get_tree( int i ) const
{
  auto* t = new vil_nitf2_field::field_tree;
  std::stringstream name_stream;
  name_stream << "Image Subheader";
  if ( i > 0 ) name_stream << " #" << i;
  t->columns.push_back( name_stream.str() );
  m_field_sequence.get_tree( t );
  return t;
}

// Add definitions for rational polynomial coefficients
void vil_nitf2_image_subheader::add_rpc_definitions()
{
  vil_nitf2_tagged_record_definition* tr =
    vil_nitf2_tagged_record_definition::find("RPC00B");
  if (!tr)
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

    // Now come the 4 sequential groups of (20 each) polynomial coefficients
    //   for line number, line density, sample number and sample density.
    // As these values are in <+/->n.nnnnnnE<+/->n (exponential) format,
    //   let's just read as strings now and convert into doubles later.

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
  if (!tr)
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

    // Now come the 4 sequential groups of (20 each) polynomial coefficients
    //   for line number, line density, sample number and sample density.
    // As these values are in <+/->n.nnnnnnE<+/->n (exponential) format,
    //   let's just read as strings now and convert into doubles later.

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

void vil_nitf2_image_subheader::add_USE_definitions()
{
  vil_nitf2_tagged_record_definition* tr =vil_nitf2_tagged_record_definition::find("USE00A");
  if (!tr)
  {
    vil_nitf2_tagged_record_definition::define("USE00A", "EXPLOITATION USABILITY EXTENSION FORMAT" )

    .field("ANGLE_TO_NORTH","Angle to North",  NITF_INT(3))                 // not used, but must read
    .field("MEAN_GSD",      "Mean Ground Sample Distance",NITF_DBL(5, 1, false), false) // not used, but must read
    .field("Reserved1", "",NITF_STR(1), false)
    .field("DYNAMIC_RANGE", "Dynamic Range",NITF_LONG(5,  false), true) // not used
    .field("Reserved2", "",NITF_STR(7), false)
    .field("OBL_ANG",       "Obliquity Angle",        NITF_DBL(5,2,false), true)
    .field("ROLL_ANG",      "Roll Angle",     NITF_DBL(6,2,true), true)
    .field("Reserved3", "",NITF_STR(37), false)
    .field("N_REF",         "Number of Reference Lines.",      NITF_INT(2, false), false)
    .field("REV_NUM",       "Revolution Number",    NITF_LONG(5, false), false)
    .field("N_SEG",         "Number of Segments",   NITF_INT(3,  false), false)
    .field("MAX_LP_SEG",    "Maximum Lines Per Segment",         NITF_LONG(6,false), true)
    .field("Reserved4", "",NITF_STR(12), false)
    .field("SUN_EL",        "Sun Elevation",       NITF_DBL(5,1,true),false)
    .field("SUN_AZ",        "Sun Azimuth",     NITF_DBL(5,1,false),false)


    .end();  // of USE00A TRE
  }
}

// Collect the Sun angles
bool vil_nitf2_image_subheader::
get_sun_params( double& sun_el, double& sun_az)
{
  // Now get the sub-header TRE parameters
  vil_nitf2_tagged_record_sequence isxhd_tres;
  vil_nitf2_tagged_record_sequence::iterator tres_itr;
  this->get_property("IXSHD", isxhd_tres);


  bool success=false;
  // Check through the TREs to find "RPC"
  for (tres_itr = isxhd_tres.begin(); tres_itr != isxhd_tres.end(); ++tres_itr)
  {
    std::string type = (*tres_itr)->name();
    if ( type == "USE00A")
    {
      success = (*tres_itr)->get_value("SUN_EL", sun_el);
      success = success && (*tres_itr)->get_value("SUN_AZ", sun_az);
      if (!success)
        std::cout<<"\n Error reading USE00A\n";
      else
        return success;
    }
    else if ( type == "MPD26A")
    {
      success = (*tres_itr)->get_value("SUN_EL", sun_el);
      success = success && (*tres_itr)->get_value("SUN_AZ", sun_az);
      if (!success)
        std::cout<<"\n Error reading MPD26A\n";
      else
        return success;
    }
  }
  return success;
}

void vil_nitf2_image_subheader::add_ICHIPB_definitions()
{
  vil_nitf2_tagged_record_definition* tr =vil_nitf2_tagged_record_definition::find("ICHIPB");
  if (!tr)
  {
    vil_nitf2_tagged_record_definition::define("ICHIPB", "ICHIPB SUPPORT DATA EXTENSION" )

    .field("XFRM_FLAG",    "Non-linear Transformation Flag",  NITF_INT(2),false)                 // not used, but must read
    .field("SCALE_FACTOR", "Scale Factor Relative to R0",NITF_DBL(10, 5, false), false) // not used, but must read
    .field("ANAMRPH_CORR", "Anamorphic Correction Indicator", NITF_INT(2),false)
    .field("SCANBLK_NUM",  "Scan Block Number",NITF_INT(2,  false), true) // not used
    //: intelligent data
    .field("OP_ROW_11", "Output product row number component of grid point index (1,1)",NITF_DBL(12,3,false), false)
    .field("OP_COL_11", "Output product column number component of grid point index (1,1)",NITF_DBL(12,3,false), false)

    .field("OP_ROW_12", "Output product row number component of grid point index (1,2)",NITF_DBL(12,3,false), false)
    .field("OP_COL_12", "Output product column number component of grid point index (1,2)",NITF_DBL(12,3,false), false)

    .field("OP_ROW_21", "Output product row number component of grid point index (2,1)",NITF_DBL(12,3,false), false)
    .field("OP_COL_21", "Output product column number component of grid point index (2,1)",NITF_DBL(12,3,false), false)

    .field("OP_ROW_22", "Output product row number component of grid point index (2,2)",NITF_DBL(12,3,false), false)
    .field("OP_COL_22", "Output product column number component of grid point index (2,2)",NITF_DBL(12,3,false), false)

    //: full image coordinate system
    .field("FI_ROW_11", "Output product row number component of grid point index (1,1)",NITF_DBL(12,3,false), false)
    .field("FI_COL_11", "Output product column number component of grid point index (1,1)",NITF_DBL(12,3,false), false)

    .field("FI_ROW_12", "Output product row number component of grid point index (1,2)",NITF_DBL(12,3,false), false)
    .field("FI_COL_12", "Output product column number component of grid point index (1,2)",NITF_DBL(12,3,false), false)

    .field("FI_ROW_21", "Output product row number component of grid point index (2,1)",NITF_DBL(12,3,false), false)
    .field("FI_COL_21", "Output product column number component of grid point index (2,1)",NITF_DBL(12,3,false), false)

    .field("FI_ROW_22", "Output product row number component of grid point index (2,2)",NITF_DBL(12,3,false), false)
    .field("FI_COL_22", "Output product column number component of grid point index (2,2)",NITF_DBL(12,3,false), false)

    .field("FI_ROW",      "Full Image Number of Rows",     NITF_LONG(8,false), false)
    .field("FI_COL",      "Full Image Number of COlumns",     NITF_LONG(8,false), false)

    .end();  // of ICHIPB TRE
  }
}


void vil_nitf2_image_subheader::add_STDIDC_definitions()
{
  vil_nitf2_tagged_record_definition* tr =vil_nitf2_tagged_record_definition::find("STDIDC");
  if (!tr)
  {
    vil_nitf2_tagged_record_definition::define("STDIDC", "STDIDC SUPPORT DATA EXTENSION" )

    .field("ACQUISITION_DATE",    "",  NITF_STR(14),false)                 // not used, but must read
    .field("MISSION", "",NITF_STR(14), false) // not used, but must read
    .field("PASS", "", NITF_STR(2),false)
    .field("OP_NUM",  "",NITF_INT(3,  false), false) // not used
    .field("START_SEGMENT", "",NITF_STR(2), false)
    .field("REPRO_NUM", "",NITF_INT(2,false), false)
    .field("REPLAY_REGEN", "",NITF_STR(3), false)
    .field("BLANK_FILL", "",NITF_STR(1), true)
    .field("START_COLUMN", "",NITF_INT(3,false), false)
    .field("START_ROW", "",NITF_INT(5,false), false)
    .field("END_SEGMENT", "",NITF_STR(2), false)
    .field("END_COLUMN", "",NITF_INT(3,false), false)
    .field("END_ROW", "",NITF_INT(5,false), false)
    .field("COUNTRY", "",NITF_STR(2), true)
    .field("WAC", "",NITF_INT(4,false), true)
    .field("LOCATION", "",NITF_STR(11), false)
    .field("RESERVED1", "",NITF_STR(5), true)
    .field("RESERVED2", "",NITF_STR(8), true)

    .end();  // of STDIDC TRE
  }
}


void vil_nitf2_image_subheader::add_STDIDB_definitions()
{
  vil_nitf2_tagged_record_definition* tr =vil_nitf2_tagged_record_definition::find("STDIDB");
  if (!tr)
  {
    vil_nitf2_tagged_record_definition::define("STDIDB", "STDIDB SUPPORT DATA EXTENSION" )

    .field("unk1",    "",  NITF_INT(2,false),false)                 // not used, but must read
    .field("unk2",    "",  NITF_STR(3),false)                 // not used, but must read
    .field("unk3",    "",  NITF_INT(2,false),false)                 // not used, but must read
    .field("unk4",    "",  NITF_INT(4,false),false)                 // not used, but must read
    .field("unk5",    "",  NITF_STR(2),false)                 // not used, but must read
    .field("unk6",    "",  NITF_INT(3,false),false)                 // not used, but must read
    .field("unk7",    "",  NITF_STR(2),false)                 // not used, but must read
    .field("unk8",    "",  NITF_INT(2,false),false)
    .field("unk9",    "",  NITF_STR(3),false)    // not used, but must read
    .field("unk10",    "",  NITF_STR(1),false)
    .field("START_COLUMN",    "",  NITF_STR(2),false)
    .field("START_ROW",    "",  NITF_INT(5,false),false)
    .field("unk11",    "",  NITF_STR(2),false)                 // not used, but must read
    .field("unk12",    "",  NITF_STR(2),false)                 // not used, but must read
    .field("unk13",    "",  NITF_INT(5,false),false)                 // not used, but must read
    .field("unk14",    "",  NITF_STR(2),false)                 // not used, but must read
    .field("unk15",    "",  NITF_INT(4,false),false)                 // not used, but must read
    .field("unk16",    "",  NITF_STR(4),false)                 // not used, but must read
    .field("unk17",    "",  NITF_STR(1),false)                 // not used, but must read
    .field("unk18",    "",  NITF_STR(5),false)
    .field("unk19",    "",  NITF_STR(1),false)    // not used, but must read
    .field("unk20",    "",  NITF_STR(5),false)
    .field("unk21",    "",  NITF_STR(7),false)

    .end();  // of STDIDC TRE
  }
}


void vil_nitf2_image_subheader::add_MPD26A_definitions()
{
  vil_nitf2_tagged_record_definition* tr =vil_nitf2_tagged_record_definition::find("MPD26A");
  if (!tr)
  {
    vil_nitf2_tagged_record_definition::define("MPD26A", "MPD26A SUPPORT DATA EXTENSION" )

    .field("UNK1",    "Unknown ",  NITF_DBL(11,7,true), false) // not used, but must read
    .field("UNK2",    "Unknown ",  NITF_STR(2), false) // not used, but must read
    .field("UNK3",    "Unknown ",  NITF_INT(7,false),false)                 // not used, but must read
    .field("UNK31",    "Unknown ", NITF_STR(10),false)                 // not used, but must read
    .field("UNK4",    "Unknown ",  NITF_DBL(6,3,false), true) // not used, but must read
    .field("UNK5",    "Unknown ",  NITF_DBL(9,5,false), false) // not used, but must read
    .field("UNK6",    "Unknown ",  NITF_STR(3), false) // not used, but must read
    .field("UNK7",    "Unknown ",  NITF_STR(1), false) // not used, but must read
    .field("UNK8",    "Unknown ",  NITF_STR(1), false) // not used, but must read
    .field("SUN_EL", "Sun Elevation angle", NITF_DBL(5,1,true),false)
    .field("SUN_AZ", "Sun Azimuthal angle", NITF_DBL(5,1,false),false)
    .field("UNK9",    "Unknown ",  NITF_STR(1), false) // not used, but must read
    .field("UNK10",    "Unknown ",  NITF_STR(1), false) // not used, but must read
    .field("UNK11",    "Unknown ",  NITF_INT(4,true), false) // not used, but must read
    .field("UNK12",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK13",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK14",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK15",    "Unknown ",  NITF_STR(10), false) // not used, but must read
    .field("UNK16",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK17",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK18",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK19",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK20",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK21",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK22",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK23",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK24",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK16",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK17",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK18",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK19",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK20",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK21",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK22",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK23",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK24",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK16",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK17",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK18",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK19",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK20",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK21",    "Unknown ",  NITF_INT(9,true), false) // not used, but must read
    .field("UNK22",    "Unknown ",  NITF_INT(9,true), true) // not used, but must read
    .field("UNK23",    "Unknown ",  NITF_INT(9,true), true) // not used, but must read
    .field("UNK24",    "Unknown ",  NITF_INT(9,true), true) // not used, but must read

    .end();  // of MPD26A TRE
  }
}


// obtain column and row offset from STDIDB /SDTDIDC
bool vil_nitf2_image_subheader::
get_correction_offset(double & u_off, double & v_off)
{
  // Now get the sub-header TRE parameters
  vil_nitf2_tagged_record_sequence isxhd_tres;
  vil_nitf2_tagged_record_sequence::iterator tres_itr;
  this->get_property("IXSHD", isxhd_tres);

  double ulr=0;
  double ulc=0;
  // Check through the TREs to find "RPC"
  for (tres_itr = isxhd_tres.begin(); tres_itr != isxhd_tres.end(); ++tres_itr)
  {
    std::string type = (*tres_itr)->name();
    if ( type == "ICHIPB" )
    {
      double r_off=1.0; // dummy initialisation
      double c_off=1.0; // to silence a compiler warning
      if ( (*tres_itr)->get_value("FI_ROW_11", r_off) &&
           (*tres_itr)->get_value("FI_COL_11", c_off) )
      {
        ulr+=r_off;
        ulc+=c_off;
      }
      else
        return false;
    }
    else if ( type == "STDIDC" )
    {
      int r_off=1;
      int c_off=1;
      if ( (*tres_itr)->get_value("START_ROW",    r_off) &&
           (*tres_itr)->get_value("START_COLUMN", c_off) )
      {
        ulr+=(double)((r_off-1)*get_pixels_per_block_y());
        ulc+=(double)((c_off-1)*get_pixels_per_block_x());
      }
      else
        return false;
    }
    else if ( type == "STDIDB" )
    {
      int r_off=1;
      int c_off=1;
      std::string temp_off;
      if ( (*tres_itr)->get_value("START_ROW",    r_off) &&
           (*tres_itr)->get_value("START_COLUMN", temp_off) )
      {
        if ((int)temp_off[0]>=65)
          c_off=((int)temp_off[0]-55)*10;
        else
          c_off=((int)temp_off[0]-48)*10;
        c_off+=(int)temp_off[1]-48;
        ulr+=(r_off-1)*get_pixels_per_block_y();
        ulc+=(c_off-1)*get_pixels_per_block_x();
      }
      else
        return false;
    }
  }
  u_off=ulc;
  v_off=ulr;
  return true;
}

// Collect the RPC parameters for the current image. Image corners are reported
// as a string of geographic coordinates,one for each image corner.
  bool vil_nitf2_image_subheader::
get_rpc_params( std::string& rpc_type, std::string& image_id,
                std::string& image_corner_geo_locations,
                double* rpc_data )
{
  // Get image ID and location from main header values
  std::string iid2 = "";
  bool success = this->get_property("IID2", iid2);
  if (!success) {
    std::cout << "IID2 Property failed in vil_nitf2_image_subheader\n";
    return false;
  }
  image_id = iid2.substr(0,39);// trim length to NN characters to match file ID
  std::string igeolo = "";
  success = this->get_property("IGEOLO", igeolo);
  if (!success) {
    std::cout << "IGEOLO Property failed in vil_nitf2_image_subheader\n";
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
    std::string type = (*tres_itr)->name();

    if ( type == "RPC00B" || type == "RPC00A") // looking for "RPC..."
    {
      // set type in return value
      rpc_type = type;

      // get offsets and scales, print a couple to make sure values are correct
      int line_off;
      success = (*tres_itr)->get_value("LINE_OFF", line_off);
      if (!success) {
        std::cout << "LINE_OFF Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      rpc_data[80] = line_off;

      int samp_off;
      success = (*tres_itr)->get_value("SAMP_OFF", samp_off);
      if (!success) {
        std::cout << "SAMP_OFF Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      rpc_data[81]  = samp_off;

      double lat_off;
      success = (*tres_itr)->get_value("LAT_OFF", lat_off);
      if (!success) {
        std::cout << "LAT_OFF Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      rpc_data[82] = lat_off;

      double lon_off;
      success = (*tres_itr)->get_value("LON_OFF", lon_off);
      if (!success) {
        std::cout << "LON_OFF Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      rpc_data[83] = lon_off;

      int height_off;
      success = (*tres_itr)->get_value("HEIGHT_OFF", height_off);
      if (!success) {
        std::cout << "HEIGHT_OFF Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      rpc_data[84] = height_off;

      int line_scale;
      success = (*tres_itr)->get_value("LINE_SCALE", line_scale);
      if (!success) {
        std::cout << "LINE_SCALE Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      rpc_data[85] = line_scale;

      int samp_scale;
      success = (*tres_itr)->get_value("SAMP_SCALE", samp_scale);
      if (!success) {
        std::cout << "SAMP_SCALE Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      rpc_data[86] = samp_scale;

      double lat_scale;
      success = (*tres_itr)->get_value("LAT_SCALE", lat_scale);
      if (!success) {
        std::cout << "LAT_SCALE Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      rpc_data[87] = lat_scale;

      double lon_scale;
      success = (*tres_itr)->get_value("LON_SCALE", lon_scale);
      if (!success) {
        std::cout << "LON_SCALE Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      rpc_data[88] = lon_scale;

      int height_scale;
      success = (*tres_itr)->get_value("HEIGHT_SCALE", height_scale);
      if (!success) {
        std::cout << "HEIGHT_SCALE Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      rpc_data[89] = height_scale;

      // finally get the 80 polynomial coefficients  ##################
      std::vector<double> LNC;
      success = (*tres_itr)->get_values("LNC", LNC);
      if (!success) {
        std::cout << "LNC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      for (int i = 0; i < 20; i++) {
        rpc_data[i] = LNC[i];      // copy from vector to regular array.
      }

      std::vector<double> LDC;
      success = (*tres_itr)->get_values("LDC", LDC);
      if (!success) {
        std::cout << "LDC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      for (int i = 0; i < 20; i++) {
        rpc_data[i+20] = LDC[i];   // copy from vector to regular array.
      }

      std::vector<double> SNC;
      success = (*tres_itr)->get_values("SNC", SNC);
      if (!success) {
        std::cout << "SNC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      for (int i = 0; i < 20; i++) {
        rpc_data[i+40] = SNC[i];   // copy from vector to regular array.
      }

      std::vector<double> SDC;
      success = (*tres_itr)->get_values("SDC", SDC);
      if (!success) {
        std::cout << "SDC Property failed in vil_nitf2_image_subheader\n";
        return false;
      }
      for (int i = 0; i < 20; i++) {
        rpc_data[i+60] = SDC[i];   // copy from vector to regular array.
      }
    }
  }

  return true;
}
