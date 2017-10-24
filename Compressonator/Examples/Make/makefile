CC := g++-6
srcdir := $(CURDIR)/..
objdir := $(CURDIR)/../../Build/Linux/Example/Temp
sources := $(shell find $(srcdir) -name "*.cpp")
headers := $(shell find $(srcdir) -name "*.h") $(shell find $(srcdir) -name "*.hxx")
objects := $(patsubst $(srcdir)/%,$(objdir)/%.o,$(sources))
INCLUDE := \
    -I$(CURDIR)/../../Header
LIBDIRS := \
    -L$(CURDIR)/../../Build/Linux
BUILD_DIR := \
    $(CURDIR)

main : $(objects) $(headers) compressonator ; \
    cd $(BUILD_DIR) ; \
    $(CC) $(LIBDIRS) -o $(CURDIR)/../../Build/Linux/Example/cmpcli $(objects) -l compressonator -pthread

$(objdir)/%.o : $(srcdir)/% ; \
    mkdir -p $(dir $@) ; \
    $(CC) -O3 -o $(objdir)/$(patsubst $(objdir)/%.o,%.o,$@) -c $< -Wno-invalid-offsetof $(INCLUDE) ;

compressonator : ; \
    cd $(BUILD_DIR)/../../Make/ExampleMake ; \
    make
    
clean : compressonator.clean ; \
    rm -rf $(objdir)

compressonator.clean : ; \
    cd $(BUILD_DIR)/../../Make/ExampleMake/ ; \
    make clean

print-%: ; @echo '$*=$($*)'