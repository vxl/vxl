// This is brl/bbas/volm/desc/volm_desc_matcher.h

//:
// \file
// \brief  A class to represent 
//
// \author
// \date May 29, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#if !defined(_VOLM_DESC_MATCHER_H)
#define _VOLM_DESC_MATCHER_H

#include <volm/desc/volm_desc.h>

class volm_desc_matcher
{
public:
    void score(volm_desc_sptr a, volm_desc_sptr b);
    volm_desc_sptr create_query_desc();
};

#endif  //_VOLM_DESC_MATCHER_H
