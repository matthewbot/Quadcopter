all: stmos FC

.PHONY: stmos
stmos:
	(cd stmos; make)

.PHONY: FC
FC:
	(cd FC; make)
	
.PHONY: clean
clean:
	(cd stmos; make clean;)
	(cd FC; make clean;)
	
