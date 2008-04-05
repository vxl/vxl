#include "vil_block_cache.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vcl_cassert.h>

unsigned long bcell::time_ = 0;

vil_block_cache::~vil_block_cache()
{
  assert(queue_.size()==blocks_.size());
  blocks_.clear();//empty the index
  //empty the queue
  unsigned nq = queue_.size();
  for (unsigned i = 0; i<nq; ++i)
  {
    bcell* fb = queue_.top();
    queue_.pop();
    delete fb;
  }
}

//:add a block to the buffer.
bool vil_block_cache::add_block(const unsigned& block_index_i,
                                const unsigned& block_index_j,
                                vil_image_view_base_sptr const& blk)
{
  //create a cell

  bcell* cell = new bcell(block_index_i, block_index_j, blk);
  if (queue_.size()>=nblocks_)
    if (!this->remove_block())
      return false;
  queue_.push(cell);
  blocks_.push_back(cell);
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

//:remove the lowest priority block
bool vil_block_cache::remove_block()
{
  if (queue_.size()==0)
    return false;

  //force the queue to reorder
  bcell* temp = queue_.top();
  queue_.pop();
  //
  queue_.push(temp);
  bcell* top_cell = queue_.top();
  vcl_vector<bcell*>::iterator bit;
  bit = vcl_find(blocks_.begin(), blocks_.end(), top_cell);
  if (bit == blocks_.end())
    return false;
  blocks_.erase(bit);
  queue_.pop();//finally remove the top cell from the queue
  delete top_cell;
  return true;
}
