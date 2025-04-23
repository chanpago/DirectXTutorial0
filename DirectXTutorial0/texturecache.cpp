// TextureCache.cpp
#include "texturecache.h"

std::unordered_map<std::string, TextureClass*> TextureCache::m_cache;

TextureClass* TextureCache::GetTexture(const std::string& filename, ID3D11Device* device, ID3D11DeviceContext* context) {
    if (filename.empty()) {
        OutputDebugStringA("Error: TextureCache::GetTexture() received empty filename\n");
        return nullptr;
    }

    auto it = m_cache.find(filename);
    if (it != m_cache.end()) {
        return it->second;
    }

    TextureClass* texture = new TextureClass();
    if (!texture->Initialize(device, context, const_cast<char*>(filename.c_str()))) {
        delete texture;
        return nullptr;
    }

    m_cache[filename] = texture;
    return texture;
}


void TextureCache::ShutdownAll() {
    for (auto& pair : m_cache) {
        if (pair.second) {
            pair.second->Shutdown();
            delete pair.second;
        }
    }
    m_cache.clear();
}
