# Default build type
BUILD_TYPE ?= Debug
BUILD_DIR = build

.PHONY: all clean rebuild run

all: $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) .. && make -j$$(nproc)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

clean:
	@rm -rf $(BUILD_DIR)

rebuild: clean all

run: all
	@./$(BUILD_DIR)/tarius_ai

# Development helper targets
debug: BUILD_TYPE=Debug
debug: all

release: BUILD_TYPE=Release
release: all 