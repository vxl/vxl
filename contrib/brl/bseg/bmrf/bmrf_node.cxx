// This is contrib/brl/bseg/bmrf/bmrf_node.cxx
//:
// \file

#include "bmrf_node.h"


//: Constructor
bmrf_node::bmrf_node( int frame_num, double probability )
  : frame_num_(frame_num), probability_(probability),
    neighbors_(), boundaries_(ALL+1, neighbors_.end()), sizes_(ALL,0)
{
}


//: Calculate the conditional probability that this node is correct give its neighbors
double
bmrf_node::probability(  )
{
  // Have not yet determined how this will be computed
  return probability_;
}


//: Add \param node as a neighbor of type \param type
bool
bmrf_node::add_neighbor( bmrf_node *node, neighbor_type type )
{
  if(!node || node == this || type == ALL)
    return false;
    
  // verify that this node is not already present
  neighbor_iterator itr = boundaries_[type];
  for(; itr != boundaries_[type+1]; ++itr)
    if(*itr == node) return false;
    
  // add the node 
  neighbors_.insert(boundaries_[type+1], node);
  ++sizes_[type];

  // adjust boundaries if necessary
  for(int t=type; t>=0 && (boundaries_[type+1] == boundaries_[t]); --t)
    --boundaries_[t];

  return true;
}


//: Remove \param node from the neighborhood
bool
bmrf_node::remove_neighbor( bmrf_node *node, neighbor_type type )
{
  if(!node || node == this)
    return false;

  bool removed = false;
  int init_t = type;
  if(type == ALL) init_t=0;

  for(int t = init_t; t<ALL; ++t){
    neighbor_iterator itr = boundaries_[t];
    for(; itr != boundaries_[t+1]; ++itr){
      if(*itr == node){
        neighbors_.erase(itr);
        --itr;          // back up
        --sizes_[type]; // decrease count
        if (type != ALL) return true;
        removed = true;
      }
    }
  }
  return removed;
}


//: Returns an iterator to the beginning of the type \param type neighbors
bmrf_node::neighbor_iterator
bmrf_node::begin( neighbor_type type )
{
  if (type == ALL) return neighbors_.begin();
  return boundaries_[type];
}

  
//: Returns an iterator to the end of the type \param type neighbors
bmrf_node::neighbor_iterator
bmrf_node::end( neighbor_type type )
{
  if (type == ALL) return neighbors_.end();
  return boundaries_[type+1];
}


//: Returns the number of outgoing neighbors to this node of type \param type
int
bmrf_node::num_neighbors( neighbor_type type )
{
  if (type == ALL) return neighbors_.size();
  return sizes_[type];
}
    

//: Binary save self to stream.
void
bmrf_node::b_write( vsl_b_ostream& os ) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->frame_num_);
  vsl_b_write(os, this->probability_);

  // write the number of nodes of each type
  for(int t=0; t<ALL; ++t)
    vsl_b_write(os, sizes_[t]);

  // write all the neighbors
  vcl_list<bmrf_node*>::const_iterator itr = neighbors_.begin();
  for(; itr != neighbors_.end(); ++itr){
    // Get a serial_number for this node
    unsigned long id = os.get_serial_number(*itr);
    if (id == 0){  // The node has not yet been saved
      id = os.add_serialisation_record(*itr);
      vsl_b_write(os, id);    // Save the serial number
      vsl_b_write(os, *itr);  // Save the node
    }
    else{
      vsl_b_write(os, id);    // Save the serial number only
    }
  }
}

  
//: Binary load self from stream.
void
bmrf_node::b_read( vsl_b_istream& is )
{
  if (!is) return;
  
  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    {
      vsl_b_read(is, this->frame_num_);
      vsl_b_read(is, this->probability_);

      neighbors_.clear();
      boundaries_.clear();
      boundaries_.resize(ALL+1, neighbors_.end());

      int num_neighbors = 0;
      for(int t=0; t<ALL; ++t){
        vsl_b_read(is, sizes_[t]);
        num_neighbors += sizes_[t];
      }
      int type = 0;
      int b_loc = sizes_[0];
      for(int n=0; n<num_neighbors; ++n){
        unsigned long id; // Unique serial number identifying object
        vsl_b_read(is, id);

        bmrf_node* node_ptr = (bmrf_node*) is.get_serialisation_pointer(id);
        if (node_ptr == 0) { // Not loaded before
          bool not_null_ptr;
          vsl_b_read(is, not_null_ptr);
          if (not_null_ptr){
            node_ptr = new bmrf_node();
            // SERIALISATION MUST BE DONE BEFORE READING !!!
            is.add_serialisation_record(id, node_ptr);
            node_ptr->b_read(is);
          }
        }
        neighbors_.push_back(node_ptr);
        
        while(type < ALL && n == b_loc){
          boundaries_[++type] = neighbors_.end();
          --boundaries_[type];
          b_loc += sizes_[type];
        }
      }
      boundaries_[0] = neighbors_.begin();
    }   
    break;

  default:
    vcl_cerr << "I/O ERROR: bmrf_node::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Return IO version number;
short
bmrf_node::version(  ) const
{
  return 1;
}


//: Print an ascii summary to the stream
void
bmrf_node::print_summary( vcl_ostream& os ) const
{
  os << "bmrf_node{ pr=" << probability_ << ", frame=" << frame_num_ << " }";
}


//: Return a platform independent string identifying the class
vcl_string
bmrf_node::is_a(  ) const
{
  return vcl_string("bmrf_node");
}


//: Return true if the argument matches the string identifying the class or any parent class
bool
bmrf_node::is_class( const vcl_string& cls ) const
{
  return cls==bmrf_node::is_a();
}


//-----------------------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------------------

//: Binary save bmrf_node to stream.
void
vsl_b_write(vsl_b_ostream &os, const bmrf_node* n)
{
  if (n==0){
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    n->b_write(os);
  }
}


//: Binary load bmrf_node from stream.
void
vsl_b_read(vsl_b_istream &is, bmrf_node* &n)
{
  delete n;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr){
    n = new bmrf_node();
    n->b_read(is);
  }
  else
    n = 0;
}

