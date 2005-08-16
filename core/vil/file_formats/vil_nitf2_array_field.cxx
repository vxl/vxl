// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_array_field.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_index_vector.h"

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
      << index << ": invalid partial index!" << vcl_endl;
    return;
  }
  if (next_dimension(index) > 0) {
    vcl_cerr << "vil_nitf2_array_field::set_next_dimension" 
      << index << ": bound previously set!" << vcl_endl;
  }
  m_dimensions_map[index] = bound;
}

bool vil_nitf2_array_field::
check_index(const vil_nitf2_index_vector& indexes) const
{
  if ((int)indexes.size() != m_num_dimensions) {
    vcl_cerr << "index length does not match value dimensions!" << vcl_endl;
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
    vcl_cerr << "Tag " << tag() << indexes << ": index out of bounds!" << vcl_endl;
    return false;
  }
}
