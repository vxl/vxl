#!/usr/local/bin/perl

use POSIX;

# @oldmax= ( -10000, -10000, -10000 );
# @oldmin= ( 10000, 10000, 10000 );
# for( $n=1; $n <= 348; ++$n ) {
#     open(F, "out2-view-$n" ) or die("Couldn't open view: $_\n" );
#     @min = ( 10000, 10000, 10000 );
#     @max = ( -10000, -10000, -10000 );
#     while(<F>) {
#         if( /(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*)/ ) {
#             if( $1 < $min[0] ){ $min[0] = $1;}
#             if( $1 > $max[0] ){ $max[0] = $1;}
#             if( $2 < $min[1] ){ $min[1] = $2;}
#             if( $2 > $max[1] ){ $max[1] = $2;}
#             if( $3 < $min[2] ){ $min[2] = $3;}
#             if( $3 > $max[2] ){ $max[2] = $3;}
#         }
#     }
#     close(F);
#     if( $oldmax[0] != $max[0] or
#         $oldmax[1] != $max[1] or
#         $oldmax[2] != $max[2] or
#         $oldmin[0] != $min[0] or
#         $oldmin[1] != $min[1] or
#         $oldmin[2] != $min[2] ) {
#         print "View change at $n\n";
#     }
#     @oldmax = @max;
#     @oldmin = @min;
# }

#exit;


print <<HEADER;

# Use:
#   pstopnm -portrait -xborder 0 -yborder 0 -llx 0.5 -ury 7 frame.ps 
# to get images
#
#
# Encode with
#   ~/projects/packages/freebsd/MPlayer-0.90rc4/bin/mencoder -mf on:w=604:h=604:fps=5:type=png -ovc copy -o output.avi frame2\*.png
# Provided the png are full 24-bit pngs, not indexed color. ImageMagick's convert produces a indexed
# PNG whenever it can.

#set xtics 0,600,600
#set ytics 0,400,400
#set ztics 0,200,200
set noxtics
set noytics
set noztics
set border 4095 lt -1
set xlabel
set ylabel
set zlabel
#set terminal pbm color
set terminal post portrait color solid 8
set output 'frame.ps'
HEADER
#    print sprintf("set output 'frame2-%03d.ppm'\n",$n);
$MAP = 3;
$TAR = 1;
$VIEW = $viewcolours[0];
for( $n=0; $n <= 8; $n += 1 ) {
    print <<ENDCMDS;
print "Doing frame $n"

#set xtics 100
#set ytics 100
#set ztics 100

set size 1.0, 3.5/10.0
set origin 0.0, 0.0
set multiplot
set xrange [-20:180]
set yrange [0:180]
set zrange [0:160]

set size 3.5/7.0, 3.5/10.0
set nolabel
set label "xy" at screen 0.03, 0.03
plot 'out2-to.txt' notitle w l lt $TAR lw 3, 'out2-dump-$n' notitle w l lt $MAP lw 3

set origin 0.5, 0.0
set ticslevel (0.0 - 0.0)/(160.0-0.0)

set view 75,51,1.2
splot 'out2-to.txt' notitle w l lt $TAR lw 3, 'out2-dump-$n' notitle w l lt $MAP lw 3

set nomultiplot
#set output
ENDCMDS
}
print "set output\n";
