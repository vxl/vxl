#include <vbl/vbl_protection_traits.h>

//------------------------------------------------------------
// -- Specialize for different protect/unprotect.
#define VBL_PROTECTION_TRAITS(Type,prot,unprot) \
void vbl_protection_traits<Type>::protect(Type t) { if (t) t->prot(); } \
void vbl_protection_traits<Type>::unprotect(Type t) { if (t) t->unprot(); } \
void vbl_protection_traits<Type>::def(Type& t) { t = (Type)0; } 

//------------------------------------------------------------
// -- Instantiate 
#define VBL_PROTECTION_TRAITS_Protect(Type) \
VBL_PROTECTION_TRAITS(Type, Protect, UnProtect);

//------------------------------------------------------------
// -- Instantiate 
#define VBL_PROTECTION_TRAITS_ref(Type) \
VBL_PROTECTION_TRAITS(Type, ref, unref);

//------------------------------------------------------------
// -- Specialize if you are managing contained objects yourself,
// so the container should not delete.
#define VBL_PROTECTION_TRAITS_none(Type) \
void vbl_protection_traits<Type>::protect(Type ) {} \
void vbl_protection_traits<Type>::unprotect(Type ) {} \
void vbl_protection_traits<Type>::def(Type& t) { t = (Type)0; } 
