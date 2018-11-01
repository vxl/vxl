// This is core/vil/vil_block_cache.h
#ifndef vil_block_cache_h_
#define vil_block_cache_h_
//:
// \file
// \brief A block cache with block population prioritized by age
// \author J. L. Mundy
//
#include <iostream>
#include <queue>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view_base.h>

// \verbatim
//  Modifications
//   J.L. Mundy replaced priority queue with sort on block vector
//   container for simplicity, January 01, 2012
// \endverbatim

//container for blocks to maintain a timestamp
//note that the larger value of time corresponds to the newest block
struct bcell
{
  bcell(const unsigned bindex_i, const unsigned bindex_j,
        vil_image_view_base_sptr const& blk) :
    bindex_i_(bindex_i), bindex_j_(bindex_j), birthdate_(time_++), blk_(blk)
  {}

  //:block indices
  unsigned bindex_i_;  unsigned bindex_j_;
  //:the time of insertion into the queue
  unsigned long birthdate_;
  //:the block itself
  vil_image_view_base_sptr blk_;
  //:update the age of a block
  void touch(){birthdate_=time_++;}
  //: for debug
  void print() const { std::cout << '[' << bindex_i_ << ' ' << bindex_j_
                                << "](" << birthdate_ << ")\n"; }
 private:
  static unsigned long time_; //static timekeeper
};
// the ordering predicate for block birthdate. Oldest block is at
// blocks_.begin()
class bcell_less
{
 public:
  bcell_less()= default;
  //the predicate function
  bool operator()(bcell* const& ba, bcell* const& bb) const
  {
    return ba->birthdate_ < bb->birthdate_;
  }
};
class vil_block_cache
{
 public:
  vil_block_cache(const unsigned block_capacity):nblocks_(block_capacity){}
  ~vil_block_cache();

  //:add a block to the buffer
  bool add_block(const unsigned& block_index_i, const unsigned& block_index_j,
                 vil_image_view_base_sptr const& blk);

  //:retrieve a block from the buffer
  bool get_block(const unsigned& block_index_i, const unsigned& block_index_j,
                 vil_image_view_base_sptr& blk) const;

  //:block capacity
  unsigned block_size() const{return nblocks_;}
 private:
  //:block index member
  std::vector<bcell*> blocks_;
  //:capacity in blocks
  unsigned nblocks_;
  //:remove the lowest priority block
  bool remove_block();
};

#endif // vil_block_cache_h_
