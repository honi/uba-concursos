LATEX := lualatex
LATEXFLAGS := -interaction=nonstopmode --shell-escape -synctex=1

BekensteinJonathan-2023-Ay2-Sistemas.pdf: sistemas.tex
	$(LATEX) $(LATEXFLAGS) --jobname $(basename $@) $<

clean:
	rm -rf *.pdf *.aux *.log *.out *.synctex.gz svg-inkscape _minted-*

.PHONY: clean
