SPECIAL_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
CUSTOM_GLTF_COLLISION := $(SPECIAL_DIR)/my_tools/gltf_collision_importer/gltf_collision

BUILD_DIR=build
T3D_INST=$(shell realpath ../..)


include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk


N64_CFLAGS += -std=gnu2x -Og -g
#N64_CXXFLAGS += -std=gu++20 -fno-exceptions -O2



PROJECT_NAME=Engine64

src = $(wildcard src/*.c) \
	  $(wildcard src/*.cpp) \
	  $(wildcard src/resources/*.c) \
	  $(wildcard src/resources/*.cpp)

assets_png = $(wildcard assets/*.png)
assets_gltf = $(wildcard assets/*.glb)
assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
			  $(addprefix filesystem/,$(notdir $(assets_gltf:%.glb=%.t3dm)))

all: $(PROJECT_NAME).z64

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -o filesystem $@

filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -o $(dir $@) $@

filesystem/%-col.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL-FOR-COLLISION] $@"
	$(T3D_GLTF_TO_3D) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -o $(dir $@) $@
	@echo "    [CUSTOM_COLLISION] $@"
	$(CUSTOM_GLTF_COLLISION) "$<" $@
	@echo "    [Custom Collision Finished] $@"
	$(N64_BINDIR)/mkasset -c 2 -o $(dir $@) $@

$(BUILD_DIR)/$(PROJECT_NAME).dfs: $(assets_conv)
$(BUILD_DIR)/$(PROJECT_NAME).elf: $(src:%.c=$(BUILD_DIR)/%.o)
$(BUILD_DIR)/$(PROJECT_NAME).elf: $(src:%.cpp=$(BUILD_DIR)/%.o)

$(PROJECT_NAME).z64: N64_ROM_TITLE="Tiny3D - Obj"
$(PROJECT_NAME).z64: $(BUILD_DIR)/$(PROJECT_NAME).dfs

clean:
	rm -rf $(BUILD_DIR) *.z64
	rm -rf filesystem

build_lib:
	rm -rf $(BUILD_DIR) *.z64
	make -C $(T3D_INST)
	make all

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
