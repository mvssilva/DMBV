CCC = g++
CCFLAGS = -Iinclude

SOURCE = $(wildcard src/*.cpp)
INCLUDE = $(wildcard ./include/*.hpp)
OBJECTS = $(SOURCE:.cpp=.o) main.o

TARGET = main

INPUT_FILES := $(wildcard ../Instancias/Spd_Inst_Rid_Final/*.txt)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CCC) $(CCFLAGS) -o $@ $^

%.o: %.cpp $(INCLUDE)
	$(CCC) $(CCFLAGS) -c -o $@ $<

main.o: main.cpp $(INCLUDE)
	$(CCC) $(CCFLAGS) -c -o $@ $< 

run1: 
	./$(TARGET) ../Instancias/Spd_Inst_Rid_Final/Spd_RF2_200_289_3931.txt

run2: 
	@echo "Executando o programa para todos os arquivos de entrada..."
	@rm -f 
	@for input_file in $(INPUT_FILES); do \
	    echo "Executando $$input_file ..."; \
	    ./$(TARGET) $$input_file; \
	done
	@echo "Resultados salvos!"

clean:
	rm -f src/*.o main.o $(TARGET)