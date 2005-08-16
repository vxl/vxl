// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_header.h"

#include "vil_nitf2_field_functor.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_typed_field_formatter.h"

#include <vil/vil_stream_fstream.h>

vil_nitf2_field_definitions* vil_nitf2_header::s_field_definitions_1 = 0;
vil_nitf2_field_definitions* vil_nitf2_header::s_field_definitions_20 = 0;
vil_nitf2_field_definitions* vil_nitf2_header::s_field_definitions_21 = 0;

vil_nitf2_header::vil_nitf2_header()
  : m_field_sequence1(*get_field_definitions_1()),
    m_field_sequence2(0),
    m_field_sequence_classification(0)
{ }

vil_nitf2_header::~vil_nitf2_header()
{
  if (m_field_sequence_classification) delete m_field_sequence_classification;
  if (m_field_sequence2) delete m_field_sequence2;
}

bool vil_nitf2_header::read(vil_stream* stream)
{
  //first read the first part of the header
  bool success = m_field_sequence1.read(*stream);
  
  //now read the classification stuff which is dependent
  //on the nitf file version
  if (success){
    if (m_field_sequence_classification) delete m_field_sequence_classification;
    m_field_sequence_classification = 
      new vil_nitf2_field_sequence(*vil_nitf2_classification::get_field_definitions(file_version(), "F", "File"));
    success &= m_field_sequence_classification->read(*stream);
  }

  //now read in the rest of the header
  if (success) {
    if (m_field_sequence2) delete m_field_sequence2;
    m_field_sequence2 = new vil_nitf2_field_sequence(*get_field_definitions_2(file_version()));
    success &= m_field_sequence2->read(*stream);
  }

  return success;
}

vil_nitf2_field_definitions* vil_nitf2_header::get_field_definitions_2(vil_nitf2_classification::file_version version)
{
  vil_nitf2_field_definitions* field_defs = (version == vil_nitf2_classification::V_NITF_20) ?
                                       s_field_definitions_20 : 
                                       s_field_definitions_21;
  if (field_defs) return field_defs;
 
  // initialize field definitions
  field_defs = new vil_nitf2_field_definitions();

  (*field_defs)
    .field("FSCOP",  "File Copy Number",      NITF_INT(5), true /*in NITF2.0*/, 0, 0)
    .field("FSCPYS", "File Number of Copies", NITF_INT(5), true /*in NITF2.0*/, 0, 0)
    .field("ENCRYP", "Encryption",            NITF_INT(1), false, 0, 0);

  if (version == vil_nitf2_classification::V_NITF_20) {
    (*field_defs)
      .field("ONAME", "Originator's Name",    NITF_STR_ECSA(27), true, 0, 0);
  } else {
    // TODO: These next three values should actually be read in as a vcl_vector of 3 (8bit) unsigned binary integers
    (*field_defs)
      .field("FBKGC", "File Background Color 1", NITF_BIN(3),       true, 0, 0)
      .field("ONAME", "Originator's Name",       NITF_STR_ECSA(24), true, 0, 0);
  }
    
  (*field_defs)
    .field("OPHONE", "Originator's Phone Number", NITF_STR_ECSA(18), true,  0, 0)
    .field("FL",     "File Length",               NITF_LONG(12),     false, 0, 0)
    .field("HL",     "NITF File Header Length",   NITF_INT(6),       false, 0, 0)
    .field("NUMI",   "Number of Image Segments",  NITF_INT(3),       false, 0, 0)

    .repeat("NUMI", vil_nitf2_field_definitions()

       .field("LISH", "Length of nth Image Subheader", NITF_INT(6),   false, 0, 0)
       .field("LI",   "Length of nth Image Segment",   NITF_LONG(10), false, 0, 0))

    .field("NUMS",   "Number of Graphic Segments",     NITF_INT(3), false, 0, 0)

    .repeat("NUMS", vil_nitf2_field_definitions() 
       
      .field("LSSH", "Length of nth Graphic Subheader",  NITF_INT(4), false, 0, 0)
      .field("LS",   "Length of nth Graphic Segment",    NITF_INT(6), false, 0, 0))

     // In NITF2.1 this will always be zero. NITF2.0 can have these though.
    .field("NUML",   "Number of Labels",        NITF_INT(3), false, 0, 0)
    
    // begin NITF2.0-specific fields
    .repeat("NUML", vil_nitf2_field_definitions()
                                          
      .field("LLSH", "Length of nth Label Subheader",  NITF_INT(4), false, 0, 0)
      .field("LL",   "Length of nth Label",    NITF_INT(3), false, 0, 0))
    // end NITF2.0-specific fields

    .field("NUMT",   "Number of Text Segments",        NITF_INT(3), false, 0, 0)
    
    .repeat("NUMT", vil_nitf2_field_definitions()
    
      .field("TSSH", "Length of nth Text Subheader",   NITF_INT(4), false, 0, 0)
      .field("LT",   "Length of nth Text Segment",     NITF_INT(5), false, 0, 0))

    .field("NUMDES", "Number of Data Extension Segments", NITF_INT(3), false, 0, 0)

    .repeat("NUMDES", vil_nitf2_field_definitions()

       .field("LDSH", "Length of nth Data Extension Segment Subheader", NITF_INT(4), false, 0, 0)
       .field("LD",   "Length of nth Data Extension Segment",           NITF_INT(9), false, 0, 0))

    .field("NUMRES", "Number of Reserved Extension Segments", NITF_INT(3), false, 0, 0)
    
    .repeat("NUMRES", vil_nitf2_field_definitions()
    
      .field("LRESH", "Length of nth Reserved Extension Segment Subheader", NITF_INT(4), false, 0, 0)
      .field("LRE",   "Length of nth Reserved Extension Segment",           NITF_INT(7), false, 0, 0))
    
    .field("UDHDL",  "User Defined Header Data Length", NITF_INT(5), false, 0, 0) // range [00000,00003-99999]
    .field("UDHOFL", "User Defined Header Overflow",    NITF_INT(3), false, 0, 
           new vil_nitf2_field_value_greater_than<int>("UDHDL", 0))

    // TODO: UDHD is a variable length TRE (max(UDHDL-3,0) bytes), for now, read as binary 
    // data, someday read as a TRE
    .field("UDHD", "User Defined Header Data",          NITF_BIN(1), false, 
           new vil_nitf2_max_field_value_plus_offset_and_threshold("UDHDL", -3), 0)

    .field("XHDL",    "Extended Header Data Length",    NITF_INT(5), false, 0, 0)
    .field("XHDLOFL", "Extended Header Data Overflow",  NITF_INT(3), false, 0, 
           new vil_nitf2_field_value_greater_than<int>("XHDL", 0))

    // TODO: XHD is a variable length TRE (max(XHDL-3,0) bytes), for now, read as binary 
    // data, someday read as a TRE
    .field("XHD", "Extended Header Data",              NITF_BIN(1),  false, 
           new vil_nitf2_max_field_value_plus_offset_and_threshold("XHDL", -3), 0);

  return field_defs;
}

