/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once

#include "structs.h"

T3DMData parseGLTF(const char* gltfPath, float modelScale);

std::vector<ModelCustom> parseGLTFCustom(const char *gltfPath, const char *t3dmPath, float modelScale);