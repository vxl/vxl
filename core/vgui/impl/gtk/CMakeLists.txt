The toolkit implementations are no longer built independently because of
issues in reliably registering toolkits when using static libraries. The
toolkits are now a direct part of the main vgui library. Look in the main
(and only) vgui CMakeLists.txt, and in vgui_register_all.cxx.

-- Amitha Perera, Aug 2002
