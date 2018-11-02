#ifndef boxm_scene_h_
#define boxm_scene_h_
//:
// \file
// \brief  The main class to keep the 3D world data. It is a grid of boxm_blocks containing octrees.
//
// \author Gamze Tunali
// \date Apr 03, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <string>
#include <iostream>
#include <iosfwd>
#include <set>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_block.h>
#include <boxm/sample/boxm_sample.h>
#include <bvgl/bvgl_point_3d_cmp.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vbl/vbl_array_3d.h>
#include <vpgl/vpgl_lvcs.h>
#include <boct/boct_tree.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

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
  boxm_scene(const vpgl_lvcs& lvcs,
             const vgl_point_3d<double>& origin,
             const vgl_vector_3d<double>& block_dim,
             const vgl_vector_3d<unsigned>& world_dim,
             const bool load_all_blocks=false,
             const bool save_internal_nodes_ = false,
             const bool save_platform_independent_ = true);

  //: Constructor from lvcs, world origin, dimension of the block, number of blocks and initialization level
  boxm_scene(const vpgl_lvcs& lvcs,
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

  //: Constructor from base_scene
  boxm_scene(const boxm_scene_base& scene_base):
  boxm_scene_base(scene_base),active_block_(vgl_point_3d<int>(-1,-1,-1)), save_internal_nodes_(false), save_platform_independent_(true) {}

  //: Destructor
  ~boxm_scene() override;

  bool discover_block(vgl_point_3d<unsigned> index) { return discover_block(index.x(),index.y(),index.z()); }

  bool discover_block(unsigned i, unsigned j, unsigned k);

  //: Loads block into memory.
  // Returns true if the block bin file is found on disc, otherwise returns false and creates a new tree for the block
  bool load_block(unsigned i, unsigned j, unsigned k);

  // Load block into memory without using global variables
  bool load_block_thread_safe(unsigned i, unsigned j, unsigned k);

  //: Loads block into memory.
  bool load_block(vgl_point_3d<int> i) { return load_block(i.x(), i.y(), i.z()); }

  //: Loads a block and all its neighboring(adjacent) blocks
  bool load_block_and_neighbors(unsigned i, unsigned j, unsigned k);

  //: Loads a block and all its neighboring(adjacent) blocks
  bool load_block_and_neighbors(vgl_point_3d<int> i) { return load_block_and_neighbors(i.x(), i.y(), i.z()); }

  //: Reads and loads all blocks into memorty
  bool read_all_blocks();

  //: Write the active block to disk
  void write_active_block(bool unload_block=true);

  //: Write the active block to disk
  void write_active_block(bool unload_block=false) const;

  //: Write the active blocks to disk
  void write_active_blocks(bool unload_block=true);

  //: Write the active blocks to disk
  void write_active_blocks(bool unload_block=false) const;

  //: Write the specified block to disk without changing global variables
  void write_block_thread_safe(unsigned i, unsigned j, unsigned k);

  //: Returns the active block(in memory)
  boxm_block<T>* get_active_block();

  //: Returns the index of active block
  vgl_point_3d<int> active_block() const { return active_block_; }

  //: Returns the indices of active neighbors;
  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >& active_blocks() { return active_blocks_; }

  vpgl_lvcs lvcs() const { return lvcs_; }

  vgl_point_3d<double> origin() const { return origin_; }

  void set_origin(vgl_point_3d<double> o) { origin_=o; }

  vgl_vector_3d<double> block_dim() const { return block_dim_; }

  bool save_internal_nodes() const { return save_internal_nodes_; }

  bool save_platform_independent() const { return save_platform_independent_; }

  bool load_all_blocks() const { return load_all_blocks_; }

  void block_num(int &x, int &y, int &z) const {
    x=(int) blocks_.get_row1_count();
    y=(int) blocks_.get_row2_count();
    z=(int) blocks_.get_row3_count();
  }

  vgl_vector_3d<unsigned> world_dim() const override {
    unsigned x=(unsigned) blocks_.get_row1_count();
    unsigned y=(unsigned) blocks_.get_row2_count();
    unsigned z=(unsigned) blocks_.get_row3_count();
    return {x,y,z};
  }

  std::string path() const { return scene_path_; }

  std::string block_prefix() const { return block_pref_; }

  void set_path(std::string path, std::string block_prefix) { scene_path_=path; block_pref_= block_prefix; }

  void b_read(vsl_b_istream & s);

  void b_write(vsl_b_ostream& s) const;

  //: Returns the block this point resides in
  boxm_block<T>* get_block(vgl_point_3d<double> const& p) const;

  //: Returns the index of the block containing this point
  bool get_block_index(vgl_point_3d<double> const& p, vgl_point_3d<int>& index);

  //: Returns a block, assumes block is in memory - if not it returns null
  boxm_block<T>* get_block(unsigned i, unsigned j, unsigned k) const { return blocks_(i,j,k); }

  //: Returns a block, assumes block is in memory - if not it returns null
  boxm_block<T>* get_block(const vgl_point_3d<int>& idx) { return blocks_(idx.x(), idx.y(), idx.z()); }

  //: Returns a read-only block, assumes block is in memory - if not it returns null
  const boxm_block<T>* get_block_read_only(unsigned i, unsigned j, unsigned k) { return blocks_(i,j,k); }

  //: Return all leaf cells in a region
  void leaves_in_region(vgl_box_3d<double>, std::vector<boct_tree_cell<loc_type, datatype>* >& cells);

  //: Return the level of the smallest cell entirely containing the 3d region
  short level_region(vgl_box_3d<double> box);

  //: Return all leaf_data in a region - data is copied to new memory location
  void leaves_data_in_region(vgl_box_3d<double> box, std::vector<boct_cell_data<loc_type, datatype> > &cell_data);

  //: Returns the data of all cells at a given level, that are contained in the specified region
  void cell_data_in_region(vgl_box_3d<double> box, std::vector<boct_cell_data<loc_type, datatype> > &cell_data, unsigned level);

  //: Return all leaf cells between an inner box and an outter box
  void leaves_in_hollow_region(vgl_box_3d<double> outer_box, vgl_box_3d<double> inner_box, std::vector<boct_tree_cell<loc_type, datatype>* >& cells);

  //: Locates and modifies the value of all cells within a 3d region, which coordinates are given in scene coordinates
  void change_leaves_in_region(vgl_box_3d<double> box, const datatype &cell_data);
  void change_leaves_in_regions(std::vector<vgl_box_3d<double> > boxes, const std::vector<datatype> &all_data);

  //: Locate point in scene coordinates. Assumes that the block containing the point is already loaded into memory
  boct_tree_cell<loc_type, datatype>* locate_point_in_memory(vgl_point_3d<double> &p, unsigned level = 0);

  void set_block(vgl_point_3d<int> const& idx, boxm_block<T>* block)
  {
    if (blocks_(idx.x(),idx.y(),idx.z())) // release memory
      delete blocks_(idx.x(),idx.y(),idx.z());

    blocks_(idx.x(),idx.y(),idx.z()) = block; active_block_=idx;
  }

  void set_block(int i, int j, int k , boxm_block<T>* block) { set_block(vgl_point_3d<int>(i,j,k), block); }

  //: Write scene xml file at the scenes' path
  void write_scene(std::string filename = "scene.xml");

  //: Write scene xml file at the location indicated by path
  void write_scene(std::string path, std::string filename);

  void load_scene(std::string filename);

  void load_scene(boxm_scene_parser& parser);

  void force_write_blocks();

  static short version_no() { return 1; }

  boxm_block_iterator<T> iterator() { return boxm_block_iterator<T>(this); }

  boxm_block_iterator<T> const_iterator() const { return boxm_block_iterator<T>(this); }

  boxm_cell_iterator<T> cell_iterator(bool (boxm_scene<T>::*block_loading_func)(unsigned,unsigned, unsigned), bool read_only = false)
  {
    boxm_cell_iterator<T> cell_iter(this->iterator(), block_loading_func, read_only);
    return cell_iter;
  }

  boxm_cell_iterator<T> cell_iterator(bool (boxm_scene<T>::*block_loading_func)(unsigned,unsigned, unsigned) const, bool read_only)
  {
    assert (read_only);
    boxm_cell_iterator<T> cell_iter(this->const_iterator(), block_loading_func, read_only);
    return cell_iter;
  }

  vgl_box_3d<double> get_world_bbox() const override;

  //: Return the dimensions of the scene along each axis - this are equivalent to bbox width, length and depth
  void axes_length(double &x_length,double &y_length, double &z_length) const;

  //: Return true if the block index is valid
  bool valid_index(vgl_point_3d<int> const& idx) const { return valid_index(idx.x(), idx.y(), idx.z()); }


  //: Return true if the block index is valid
  inline bool valid_index(int const& x, int const& y, int const& z) const {
    return  x >= (int)0 && x < (int)blocks_.get_row1_count() &&
            y >= (int)0 && y < (int)blocks_.get_row2_count() &&
            z >= (int)0 && z < (int)blocks_.get_row3_count();
   }

  void set_pinit(float pinit) { pinit_=pinit; }

  float pinit() const { return pinit_; }

  vgl_box_3d<double> get_block_bbox(vgl_point_3d<int> const& idx) const { return get_block_bbox(idx.x(), idx.y(), idx.z()); }

  vgl_box_3d<double> get_block_bbox(int x, int y, int z) const;

  //: generates a name for the block binary file based on the 3D vector index
  std::string gen_block_path(int x, int y, int z) const;

  void clean_scene();

  //: Returns a scene with the same structure and initialized with given value
  void clone_blocks(boxm_scene<T> &scene_out, datatype data);

  //: Returns a scene with the same structure and data
  void clone_blocks(boxm_scene<T> &scene_out);

  //: Returns a scene of a different type with the same structure  and initialized with given value
  template <class T_out>
  void clone_blocks_to_type(boxm_scene<T_out> &scene_out, typename boxm_scene<T_out>::datatype data )
  {
    std::cout << "Clone blocks to type\n";
    boxm_block_iterator<T> iter(this);
    boxm_block_iterator<T_out> iter_out = scene_out.iterator();
    iter.begin();
    iter_out.begin();
    while (!iter.end())
    {
      load_block(iter.index());
      scene_out.load_block(iter.index());
      T_out  *tree_out =(*iter)->get_tree()->template clone_to_type<typename boxm_scene<T_out>::datatype>();
      (*iter_out)->init_tree(tree_out);
      tree_out->init_cells(data);
      scene_out.write_active_block();
      ++iter;
      ++iter_out;
    }
  }

  //: Returns a scene of a vector type with the same structure and vector initialized to the the same data
  template <unsigned DIM>
  void clone_blocks_to_vector(boxm_scene<boct_tree<short, vnl_vector_fixed<datatype, DIM> > > &scene_out)
  {
    typedef boct_tree<short, vnl_vector_fixed<datatype, DIM> > type_tree_out;
    std::cout << "Clone blocks to type\n";
    boxm_block_iterator<T> iter(this);
    boxm_block_iterator<type_tree_out> iter_out = scene_out.iterator();
    iter.begin();
    iter_out.begin();
    while (!iter.end())
    {
      load_block(iter.index());
      scene_out.load_block(iter.index());
      type_tree_out  *tree_out =(*iter)->get_tree()->template clone_to_vector<DIM>();
      (*iter_out)->init_tree(tree_out);
      scene_out.write_active_block();
      ++iter;
      ++iter_out;
    }
  }

  //: Unload active blocks
  void unload_active_blocks() const;

  //: Print out the trees in the scene
  void print();

  //: Return the finest level in the scene
  short finest_level();

  //: Return the length of finest-level cell in the scene
  double finest_cell_length();

  //: Return the length of cells at a given level in the scene
  double cell_length(unsigned level);

  //: Return the length of finest-level cell in the scene. Iterates through blocks assuming they are all in memory
  double finest_cell_length_in_memory() const;

  //: Return the finest level in the scene. Iterates through blocks assuming they are all in memory
  short finest_level_in_memory() const;

  //: Return the number of leaf nodes in the scene
  unsigned long size();

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
  vpgl_lvcs lvcs_;
  vgl_point_3d<double> origin_;
  vgl_point_3d<double> rpc_origin_;

  //: World dimensions of a block .e.g 1 meter x 1 meter x 1 meter
  vgl_vector_3d<double> block_dim_;

  vbl_array_3d<boxm_block<T>*> blocks_;

  //: index of the blocks (3D array) that is active; only one active block at a time
  mutable vgl_point_3d<int> active_block_;

  //: if neighbors of the active block are loaded into memory, their indices are stored in this set
  mutable std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >  active_blocks_;

  float pinit_;

  //: Flag that indicates whether internal nodes of the trees should be saved
  bool save_internal_nodes_;

  //: Flag that indicates whether the octree data should be saved in a platform-independent way (slower)
  bool save_platform_independent_;

  //: Flag to load all the blocks in the memory
  bool load_all_blocks_;

  // ************** private methods *******************

  void create_block(unsigned i, unsigned j, unsigned k);

  void create_blocks(const vgl_vector_3d<double>& block_dim, const vgl_vector_3d<unsigned>& world_dim);
  bool parse_config(boxm_scene_parser& parser);

  bool parse_xml_string(std::string xml, boxm_scene_parser& parser);

  //: Load all blocks in between min-max indices. This method is private and the user needs to take care of unloading the blocks
  bool load_blocks(vgl_point_3d<int> min_idx, vgl_point_3d<int> max_idx);

  //: Unload all blocks in between min-max indices.
  bool unload_blocks(vgl_point_3d<int> min_idx, vgl_point_3d<int> max_idx);


  //: A helper function to generate the indices of neighboring blocks
  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >  neighboring_blocks(vgl_point_3d<int> idx)
  {
    std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> > neighbors;
    for (int i = -1; i <= 1; i++)
    {
      int active_i = idx.x() + i;
      if (active_i >= 0 && active_i < int(blocks_.get_row1_count()))
        for (int j = -1; j <= 1; j++)
        {
          int active_j = idx.y() + j;
          if (active_j >= 0 && active_j < int(blocks_.get_row2_count()) )
            for (int k = -1; k <= 1; k++)
            {
              int active_k = idx.z() + k;
              if (active_k >= 0 && active_k < int(blocks_.get_row3_count()))
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
  boxm_block_iterator(boxm_scene<T> const* scene): i_(0), j_(0), k_(0), scene_(scene) {}

  //: Copy constructor
  boxm_block_iterator(boxm_block_iterator<T> const& other): i_(other.i_), j_(other.j_), k_(other.k_), scene_(other.scene_) {}

  ~boxm_block_iterator() = default;

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

  boxm_scene<T> const* scene_;
};


//: A class to iterate through all -or only leaf- cells in a boxm_scene
template <class T>
class boxm_cell_iterator
{
 public:
  typedef bool (boxm_scene<T>::*ptr2func)(unsigned, unsigned, unsigned);
  typedef bool (boxm_scene<T>::*ptr2constfunc)(unsigned, unsigned, unsigned) const;
  typedef typename T::loc_type loc_type;
  typedef typename T::datatype datatype;

  //: Copy constructor
  boxm_cell_iterator(boxm_cell_iterator<T> const& other)
   : block_iterator_(other.block_iterator_), cells_(other.cells_),
     block_loading_func_(other.block_loading_func_), read_only_(other.read_only_) {}

  //: Constructor from a block iterator and function pointer to loading mechanism i.e load_block() or load_block_and_neighbors()
  boxm_cell_iterator(boxm_block_iterator<T> iter, ptr2func block_loading_func, bool read_only = false, bool use_internal_cells = false)
    : block_iterator_(iter), block_loading_func_((ptr2constfunc)block_loading_func), read_only_(read_only), use_internal_cells_(use_internal_cells) {}

  //: "Const" constructor from a block iterator and function pointer to loading mechanism i.e load_block() or load_block_and_neighbors()
  boxm_cell_iterator(boxm_block_iterator<T> iter, ptr2constfunc block_loading_func, bool read_only, bool use_internal_cells = false)
    : block_iterator_(iter), block_loading_func_(block_loading_func), read_only_(read_only), use_internal_cells_(use_internal_cells) { assert(read_only); }

  //: Destructor
  ~boxm_cell_iterator() = default;

  //: Iterator begin
  boxm_cell_iterator<T>& begin(bool use_internal_cells=false);

  //: Iterator end
  bool end();

  /*************** Operators ****************/

  boxm_cell_iterator<T>& operator=( const boxm_cell_iterator<T>& that);

  bool operator==(const boxm_cell_iterator<T>& that);

  bool operator!=(const boxm_cell_iterator<T>& that);

  //: Prefix increment. When the end of the block is reached, it writes the block to disk and loads the next one
  boxm_cell_iterator<T>& operator++();

  //: Increment. When the end of the block is reached, it writes the block to disk and loads the next one
  boxm_cell_iterator<T> operator+=(unsigned const &rhs);

  boct_tree_cell<loc_type, datatype>* operator*();

  boct_tree_cell<loc_type, datatype>* operator->();

  /*************** Data accessor *************/
  //: Return the global origin of the current cell
  vgl_point_3d<double> global_origin();

  //: Return the global centroid of the current cell - use only if cells are cubical
  vgl_point_3d<double> global_centroid();

  //: Return the global length of this cell
  double length();

 private:

  boxm_block_iterator<T> block_iterator_;
  std::vector<boct_tree_cell<loc_type ,datatype >* > cells_;
  typename std::vector< boct_tree_cell<loc_type , datatype >* >::const_iterator cells_iterator_;
  ptr2constfunc block_loading_func_;
  bool read_only_;
  bool use_internal_cells_;
};


//: generates an XML file from the member variables
template <class T>
void x_write(std::ostream &os, boxm_scene<T>& scene, std::string name="boxm_scene");

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const &scene);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const * &scene);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> &scene);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> *&scene);


#endif // boxm_scene_h_
