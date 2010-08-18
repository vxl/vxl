#ifndef boxm_scene_h_
#define boxm_scene_h_
//:
// \file
// \brief  The main class to keep the 3D world data. It is a grid of boxm_blocks containg octrees. 
//
// \author Gamze Tunali
// \date Apr 03, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_block.h>
#include <boxm/sample/boxm_sample.h>
#include <bvgl/bvgl_point_3d_cmp.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vbl/vbl_array_3d.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <boct/boct_tree.h>
#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vcl_set.h>


class boxm_scene_parser;
template <class T> class boxm_block_iterator;
template <class T> class boxm_cell_iterator;

template <class T>
class boxm_scene :public boxm_scene_base
{
 public:
  typedef typename T::loc_type loc_type;
  typedef typename T::datatype datatype;
  
  //: Default constructor
  boxm_scene() : active_block_(vgl_point_3d<int>(-1,-1,-1)), save_internal_nodes_(false), save_platform_independent_(true) {}

  //: Constructor from lvcs, world origin, dimension of the block (in world coordinates) and number of blocks(world_dim)
  boxm_scene(const bgeo_lvcs& lvcs,
             const vgl_point_3d<double>& origin,
             const vgl_vector_3d<double>& block_dim,
             const vgl_vector_3d<unsigned>& world_dim,
             const bool load_all_blocks=false,
             const bool save_internal_nodes_ = false,
             const bool save_platform_independent_ = true);

  //: Constructor from lvcs, world origin, dimension of the block, number of blocks and initialization level
  boxm_scene(const bgeo_lvcs& lvcs,
             const vgl_point_3d<double>& origin,
             const vgl_vector_3d<double>& block_dim,
             const vgl_vector_3d<unsigned>& world_dim,
             unsigned max_level, unsigned init_level,
             const bool load_all_blocks=false,
             const bool save_internal_nodes_ = false,
             const bool save_platform_independent_ = true);

  //: Constructor when lvcs is not available. World origin, block dimensions and number of blocks
  boxm_scene( const vgl_point_3d<double>& origin,
              const vgl_vector_3d<double>& block_dim,
              const vgl_vector_3d<unsigned>& world_dim,
              const bool load_all_blocks=false,
              const bool save_internal_nodes_ = false,
              const bool save_platform_independent_ = true);

  //: Copy constructor
  boxm_scene(const boxm_scene& scene);

  //: Destructor
  ~boxm_scene();

  bool discover_block(vgl_point_3d<unsigned> index){ return discover_block(index.x(),index.y(),index.z()); }

  bool discover_block(unsigned i, unsigned j, unsigned k);

  //: Loads block into memory. 
  // Returns true if the block bin file is found on disc, otherwise returns false and creates a new tree for the block
  bool load_block(unsigned i, unsigned j, unsigned k);
  
  //: Loads block into memory. 
  bool load_block(vgl_point_3d<int> i) { return load_block(i.x(), i.y(), i.z()); }
  
  //: Loads a blcok and all its neighboring(adjacent) blocks
  bool load_block_and_neighbors(unsigned i, unsigned j, unsigned k);

  //: Write the active block to disk
  void write_active_block();
  
  //: Returns the active block(in memory)
  boxm_block<T>* get_active_block();
  
  //: Returns the indeces of active neighbors;
  vcl_set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >& active_blocks(){ return active_blocks_;}

  bgeo_lvcs lvcs() const { return lvcs_; }

  vgl_point_3d<double> origin() const { return origin_; }

  void set_origin(vgl_point_3d<double> o) { origin_=o; }

  vgl_vector_3d<double> block_dim() const { return block_dim_; }

  bool save_internal_nodes() const {return save_internal_nodes_;}

  bool save_platform_independent() const {return save_platform_independent_;}

  bool load_all_blocks() const {return load_all_blocks_;}
  
  void block_num(int &x, int &y, int &z) const {
    x=(int) blocks_.get_row1_count();
    y=(int) blocks_.get_row2_count();
    z=(int) blocks_.get_row3_count();
  }

  vgl_vector_3d<unsigned> world_dim() const {
    unsigned x=(unsigned) blocks_.get_row1_count();
    unsigned y=(unsigned) blocks_.get_row2_count();
    unsigned z=(unsigned) blocks_.get_row3_count();
    return vgl_vector_3d<unsigned>(x,y,z);
  }

  vcl_string path() const { return scene_path_; }

  vcl_string block_prefix() const { return block_pref_; }

  void set_path(vcl_string path, vcl_string block_prefix) { scene_path_=path; block_pref_= block_prefix; }

