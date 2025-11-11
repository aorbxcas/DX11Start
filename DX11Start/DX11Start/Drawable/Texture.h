#pragma once
#include "../Bindable/Bindable.h"
class Surface;

namespace Bind
{
    class Texture : public Bindable
    {
    public:
        Texture( Graphics& gfx,const Surface& s ,unsigned int slot = 0);
        void Bind( Graphics& gfx ) noexcept override;
    protected:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
    private:
        unsigned int slot;
    };   
}

