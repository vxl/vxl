set multiplot
s=0.7
ox=0.0
oy=0.0
hsh=0.4
vsh=0.4
set size s,s
set xlabel ""
set ylabel ""
set zlabel ""
set title ""
set origin ox,oy
set view 0,0
#set title "xy"
set label 1 "xy" at graph 0,0,0
splot "$0" title '' pt 0, "$1" title '' pt 0, "$2" title '' pt 0
set view 90,0
set origin hsh,0
#set title "yz"
set label 1 "yz" at graph 0,0,0
splot "$0" title '' pt 0, "$1" title '' pt 0, "$2" title '' pt 0
set view 90,90
set origin 0,vsh
#set title "xz"
set label 1 "xz" at graph 0, 0, 0
splot "$0" title '' pt 0, "$1" title '' pt 0, "$2" title '' pt 0
set nolabel 1
set xlabel 'x'
set ylabel 'y'
set zlabel 'z'
set size 0.5, 0.5
set origin 0.5,0.5
set title "3d"
set view 60,30
splot "$0" pt 0, "$1" pt 0, "$2" pt 0
set nomultiplot
