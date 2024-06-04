PRESET?=x64-windows
BUILD_TYPE?=Release

BUILD_PATH=$(shell cmake --preset $(PRESET) -N | grep BUILD_DIR | sed 's/.*BUILD_DIR="\(.*\)"/\1/')
INSTALL_PREFIX?=install

.PHONY: build  prebuild install clean

all: build

# cmake 処理実行
# CMAKEOPT で引数定義追加
prebuild:
	cmake --preset $(PRESET) ${CMAKEOPT}

build:
	cmake --build $(BUILD_PATH) --config $(BUILD_TYPE)

clean:
	cmake --build $(BUILD_PATH) --config $(BUILD_TYPE) --target clean

install:
	cmake --install $(BUILD_PATH) --config $(BUILD_TYPE) --prefix $(INSTALL_PREFIX)
