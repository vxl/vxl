ADD_LIBRARY( png png_sources )

INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindZLIB.cmake )

SOURCE_FILES( png_sources
png
pngset
pngget
pngrutil
pngtrans
pngwutil
pngread
pngrio
pngwio
pngwrite
pngrtran
pngwtran
pngmem
pngerror
pngpread
)
