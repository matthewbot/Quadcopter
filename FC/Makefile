include gmsl

sources := $(shell find src -name "*.c" )
not-sources := src/math/test.c
sources := $(filter-out $(not-sources), $(sources))
headers := $(shell find src -name "*.h" )
objects := $(addprefix build/, $(patsubst src/%,%, $(sources:.c=.o)))

cpuflags := -mcpu=cortex-m3 -msoft-float -mthumb
sharedflags := \
	-Isrc -Isrc/stm32 -Wall -Werror \
	-ffunction-sections -fdata-sections -pipe \
	-Os -ffast-math -fomit-frame-pointer -fstrict-aliasing
cflags := $(cpuflags) $(sharedflags) -std=gnu99 
cppflags := $(cpuflags) $(sharedflags) -fno-exceptions -fno-rtti
ldflags := $(cpuflags) -lm -nostartfiles -Wall -Wl,--gc-sections

prefix := arm-none-eabi
cc := $(prefix)-gcc
cpp := $(prefix)-g++
ld := $(prefix)-gcc
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

all: out/FC.bin

out/FC.bin: build/FC.out
	$(mkdirs)
	$(objcopy) --output-target=binary $< $@
	du out/FC.bin -h
	
build/FC.out: $(objects) src/crt/FC.ld
	$(mkdirs)
	$(ld) -Tsrc/crt/FC.ld $(objects) -o build/FC.out -Wl,-Map=out/FC.map $(ldflags)
	$(objdump) -d build/FC.out > out/FC.dis

build/%.o: src/%.c
	$(mkdirs)
	$(cc) $(cflags) -c $< -o $@
	$(mkdeps)
	
build/%.o: src/%.s
	$(mkdirs)
	$(cc) $(cflags) -c $< -o $@
	$(mkdeps)
	
build/%.o: src/%.cpp
	$(mkdirs)
	$(cpp) $(cppflags) -c $< -o $@
	$(mkdeps)
	
.PHONY: clean
clean:
	rm -rf build out
	
.SECONDARY:

-include $(objects:.o=.d)

