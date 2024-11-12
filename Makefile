CC=gcc
CFLAGS=-Wall -Wextra -ggdb -pedantic
LDFLAGS=

OUT=obj
SRC=src
TARGET=$(notdir $(shell pwd))

$(TARGET): $(OUT)/cpc.o
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo -e "LD $@\e[32;1m PASS\e[0m"

$(OUT)/%.o: $(SRC)/%.c $(SRC)/cpu.h | $(OUT)
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo -e "CC $@\e[32;1m PASS\e[0m" 

$(OUT):
	@mkdir -p $@

.PHONY: clean
clean:
	@rm -rvf $(OUT) $(TARGET)
