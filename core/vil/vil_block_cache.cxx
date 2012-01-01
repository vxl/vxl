#include "vil_block_cache.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vcl_cassert.h>

unsigned long bcell::time_ = 0;

vil_block_cache::~vil_block_cache()
{
  for(vcl_vector<bcell*>::iterator bit = blocks_.begin();
      bit != blocks_.end(); ++bit){
    delete *bit;
    *bit = 0;
  }
  blocks_.clear();//empty the index
}

//:add a block to the buffer.
bool vil_block_cache::add_block(const unsigned& block_index_i,
                                const unsigned& block_index_j,
                                vil_image_view_base_sptr const& blk)
{
  //create a cell

  bcell* cell = new bcell(block_index_i, block_index_j, blk);
  if (blocks_.size()>=nblocks_)
    if (!this->remove_block())
      return false;
  blocks_.push_back(cell);
  vcl_sort(blocks_.begin(), blocks_.end(), bcell_less());
  return true;
}

bool vil_block_cache::get_block(const unsigned& block_index_i,
                                const unsigned& block_index_j,
                                vil_image_view_base_sptr& blk) const
{
  bool found = false;
  for (vcl_vector<bcell*>::const_iterator bit=blocks_.begin(); bit!= blocks_.end()&&!found; ++bit)
  {
    if ((*bit)->bindex_i_!=block_index_i||(*bit)->bindex_j_!=block_index_j)
      continue;
    else
    {
      found = true;
      blk = (*bit)->blk_;
      (*bit)->touch();//block is in demand so update the age to zero
    }
  }
  return found;
}

//:remove the oldest priority block
bool vil_block_cache::remove_block()
{
  if(!blocks_.size()){
    vcl_cerr << "warning: attempt to remove block from empty cache\n";
    return false;
  }
  // queue should already be sorted
  // remove oldest
  vcl_vector<bcell*>::iterator bit = blocks_.begin();
  blocks_.erase(bit);
  return true;
}
