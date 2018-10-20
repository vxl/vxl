#ifndef boct_tree_cell_reader_hxx_
#define boct_tree_cell_reader_hxx_

#include "boct_tree_cell_reader.h"

#include <vgl/vgl_box_3d.h>
#include <vgl/io/vgl_io_box_3d.h>


template <class T_loc,class T_data>
void boct_tree_cell_reader<T_loc,T_data>::begin()
{
  // read header info of the tree
  if (!is_) return;

  short v;
  short max_level;
  vgl_box_3d<double> global_bbox, tree_bb;
  vsl_b_read(*is_, v);

  switch (v) {
    case (1):
    {
      vsl_b_read(*is_, global_bbox);

      // read the tree header
      vsl_b_read(*is_, v);
      switch (v) {
        case (1):
        {
          vsl_b_read(*is_, max_level);
          vsl_b_read(*is_, tree_bb);
          version_=1;
          break;
        }

        case (2):
        {
          vsl_b_read(*is_, max_level);
          vsl_b_read(*is_, tree_bb);
          vsl_b_read(*is_, num_cells_);
          version_=2;
          break;
        }
        case (3):
          {
            vsl_b_read(*is_, max_level);
            vsl_b_read(*is_, tree_bb);
            vsl_b_read(*is_, num_cells_);
            version_ = 3;
            break;
          }
        default:
          std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boct_tree<T_loc,T_data>&)\n"
                   << "           Unknown version number of the tree "<< v << '\n';
          return ;
        }
     }
     break;
   default:
     std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boct_tree<T_loc,T_data>&)\n"
              << "           Unknown version number "<< v << '\n';
#if 0
                *is_.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
#endif
     return;
  }
}

template <class T_loc,class T_data>
bool boct_tree_cell_reader<T_loc,T_data>::next(boct_tree_cell<T_loc,T_data>& c)
{
  if (!is_)
    return false;

  if (version_==1) {
     bool leaf=false;
     while (!leaf) {
       short version;
       vsl_b_read(*is_,version);
       switch (version) {
         case 1:
         {
           vsl_b_read(*is_, c.code_);
           T_data data;
           vsl_b_read(*is_, data);
           c.set_data(data);
           c.set_vis_node(nullptr);
           vsl_b_read(*is_, leaf);
           if (leaf)
             return true;
           break;
         }
       }
     }
  }
  else if (version_==2) {
    if (num_cells_-- > 0) {
      vsl_b_read(*is_, c.code_);
      T_data data;
      vsl_b_read(*is_, data);
      c.set_data(data);
      c.set_vis_node(nullptr);
      return true;
    }
    else {
      is_->close();
    }
  }
  else if (version_ ==3) {
    struct store_struct {boct_loc_code<T_loc> code; T_data data;};
    if (num_cells_-- > 0) {
      //is_->is().read(reinterpret_cast<char*>(&c.code_),sizeof(boct_loc_code<T_loc>));
      //T_data data;
      //is_->is().read(reinterpret_cast<char*>(&data),sizeof(T_data));
      //c.set_data(data);
      store_struct read_cell;
      is_->is().read(reinterpret_cast<char*>(&read_cell),sizeof(store_struct));
      c.code_ = read_cell.code;
      c.set_data(read_cell.data);
      c.set_vis_node(nullptr);
      return true;
    }
    else {
      is_->close();
    }
  }
  else
    std::cerr << "bool boct_tree_cell_reader<T_loc,T_data>::next -- Wrong version number!" << std::endl;
  return false;
}

#define BOCT_TREE_CELL_READER_INSTANTIATE(T_loc,T_data) \
template class boct_tree_cell_reader<T_loc,T_data >

#endif // boct_tree_cell_reader_hxx_
