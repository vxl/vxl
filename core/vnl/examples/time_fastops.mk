#! /bin/sh
make -n
DSTDIR=`make echovar-RELOBJDIR`
rm -f $DSTDIR/time_fastops.o
make DEFINES="-DMETHOD=1" $DSTDIR/time_fastops
$DSTDIR/time_fastops > $DSTDIR/time_fastops.10 2>&1
rm -f $DSTDIR/time_fastops.o
make DEFINES="-DMETHOD=2" $DSTDIR/time_fastops
$DSTDIR/time_fastops > $DSTDIR/time_fastops.20 2>&1
rm -f $DSTDIR/time_fastops.o
make DEFINES="-DMETHOD=3" $DSTDIR/time_fastops
$DSTDIR/time_fastops > $DSTDIR/time_fastops.30 2>&1
rm -f $DSTDIR/time_fastops.o
make DEFINES="-DMETHOD=4" $DSTDIR/time_fastops
$DSTDIR/time_fastops > $DSTDIR/time_fastops.40 2>&1
DSTDIR=$DSTDIR.noshared
rm -f $DSTDIR/time_fastops.o
make BUILD=noshared DEFINES="-DOPTIMIZED -DMETHOD=1" $DSTDIR/time_fastops
$DSTDIR/time_fastops > $DSTDIR/time_fastops.10 2>&1
rm -f $DSTDIR/time_fastops.o
make BUILD=noshared DEFINES="-DOPTIMIZED -DMETHOD=2" $DSTDIR/time_fastops
$DSTDIR/time_fastops > $DSTDIR/time_fastops.20 2>&1
rm -f $DSTDIR/time_fastops.o
make BUILD=noshared DEFINES="-DOPTIMIZED -DMETHOD=3" $DSTDIR/time_fastops
$DSTDIR/time_fastops > $DSTDIR/time_fastops.30 2>&1
rm -f $DSTDIR/time_fastops.o
make BUILD=noshared DEFINES="-DOPTIMIZED -DMETHOD=4" $DSTDIR/time_fastops
$DSTDIR/time_fastops > $DSTDIR/time_fastops.40 2>&1
