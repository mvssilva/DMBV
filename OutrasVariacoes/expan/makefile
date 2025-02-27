#CPPFLAGS = -w -std=c++11 -O3 -I include
#LNKFLAGS1 = -pg -w -std=c++11 -O3 -I include

CPPFLAGS = -std=c++11 -m64 -I include
LNKFLAGS1 = -std=c++11 -m64 -I include

# Uncomment for CPLEX_Studio1263
BINDIST    = x86-64_linux
SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio2211/cplex/
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio2211/concert/

################
CCC = -g  #Para usar o debug, acrescentar aqui: -g -Wall
CCOPT = -m64 -O3 -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD 
COMP = g++
OBJ_DIR = obj

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread -ldl -ligraph -fopenmp

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 

TARGET = main.exe

# Lista de arquivos de entrada
INPUT_FILES := $(wildcard ../../Instancias/Spd_Inst_Rid_Final/*.txt)

# Arquivo de saída
OUTPUT_FILE := R-BEP.csv

# Obtém automaticamente a lista de todos os arquivos .cpp 
SRCS := $(wildcard src/*.cpp)
SRCS += main.cpp

# Gera a lista de arquivos de objeto correspondentes na pasta obj
OBJ := $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(COMP) $(CCC) $(CCFLAGS) $(LNKFLAGS1) $(OBJ) -o $@ $(CCLNFLAGS)

# Regra para compilar cada arquivo .cpp e colocar o arquivo de objeto na pasta obj
$(OBJ_DIR)/%.o: src/%.cpp main.cpp
	$(COMP) $(CPPFLAGS) -c $< -o $@

# Regra para executar o programa para cada arquivo de entrada e salvar o resultado em RBEP.csv
run: $(TARGET)
	@echo "Executando o programa para todos os arquivos de entrada..."
	@rm -f 
	@for input_file in $(INPUT_FILES); do \
	    echo "Executando $$input_file ..."; \
	    ./$(TARGET) $$input_file; \
	done
	@echo "Resultados salvos!"

run1: 
	./$(TARGET) ../../Instancias/Spd_Inst_Rid_Final/Spd_RF2_20_27_211.txt

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f *.o

