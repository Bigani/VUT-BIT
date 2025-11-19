filename=proj3

$(filename).pdf: 	$(filename).ps
			ps2pdf -sPAPERSIZE=a4 $^

$(filename).ps:		$(filename).dvi
			dvips $^

$(filename).dvi:	$(filename).tex		
			latex $^
			latex $^

clean:
			rm -f ${filename}.{ps,pdf,log,aux,out,dvi,bbl,blg}