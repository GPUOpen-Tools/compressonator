CC := g++
srcdir := $(CURDIR)/../Source
headerdir := $(CURDIR)/../Header
commondir := $(CURDIR)/../../Common
objdir := $(CURDIR)/../Build/Linux/Temp
sources := $(shell find $(srcdir)/Codec -name "*.cpp") \
    $(shell find $(srcdir)/Codec -name "*.c") \
    $(shell find $(srcdir)/Codec -name "*.cxx") \
    $(shell find $(srcdir)/Common -name "*.cpp") \
    $(shell find $(srcdir)/Common -name "*.c") \
    $(shell find $(srcdir)/Common -name "*.cxx") \
    $(wildcard $(srcdir)/*.cpp)
commonsources := \
    $(commondir)/Lib/Ext/OpenEXR/ilmbase-2.2.0/Half/half.cpp
headers := $(shell find $(srcdir)/Codec -name "*.h") \
    $(shell find $(srcdir)/Common -name "*.h") \
    $(shell find $(headerdir)/Codec -name "*.h") \
    $(wildcard $(srcdir)/*.h)
objects := $(patsubst $(srcdir)/%,$(objdir)/%.o,$(sources))
commonobjects := $(patsubst $(commondir)/%,$(objdir)/%.co,$(commonsources))
LIBMAKE = ar
INCLUDE := -I$(headerdir) \
    -I$(srcdir)/Common \
    -I$(commondir)/Lib/Ext/OpenEXR/ilmbase-2.2.0/Half \
    -I$(commondir)/Lib/AMD/APPSDK/3-0/include \
    -I$(headerdir)/Codec \
    $(patsubst %,-I%,$(shell find $(headerdir)/Codec -type d))\
    -I$(headerdir)/Internal
    
main : $(objects) $(commonobjects) $(headers) ; mkdir -p ../Build/Linux/ ; \
    $(LIBMAKE) rcs ../Build/Linux/libcompressonator.a $(objects) $(commonobjects)

$(objdir)/%.o : $(srcdir)/% ; \
    mkdir -p $(dir $@) ; \
    $(CC) -O3 -o $@ -c $< -Wno-conversion-null $(INCLUDE) ;

$(objdir)/%.co : $(commondir)/% ; \
    mkdir -p $(dir $@) ; \
    $(CC) -O3 -o $@ -c $< $(INCLUDE) ;

clean : ; \
    rm -rf $(objdir)

print-%: ; @echo '$*=$($*)'