// This is contrib/brl/bseg/bmrf/bmrf_node.cxx
//:
// \file

#include "bmrf_node.h"


// Make sure this is set to the number of types defined in neighbor_types
const int bmrf_node::number_of_types = 3;


//: Constructor
bmrf_node::bmrf_node( int frame_num, double probability )
  : frame_num_(frame_num), probability_(probability), neighbors_(number_of_types)
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
  if(node && node != this){
    // verify that this node is not already present
    vcl_vector<bmrf_node*>::iterator itr = neighbors_[type].begin();
    for(; itr != neighbors_[type].end(); ++itr)
      if(*itr == node) return false;
    // add the node  
    neighbors_[type].push_back(node);
    return true;
  }
  return false;
}


//: Remove \param node from the neighborhood
bool
bmrf_node::remove_neighbor( bmrf_node *node, neighbor_type type )
{
  if(!node || node == this)
    return false;
    
  // search for the node
  vcl_vector<bmrf_node*>::iterator itr = neighbors_[type].begin();
  for(; itr != neighbors_[type].end(); ++itr)
    if(*itr == node){
      neighbors_[type].erase(itr);
      return true;
    }
  return false;
}


//: Binary save self to stream.
void
bmrf_node::b_write( vsl_b_ostream& os ) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->frame_num_);
  vsl_b_write(os, this->probability_);
  
  for(int t=0; t<number_of_types; ++t){
    // write the number of this type of neighbor
    vsl_b_write(os, neighbors_[t].size());
    vcl_vector<bmrf_node*>::const_iterator itr = neighbors_[t].begin();
    for(; itr != neighbors_[t].end(); ++itr){
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
    vsl_b_read(is, this->frame_num_);
    vsl_b_read(is, this->probability_);
    for(int t=0; t<number_of_types; ++t){
      int num_neighbors;
      vsl_b_read(is, num_neighbors);
      for(int n=0; n<num_neighbors; ++n){
        unsigned long id; // Unique serial number identifying object
        vsl_b_read(is, id);

        bmrf_node* node_ptr = (bmrf_node*) is.get_serialisation_pointer(id);
        if (node_ptr == 0) { // Not loaded before
          vsl_b_read(is, node_ptr);                  // load the node
          is.add_serialisation_record(id, node_ptr); // remember location of this node
        }
        neighbors_[t].push_back(node_ptr);
      }
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

