// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_tagged_record.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <vcl_sstream.h>
#include <vcl_iomanip.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

#include <vil/vil_stream_core.h>
#include <vil/vil_stream_section.h>

#include "vil_nitf2_tagged_record_definition.h"
#include "vil_nitf2_field.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_field_sequence.h"
#include "vil_nitf2_index_vector.h"
#include "vil_nitf2_typed_field_formatter.h"
#include "vil_nitf2_scalar_field.h"

vil_nitf2_field_definition* vil_nitf2_tagged_record::s_tag_definition = new
  vil_nitf2_field_definition("CETAG", "Extension Tag", new vil_nitf2_string_formatter(6));

vil_nitf2_field_definition* vil_nitf2_tagged_record::s_length_definition = new
  vil_nitf2_field_definition("CEL", "Extension Length", new vil_nitf2_integer_formatter(5));

vil_nitf2_integer_formatter* vil_nitf2_tagged_record::s_length_formatter = new
  vil_nitf2_integer_formatter(5);

vil_nitf2_string_formatter* vil_nitf2_tagged_record::s_tag_formatter = new
  vil_nitf2_string_formatter(6);

vcl_string vil_nitf2_tagged_record::name() const { 
  return m_definition->m_name;
}

vil_nitf2_tagged_record* vil_nitf2_tagged_record::create(vil_nitf2_istream& input)
{
  vil_nitf2_tagged_record* record = new vil_nitf2_tagged_record();
  if (record->read(input)) {
    return record;
  } else {
    delete record;
    return 0;
  }
}

bool vil_nitf2_tagged_record::read(vil_nitf2_istream& input)
{
  // Read TRE tag
  m_tag_field = vil_nitf2_scalar_field::read(input, s_tag_definition);
  if (!m_tag_field) {
    vcl_cerr << "Error reading extension tag at offset " << input.tell() << "." << vcl_endl;
    // Can't continue reading file
    return false;
  }
  vcl_string cetag;
  m_tag_field->value(cetag);

  // Read TRE data length
  m_length_field = vil_nitf2_scalar_field::read(input, s_length_definition);
  if (!m_length_field) {
    vcl_cerr << "Error reading extension length for tag " << cetag << "." << vcl_endl;
    // Can't continue reading file
    return false;
  }
  m_length_field->value(m_length);

  // See if this record is defined ...
  vil_nitf2_tagged_record_definition* record_definition = 
    vil_nitf2_tagged_record_definition::find(cetag);

  // ... if not, skip ahead to next record ...
  if (record_definition == 0) {
    VIL_NITF2_LOG(log_info) << "Skipping unknown record " << cetag << "." << vcl_endl;
    // Return whether I've found end of record
    //input.seekg(ceLength, vcl_ios::cur);
    input.seek(input.tell()+m_length);
    return input.ok();
  }
  // ... otherwise, populate this record's fields.
  // First save the position to check later that we read entire record
  vil_streampos record_data_start_pos = input.tell();
  m_definition = record_definition;
  m_field_sequence = new vil_nitf2_field_sequence(record_definition->field_definitions()); 
  m_field_sequence->read(input);

  // Check that the expected amount of data was read
  vil_streampos expected_pos = record_data_start_pos;
  expected_pos += m_length;
  if (input.tell() != expected_pos) {
    vcl_cerr << "vil_nitf2_tagged_record::read(): Read " << input.tell() - record_data_start_pos 
         << " bytes instead of " << m_length << " as expected." << vcl_endl;
    // Attempt to reposition input stream to end of record; return whether 
    // successful
    input.seek(expected_pos);
    return input.ok();
  } 
  // At end of record, as expected
  return true;
}

