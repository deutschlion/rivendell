## automake.am
##
## docs/docbook/automake.am for Rivendell
##
## (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
## Use automake to process this into a Makefile.in

cae.html:	cae.xml
	xsltproc -o cae.html $(DOCBOOK_STYLESHEETS)/xhtml/docbook.xsl cae.xml
cae.pdf:	cae.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/fo/docbook.xsl cae.xml | fop - -pdf cae.pdf

web_api.html:	web_api.xml
	xsltproc -o web_api.html $(DOCBOOK_STYLESHEETS)/xhtml/docbook.xsl web_api.xml
web_api.pdf:	web_api.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/fo/docbook.xsl web_api.xml | fop - -pdf web_api.pdf

%.1:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<

%.8:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<


all-local:	cae.html cae.pdf web_api.html web_api.pdf

EXTRA_DIST = cae.html\
             cae.pdf\
             cae.xml\
             web_api.html\
             web_api.pdf\
             web_api.xml

CLEANFILES = *~
MAINTAINERCLEANFILES = *~\
                       *.1\
                       *.8\
                       *.html\
                       *.pdf\
                       aclocal.m4\
                       configure\
                       Makefile.in
