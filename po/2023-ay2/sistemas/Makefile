LATEX := lualatex
LATEXFLAGS := -interaction=nonstopmode --shell-escape -synctex=1

all:
	$(LATEX) $(LATEXFLAGS) --jobname BekensteinJonathan-2023-Ay2-Sistemas sistemas.tex

clean:
	rm -rf *.pdf *.aux *.log *.out *.synctex.gz svg-inkscape _minted-*

.PHONY: all clean
