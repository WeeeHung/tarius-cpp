# Default build type
BUILD_TYPE ?= Debug
BUILD_DIR = build

.PHONY: all clean rebuild run run-silent run-no-errors run-no-output run-quiet

all: $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) .. && make -j$$(nproc)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

clean:
	@rm -rf $(BUILD_DIR)

rebuild: clean all

run: all
	@./$(BUILD_DIR)/tarius_ai

# Run without showing logs
run-silent: all
	@./$(BUILD_DIR)/tarius_ai > /dev/null 2>&1

# To hide only stderr but keep stdout
run-no-errors: all
	@./$(BUILD_DIR)/tarius_ai 2> /dev/null

# To hide only stdout but keep stderr
run-no-output: all
	@./$(BUILD_DIR)/tarius_ai > /dev/null

# Run with minimal logging
run-no-logs: all
	@./$(BUILD_DIR)/tarius_ai_release

# Development helper targets
debug: BUILD_TYPE=Debug
debug: all

release: BUILD_TYPE=Release
release: all 