// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_tagged_record.h"

#include <vcl_sstream.h>
#include <vcl_iomanip.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

#include <vil/vil_stream_core.h>
#include <vil/vil_stream_section.h>

#include "vil_nitf2_tagged_record_definition.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_field_sequence.h"
#include "vil_nitf2_index_vector.h"
#include "vil_nitf2_typed_field_formatter.h"
#include "vil_nitf2_scalar_field.h"

vil_nitf2_field_definition& vil_nitf2_tagged_record::s_length_definition()
{
  static vil_nitf2_field_definition length_definition(
    "CEL", "Extension Length", new vil_nitf2_integer_formatter(5));
  return length_definition;
}

vil_nitf2_field_definition& vil_nitf2_tagged_record::s_tag_definition()
{
  static vil_nitf2_field_definition tag_definition (
    "CETAG", "Extension Tag", new vil_nitf2_string_formatter(6));
  return tag_definition;
}

vil_nitf2_integer_formatter& vil_nitf2_tagged_record::s_length_formatter()
{
  static vil_nitf2_integer_formatter length_formatter(5);
  return length_formatter;
}

vil_nitf2_string_formatter& vil_nitf2_tagged_record::s_tag_formatter()
{
  static vil_nitf2_string_formatter tag_formatter(6);
  return tag_formatter;
}

vcl_string vil_nitf2_tagged_record::name() const
{
  vcl_string cetag;
  if ( m_tag_field->value(cetag) ) return cetag;
  else                             return "<Unknown>";
}

vcl_string vil_nitf2_tagged_record::pretty_name() const
{
  if ( m_definition ) return m_definition->m_pretty_name;
  else                return "<unknown>";
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
  m_tag_field = vil_nitf2_scalar_field::read(input, &s_tag_definition());
  if (!m_tag_field) {
    vcl_cerr << "Error reading extension tag at offset " << input.tell() << ".\n";
    // Can't continue reading file
    return false;
  }
  vcl_string cetag;
  m_tag_field->value(cetag);

  // Read TRE data length
  m_length_field = vil_nitf2_scalar_field::read(input, &s_length_definition());
  if (!m_length_field) {
    vcl_cerr << "Error reading extension length for tag " << cetag << ".\n";
    // Can't continue reading file
    return false;
  }
  m_length_field->value(m_length);

  // See if this record is defined ...
  vil_nitf2_tagged_record_definition* record_definition =
    vil_nitf2_tagged_record_definition::find(cetag);

  // ... if not, skip ahead to next record ...
  if (record_definition == 0) {
    VIL_NITF2_LOG(log_info) << "Skipping unknown record " << cetag << ".\n";
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
             << " bytes instead of " << m_length << " as expected.\n";
    // Attempt to reposition input stream to end of record; return whether
    // successful
    input.seek(expected_pos);
    return input.ok();
  }
  // At end of record, as expected
  return true;
}