  void b_read(vsl_b_istream & s);

  void b_write(vsl_b_ostream& s) const;

  //: Returns the block this point resides in
  boxm_block<T>* get_block(vgl_point_3d<double>& p);

  //: Returns the index of the block containing this point
  bool get_block_index(vgl_point_3d<double>& p, vgl_point_3d<int> & index);

  //: what is the use of this?
  boxm_block<T>* get_block(unsigned i, unsigned j, unsigned k) { return blocks_(i,j,k); }

  boxm_block<T>* get_block(vgl_point_3d<int>& idx) { return blocks_(idx.x(), idx.y(), idx.z()); }
  
  //: Return all cells in a region
  void cells_in_region(vgl_box_3d<double>, vcl_vector<boct_tree_cell<loc_type, datatype>* >& cells);
  
  //: Locate point
  boct_tree_cell<loc_type, datatype> locate_point(vgl_point_3d<double>& p);
  

  void set_block(vgl_point_3d<int> const& idx, boxm_block<T>* block)
  { blocks_(idx.x(),idx.y(),idx.z()) = block; active_block_=idx; }

  void write_scene(vcl_string filename = "/scene.xml");

  void load_scene(vcl_string filename);

  void load_scene(boxm_scene_parser& parser);

  void force_write_blocks();
  static short version_no() { return 1; }

  boxm_block_iterator<T> iterator() { boxm_block_iterator<T> iter(this); return iter; }
  
  boxm_block_iterator<T> const_iterator() { const boxm_block_iterator<T> iter(this); return iter; }
  
  boxm_cell_iterator<T> cell_iterator(bool (boxm_scene<T>::*block_loading_func)(unsigned,unsigned, unsigned)){ boxm_cell_iterator<T> cell_iter(this->iterator(), block_loading_func); return cell_iter; }

  virtual vgl_box_3d<double> get_world_bbox();

  bool valid_index(vgl_point_3d<int> idx);

  void set_pinit(float pinit){pinit_=pinit;}
  float pinit(){return pinit_;}

  vgl_box_3d<double> get_block_bbox(vgl_point_3d<int>& idx){return get_block_bbox(idx.x(), idx.y(), idx.z());}

  vgl_box_3d<double> get_block_bbox(int x, int y, int z);

  //: generates a name for the block binary file based on the 3D vector index
  vcl_string gen_block_path(int x, int y, int z);

  void clean_scene();
  
  //: Unload active blocks
  void unload_active_blocks();
  
  //: Print out the trees in the scene
  void print();

  vgl_point_3d<double> rpc_origin() const { return rpc_origin_; }

  void set_rpc_origin(vgl_point_3d<double>& new_rpc_origin) {
    vgl_point_3d<double> old_origin = origin();
    vgl_point_3d<double> new_origin(
      old_origin.x() + new_rpc_origin.x() - rpc_origin_.x(),
      old_origin.y() + new_rpc_origin.y() - rpc_origin_.y(),
      old_origin.z() + new_rpc_origin.z() - rpc_origin_.z());
    set_origin(new_origin);
    rpc_origin_ = new_rpc_origin;
  }

 protected:
  bgeo_lvcs lvcs_;
  vgl_point_3d<double> origin_;
  vgl_point_3d<double> rpc_origin_;
  
  //: World dimensions of a block .e.g 1 meter x 1 meter x 1 meter
  vgl_vector_3d<double> block_dim_;
  
  vbl_array_3d<boxm_block<T>*> blocks_;

  //: index of the blocks (3D array) that is active; only one active block at a time
  vgl_point_3d<int> active_block_;
  
  //: if neighbors of the active block are loaded into memory, their indeces are stored in this set
  vcl_set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >  active_blocks_;

  float pinit_;

  //: Flag that indicates whether internal nodes of the trees should be saved
  bool save_internal_nodes_;
  
  //: Flag that indicates whether the octree data should be saved in a platform-independent way (slower)
  bool save_platform_independent_;
  
  //: Flag to load all the blocks in the memory
  bool load_all_blocks_;
  
  //************** private methods
  void create_block(unsigned i, unsigned j, unsigned k);

  void create_blocks(const vgl_vector_3d<double>& block_dim, const vgl_vector_3d<unsigned>& world_dim);
  bool parse_config(boxm_scene_parser& parser);

  bool parse_xml_string(vcl_string xml, boxm_scene_parser& parser);
  
  //: Load all blocks in bewteen min-max indeces. This method is private and the user needs to take care of unloading the blocks
  bool load_blocks(vgl_point_3d<int> min_idx, vgl_point_3d<int> max_idx);
  
