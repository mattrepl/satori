#
# Satori Makefile
#
# Usage:
#    make        	(to build program
#    make clean         (to remove old files)
#

#
# Variables to Use
#

# Compiler to use
CC = g++
# Flags for stricter compile options
CFLAGS = -Wall -O3
# Flags for debugging output
DFLAGS = -g
# OpenCV includes
OPENCVI = `pkg-config --cflags opencv`
# OpenCV libraries
OPENCVL = `pkg-config --libs opencv`
# Boost includes
OPTI = -I/opt/local/include
# Boost Filesystem libraries
BOOSTFSL = -lboost_filesystem
# Name of program executable
POUT = satori  

#
# Makefile
#

# build program
all: satori.o satori_app.o flow.o track.o focus.o common.o
	$(CC) $(CFLAGS) $(OPENCVL) $(BOOSTFSL) satori.o satori_app.o flow.o track.o focus.o common.o -o $(POUT)

# compile program
satori.o: satori.cxx satori.h
	$(CC) -c $(DFLAGS) $(OPENCVI) $(OPTI) satori.cxx

# compile satori app class
satori_app.o: satori_app.cxx satori_app.h
	$(CC) -c $(DFLAGS) $(OPENCVI) $(OPTI) satori_app.cxx

# compile flow component of program
flow.o: flow.cxx flow.h img_template.tpl
	$(CC) -c $(DFLAGS) $(OPENCVI) $(OPTI) flow.cxx

# compile track component of program
track.o: track.cxx track.h img_template.tpl
	$(CC) -c $(DFLAGS) $(OPENCVI) $(OPTI) track.cxx

# compile focus component of program
focus.o: focus.cxx focus.h img_template.tpl
	$(CC) -c $(DFLAGS) $(OPENCVI) $(OPTI) focus.cxx

# compile common functions
common.o: common.cxx
	$(CC) -c $(DFLAGS) $(OPENCVI) $(OPTI) common.cxx	

# clean products of compile and build
clean:
	rm -f *.o
	rm -f *~*
	rm -f $(POUT)

# build TAGS
tags: 	
	etags *.cxx *.h *.tpl