The toolkit implementations are no longer built independently because of
reliable registration issues when using shared libraries. The toolkits
are now directly part of the main vgui library. Look in the main
CMakeLists.txt, and in vgui_register_all.cxx.

-- Amitha Perera, Aug 2002
