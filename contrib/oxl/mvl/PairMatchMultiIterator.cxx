// This is oxl/mvl/PairMatchMultiIterator.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "PairMatchMulti.h"

PairMatchMultiIterator::PairMatchMultiIterator(PairMatchMulti const& pmm):
  first_(pmm.matches12_.begin()),
  last_(pmm.matches12_.end())
{
}
