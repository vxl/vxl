#
# ./v3p/netlib/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#
ADD_LIBRARY(netlib netlib_sources)

SOURCE_FILES( netlib_sources
F77_aloc  dgpfa5f   dtrans         r_imag   slarf   zgebak
c_abs     dlabad    dzasum         r_sign   slarfg  zgebal
c_div     dlacpy    eisrsg         rg       slartg  zgeev
c_sqrt    dladiv    enorm          rpoly    slas2   zgehd2
cabs      dlags2    exit           rs       slaset  zgehrd
camsun    dlamch    fdjac2         rsg      slassq  zgemm
caxpy     dlange    full-dpoco     s_cat    slasv2  zgemv
ccopy     dlapll    gpfa           s_cmp    smvpc   zgerc
cdabs     dlapmt    gpfa2f         s_copy   snlaso  zhseqr
cdotc     dlapy2    gpfa3f         sasum    snppla  zlacgv
cdsqrt    dlapy3    gpfa5f         saxpy    snrm2   zlacpy
cg        dlarf     idamax         scnrm2   sorg2r  zladiv
cqrdc     dlarfg    ilaenv         scopy    sorm2r  zlahqr
cqrsl     dlartg    isamax         sdot     sormr2  zlahrd
cscal     dlas2     izamax         setgpfa  sortqr  zlange
csrot     dlaset    izmax1         sgemv    sqrdc   zlanhs
csvdc     dlassq    lbfgs-example  sgeqpf   sqrsl   zlarf
cswap     dlasv2    lbfgs-lb1      sgeqr2   srot    zlarfb
d_cnjg    dnlaso    lbfgs          sger     srotg   zlarfg
d_imag    dnrm2     lmder          sgerq2   sscal   zlarft
d_lg10    dorg2r    lmder1         sggsvd   ssvdc   zlarfx
d_sign    dorm2r    lmdif          sggsvp   sswap   zlascl
dasum     dormr2    lmpar          sig_die  stgsja  zlaset
daxpy     dpoco     lsame          slabax   svsort  zlassq
dcopy     dpodi     lsqr-test      slabcm   tql1    zlatrs
ddot      dpofa     lsqr           slabfc   tql2    zqrdc
dgemv     dposl     machineparams  slacpy   trans   zqrsl
dgeqpf    dqrdc     main           slaeig   tred1   zsvdc
dgeqr2    dqrsl     pow_dd         slager   tred2   ztrevc
dger      drot      pow_di         slags2   xerbla  ztrmm
dgerq2    drotg     pow_ii         slamch   z_abs   ztrmv
dggsvd    dscal     pow_ri         slange   z_div   ztrsv
dggsvp    dsetgpfa  pythag         slapll   z_sqrt  zung2r
dgpfa     dsvdc     qrfac          slapmt   zcopy   zunghr
dgpfa2f   dswap     qrsolv         slapy2   zdotu   zungqr
dgpfa3f   dtgsja    r_cnjg         slaran   zdscal
)



