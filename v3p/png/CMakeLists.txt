ADD_LIBRARY( png png_sources )

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/v3p/zlib )

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
