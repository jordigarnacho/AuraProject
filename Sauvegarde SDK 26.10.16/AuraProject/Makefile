EXEC_NAME=AuraProject #output filename

SDK_DIR=../../../../out/Unix-base/
IDIR=./
CFLAGS=-I$(IDIR) -I$(SDK_DIR)/staging/usr/include

OBJDIR=obj
LDIR = $(SDK_DIR)/final/usr/lib

LIBS=-L$(LDIR) -larsal -larcontroller -lardiscovery $(EXTERNAL_LIB) -Wl,-rpath=$(LDIR)
LIBS_DBG=-L$(LDIR) -larsal_dbg -larcontroller_dbg -lardiscovery_dbg $(EXTERNAL_LIB) -Wl,-rpath=$(LDIR)

_DEPS = AuraProject.h AuraProjectIHM.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = AuraProject.o AuraProjectIHM.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

FIFO = video_fifo_AuraProject.h264

all: $(EXEC_NAME)
	@[ -p $(FIFO) ] || mkfifo $(FIFO)

$(OBJDIR)/%.o: %.c $(DEPS)
	@ [ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	@ $(CC) -c -o $@ $< $(CFLAGS)

$(EXEC_NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	@ rm -f $(OBJDIR)/*.o *~ core $(INCDIR)/*~
	@ rm -rf $(OBJDIR)
	@ rm -r $(EXEC_NAME)
	@ rm -f $(FIFO)

run:
	LD_LIBRARY_PATH=$(LDIR) ./$(EXEC_NAME)

valgrind:
	LD_LIBRARY_PATH=$(LDIR) valgrind ./$(EXEC_NAME)
