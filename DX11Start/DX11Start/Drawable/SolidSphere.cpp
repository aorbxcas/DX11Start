#include "SolidSphere.h"
#include "../Bindable/BindableBase.h"
#include "../Macros/GraphicsThrowMacros.h"
#include "Sphere.h"


SolidSphere::SolidSphere( Graphics& gfx,float radius )
{
    using namespace  Bind;
    namespace dx = DirectX;
    auto model = Sphere::Make();
    model.Transform( dx::XMMatrixScaling( radius,radius,radius ) );
    const auto geometryTag = "$sphere." + std::to_string( radius );
    AddBind( VertexBuffer::Resolve( gfx,geometryTag,model.vertices ) );
    AddBind( IndexBuffer::Resolve( gfx,geometryTag,model.indices ) );

    auto pvs = VertexShader::Resolve( gfx,"SolidVS.cso" );
    //auto pvsbc = pvs->GetBytecode();
    auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();;
    AddBind( std::move( pvs ) );

    //AddBind( std::make_shared<PixelShader>( gfx,"SolidPS.cso" ) );
    AddBind( PixelShader::Resolve( gfx,"SolidPS.cso" ) );

    struct PSColorConstant
    {
        dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
        float padding;
    } colorConst;
    //AddBind( std::make_shared<PixelConstantBuffer<PSColorConstant>>( gfx,colorConst ) );
    AddBind( PixelConstantBuffer<PSColorConstant>::Resolve( gfx,colorConst ) );
    
    AddBind( InputLayout::Resolve( gfx,model.vertices.GetLayout(),pvsbc ) );

    AddBind( Topology::Resolve( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
    // 不使用静态绑定、每个对象单独创建顶点缓存
    AddBind( std::make_shared<TransformCbuf>( gfx,*this ) );
}


void SolidSphere::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
    this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixTranslation( pos.x,pos.y,pos.z );
}
