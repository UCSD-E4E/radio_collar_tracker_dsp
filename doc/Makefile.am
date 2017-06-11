EXTRA_DIST = payload_ui_wiring.sch
doc_DATA = 3dr_solo.pdf rct_manual.pdf trm.pdf

maintainer-clean-local:
	rm Makefile.in
	rm rct_manual.pdf
	rm trm.pdf

rct_manual.pdf: rct_manual.tex mp_flight_plan_screen.jpg mp_hud_full.jpg MisionPlanner_ConnectButton.png
	pdflatex -halt-on-error rct_manual.tex -output-directory .
	makeglossaries rct_manual
	pdflatex -halt-on-error rct_manual.tex -output-directory .
	makeglossaries rct_manual
	pdflatex -halt-on-error rct_manual.tex -output-directory .
	rm *.aux *.glg *.glo *.gls *.ist *.log *.toc

trm.pdf: trm.tex
	pdflatex -halt-on-error trm.tex -output-directory .
	makeglossaries trm
	pdflatex -halt-on-error trm.tex -output-directory .
	makeglossaries trm
	pdflatex -halt-on-error trm.tex -output-directory .
	rm *.aux *.glg *.glo *.gls *.ist *.log *.toc
