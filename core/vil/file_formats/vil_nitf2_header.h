// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_HEADER_H
#define VIL_NITF2_HEADER_H

#include "vil_nitf2_tagged_record.h"
#include "vil_nitf2_classification.h"
#include "vil_nitf2.h"

class vil_stream;


/// Parses a NITF 2.1 file header for vil_nitf2_image. 
/// Use get_property() to get a field value.
//
class vil_nitf2_header 
{
public:
  vil_nitf2_header();

  virtual ~vil_nitf2_header();
  
  enum Portion { SubHeader, Data };
  enum Section { FileHeader = 0, ImageSegments, GraphicSegments, LabelSegments,
                TextSegments, DataExtensionSegments, ReservedExtensionSegments };
  static vcl_string section_num_tag( Section sec, bool pretty = false );
  static vcl_string section_len_header_tag( Section sec, bool pretty = false );
  static vcl_string section_len_data_tag( Section sec, bool pretty = false );
  
  // Read the image header starting at stream's current position. Return success.
  virtual bool read( vil_stream* stream );
  //virtual bool write( vil_stream* stream );

  // Sets out_value to the value of field specified by tag. 
  // Returns 0 if such a field is not found or is of the wrong type.
  template< class T >
  bool get_property(vcl_string tag, T& out_value) const
  {
    if ( m_field_sequence_classification && m_field_sequence_classification->get_value(tag, out_value) ) return true;
    else if ( m_field_sequence2 && m_field_sequence2->get_value(tag, out_value) ) return true;
    else return m_field_sequence1.get_value(tag, out_value);
  }

  // Sets out_value to the value of vcl_vector field element specified by tag and index.
  // Returns 0 if such a field is not found or is of the wrong type.
  template< class T >
  bool get_property(vcl_string tag, int i, T& out_value) const
  {
    if ( m_field_sequence_classification && m_field_sequence_classification->get_value(tag, i, out_value) ) return true;
    else if ( m_field_sequence2 && m_field_sequence2->get_value(tag, i, out_value) ) return true;
    else return m_field_sequence1.get_value(tag, i, out_value);
  }

  vil_nitf2_classification::file_version file_version() const;

  // I allocate the return value, but you own it after I return it to you
  // so you need to delete it.  
  virtual vil_nitf2_field::field_tree* get_tree() const;
protected:
  static void add_section( Section sec, int l1, int l2, vil_nitf2_field_definitions& defs, bool long_long = false);

  vil_nitf2_field_sequence m_field_sequence1;
  vil_nitf2_field_sequence* m_field_sequence2;
  vil_nitf2_field_sequence* m_field_sequence_classification;
  static vil_nitf2_field_definitions* get_field_definitions_1();
  static vil_nitf2_field_definitions* get_field_definitions_2( vil_nitf2_classification::file_version version );
private:
  // so these static members can be cleaned up when the program is done 
  // using nitf files
  friend static void vil_nitf2::cleanup_static_members();
  static vil_nitf2_field_definitions* s_field_definitions_1;
  static vil_nitf2_field_definitions* s_field_definitions_21;
  static vil_nitf2_field_definitions* s_field_definitions_20;
};


#endif // VIL_NITF2_HEADER_H
