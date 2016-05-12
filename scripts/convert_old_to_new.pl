#perl

# Convert CMakeLists.txt files from the old source tree to the
# new source tree.
#
# Usage: convert_old_to_new.pl <list of CMakeLists.txt files>

foreach $file (@ARGV) {

    if( ! open(F, $file) ) {
        warn("Couldn't open $file\n");
    } else {
        $wholefile = join("",<F>);
        close(F);

        my $backedup = 0;
        for($backext=0;$backext<50;++$backext) {
            if( open(F2,">${file}.bak$backext") ) {
                print F2 $wholefile;
                close(F2);
                $backedup = 1;
                last;
            }
        }
        if( ! $backedup ) {
            warn( "Couldn't backup ${file}. Skipping\n" );
            next;
        }


# 1. Fix INCLUDE_DIRECTORIES
# -----------------------------------------------------------------------------

# Remove vcl include dir. They are done at a top level, and is
# therefore implicitly present for all packages.

# when it is in a INCLUDE_DIR by itself
$wholefile =~ s:INCLUDE_DIRECTORIES\s*\(\s*\${allvxl_SOURCE_DIR}/vcl\s*\)\s*\n?::gs;
# when it is in a INCLUDE_DIR with other things
$wholefile =~ s:(INCLUDE_DIRECTORIES\s*\(\s*)\${allvxl_SOURCE_DIR}/vcl:$1.delete vcl.:gs;

# replace ${allvxl_SOURCE_DIR}/xxx stuff with the ${xxx_INCLUDE_DIR} version
%map = (
        "vxl" => "VXLCORE",
        "core" => "VXLCORE",
        "conversions" => "CONVERSIONS",
        "brl" => "BRL",
        "gel" => "GEL",
        "mul" => "MUL",
        "oul" => "OUL",
        "oxl" => "OXL",
        "rpl" => "RPL",
        "tbl" => "TBL"
       );
foreach $f (keys %map) {
    my $d = $map{$f};
    $wholefile =~ s:\$\{(all)?CMAKE_SOURCE_DIR\}/$f:\$\{${d}_INCLUDE_DIR\}:gm;
}

# 2. Any remaning allvxl_SOURCE_DIR should be changed
# -----------------------------------------------------------------------------
$wholefile =~ s:\$\{allvxl_(SOURCE|BINARY)_DIR\}:\$\{vxl_$1_DIR\}:gm;

# 3. vgui has moved
# -----------------------------------------------------------------------------

$wholefile =~ s:\$\{OXL_INCLUDE_DIR\}/vgui/UseVGUI.cmake:\$\{VXLCORE_INCLUDE_DIR\}/vgui/UseVGUI.cmake:gm;

    }

    if( !open(F2, ">${file}") ) {
        warn("Couldn't write ${file}\n");
    } else {
        print F2 $wholefile;
        close(F2);
    }
}
