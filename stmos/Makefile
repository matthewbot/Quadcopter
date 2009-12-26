stmos := out/libstmos.a

sources := $(shell find src -name "*.cpp" -or -name "*.c")
headers := $(shell find src -name "*.h")
objects := $(addprefix build/, $(patsubst src/%,%, $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(sources)))))
ldscript := src/crt/linker.ld

test-sources := $(shell find tests -name "*.cpp")
test-headers := $(shell find tests -name "*.h")
test-objects := $(addprefix build/tests/, $(patsubst tests/%,%, $(test-sources:.cpp=.0)))
test-binaries := $(addprefix out/, $(patsubst tests/%,%, $(test-sources:.cpp=.bin)))

cpuflags := -mcpu=cortex-m3 -msoft-float -mthumb
sharedflags := \
	-Iinclude -Wall -Werror \
	-ffunction-sections -fdata-sections -pipe \
	-Os -ffast-math -fstrict-aliasing
cppopts := $(cpuflags) $(sharedflags) -fno-exceptions -fno-rtti -std=gnu++0x
copts := $(cpuflags) $(sharedflags) -std=gnu99
ldflags := -nostdlib $(cpuflags) -Wall -Wl,--gc-sections -T$(ldscript)

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

all: $(stmos) $(test-binaries)

$(stmos): $(objects)
	$(mkdirs)
	$(ar) rcs $@ $(objects)
	$(ranlib) $@
	
out/%.bin: build/tests/%.out
	$(mkdirs)
	$(objcopy) --output-target=binary $< $@
	
build/tests/%.out out/%.dis: build/tests/%.o $(stmos) $(ldscript)
	$(mkdirs)
	$(ld) -Lout -o $@ $< $(ldflags) -Wl,-Map,$(patsubst build/tests/%.out,out/%.map,$@)
	$(objdump) -d $@ > $(patsubst build/tests/%.out, out/%.dis, $@)

build/tests/%.o: tests/%.cpp $(headers)
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

