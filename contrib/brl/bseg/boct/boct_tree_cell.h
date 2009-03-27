#ifndef boct_tree_cell_h_
#define boct_tree_cell_h_

#include <vbl/vbl_ref_count.h>
#include "boct_loc_code.h"
#include "boct_tree_cell_base.h"
#include "boct_tree_cell_base_sptr.h"

template <class T>
class boct_tree_cell : public boct_tree_cell_base
{
public:
  boct_tree_cell();
  boct_tree_cell(const boct_loc_code& code, boct_tree_cell_base_sptr p, short l) {code_=code; children_=0; parent_=p; level_=l; }
  //constructor given code and level
  boct_tree_cell(const boct_loc_code& code, short level) : boct_tree_cell_base(code,level) {}
  boct_tree_cell(const boct_tree_cell& rhs);
  ~boct_tree_cell() {}
  void set_data(T& data) {data_=data; }
  T get_data() {return data_; }
private:
  T data_;
};

void vsl_b_write(vsl_b_ostream & os, const boct_tree_cell<T>& cell);

#endif