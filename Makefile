VERSION ?= us


# Build tools
HOSTCC = cc
CPP = $(HOSTCC) -E
PYTHON = python3

TOOLCHAIN ?= arm-none-eabi-
AS = $(TOOLCHAIN)as
LD = $(TOOLCHAIN)ld
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump

DIFF = diff -u
MD5SUM = md5sum

TOOL_DIR = tools
GBAFIX = $(TOOL_DIR)/gbafix/gbafix
PROGRESS = $(TOOL_DIR)/gen_decomp_progress.py

VENV = .venv
REQUIREMENTS = $(TOOL_DIR)/requirements.txt
EXTRACTOR = $(TOOL_DIR)/extract_assets.py

AGBCC_DIR = $(TOOL_DIR)/agbcc
CC = $(AGBCC_DIR)/agbcc


# Files
GAME_NAME = warioland4
FILENAME = $(GAME_NAME)_$(VERSION)
BASEROM = baserom_$(VERSION).gba
DATABASE = asset_database.yaml
MD5FILE = $(FILENAME).md5

BUILD = build/$(VERSION)
ASM = asm
SRC = src
INCLUDE = include
BUILT_ASM = $(BUILD)/asm
OBJ = $(BUILD)/obj

SRCS_C = $(shell find $(SRC) -type f -name '*.c')
SRCS_ASM = $(shell find $(ASM) -type f -name '*.s')
BUILT_ASMS = $(SRCS_C:$(SRC)/%.c=$(BUILT_ASM)/%.s)
OBJS = $(SRCS_ASM:$(ASM)/%.s=$(OBJ)/%.o) $(SRCS_C:$(SRC)/%.c=$(OBJ)/%.o)
.PRECIOUS: $(BUILT_ASMS)

TARGET = $(BUILD)/$(FILENAME).gba
ELF = $(TARGET:.gba=.elf)
MAP = $(TARGET:.gba=.map)
DUMPS = $(BASEROM).dump $(TARGET).dump


# ROM header
ifeq ($(VERSION), us)
	GAME_TITLE = WARIOLANDE
	GAME_CODE = AWAE
	CPPFLAGS += -DVERSION_US
	ASFLAGS += --defsym VERSION_US=1
else ifeq ($(VERSION), jp)
	GAME_TITLE = WARIOLAND
	GAME_CODE = AWAJ
	CPPFLAGS += -DVERSION_JP
	ASFLAGS += --defsym VERSION_JP=1
else
$(error Unupported version $(VERSION))
endif

MAKER_CODE = 01
GAME_REVISION = 00


# Flags
ASFLAGS += -I$(ASM) -mcpu=arm7tdmi
CFLAGS = -O2 -mthumb-interwork -fhex-asm -fprologue-bugfix -Wall
CPPFLAGS += -I$(INCLUDE) -nostdinc
LDFLAGS += -n


# Quiet/verbose output
ifeq ($(V), 1)
	Q =
	MSG = @:
else
	Q = @
	MSG = @echo " "
endif

USE_VENV = . $(VENV)/bin/activate &&


.PHONY: all check progress dump diff extract tools clean help

all: check

check: $(TARGET) $(PROGRESS)
	$(MSG) MD5SUM $(MD5FILE)
	$Q$(MD5SUM) -c $(MD5FILE)
	$Q$(PYTHON) $(PROGRESS) --root . >/dev/null

progress: $(ELF) $(PROGRESS)
	$Q$(PYTHON) $(PROGRESS) --root . >/dev/null

dump: $(DUMPS)

diff: $(DUMPS)
	$(MSG) DIFF $^
	$Q$(DIFF) $^

extract: $(DATABASE) $(EXTRACTOR)
	$(MSG) PYTHON $(EXTRACTOR)
	$Q$(USE_VENV) $(PYTHON) $(EXTRACTOR) -v $(VERSION)

tools: $(CC) $(GBAFIX) $(VENV) $(REQUIREMENTS)
	$(MSG) PIP $(REQUIREMENTS)
	$Q$(USE_VENV) $(PYTHON) -m pip install -r $(REQUIREMENTS) --quiet

