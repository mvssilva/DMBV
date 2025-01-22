################################

# Uncomment for CPLEX_Studio1263
BINDIST    = x86-64_linux
SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio2211/cplex/
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio2211/concert/

################

CCC = g++ #-g -Wall #Para usar o debug, acrescentar aqui: -g -Wall

CCOPT = -m64 -O3 -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD 

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread -ldl -ligraph -fopenmp

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 

INPUT_FILES := $(wildcard ../../Instancias/Spd_Inst_Rid_Final/*.txt)

all : main

main: main.o
	$(CCC) $(CCFLAGS) main.cpp src/Heuristic.cpp src/Graph.cpp src/GrafoRBEP.cpp src/RBEP.cpp src/Roleta.cpp -o dmbv $(CCLNFLAGS)

.cpp.o:
	$(CCC) $(CCFLAGS)  $< -c -o $@ $(CCLNFLAGS)

run: main.o
	@echo "Executando o programa para todos os arquivos de entrada..."
	@rm -f 
	@for input_file in $(INPUT_FILES); do \
	    echo "Executando $$input_file ..."; \
	    ./dmbv $$input_file 2 2 1 1 1 1; \
	done
	@echo "Resultados salvos!"

runsimples: main.o
	./dmbv ../../Instancias/Spd_Inst_Rid_Final2_20_500_Orig/Spd_RF2_20_27_211.txt 2 2 0 0 0 0;

clean:
	rm --force main main.o
