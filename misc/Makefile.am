MAINTAINERCLEANFILES = Makefile.in

dist_man_MANS = chromium-bsu.6

icon_DATA = chromium-bsu.png
icondir = $(datarootdir)/icons/hicolor/64x64/apps

menu_in_files = chromium-bsu.desktop.in
menu_DATA = $(menu_in_files:.desktop.in=.desktop)
menudir = $(datarootdir)/applications

CLEANFILES = $(menu_DATA) $(icon_DATA)
EXTRA_DIST = $(menu_in_files)

%.desktop: %.desktop.in
	msgfmt --desktop -d $(top_srcdir)/po --template $< -o $@

chromium-bsu.png: $(top_srcdir)/data/png/hero.png
	cp -a $< $@