  //: Unload all blocks in bewteen min-max indeces.
  bool unload_blocks(vgl_point_3d<int> min_idx, vgl_point_3d<int> max_idx);
  
 
  //: A helper function to generate the indeces of neighboring blocks
  vcl_set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >  neighboring_blocks(vgl_point_3d<int> idx)
  {
    vcl_set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> > neighbors;
    for(int i = -1; i <= 1; i++)
    {
      int active_i = idx.x() + i;
      if(active_i >= 0 && active_i < int(blocks_.get_row1_count()))
        for(int j = -1; j <= 1; j++)
        {
          int active_j = idx.y() + j;
          if(active_j >= 0 && active_j < int(blocks_.get_row2_count()) )
            for(int k = -1; k <= 1; k++)
            {
              int active_k = idx.z() + k;
              if(active_k >= 0 && active_k < int(blocks_.get_row3_count()))
                neighbors.insert(vgl_point_3d<int>(active_i, active_j, active_k));
              
            }
          
        }
    }  
    return neighbors;
  }
  
};

template <class T>
class boxm_block_iterator
{
  
  friend class boxm_cell_iterator<T>;

 public:
  boxm_block_iterator(boxm_scene<T>* const scene): i_(0), j_(0), k_(0), scene_(scene) {}
  
  //: Copy constructor
  boxm_block_iterator(const boxm_block_iterator<T>& other): i_(other.i_), j_(other.j_), k_(other.k_), scene_(other.scene_) {}

  ~boxm_block_iterator() {}

  boxm_block_iterator<T>& begin();

  bool end();

  boxm_block_iterator<T>& operator=(const boxm_block_iterator<T>& that);

  bool operator==(const boxm_block_iterator<T>& that);

  bool operator!=(const boxm_block_iterator<T>& that);

  boxm_block_iterator<T>& operator++();  // pre-inc

  boxm_block_iterator<T> operator++(int); // post-inc

  boxm_block_iterator<T>& operator--();

  boxm_block<T>* operator*();

  boxm_block<T>* operator->();

  vgl_point_3d<int> index() const { return vgl_point_3d<int>(i_,j_,k_); }

 private:
  int i_;
  int j_;
  int k_;

  boxm_scene<T>* const scene_;
};


//: A class to iterate through all LEAF cells in a boxm_scene
template <class T>
class boxm_cell_iterator
{
public:
  typedef bool (boxm_scene<T>::*ptr2func)(unsigned, unsigned, unsigned);
  typedef typename T::loc_type loc_type;
  typedef typename T::datatype datatype;
  
  //: Copy constructor
  boxm_cell_iterator(const boxm_cell_iterator<T>& other): block_iterator_(other.block_iterator_), cells_(other.cells_), block_loading_func_(other.block_loading_func_){}
  
  //: Constructor from a block iterator and function pointer to loading mechanism i.e load_active_block or active_bloack_and_neighbors
  boxm_cell_iterator(boxm_block_iterator<T> iter, ptr2func block_loading_func): 
  
  block_iterator_(iter), block_loading_func_(block_loading_func){}
    
  //: Destructor
  ~boxm_cell_iterator() {}
  
  //: Iterator begin
  boxm_cell_iterator<T>& begin();
  
  //: Iterator end
  bool end();
  
  /*************** Operators ****************/
  
  boxm_cell_iterator<T>& operator=( const boxm_cell_iterator<T>& that);
  
  bool operator==(const boxm_cell_iterator<T>& that);
  
  bool operator!=(const boxm_cell_iterator<T>& that);
  
  //: Prefix increment. When the end of the block is reached, it writes the block to disk and loads the next one
  boxm_cell_iterator<T>& operator++();  
  
  boct_tree_cell<loc_type, datatype>* operator*();
  
  boct_tree_cell<loc_type, datatype>* operator->();
  
  /*************** Data accessor *************/
  //: Return the global origin of the current cell
  vgl_point_3d<double> global_origin();
  
 
private:
  
  boxm_block_iterator<T> block_iterator_;
  
  vcl_vector<boct_tree_cell<loc_type ,datatype >* > cells_;
  
  typename vcl_vector< boct_tree_cell<loc_type , datatype >* >::const_iterator cells_iterator_;
  
  ptr2func block_loading_func_;
};



//: generates an XML file from the member variables
template <class T>
void x_write(vcl_ostream &os, boxm_scene<T>& scene, vcl_string name="boxm_scene");

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const &scene);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const * &scene);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> &scene);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> *&scene);


#endif // boxm_scene_h_
