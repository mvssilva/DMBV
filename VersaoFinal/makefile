CCC = g++
CCFLAGS = -Iinclude

SOURCE = $(wildcard src/*.cpp)
INCLUDE = $(wildcard ./include/*.hpp)
OBJECTS = $(SOURCE:.cpp=.o) main.o

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CCC) $(CCFLAGS) -o $@ $^

%.o: %.cpp $(INCLUDE)
	$(CCC) $(CCFLAGS) -c -o $@ $<

main.o: main.cpp $(INCLUDE)
	$(CCC) $(CCFLAGS) -c -o $@ $< 

run: 
	./$(TARGET) ../Instancias/Spd_Inst_Rid_Final2_20_500_Orig/Spd_RF2_20_27_211.txt

clean:
	rm -f src/*.o main.o $(TARGET)