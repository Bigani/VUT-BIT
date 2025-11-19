filename=proj4
INCLUDE=czechiso.bst

.PHONY: clean pack bibtex pdflatex

$(filename).pdf: 	$(filename).ps
			ps2pdf -sPAPERSIZE=a4 $^

$(filename).ps:		$(filename).dvi
			dvips $^

$(filename).dvi:	$(filename).tex	
			latex $^
			make bibtex	
			latex $^
			latex $^

bibtex: 		$(filename).bib
			bibtex $(filename)

clean:
			rm -f ${filename}.{ps,pdf,log,aux,out,dvi,bbl,blg}