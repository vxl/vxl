// This is core/vgui/vgui_displaylist3D_tableau_sptr.h
#ifndef vgui_displaylist3D_tableau_sptr_h
#define vgui_displaylist3D_tableau_sptr_h
//:
// \file
// \brief  Smart-pointer to a vgui_displaylist3D_tableau.
// \author K.Y.McGaul
// \date   02-OCT-2002
//
// \verbatim
//  Modifications
//   02-OCT-2002 K.Y.McGaul - Initial version.
// \endverbatim

#include <vgui/vgui_displaybase_tableau_sptr.h>

class vgui_displaylist3D_tableau;
typedef vgui_tableau_sptr_t<vgui_displaylist3D_tableau,vgui_displaybase_tableau_sptr> vgui_displaylist3D_tableau_sptr;

#endif // vgui_displaylist3D_tableau_sptr_h
