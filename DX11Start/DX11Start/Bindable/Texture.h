#pragma once
#include "../Bindable/Bindable.h"
class Surface;

namespace Bind
{
    class Texture : public Bindable
    {
    public:
        Texture( Graphics& gfx,const std::string& path,UINT slot = 0);
        void Bind( Graphics& gfx ) noexcept override;
        static std::shared_ptr<Bindable> Resolve( Graphics& gfx,const std::string& path,UINT slot = 0);
        static std::string GenerateUID( const std::string& path,UINT slot = 0);
        std::string GetUID() const noexcept override;
    protected:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
    private:
        std::string path;
        unsigned int slot;
    };   
}

