CXX = g++
CXXFLAGS = -Wno-unused-result -Wno-unused-parameter -g
LIBED_DEPS = $(patsubst ./include/%.cpp,obj/%.o,$(wildcard ./include/*.cpp))
OBJDIR = obj

all: main

# Compilação dos arquivos .cpp para objetos .o na pasta obj/
$(OBJDIR)/%.o: ./include/%.cpp ./include/%.h | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Criação da pasta obj/ se não existir
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Criação do arquivo estático libed.a com os objetos na pasta obj/
libed.a: $(LIBED_DEPS)
	ar -crs libed.a $(LIBED_DEPS)

# Compilação do programa principal main.cpp usando libed.a
main: main.cpp libed.a
	$(CXX) $(CXXFLAGS) -o main main.cpp -I ./include -L . -led -lm

# Limpeza dos arquivos gerados e objetos
clean:
	rm -f main libed.a
	rm -rf $(OBJDIR)

# Execução do programa principal
run:
	./main