// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_array_field.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_index_vector.h"

#include <vcl_cstddef.h> // for size_t
#include <vcl_sstream.h>
#include <vil/vil_stream_core.h>

int vil_nitf2_array_field::num_dimensions() const
{
  return m_num_dimensions;
}

int vil_nitf2_array_field::
next_dimension(const vil_nitf2_index_vector& index) const
{
  vcl_map<vil_nitf2_index_vector,int>::const_iterator dimension_bounds_entry = m_dimensions_map.find(index);
  if (dimension_bounds_entry != m_dimensions_map.end()) {
    return dimension_bounds_entry->second;
  } else {
    return 0;
  }
}

void vil_nitf2_array_field::
set_next_dimension(const vil_nitf2_index_vector& index, int bound)
{
  if ((int)index.size() >= m_num_dimensions) {
    vcl_cerr << "vil_nitf2_array_field::set_next_dimension"
             << index << ": invalid partial index!\n";
    return;
  }
  if (next_dimension(index) > 0) {
    vcl_cerr << "vil_nitf2_array_field::set_next_dimension"
             << index << ": bound previously set!\n";
  }
  m_dimensions_map[index] = bound;
}

bool vil_nitf2_array_field::
check_index(const vil_nitf2_index_vector& indexes) const
{
  if ((int)indexes.size() != m_num_dimensions) {
    vcl_cerr << "index length does not match value dimensions!\n";
    return false;
  }
  // Remove the last element from index and look it up in the dimensions map.
  // Then check if the removed element is less than the returned dimension bound.
  // If no dimension bound is found, then one of the other index values is
  // invalid.
  vil_nitf2_index_vector dimension_index;
  for (int dim=0; dim < m_num_dimensions-1; ++dim) {
    dimension_index.push_back(indexes[dim]);
  }
  int dimension_bound = next_dimension(dimension_index);
  int last_index = indexes[indexes.size()-1];
  if (last_index < dimension_bound) {
    return true;
  } else {
    vcl_cerr << "Tag " << tag() << indexes << ": index out of bounds!\n";
    return false;
  }
}

vcl_string int_to_string( int i )
{
  vcl_stringstream s;
  s << i;
  return s.str();
}

vcl_string index_string( const vil_nitf2_index_vector& indices )
{
  vcl_string ret_val = "";
  for ( unsigned int i = 0 ; i < indices.size() ; i++ ){
    ret_val += "[" + int_to_string( indices[i] ) + "]";
  }
  return ret_val;
}

vcl_string vil_nitf2_array_field::get_value_string(const vil_nitf2_index_vector& in_indices) const
{
  vil_stream_core* str = new vil_stream_core;
  write_vector_element( *str, in_indices, -1 );
  vil_streampos num_to_read = str->tell();
  str->seek( 0 );
  char* buffer;
  buffer = (char*)malloc( (vcl_size_t) num_to_read+1 );
  str->read( (void*)buffer, num_to_read );
  buffer[(vcl_size_t) num_to_read] = 0;
  return vcl_string( buffer );
}

void vil_nitf2_array_field::do_dimension( const vil_nitf2_index_vector& in_indices,
                                          vil_nitf2_field::field_tree* inTree ) const
{
  int dim = next_dimension( in_indices );
  for ( int i = 0 ; i < dim ; i++ )
  {
    //this is the index list we're dealing with in this loop
    vil_nitf2_index_vector curr_indices = in_indices;
    curr_indices.push_back( i );
    //create our tree node and add it to inTree's child list
    vil_nitf2_field::field_tree* tr = new vil_nitf2_field::field_tree;
    vcl_string tag_str = tag();
    vcl_string index_str = index_string( curr_indices );
    vcl_string p_name;
    if ( index_str == "" ) p_name = pretty_name();
    else p_name = "";
    tr->columns.push_back( tag_str + index_str );
    tr->columns.push_back( p_name );
    if( check_index( curr_indices ) ) {
      tr->columns.push_back( get_value_string( curr_indices ) );
    }
    inTree->children.push_back( tr );
    //recursive call
    do_dimension( curr_indices, tr );
  }
}

vil_nitf2_field::field_tree* vil_nitf2_array_field::get_tree() const
{
  field_tree* tr = vil_nitf2_field::get_tree();
  do_dimension( vil_nitf2_index_vector(), tr );
  return tr;
}
