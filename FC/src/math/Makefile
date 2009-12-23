sources := $(shell find . -name "*.c" )
headers := $(shell find . -name "*.h" )
objects := $(addprefix build/, $(patsubst src/%,%, $(sources:.c=.o)))

cflags := -std=gnu99 -g -Wall -Werror -I..
ldflags := -lm

cc := gcc
ld := gcc

mkdirs = @mkdir -p $(dir $@)

all: testmath

testmath: $(objects)
	$(mkdirs)
	$(ld) $(ldflags) -o testmath $(objects)

build/%.o: %.c $(headers)
	$(mkdirs)
	$(cc) $(cflags) -c $< -o $@
	
.PHONY: clean
clean:
	rm -rf build

