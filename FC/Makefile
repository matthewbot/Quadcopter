sources := $(shell find src -name "*.cpp")
headers := $(shell find src -name "*.h")
tests := $(shell find tests -name "*.cpp")

objects := $(filter-out build/main.o, $(patsubst src/%.cpp, build/%.o, $(sources)))
testbinaries := $(patsubst tests/%.cpp,  out/%.bin, $(tests))
stmos := ../stmos/out/libstmos.a

cpuflags := -mcpu=cortex-m3 -msoft-float -mthumb
sharedflags := \
	-Iinclude -Wall -Werror \
	-ffunction-sections -fdata-sections -pipe \
	-Os -ffast-math -fstrict-aliasing
cppopts := $(cpuflags) $(sharedflags) -fno-exceptions -fno-rtti
ldscript := ../stmos/src/crt/linker.ld
ldflags := -nostdlib $(cpuflags) -Wall -Wl,--gc-sections -T$(ldscript) -L../stmos/out

prefix := arm-elf
ar := $(prefix)-ar
ranlib := $(prefix)-ranlib
cc := $(prefix)-gcc
cpp := $(prefix)-g++
ld := $(prefix)-g++
objdump := $(prefix)-objdump
objcopy := $(prefix)-objcopy
mkdirs = @mkdir -p $(dir $@) out

define mkdeps 
@$(cc) -MM $(cflags) $< > build/$*.d 
@mv -f build/$*.d build/$*.d.tmp 
@sed -e 's|.*:|build/$*.o:|' < build/$*.d.tmp > build/$*.d 
@sed -e 's/.*://' -e 's/\\$$//' < build/$*.d.tmp | fmt -1 | \
    sed -e 's/^ *//' -e 's/$$/:/' >> build/$*.d 
@rm -f build/$*.d.tmp 
endef

all: out/FC.bin $(testbinaries)
	
out/FC.bin: build/FC.out
	$(mkdirs)
	$(objcopy) --output-target=binary $< $@	
	
out/%.bin: build/tests/%.out
	$(mkdirs)
	$(objcopy) --output-target=binary $< $@
	
build/FC.out: build/main.o $(objects) $(stmos) $(ldscript)
	$(mkdirs)
	$(ld) -o $@ $< $(objects) $(ldflags) -Wl,-Map=out/FC.map
	$(objdump) -d $@ > out/FC.dis
	
build/tests/%.out: build/tests/%.o $(stmos) $(objects) $(ldscript)
	$(mkdirs)
	$(ld) -o $@ $< $(objects) $(ldflags) -Wl,-Map=$(patsubst build/tests/%.out,out/%.map, $@)
	$(objdump) -d $@ > $(patsubst build/tests/%.out, out/%.dis, $@)

build/tests/%.o: tests/%.cpp
	$(mkdirs)
	$(cpp) $(cppopts) -c $< -o $@
	$(mkdeps)

build/%.o: src/%.cpp
	$(mkdirs)
	$(cpp) $(cppopts) -c $< -o $@
	$(mkdeps)
	
build/%.o: src/%.c
	$(mkdirs)
	$(cc) $(copts) -c $< -o $@
	$(mkdeps)
	
.PHONY: clean
clean:
	rm -rf build out
	
.SECONDARY:

-include $(objects:.o=.d)

