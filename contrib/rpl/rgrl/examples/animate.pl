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

$t=join("",<>);
$t =~ y/\n/X/;
@new_views= $t =~ /new view.+?dump-(\d+)/g;

if( ! @new_views ) {
    die("Need log file to determine new views");
}
push( @new_views, 0 );

@viewcolours = ( 2, 4 );

open(F,">databorder.animate") or die("Couldn't create databorder\n");
print F <<ENDBORDER;
-100 -50 0
700  -50 0
700  450 0
-100 450 0
-100 -50 0


-100 -50 200
700  -50 200
700  450 200
-100 450 200
-100 -50 200


-100 -50 0
-100 -50 200


700 -50 0
700 -50 200


700 450 0
700 450 200


-100 450 0
-100 450 200
ENDBORDER

$view_num = 0;

print <<HEADER;

# Use:
#   pstopnm -portrait -xborder 0 -yborder 0 -llx 0.5 -ury 7 frame2.ps 
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
set output 'frame2.ps'
HEADER
#    print sprintf("set output 'frame2-%03d.ppm'\n",$n);
$MAP = 3;
$TAR = 1;
$VIEW = $viewcolours[0];
for( $n=1; $n <= 204; $n += 1 ) {
#for( $n=1; $n <= 30; $n += 1 ) {
    if( $n == $new_views[$view_num] ) {
        print "# New view\n";
        $view_num++;
        $VIEW = $viewcolours[ $view_num % @viewcolours ];
    }
    open(F, "out2-view-$n" ) or die("Couldn't open view: $_\n" );
    @min = ( 10000, 10000, 10000 );
    @max = ( -10000, -10000, -10000 );
    while(<F>) {
        if( /(\d+\.?\d*)\s+(\d+\.?\d*)\s+(\d+\.?\d*)/ ) {
            if( $1 < $min[0] ){ $min[0] = $1;}
            if( $1 > $max[0] ){ $max[0] = $1;}
            if( $2 < $min[1] ){ $min[1] = $2;}
            if( $2 > $max[1] ){ $max[1] = $2;}
            if( $3 < $min[2] ){ $min[2] = $3;}
            if( $3 > $max[2] ){ $max[2] = $3;}
        }
    }
    close(F);
    print "#Range: ",join(",",@min),"-",join(",",@max),"\n";

    $full_x_scale = 800.0/800.0;
    $full_y_scale = 800.0/800.0;
    $full_z_scale = 200.0/800.0;

    for($i=0;$i<3;$i++) {
        $min[$i] = POSIX::floor($min[$i] / 10)*10;
        $max[$i] = POSIX::ceil($max[$i] / 10)*10;
    }

    $x_len = $max[0] - $min[0];
    $y_len = $max[1] - $min[1];

    if( $x_len > $y_len ) {
        $min[1] -= ($x_len-$y_len) / 2.0;
        $max[1] += ($x_len-$y_len) / 2.0;
    } elsif( $x_len < $y_len ) {
        $min[0] -= ($y_len-$x_len) / 2.0;
        $max[0] += ($y_len-$x_len) / 2.0;
    }

    print "# x_len = ",$max[0]-$min[0],"\n";
    print "# y_len = ",$max[1]-$min[1],"\n";

    $max_len = $max[0] - $min[0];
    $max_len = $max[1] - $min[1] if $max[1] - $min[1] > $max_len;
    $max_len = $max[2] - $min[2] if $max[2] - $min[2] > $max_len;

    $zoom_x_scale = ($max[0] - $min[0]) / $max_len;
    $zoom_y_scale = ($max[1] - $min[1]) / $max_len;
    $zoom_z_scale = ($max[2] - $min[2]) / $max_len;

#    print sprintf("set output 'frame2-%03d.ppm'\n",$n);
    print <<ENDCMDS;
print "Doing frame $n"

#set xtics 100
#set ytics 100
#set ztics 100

#set size 1.0, 1.0
set size 1.0, 0.7
set origin 0.0, 0.0
set multiplot
#set size 0.5, 0.5
set xrange [-100:700]
#set yrange [-50:450]
set yrange [-200:600]
set zrange [0:200]
set size 0.5*$full_x_scale, 0.35*$full_y_scale
set origin 0.0, 0.0
set nolabel
#set border 4095 lt -1
set noborder
set label "xy" at screen 0.03, 0.1
set view 0,0,1.5,$full_z_scale
splot 'databorder.animate' notitle w l lt -1, 'out2-to.txt' notitle w l lt $TAR lw 3, 'out2-dump-$n' notitle w l lt $MAP lw 3, 'out2-view-$n' notitle w l lt $VIEW lw 3
#plot 'out2-to.txt' using 1:2 notitle w l lt $TAR lw 3, 'out2-dump-$n' using 1:2 notitle w l lt $MAP lw 3, 'out2-view-$n' using 1:2 notitle w l lt $VIEW lw 3


#set origin 0.0, 0.45
#set size 0.6,0.6
#set origin 0.0, 0.315
set origin 0.45, 0.0
#set border 4095 lt 0
set ticslevel -0.5
set view 60,30,1.45,$full_z_scale
splot 'databorder.animate' notitle w l lt 0, 'out2-to.txt' notitle w l lt $TAR lw 3, 'out2-dump-$n' notitle w l lt $MAP lw 3, 'out2-view-$n' notitle w l lt $VIEW lw 3

#set xtics 10
#set ytics 10
#set ztics 10

#set size 0.5, 0.5
#set origin 0.5, 0.0
set xrange [$min[0]:$max[0]]
set yrange [$min[1]:$max[1]]
set zrange [$min[2]:$max[2]]
set size 0.5*$zoom_x_scale,0.35*$zoom_y_scale
#set origin 0.5, 0.0
set origin 0.0, 0.25
set noborder
set label "xy" at screen 0.03, 0.32
#plot 'out2-to.txt' using 1:2 notitle w l lt $TAR lw 3, 'out2-dump-$n' using 1:2 notitle w l lt $MAP lw 3, 'out2-view-$n' using 1:2 notitle w l lt $VIEW lw 3
set view 0,0,1.3,$zoom_z_scale
splot  'out2-view-$n' notitle w l lt $VIEW lw 3, 'out2-to.txt' notitle w l lt $TAR lw 3, 'out2-dump-$n' notitle w l lt $MAP lw 3

#set origin 0.45, 0.45
#set size 0.6,0.6
#set origin 0.45, 0.315
set origin 0.45, 0.25
#set size 0.6,0.42
set view 60,30,1.2,$zoom_z_scale
set ticslevel 0
splot 'out2-view-$n' notitle w l lt $VIEW lw 3, 'out2-to.txt' notitle w l lt $TAR lw 3, 'out2-dump-$n' notitle w l lt $MAP lw 3


set nomultiplot
#set output
ENDCMDS
}
print "set output\n";