bool vil_nitf2_tagged_record::get_value(vcl_string tag, long long& out_value) const {
  return m_field_sequence->get_value(tag, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, int& out_value) const {
  return m_field_sequence->get_value(tag, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, double& out_value) const {
  return m_field_sequence->get_value(tag, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, char& out_value) const {
  return m_field_sequence->get_value(tag, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, void*& out_value) const {
  return m_field_sequence->get_value(tag, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, vcl_string& out_value) const {
  return m_field_sequence->get_value(tag, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, vil_nitf2_location*& out_value) const {
  return m_field_sequence->get_value(tag, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, vil_nitf2_date_time& out_value) const {
  return m_field_sequence->get_value(tag, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, long long& out_value) const {
  return m_field_sequence->get_value(tag, indexes, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, int& out_value) const {
  return m_field_sequence->get_value(tag, indexes, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, double& out_value) const {
  return m_field_sequence->get_value(tag, indexes, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, char& out_value) const {
  return m_field_sequence->get_value(tag, indexes, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, void*& out_value) const {
  return m_field_sequence->get_value(tag, indexes, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vcl_string& out_value) const {
  return m_field_sequence->get_value(tag, indexes, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vil_nitf2_location*& out_value) const {
  return m_field_sequence->get_value(tag, indexes, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vil_nitf2_date_time& out_value) const {
  return m_field_sequence->get_value(tag, indexes, out_value); }


vil_nitf2_tagged_record::vil_nitf2_tagged_record() 
  : m_length_field(0), m_tag_field(0), m_length(0), m_definition(0), m_field_sequence(0)
{}

bool vil_nitf2_tagged_record::test()
{
  bool error = false;
  // Example Tagged Record Extension definition
  vil_nitf2_tagged_record_definition MTIRPB = 
    vil_nitf2_tagged_record_definition::define("MTIRPB")
    .field("MTI_DP",           "Destination Point",     NITF_INT(2))
    .field("MTI_PACKET_ID",    "MTI Packed ID Number",  NITF_INT(3))
    .field("DATIME",           "Scan Date & Time",      NITF_DAT(14), true)
    .field("ACFT_LOC",         "Aircraft Position",     NITF_LOC(21)) 
    .field("ACFT_LOC2",        "Aircraft Position 2",   NITF_LOC(21))
    .field("SQUINT_ANGLE",     "Squint Angle",          NITF_DBL(6, 2, true), true)
    .field("NO_VALID_TARGETS", "Number of Targets",     NITF_INT(3))
    .field("TGT_CAT",        "Target Classification Category", 
           NITF_ENUM(1, vil_nitf2_enum_values()
            .value("H", "Helicopter")
            .value("T", "Tracked")
            .value("U", "Unknown")
            .value("W", "Wheeled")
            .value("TOO LONG", "Too long value test")
            .value("T", "Duplicate value test")))
    .repeat(new vil_nitf2_field_value<int>("NO_VALID_TARGETS"),
            vil_nitf2_field_definitions()
        .field("TGT_n_SPEED", "Target Estimated Ground Speed", NITF_INT(4),
             true)
        .field("TGT_n_CAT",   "Target Classification Category", 
             NITF_ENUM(1, vil_nitf2_enum_values()
               .value("H", "Helicopter")
               .value("T", "Tracked")
               .value("U", "Unknown")
               .value("W", "Wheeled")),
             true) )
    .field("TEST_NEG_COND", "Test False Condition", NITF_STR_BCSA(14), false, 
           0, new vil_nitf2_field_value_greater_than<int>("MTI_DP", 5))
    .field("TEST_POS_COND", "Test True Condition",  NITF_STR_BCSA(14), false,
           0, new vil_nitf2_field_value_greater_than<int>("MTI_DP", 1))
    .field("CLASS",  "Security Classification", 
           NITF_ENUM(1, vil_nitf2_enum_values()
            .value("T", "Top Secret")
            .value("S", "Secret")
            .value("C", "Confindential")
            .value("R", "Restricted")
            .value("U", "Unclassified")),
            true, 0, 0)
     .field( "CODEW", "Code Words",                  NITF_STR_BCSA(15), false,
              0, new vil_nitf2_field_value_one_of<vcl_string>( "CLASS", "T" ) )
     .field( "CWTEST", "Another Code Word Test",     NITF_STR_BCSA(15), false,
              0, new vil_nitf2_field_value_one_of<vcl_string>( "CLASS", "U" ) )
     .field( "NBANDS", "Number of bands",            NITF_INT(1), false,
             0, 0 )
     .field( "XBANDS", "Large number of bands",      NITF_INT(2), false,
             0, new vil_nitf2_field_value_one_of<int>("NBANDS",0))
     .repeat(new vil_nitf2_choose_field_value<int>("NBANDS", "XBANDS", 
                new vil_nitf2_field_value_greater_than<int>("NBANDS", 0)),
             vil_nitf2_field_definitions()
       .field( "BAND_LTR", "Band Description",       NITF_CHAR(), true, 
              0))
     .field( "N",   "Test repeat N", NITF_INT(1))
     // test nested repeats and functor references to tags within and 
     // outside repeat loops
     .repeat(new vil_nitf2_field_value<int>("N"), vil_nitf2_field_definitions()
        .field("A", "Test repeat A", NITF_INT(1))
        .repeat(new vil_nitf2_field_value<int>("N"), vil_nitf2_field_definitions()
           .field("S", "Test repeat S", NITF_STR(1)))
        .repeat(new vil_nitf2_field_value<int>("A"), vil_nitf2_field_definitions()
           .field("B", "Test repeat B", NITF_STR_BCSA(1))
           .repeat(new vil_nitf2_field_value<int>("A"), vil_nitf2_field_definitions()
              .field("C", "Test repeat C", NITF_STR_BCSA(1)))))
          
    .end();
  // Create a test input vcl_string
  vcl_string testFieldsStr = 
    "02"                     // MTI_DP
    "003"                    // MTI_PACKET_ID
    //"19990908070605"         // DATIME
    "              "         // DATIME
    "+89.111111-159.222222"  // ACFT_LOC
    "890102.33N0091122.00W"  // ACFT_LOC2
    "-60.00"                 // SQUINT_ANGLE
    "003"                    // NO_VALID_TARGETS
    " "                      // TGT_CAT
    "2222" "H"               // TGT_1_SPEED2, TGT_1_CAT2
    "    " " "               // TGT_2_SPEED2, TGT_2_CAT2
    "4444" "T"               // TGT_3_SPEED2, TGT_3_CAT2
    ""                       // TEST_NEG_COND not present
    "True Condition"         // TEST_POS_COND
    "T"                      // CLASS
    "RandomCodeWords"        // CODEW (only present if CLASS=T)
    ""                       // CWTEST (not present because CLASS!=U
    "0"                      // NBANDS (0, so use XBANDS instead)
    "12"                     // XBANDS (present because NBANDS=0)
    "abcdefghijkl"           // 12 BAND_LTRs (XBAND=12)
    // test nested repeats
    "2"    // N
    // for i=0..N: i=0
    "1"    // A[0]
    "ST"   // S[0,0:1]
    //   for j=0..A[i]
    "B"    // B[0,0]
    //     for k=0..A[i]
    "C"    // C[0,0,0]
    // i=1:
    "2"    // A[1]
    "st"   // S[1,0:1]
    //   for j=0..A[i]: j=0
    "b"    // B[1,0]
    //     for k=0..A[i]
    "cc"    // C[1,0,0:1]
    "B"    // B[1,1]
    "CC"   //  C[1,1,0:1]
  ;
  vcl_stringstream test_stream;
  test_stream << "MTIRPB"; // CETAG
  test_stream << vcl_setw(5) << vcl_setfill('0') << testFieldsStr.length(); // CELENGTH
  test_stream << testFieldsStr; // rest of fields
  vcl_string read_string = test_stream.str();
  // Write the test input vcl_string to a vil_stream
  vil_stream_core* vs = new vil_stream_core();
  vs->write(read_string.c_str(), read_string.length());
  vs->seek(0);
  vil_stream_section* vss = new vil_stream_section(vs, 0, read_string.length());
  // Record from the vil_stream
  vil_nitf2_tagged_record* record = vil_nitf2_tagged_record::create(*vss);
  if (record) {
    vcl_cout << *record << vcl_endl;
    // Now write the record, and compare the output to the test input
    vcl_cout << "\nOriginal string:\n" << read_string;
    vcl_cout << "\nWrite() output:\n";
    vil_stream_core* vs2 = new vil_stream_core();
    record->write(*(vil_stream*)vs2);
    vil_streampos bufsize = vs2->file_size();
    char* buf = new char[(unsigned int)bufsize + 1];
    vs2->seek(0);
    vs2->read(buf, bufsize);
    buf[bufsize]='\0';
    vcl_string write_string = vcl_string(buf);
    vcl_cout << write_string << vcl_endl;
    if (read_string != write_string) {
      vcl_cout << "\nWrite failed!" << vcl_endl;
      error = true;
    }
    delete buf;
    vcl_cout << "Testing get_value: " << vcl_endl;
    int mti_dp;
    if (!record->get_value("MTI_DP", mti_dp) || mti_dp!=2) {
      vcl_cout << "Get value failed!" << vcl_endl;
      error = true;
    } else {
      vcl_cout << "MTI_DP = " << mti_dp << vcl_endl;
    }
    int tgt_speed[4];
    if (!record->get_value("TGT_n_SPEED", vil_nitf2_index_vector(0), tgt_speed[0])  ||
        tgt_speed[0] != 2222 ||
        record->get_value("TGT_n_SPEED", vil_nitf2_index_vector(1), tgt_speed[1]) /*should be null*/ || 
        !record->get_value("TGT_n_SPEED", vil_nitf2_index_vector(2), tgt_speed[2]) ||
        tgt_speed[2] != 4444) {
      vcl_cout << "Get vector value test failed!" << vcl_endl;
      error = true;
    } else {
      vcl_cout << "TGT_2_SPEED = " << tgt_speed[2] << vcl_endl;
    }
  } else {
    vcl_cout << "Didn't create record!" << vcl_endl;
    error = true;
  }
  // Try output of vector field
  vcl_cout << "Output of vector field C:" << vcl_endl;
  vcl_cout << *(record->get_field("C"));

  return !error;
}

vcl_ostream& vil_nitf2_tagged_record::output(vcl_ostream& os) const
{
  os << "CETAG: " << name() << vcl_endl;
  os << "CELEN: " << length() << vcl_endl;
  for (vil_nitf2_field_definitions::iterator fieldNode = m_definition->m_field_definitions->begin();
       fieldNode != m_definition->m_field_definitions->end(); ++fieldNode)
  {
    vil_nitf2_field_definition* field_def = (*fieldNode)->field_definition();
    // to do: handle other nodes
    if (!field_def) break;
    vil_nitf2_field* field = get_field(field_def->tag);
    os << field_def->tag << ": ";
    if (field) {
      os << *field << vcl_endl;
    } else {
      os << "(undefined)" << vcl_endl;
    }
  } 
  return os;
}

bool vil_nitf2_tagged_record::write(vil_nitf2_ostream& output)
{
  // To track of how much is written
  vil_streampos start = output.tell();
  // Write tag and length fields
  if (m_tag_field && m_length_field) {
    m_tag_field->write(output);
    m_length_field->write(output);
  } else return false;
  // Write data fields
  m_field_sequence->write(output);
  // Check whether the vcl_right amount was written
  vil_streampos end = output.tell();
  vil_streampos length_written = end - start;
  int expected_length = s_tag_formatter->field_width + s_length_formatter->field_width 
    + length();
  return length_written == expected_length;
}

vil_nitf2_tagged_record::~vil_nitf2_tagged_record()
{
  delete m_field_sequence;
}

vil_nitf2_field_definition* vil_nitf2_field_sequence::find_field_definition(vcl_string tag)
{
  for (vil_nitf2_field_definitions::const_iterator node = m_field_definitions->begin();
       node != m_field_definitions->end(); ++node)
  {
    vil_nitf2_field_definition* field_def = (*node)->field_definition();
    // to do: search other nodes
    if (!field_def) break;

    if (field_def->tag == tag) {
      return field_def;
    }
  }
  // tag definition not found
  return 0;
}

vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_tagged_record& record) 
{
  return record.output(os); 
}
