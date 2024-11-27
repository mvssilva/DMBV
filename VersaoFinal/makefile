CCC = g++
CCFLAGS = -Iinclude

SOURCE = $(wildcard src/*.cpp)
INCLUDE = $(wildcard ./include/*.hpp)
OBJECTS = $(SOURCE:.cpp=.o) main.o

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CCC) $(CCFLAGS) -o $@ $^

%.o: %.c $(INCLUDE)
	$(CCC) $(CCFLAGS) -c -o $@ $<

main.o: main.cpp $(INCLUDE)
	$(CCC) $(CCFLAGS) -c -o $@ $< 

run: 
	./$(TARGET)

clean:
	rm -f src/*.o main.o $(TARGET)