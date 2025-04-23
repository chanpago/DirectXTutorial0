// TextureCache.h
#pragma once
#include <string>
#include <unordered_map>
#include "textureclass.h"

class TextureCache {
public:
    static TextureClass* GetTexture(const std::string& filename, ID3D11Device* device, ID3D11DeviceContext* context);

    static void ShutdownAll(); // 모든 텍스처 정리

private:
    static std::unordered_map<std::string, TextureClass*> m_cache;
};
