CXX := gcc
CXX_FLAGS := -pthread

bin/main: src/main.c
	@mkdir -p bin
	$(CXX) $(CXX_FLAGS) $< -o $@

clean:
	rm -rf bin

.PHONY: clean
