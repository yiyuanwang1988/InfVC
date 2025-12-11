TARGET = Main_1
CXXFLAGS = -ansi -O3

SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPLEXDIR      = /home/wyy/ibm/ILOG/CPLEX_Studio201/cplex
CONCERTDIR    = /home/wyy/ibm/ILOG/CPLEX_Studio201/concert
CCC = g++
CCOPT = -m64 -O3 -fPIC -fexceptions -DNDEBUG -DIL_STD
CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -L$(CONCERTLIBDIR) -lconcert -L$(CPLEXLIBDIR) -lcplex -lm -lpthread
CLNFLAGS  = -L$(CPLEXLIBDIR) -lcplex -lm -lpthread
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)

all : InfVC 

InfVC : fastvc.cpp fastvc.h preprocess.cc preprocess.h
	$(CCC) $(CCFLAGS) -o $@ $^ $(CCLNFLAGS)
	
clean :
	-rm fastvc