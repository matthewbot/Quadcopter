include gmsl

sources := $(shell find src -name "*.c" -or -name "*.s")
headers := $(shell find src -name "*.h" )
objects := $(addprefix build/, $(patsubst src/%,%, $(patsubst %.s, %.o, $(patsubst %.c, %.o, $(sources)))))

cflags := -Os -Isrc -Isrc/stm32 -msoft-float -mcpu=cortex-m3 -Wall -mthumb -std=gnu99 -fno-common -DSTM32F10X_MD -DUSE_FULL_ASERT -ffunction-sections -fdata-sections 

ldflags := -lm -nostartfiles -Wall -msoft-float -mcpu=cortex-m3 -mthumb -Wl,--gc-sections

prefix := arm-none-eabi
cc := $(prefix)-gcc
ld := $(prefix)-gcc
objdump := $(prefix)-objdump
objcopy := $(prefix)-objcopy
mkdirs = @mkdir -p $(dir $@) out

all: out/flash.bin

out/%.bin: build/%.out
	$(mkdirs)
	$(objcopy) --output-target=binary $< $@
	
build/%.out: $(objects) src/ld/%.ld
	$(mkdirs)
	$(ld) -Tsrc/ld/$*.ld $(objects) -o build/$*.out -Wl,-Map=out/$*.map $(ldflags)
	$(objdump) -d build/$*.out > out/$*.dis

build/%.o: src/%.c
	$(mkdirs)
	$(cc) $(cflags) -c $< -o $@
	@$(cc) -MM $(cflags) $< > build/$*.d
	@mv -f build/$*.d build/$*.d.tmp
	@sed -e 's|.*:|build/$*.o:|' < build/$*.d.tmp > build/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < build/$*.d.tmp | fmt -1 | \
	   sed -e 's/^ *//' -e 's/$$/:/' >> build/$*.d
	@rm -f build/$*.d.tmp
	
build/%.o: src/%.s
	$(mkdirs)
	$(cc) $(cflags) -c $< -o $@
	@$(cc) -MM $(cflags) $< > build/$*.d
	@mv -f build/$*.d build/$*.d.tmp
	@sed -e 's|.*:|build/$*.o:|' < build/$*.d.tmp > build/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < build/$*.d.tmp | fmt -1 | \
	   sed -e 's/^ *//' -e 's/$$/:/' >> build/$*.d
	@rm -f build/$*.d.tmp

.PHONY: clean
clean:
	rm -rf build out
	
.SECONDARY:

-include $(objects:.o=.d)

