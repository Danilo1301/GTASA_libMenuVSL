#pragma once

#include "pch.h"

#include "rw/rwcore.h"

class TextureManager {
public:
    static std::map<std::string, RwTexture*> m_LoadedTextures;

    static RwTexture* LoadTexture(std::string texturePath, std::string textureName);
};