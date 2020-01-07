EXTRA_DIST = payload_ui_wiring.sch
doc_DATA = 
if HAVE_PDFLATEX
doc_DATA += rct_manual.pdf trm.pdf
endif
if HAVE_LUALATEX
doc_DATA += system-diagram.pdf operations_flow.pdf processing_pipeline.pdf v2_signal_detector.pdf
if HAVE_PDF2SVG
doc_DATA += system-diagram.svg operations_flow.svg processing_pipeline.svg v2_signal_detector.svg
endif
endif

clean:
	-rm doc_DATA
	-rm *.aux *.glg *.glo *.gls *.ist *.log *.toc *.lof *.lot
	

maintainer-clean-local:
	-rm Makefile.in
	-rm rct_manual.pdf

rct_manual.pdf: rct_manual.tex mp_flight_plan_screen.jpg mp_hud_full.jpg MisionPlanner_ConnectButton.png
	$(PDFLATEX) -halt-on-error $< -output-directory .
	makeglossaries rct_manual
	$(PDFLATEX) -halt-on-error $< -output-directory .
	makeglossaries rct_manual
	$(PDFLATEX) -halt-on-error $< -output-directory .
	-rm rct_manual.aux rct_manual.glg rct_manual.glo rct_manual.gls rct_manual.ist rct_manual.log rct_manual.toc rct_manual.lof rct_manual.lot

trm.pdf: trm.tex
	$(PDFLATEX) -halt-on-error $< -output-directory .
	makeglossaries trm
	$(PDFLATEX) -halt-on-error $< -output-directory .
	makeglossaries trm
	$(PDFLATEX) -halt-on-error $< -output-directory .
	-rm trm.aux trm.glg trm.glo trm.gls trm.ist trm.log trm.toc trm.lof trm.lot trm.lol

system-diagram.pdf: system-diagram.tex
	$(LUALATEX) $<
	-rm system-diagram.aux system-diagram.log
	
%.svg: %.pdf
	$(PDF2SVG) $< $@

operations_flow.pdf: operations_flow.tex
	$(LUALATEX) $<
	-rm operations_flow.aux operations_flow.log

processing_pipeline.pdf: processing_pipeline.tex
	$(LUALATEX) $<
	-rm processing_pipeline.aux processing_pipeline.log

v2_signal_detector.pdf: v2_signal_detector.tex
	$(LUALATEX) $<
	-rm v2_signal_detector.aux v2_signal_detector.log

