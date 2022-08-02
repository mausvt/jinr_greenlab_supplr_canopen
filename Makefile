BUILDDIR = build

.PHONY: build clean

build:
	mkdir -p $(BUILDDIR)
	cd $(BUILDDIR) && \
		cmake .. && \
		make

clean:
	rm -rf $(BUILDDIR)