clean:
	$(MSG) RM BUILD
	$Qrm -rf build/*
	$(MSG) RM DUMPS
	$Qrm -f baserom_*.gba.dump
ifeq ($(DATA),1)
	$(MSG) RM DATA
	$Qrm -rf data/*
	$Qrm -rf $(INCLUDE)/data
endif
ifeq ($(TOOLS),1)
	$(MSG) RM $(GBAFIX)
	$Qrm -rf $(GBAFIX)
	$(MSG) CLEAN $(CC)
	$Qcd $(AGBCC_DIR) && git clean -xdf .
	$(MSG) RM $(VENV)
	$Qrm -rf $(VENV)
endif

help:
	@echo 'Targets:'
	@echo '  all: build and checksum the ROM'
	@echo '  check: same as `all`, then update progress docs and README'
	@echo '  progress: update docs/progress-treemap.svg, docs/progress.html, and README.md'
	@echo '  dump: dump the ROMs'
	@echo '  diff: dump and compare the ROMs'
	@echo '  tools: set up build tools'
	@echo '  extract: extract assets from ROM'
	@echo '  clean: remove build files'
	@echo '    DATA=1: remove extracted data'
	@echo '    TOOLS=1: remove build files for tools, including agbcc'
	@echo '  help: show this message'
	@echo ''
	@echo 'Flags:'
	@echo '  VERSION=<version>: select the version of the ROM to build, supported versions:'
	@echo '    us'
	@echo '    jp'


%.dump: %
	$(MSG) OBJDUMP $<
	$Q$(OBJDUMP) -D -bbinary -marm7tdmi -Mforce-thumb $< > $@

$(TARGET): $(ELF) $(GBAFIX)
	$(MSG) OBJCOPY $<
	$Q$(OBJCOPY) -O binary --gap-fill 0xff --pad-to 0x08800000 $< $@
	$(MSG) GBAFIX $@
	$Q$(GBAFIX) $@ -t$(GAME_TITLE) -c$(GAME_CODE) -m$(MAKER_CODE) -r$(GAME_REVISION)

$(ELF): $(OBJS) linker.ld
	$(MSG) LD linker.ld
	$Q$(LD) $(LDFLAGS) -T linker.ld -Map=$(MAP) -L$(BUILD) $(AGBCC_DIR)/libgcc.a $(AGBCC_DIR)/libc.a -o $@

$(OBJ)/%.o: $(ASM)/%.s
	$(MSG) AS $<
	$Qmkdir -p $(shell dirname $@)
	$Q$(AS) $(ASFLAGS) $< -o $@

$(OBJ)/%.o: $(BUILT_ASM)/%.s
	$(MSG) AS $<
	$Qmkdir -p $(shell dirname $@)
	$Q$(AS) $(ASFLAGS) $< -o $@

$(BUILT_ASM)/%.s: $(SRC)/%.c
	$(MSG) CC $<
	$Qmkdir -p $(shell dirname $@)
	$Q$(CPP) $(CPPFLAGS) $< | $(CC) $(CFLAGS) -o $@
	@echo '    .align 2, 0' >> $@

# objdiff base objects: functions written in asm are renamed so they count as unmatched
$(BUILD)/objdiff/%.o: $(OBJ)/%.o $(SRC)/%.c $(TOOL_DIR)/asm_functions.py
	$(MSG) OBJDIFF $@
	$Qmkdir -p $(shell dirname $@)
	$Q$(PYTHON) $(TOOL_DIR)/asm_functions.py -v $(VERSION) --redefine-syms $(SRC)/$*.c > $@.syms
	$Q$(OBJCOPY) --redefine-syms=$@.syms $< $@

$(BUILT_ASM)/m4a.s: CC = $(TOOL_DIR)/agbcc/old_agbcc
$(BUILT_ASM)/m4a.s: CFLAGS = -O2 -mthumb-interwork -fhex-asm -Wall

$(BUILT_ASM)/sram.s: CFLAGS = -O1 -mthumb-interwork -fhex-asm -Wall

$(TOOL_DIR)/%: $(TOOL_DIR)/%.c
	$(MSG) HOSTCC $<
	$Q$(HOSTCC) $< $(HOSTCFLAGS) $(HOSTCPPFLAGS) -o $@

# Compilation doesn't work if -j is set so force default flags
$(CC):
	$(MSG) MAKE $@
	$Qcd $(shell dirname $@) && MAKEFLAGS="" ./build.sh

$(VENV):
	$(MSG) VENV $(VENV)
	$Q$(PYTHON) -m venv $(VENV)