vil_nitf2_field_definitions* vil_nitf2_header::get_field_definitions_1()
{
  if (!s_field_definitions_1) {
    s_field_definitions_1 = new vil_nitf2_field_definitions();
    vil_nitf2_field_definitions& field_defs = *s_field_definitions_1;
    field_defs
      .field("FHDR", "File Profile Name", 
             NITF_ENUM(4, vil_nitf2_enum_values()
               .value("NITF", "NITF File")
               .value("NSIF", "NSIF File")), 
              false, 0, 0)
    .field("FVER", "File Version",           NITF_STR_BCSA(5),  false, 0, 0)
    .field("CLEVEL", "Complexity Level",     NITF_INT(2),       false, 0, 0)
    // NITF2.1 - BF01, NITF2.0: <blank>
    .field("STYPE", "Standard Type", 
           NITF_ENUM(4, vil_nitf2_enum_values()
             .value("BF01", "ISO/IEC IS 12087-5")),
            true, 0, 0)
    .field("OSTAID", "Operating Station ID", NITF_STR_BCSA(10), false, 0, 0)
    // order of data/time depends on NITF2.1 or NITF2.0, so just read in as string for now
    //.field("FDT", "File date and time", NITF_DAT(14),  false, 0, 0));
    .field("FDT", "File Date and Time",      NITF_STR_BCSA(14), false, 0, 0)
    .field("FTITLE", "File Title",           NITF_STR_ECSA(80), true, 0, 0);
  }
  return s_field_definitions_1;
}

vil_nitf2_classification::file_version vil_nitf2_header::file_version() const
{
  vcl_string file_profile;
  vcl_string file_version;
  if (get_property("FHDR", file_profile) && get_property("FVER", file_version)) {
    if (file_profile == "NSIF" && file_version == "01.00") return vil_nitf2_classification::V_NSIF_10;
    if (file_profile == "NITF" && file_version == "01.00") return vil_nitf2_classification::V_NITF_10;
    if (file_profile == "NITF" && file_version == "02.00") return vil_nitf2_classification::V_NITF_20;
    if (file_profile == "NITF" && file_version == "02.10") return vil_nitf2_classification::V_NITF_21;
  }
  return vil_nitf2_classification::V_UNKNOWN;
}
