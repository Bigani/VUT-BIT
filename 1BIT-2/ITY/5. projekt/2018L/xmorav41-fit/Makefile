filename=proj5

$(filename).pdf: 	$(filename).ps
			ps2pdf $^

$(filename).ps:		$(filename).dvi
			dvips $^

$(filename).dvi:	$(filename).tex		
			pdflatex $^
			pdflatex $^

clean:
			rm -f ${filename}.{ps,pdf,log,aux,out,dvi,bbl,blg}