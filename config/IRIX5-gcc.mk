link-static := -Xlinker -Bstatic
link-dynamic := -Xlinker -Bdynamic

include $(configdir)/generic-irix.mk
soname_opt = -Wl,-soname,$(shlibname)

# include generic gcc stuff that works for any system with gcc
include $(configdir)/generic-gcc.mk