bool vil_nitf2_tagged_record::get_value(vcl_string tag, int& out_value) const
{ return m_field_sequence->get_value(tag, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, double& out_value) const
{ return m_field_sequence->get_value(tag, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, char& out_value) const
{ return m_field_sequence->get_value(tag, out_value); }
bool vil_nitf2_tagged_record::get_value(vcl_string tag, void*& out_value) const
{ return m_field_sequence->get_value(tag, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, vcl_string& out_value) const
{ return m_field_sequence->get_value(tag, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, vil_nitf2_location*& out_value) const
{ return m_field_sequence->get_value(tag, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, vil_nitf2_date_time& out_value) const
{ return m_field_sequence->get_value(tag, out_value); }

#if VXL_HAS_INT_64
// if not VXL_HAS_INT_64 isn't defined the vil_nitf2_long is the same as just plain 'int'
// and this function will be a duplicate of that get_value
bool vil_nitf2_tagged_record::get_value(vcl_string tag, vil_nitf2_long& out_value) const
{ return m_field_sequence->get_value(tag, out_value); }
#endif

bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, int& out_value) const
{ return m_field_sequence->get_value(tag, indexes, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, double& out_value) const
{ return m_field_sequence->get_value(tag, indexes, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, char& out_value) const
{ return m_field_sequence->get_value(tag, indexes, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, void*& out_value) const
{ return m_field_sequence->get_value(tag, indexes, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vcl_string& out_value) const
{ return m_field_sequence->get_value(tag, indexes, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vil_nitf2_location*& out_value) const
{ return m_field_sequence->get_value(tag, indexes, out_value); }

bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vil_nitf2_date_time& out_value) const
{ return m_field_sequence->get_value(tag, indexes, out_value); }

#if VXL_HAS_INT_64
// if not VXL_HAS_INT_64 isn't defined the vil_nitf2_long is the same as just plain 'int'
// and this function will be a duplicate of that get_value
bool vil_nitf2_tagged_record::get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vil_nitf2_long& out_value) const
{ return m_field_sequence->get_value(tag, indexes, out_value); }
#endif

// Macro to define both overloads of get_values()
#define VIL_NITF2_TAGGED_RECORD_GET_VALUES(T) \
bool vil_nitf2_tagged_record::get_values(vcl_string tag, const vil_nitf2_index_vector& indexes, \
                                         vcl_vector<T>& out_values, bool clear_out_values) const \
{ return m_field_sequence->get_values(tag, indexes, out_values, clear_out_values); } \
bool vil_nitf2_tagged_record::get_values(vcl_string tag, vcl_vector<T>& out_values) const \
{ return m_field_sequence->get_values(tag, out_values); }

VIL_NITF2_TAGGED_RECORD_GET_VALUES(int);
VIL_NITF2_TAGGED_RECORD_GET_VALUES(double);
VIL_NITF2_TAGGED_RECORD_GET_VALUES(char);
VIL_NITF2_TAGGED_RECORD_GET_VALUES(void*);
VIL_NITF2_TAGGED_RECORD_GET_VALUES(vcl_string);
VIL_NITF2_TAGGED_RECORD_GET_VALUES(vil_nitf2_location*);
VIL_NITF2_TAGGED_RECORD_GET_VALUES(vil_nitf2_date_time);
#if VXL_HAS_INT_64
  VIL_NITF2_TAGGED_RECORD_GET_VALUES(vil_nitf2_long);
#endif


vil_nitf2_tagged_record::vil_nitf2_tagged_record()
  : m_length_field(0), m_tag_field(0), m_length(0), m_definition(0), m_field_sequence(0)
{}

// TO DO: rewrite this method a sequence of unit tests!
//
bool vil_nitf2_tagged_record::test()
{
  bool error = false;
  const char* test_tre_tag = "@TEST@";
  // Example Tagged Record Extension definition
  vil_nitf2_tagged_record_definition::define(test_tre_tag, "Test Definition" )
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
           .field("TGT_n_SPEED", "Target Estimated Ground Speed", NITF_INT(4), true)
           .field("TGT_n_CAT",   "Target Classification Category",
                  NITF_ENUM(1, vil_nitf2_enum_values()
                            .value("H", "Helicopter")
                            .value("T", "Tracked")
                            .value("U", "Unknown")
                            .value("W", "Wheeled")),
                  true))
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
   .field("CODEW", "Code Words",                  NITF_STR_BCSA(15), false,
          0, new vil_nitf2_field_value_one_of<vcl_string>( "CLASS", "T" ) )
   .field("CWTEST", "Another Code Word Test",     NITF_STR_BCSA(15), false,
          0, new vil_nitf2_field_value_one_of<vcl_string>( "CLASS", "U" ) )
   .field("NBANDS", "Number of bands",            NITF_INT(1), false,
          0, 0 )
   .field("XBANDS", "Large number of bands",      NITF_INT(2), false,
          0, new vil_nitf2_field_value_one_of<int>("NBANDS",0))
   .repeat(new vil_nitf2_choose_field_value<int>("NBANDS", "XBANDS",
             new vil_nitf2_field_value_greater_than<int>("NBANDS", 0)),
             vil_nitf2_field_definitions()
     .field("BAND_LTR", "Band Description",       NITF_CHAR(), true,
            0)
   )
   .field( "EXP_TEST", "Exponential format test",  NITF_EXP(6,1))
   // test nested repeats and functor references to tags within and
   // outside repeat loops
   .field( "N",   "Test repeat N", NITF_INT(1))
   .repeat(new vil_nitf2_field_value<int>("N"), vil_nitf2_field_definitions()
           .field("A", "Test repeat A", NITF_INT(1))
           .repeat(new vil_nitf2_field_value<int>("N"), vil_nitf2_field_definitions()
                   .field("S", "Test repeat S", NITF_STR(3)))
           .repeat(new vil_nitf2_field_value<int>("A"), vil_nitf2_field_definitions()
                   .field("B", "Test repeat B", NITF_STR_BCSA(3))
                   .repeat(new vil_nitf2_field_value<int>("A"), vil_nitf2_field_definitions()
                           .field("C", "Test repeat C", NITF_STR_BCSA(4)))))
   // test fixed repeat count
   .repeat(4, vil_nitf2_field_definitions()
           .field("D", "Test fixed repeat", NITF_INT(1))
   )
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
    "+1.234567E-8"           // Exponential format test
    // test nested repeats
    "2"       // N
    // for i=0...N-1: i=0
    "1"       // A[0]
    "S00"     // S[0,0]
    "S01"     // S[0,1]
    //   for j=0...A[i]-1: j=0
    "B00"     // B[0,0]
    //     for k=0..A[i]-1: k=0
    "C000"    // C[0,0,0]
    // i=1:
    "2"       // A[1]
    "S10"     // S[1,0]
    "S11"     // S[1,1]
    //   for j=0..A[i]: j=0
    "B10"     // B[1,0]
    //     for k=0..A[i]
    "C100"      // C[1,0,0]
    "C101"      // C[1,0,1]
    "B11"       // B[1,1]
    "C110"      // C[1,1,0]
    "C111"      // C[1,1,1]
    // test fixed repeat
    "7890"
  ;
  vcl_stringstream test_stream;
  test_stream << test_tre_tag; // CETAG
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
  if (record)
  {
    vcl_cerr << *record << vcl_endl;
    // Now write the record, and compare the output to the test input
    vcl_cerr << "\nOriginal string:\n" << read_string
             << "\nWrite() output:\n";
    vil_stream_core* vs2 = new vil_stream_core();
    record->write(*(vil_stream*)vs2);
    vil_streampos bufsize = vs2->file_size();
    char* buf = new char[(unsigned int)bufsize + 1];
    vs2->seek(0);
    vs2->read(buf, bufsize);
    buf[bufsize]='\0';
    vcl_string write_string = vcl_string(buf);
    vcl_cerr << write_string << vcl_endl;
    if (read_string != write_string) {
      vcl_cerr << "\nWrite failed!\n";
      error = true;
    }
    delete buf;
    vcl_cerr << "Testing get_value:\n";
    int mti_dp;
    if (!record->get_value("MTI_DP", mti_dp) || mti_dp!=2) {
      vcl_cerr << "Get value failed!\n";
      error = true;
    } else {
      vcl_cerr << "MTI_DP = " << mti_dp << vcl_endl;
    }
    int tgt_speed[4];
    if (!record->get_value("TGT_n_SPEED", vil_nitf2_index_vector(0), tgt_speed[0])  ||
        tgt_speed[0] != 2222 ||
        record->get_value("TGT_n_SPEED", vil_nitf2_index_vector(1), tgt_speed[1]) /*should be null*/ ||
        !record->get_value("TGT_n_SPEED", vil_nitf2_index_vector(2), tgt_speed[2]) ||
        tgt_speed[2] != 4444) {
      vcl_cerr << "Get vector value test failed!\n";
      error = true;
    } else {
      vcl_cerr << "TGT_2_SPEED = " << tgt_speed[2] << vcl_endl;
    }
    int d2;
    if (!record->get_value("D", vil_nitf2_index_vector(2), d2) || d2 != 9) {
      vcl_cerr << "Get fixed repeat count test failed!\n";
      error = true;
    }
    // fetch C[*]
    vcl_cerr << "Testing get_values (all values)...\n";
    vcl_vector<vcl_string> c_values;
    if (!record->get_values("C", c_values) ||
        c_values.size() != 5 ||
        c_values[0]!="C000" ||
        c_values[1]!="C100" ||
        c_values[2]!="C101" ||
        c_values[3]!="C110" ||
        c_values[4]!="C111") {
      vcl_cerr << "failed!\n\n";
      error = true;
    }
    // Fetch A[1,*]
    vcl_cerr << "Get values (partial index)...\n";
    vil_nitf2_index_vector indexes;
    vcl_vector<int> a_values;
    indexes.push_back(1);
    if (!record->get_values("A", indexes, a_values) ||
        a_values.size() != 1 ||
        a_values[0] != 2) {
      vcl_cerr << "failed!\n\n";
      error = true;
    }
    // Fetch C[1,*]
    if (!record->get_values("C", indexes, c_values) ||
        c_values.size() != 4 ||
        c_values[0]!="C100" ||
        c_values[1]!="C101" ||
        c_values[2]!="C110" ||
        c_values[3]!="C111")
    {
      vcl_cerr << "failed!\n\n";
    }
  } else {
    vcl_cerr << "Didn't create record!\n";
    error = true;
  }
  // Try output of vector field
  vcl_cerr << "Output of vector field C:\n"
           << *(record->get_field("C"));
  // Clean up test definition and test cleanup
  if (!vil_nitf2_tagged_record_definition::undefine(test_tre_tag)) {
    vcl_cerr << "Error undefining TRE.\n";
    error = true;
  }
  return !error;
}

vcl_ostream& vil_nitf2_tagged_record::output(vcl_ostream& os) const
{
  os << "CETAG: " << name() << vcl_endl
     << "CELEN: " << length() << vcl_endl;
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
  int expected_length = s_tag_formatter().field_width + s_length_formatter().field_width
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

vil_nitf2_field::field_tree* vil_nitf2_tagged_record::get_tree() const
{
  //create our tree
  //we add the field definitions if the TRE was recognized, or we note that we
  //skipped it otherwise
  vil_nitf2_field::field_tree* tr;
  if ( m_field_sequence ) {
    tr = m_field_sequence->get_tree();
  } else {
    tr = new vil_nitf2_field::field_tree;
    vil_nitf2_field::field_tree* skipped_node = new vil_nitf2_field::field_tree;
    skipped_node->columns.push_back( "CEDATA" );
    skipped_node->columns.push_back( "<skipped unknown TRE>" );
    tr->children.push_back( skipped_node );
  }

  //add the columns describing the name of the TRE
  tr->columns.push_back( name() );
  tr->columns.push_back( pretty_name() );
  //add the CEL (length) field to the front
  vil_nitf2_field::field_tree* first_child = new vil_nitf2_field::field_tree;
  first_child->columns.push_back( "CEL" );
  first_child->columns.push_back( "Extension Length" );
  vcl_stringstream len_stream;
  len_stream << length();
  first_child->columns.push_back( len_stream.str() );
  tr->children.insert( tr->children.begin(), first_child );
  return tr;
}

vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_tagged_record& record)
{
  return record.output(os);
}

// vil_nitf2_tagged_record_sequence

vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_tagged_record_sequence& seq)
{
  os << seq.size() << " TRE's:" << vcl_endl;
  vil_nitf2_tagged_record_sequence::const_iterator it;
  for (it = seq.begin(); it != seq.end(); ++it) {
    os << *it << vcl_endl;
  }
  return os;
}
