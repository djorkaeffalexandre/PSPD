CC = gcc
LDFLAGS = 
BLDDIR = .
INCDIR = $(BLDDIR)/inc
SRCDIR = $(BLDDIR)/src
OBJDIR = $(BLDDIR)/obj
CFLAGS = -c -Wall -I$(INCDIR)
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
EXE = bin/bin

all: clean $(EXE) 
    
$(EXE): $(OBJ) 
	$(CC) $(LDFLAGS) $(OBJDIR)/*.o -o $@ 

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJDIR)/*.o $(EXE)

run:
	./bin/bin

flat:
	./bin/bin < input/flat75-90 >> generated/flat75-90; cmp --silent generated/flat75-90 output/flat75-90 && echo 'Success!' || echo 'Failure!'

simples:
	./bin/bin < input/simples >> generated/simples; cmp --silent generated/simples output/simples && echo 'Success!' || echo 'Failure!'

uf:
	./bin/bin < input/uf20-01 >> generated/uf20-01; cmp --silent generated/uf20-01 output/uf20-01 && echo 'Success!' || echo 'Failure!'
