# You'll need texi2html from
# http://www.mathematik.uni-kl.de/~obachman/Texi2Html/Distrib

ifeq ($(OS),Windows_NT)
 GS = gswin32c
 PNGTOPNM = pngtopnm.exe
 PNMCROP = pnmcrop.exe
 PNMTOPNG = pnmtopng.exe
 MAKEINFO= makeinfo.exe # exe to avoid conflict with cygnus makeinfo sh script
 TEXI2HTML= T2H_HOME=/pkg/Texi2html perl /pkg/texi2html/texi2html.pl
else
 GS = gs
 PNGTOPNM = pngtopnm
 PNMCROP = pnmcrop
 PNMTOPNG = pnmtopng
 MAKEINFO = makeinfo
 T2H_HOME = false
 TEXI2HTML = texi2html
endif

MAKEINFOFLAGS=-Dmakeinfo --no-validate

SOURCEFILES=book.texi \
            chapter_mbl.texi \
            chapter_clsfy.texi \
            chapter_vil3d.texi \
            chapter_vpdfl.texi

all: book.html book.dvi

thelot: book.info book.html book.dvi book.hpj

ifdef aaa
book.html: $(SOURCEFILES)  mbl_mz_random_graph1.png mbl_mz_random_graph2.png mbl_mz_random_graph3.png
	$(MAKEINFO) $(MAKEINFOFLAGS) book.texi --html -o $@ --number-sections
else
book.html: $(SOURCEFILES) mbl_mz_random_graph1.png mbl_mz_random_graph2.png mbl_mz_random_graph3.png
	$(TEXI2HTML) book.texi -expand info -number -split chapter -no-section_navigation -frames
endif


# TeX for paper printing
book.dvi: $(SOURCEFILES)
	tex --c-style-errors --quiet book.texi || touch -t 010100001970 book.dvi


# Windows help files. Process hpj the help compiler
book.hpj book.rtf: $(SOURCEFILES)
	-mkinfrtf.exe $(MAKEINFOFLAGS) book --force -o book.rtf --hpj-output book.hpj

# Emacs info file
book.info: $(SOURCEFILES)
	$(MAKEINFO) $(MAKEINFOFLAGS) book --force -o book.info


# Generate figure for HTML
mbl_mz_random_graph1.png: mbl_mz_random_graph1.eps
	$(GS) -sDEVICE=png16m -sOutputFile=tmp.png -dQUIET -dNOPAUSE mbl_mz_random_graph1.eps -c showpage -c quit
	$(PNGTOPNM) tmp.png | $(PNMCROP) | $(PNMTOPNG) > $@
	rm tmp.png

mbl_mz_random_graph2.png: mbl_mz_random_graph2.eps
	$(GS) -sDEVICE=png16m -sOutputFile=tmp.png -dQUIET -dNOPAUSE mbl_mz_random_graph2.eps -c showpage -c quit
	$(PNGTOPNM) tmp.png | $(PNMCROP) | $(PNMTOPNG) > $@
	rm tmp.png

mbl_mz_random_graph3.png: mbl_mz_random_graph3.eps
	$(GS) -sDEVICE=png16m -sOutputFile=tmp.png -dQUIET -dNOPAUSE mbl_mz_random_graph3.eps -c showpage -c quit
	$(PNGTOPNM) tmp.png | $(PNMCROP) | $(PNMTOPNG) > $@
	rm tmp.png

## for debugging makefile
echo-%:
	echo '$(@:echo-%=%)=$($(@:echo-%=%))'

