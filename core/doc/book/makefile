# You'll need texi2html version 1.78 or higher from
# http://www.nongnu.org/texi2html/

ifeq ($(OS),Windows_NT)
 GS = gswin32c
 PNGTOPNM = pngtopnm.exe
 PNMCROP = pnmcrop.exe
 PNMTOPNG = pnmtopng.exe
 MAKEINFO= makeinfo.exe # exe to avoid conflict with cygnus makeinfo sh script
 TEXI2HTML= T2H_HOME=/pkg/Texi2html perl /pkg/texi2html/texi2html.pl
 TEX = tex --c-style-errors --quiet
else
 GS = gs
 PNGTOPNM = pngtopnm
 PNMCROP = pnmcrop
 PNMTOPNG = pnmtopng
 MAKEINFO = makeinfo
 T2H_HOME = false
 TEXI2HTML = texi2html
 TEX = tex
endif

MAKEINFOFLAGS=-Dmakeinfo

SOURCEFILES=book.texi \
            appendix_adding_to_vnl_algo.texi \
            appendix_build.texi \
            appendix_coding_standards.texi \
            appendix_structure.texi \
            appendix_vsl_for_developers.texi \
            book_header.texi \
            chapter_examples.texi \
            chapter_intro.texi \
            chapter_vbl.texi \
            chapter_vcl.texi \
            chapter_vcsl.texi \
            chapter_vgl.texi \
            chapter_vgui.texi \
            chapter_vidl.texi \
            chapter_vil.texi \
            chapter_vil1.texi \
            chapter_vnl.texi \
            chapter_vpdl.texi \
            chapter_vsl.texi \
            chapter_vul.texi \
            book.css

all: book.html
tex: book.dvi

thelot: book.info book.html book.dvi book.hpj book.info

# Generate figure for info
rosenbrock.txt:
	echo '[ Plot of the Rosenbrock "banana" function ]' > rosenbrock.txt

# Generate figure for HTML
rosenbrock.png: rosenbrock.eps
	$(GS) -sDEVICE=png16m -sOutputFile=tmp.png -dQUIET -dNOPAUSE rosenbrock.eps -c showpage -c quit
	$(PNGTOPNM) tmp.png | $(PNMCROP) | $(PNMTOPNG) > $@
	rm tmp.png

ifdef USE_MAKEINFO
book.html: $(SOURCEFILES) rosenbrock.png
	$(MAKEINFO) $(MAKEINFOFLAGS) book.texi --html -o $@ --number-sections
else
book.html: $(SOURCEFILES) rosenbrock.png
	$(TEXI2HTML) -menu -number -split=chapter --css-include=book.css --nosec-nav -frames book.texi
endif


# TeX for paper printing
book.dvi: $(SOURCEFILES) rosenbrock.eps
	$(TEX) book.texi || touch -t 200001010000 book.dvi


# Windows help files. Process hpj the help compiler
book.hpj book.rtf: book.texi
	-mkinfrtf.exe $(MAKEINFOFLAGS) book --force -o book.rtf --hpj-output book.hpj

# Emacs info file
book.info: $(SOURCEFILES) rosenbrock.txt
	$(MAKEINFO) $(MAKEINFOFLAGS) book --force -o book.info

## awf private
publish:
	 rcp index.html book*.html volumnia.robots.ox.ac.uk.awf:WWW/vxl/book

## for debugging makefile
echo-%:
	echo '$(@:echo-%=%)=$($(@:echo-%=%))'

