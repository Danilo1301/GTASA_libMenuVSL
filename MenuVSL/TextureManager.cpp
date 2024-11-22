#include "TextureManager.h"

#include "MenuVSL.h"

std::map<std::string, RwTexture*> TextureManager::m_LoadedTextures;

RwTexture* TextureManager::LoadTexture(std::string texturePath, std::string textureName)
{
    if(m_LoadedTextures.find(texturePath) != m_LoadedTextures.end())
    {
        return m_LoadedTextures[texturePath];
    }

    auto menuVSL = MenuVSL::Instance;

    auto rwTexture = (RwTexture*)menuVSL->LoadRwTextureFromFile(texturePath, textureName, true);

    m_LoadedTextures[texturePath] = rwTexture;

    return rwTexture;
}