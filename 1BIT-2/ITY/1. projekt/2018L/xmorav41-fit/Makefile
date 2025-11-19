filename=proj1

pdf: ps
	ps2pdf ${filename}.ps

ps:	dvi
	dvips -t a4 ${filename}.dvi

dvi:
	latex ${filename}
	bibtex ${filename}||true
	latex ${filename}
	latex ${filename}

read:
	evince ${filename}.pdf &

aread:
	acroread ${filename}.pdf

clean:
	rm -f ${filename}.{ps,pdf,log,aux,out,dvi,bbl,blg}
