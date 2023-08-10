.PHONY:
.SILENT: 

all: 
	mkdir -p ./bin/deps/

	$(MAKE) -s -C stdlib
	$(MAKE) -s -C _start
	$(MAKE) -s -C shell
	$(MAKE) -s -C blank

clean: 
	rm -rf ./bin