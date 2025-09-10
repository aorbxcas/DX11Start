#pragma once
#include "Graphics.h"
#include "Drawable/SolidSphere.h"
#include "Bindable/ConstantBuffers.h"

class PointLight
{
public:
    PointLight( Graphics& gfx,float radius = 0.5f );
    void SpawnControlWindow() noexcept;
    void Reset() noexcept;
    void Draw( Graphics& gfx ) const noexcept(!_DEBUG);
    void Bind( Graphics& gfx ) const noexcept;
private:
    struct PointLightCBuf
    {
        DirectX::XMFLOAT3 pos;
        float padding;
    };
private:
    DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };;
    mutable SolidSphere mesh;
    mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};
