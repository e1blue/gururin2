EMCC = emcc

PLATSRCDIR = ../fuhahaEngine/web
CORESRCDIR = ../fuhahaEngine/core
PLUGSRCDIR = ../main/web
MAINSRCDIR = ../main/core

OUTDIR = obj
BINDIR = bin
JSSRCS = $(shell find $(PLATSRCDIR) $(PLUGSRCDIR) -name "*.js")
CSRCS = $(PLATSRCDIR)/main.c $(shell find $(CORESRCDIR) $(MAINSRCDIR) -name "*.c")
OBJS = $(addprefix $(OUTDIR)/,$(notdir $(CSRCS:.c=.o)))
TARGET = $(OUTDIR)/game.js
DEPEND = $(OUTDIR)/MakeDepends.mk

CFLAGS = -I$(PLATSRCDIR) -I$(CORESRCDIR) -I$(MAINSRCDIR)
EXFLAGS = -s EXPORTED_FUNCTIONS="$(shell sh $(PLATSRCDIR)/exFuncs.sh)"
JSFLAGS = $(addprefix --js-library , $(JSSRCS))
# TODO plugin_web用のexFuncs.shを作る

CFLAGS += -Wall -Wextra -Wpedantic -Werror 
CFLAGS += -Wfloat-equal
CFLAGS += -Wno-missing-field-initializers
CFLAGS += -Wno-missing-braces
CFLAGS += -Wno-char-subscripts
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-gnu-empty-struct

.PHONY : default debug release build clean

default: debug 

# --------------------------------

debug: build

release: EMCC += -O3
#release: EMCC += -s WASM=1
release: clean build

release11: BINDIR = bin11
release11: clean build

release12: BINDIR = bin12
release12: EMCC += -s WASM=1
release12: clean build

release21: BINDIR = bin21
release21: EMCC += -O3
release21: clean build

release22: BINDIR = bin22
release22: EMCC += -O3 -s WASM=1
release22: clean build

release31: BINDIR = bin31
release31: EMCC += -Os
release31: clean build

release32: BINDIR = bin32
release32: EMCC += -Os -s WASM=1
release32: clean build

release41: BINDIR = bin41
release41: EMCC += -Oz
release41: clean build

release42: BINDIR = bin42
release42: EMCC += -Oz -s WASM=1
release42: clean build

build: $(TARGET)
	mkdir -p $(BINDIR)
	rsync -av $(TARGET) game.html frame.html $(BINDIR)
	test ! -f $(TARGET).mem || rsync -av $(TARGET).mem $(BINDIR)
	test ! -f $(TARGET:.js=.wasm) || rsync -av $(TARGET:.js=.wasm) $(BINDIR)

clean:
	-rm $(TARGET) $(DEPEND) $(OBJS)
	test ! -f $(TARGET).mem || rm $(TARGET).mem
	test ! -f $(TARGET:.js=.wasm) || rm $(TARGET:.js=.wasm)

# --------------------------------

$(TARGET): $(DEPEND) $(OBJS) $(JSSRCS)
	$(EMCC) $(CFLAGS) $(EXFLAGS) $(JSFLAGS) -o $@ $(OBJS)

$(DEPEND): $(CSRCS)
	$(EMCC) $(CFLAGS) -MM $(CSRCS) |\
		perl -pe 's/\n/NEWLINE/g' | perl -pe 's/\\NEWLINE  //g' | perl -pe 's/NEWLINE/\n/g' |\
		perl -pe 's/^/$(OUTDIR)\//g' |\
		perl -pe 's/\n/\n\t\$$\(EMCC\) \$$\(CFLAGS\) -o \$$@ \$$\<\n/g' > $(DEPEND)

# --------------------------------

-include $(DEPEND)

# --------------------------------

