#include "bstm_multi/bstm_multi_tree_util.h"

#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <boct/boct_bit_tree.h>
#include <bstm/bstm_time_tree.h>

vcl_string ste_as_string(space_time_enum ste) {
  switch (ste) {
  case STE_SPACE:
    return "space";
  case STE_TIME:
    return "time";
  }
}

bool ste_from_string(const vcl_string &s, space_time_enum &ste) {
  if (s == "space") {
    ste = STE_SPACE;
  } else if (s == "time") {
    ste = STE_TIME;
  } else {
    return false;
  }
  return true;
}

vcl_vector<space_time_enum> parse_subdivisions(const vcl_string &s) {
  vcl_vector<space_time_enum> result;
  vcl_stringstream ss(s);
  while (ss.good() > 0) {
    vcl_string substr;
    space_time_enum ste;
    vcl_getline(ss, substr, ',');
    // skip any invalid subdivisions that are not 'space' or 'time'
    if (ste_from_string(substr, ste)) {
      result.push_back(ste);
    }
  }
  return result;
}

vcl_string print_subdivisions(const vcl_vector<space_time_enum> &subdivisions) {
  vcl_string subdivs_str;
  for (vcl_vector<space_time_enum>::const_iterator iter = subdivisions.begin();
       iter != subdivisions.end();
       ++iter) {
    if (iter != subdivisions.begin()) {
      subdivs_str += ",";
    }
    subdivs_str += ste_as_string(*iter);
  }
  return subdivs_str;
}

vcl_size_t tree_size(space_time_enum ste) {
  switch (ste) {
  case STE_SPACE:
    return sizeof(space_tree_b);
  case STE_TIME:
    return sizeof(time_tree_b);
  }
}
