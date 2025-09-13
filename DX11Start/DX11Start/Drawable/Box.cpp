#include "Box.h"
#include "../Bindable/BindableBase.h"
#include "../Macros/GraphicsThrowMacros.h"
#include "Cube.h"
#include "Sphere.h"

Box::Box( Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 material
	)
	:
	TestObject<Box>(gfx,rng,adist,ddist,odist,rdist)
{
	namespace dx = DirectX;
	if( !IsStaticInitialized() )
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};
		// const std::vector<Vertex> vertices =
		// {
		// 	{ -1.0f,-1.0f,-1.0f },
		// 	{ 1.0f,-1.0f,-1.0f },
		// 	{ -1.0f,1.0f,-1.0f },
		// 	{ 1.0f,1.0f,-1.0f },
		// 	{ -1.0f,-1.0f,1.0f },
		// 	{ 1.0f,-1.0f,1.0f },
		// 	{ -1.0f,1.0f,1.0f },
		// 	{ 1.0f,1.0f,1.0f },
		// };
		// AddStaticBind( std::make_unique<VertexBuffer>( gfx,vertices )	);
		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();
		
		model.Transform( dx::XMMatrixScaling( 1.0f,1.0f,1.2f ) );
		AddStaticBind( std::make_unique<VertexBuffer>( gfx,model.vertices ) );
		__drv_inTry
		auto pvs = std::make_unique<VertexShader>( gfx,L"PhongVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"PhongPS.cso" ) );
		// const std::vector<unsigned short> indices =
		// {
		// 	0,2,1, 2,3,1,
		// 	1,3,5, 3,7,5,
		// 	2,6,3, 3,6,7,
		// 	4,5,7, 4,7,6,
		// 	0,4,2, 2,4,6,
		// 	0,1,4, 1,5,4
		// };
		// AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,indices ) );
		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,model.indices ) );

		// struct PixelShaderConstants
		// {
		// 	struct
		// 	{
		// 		float r;
		// 		float g;
		// 		float b;
		// 		float a;
		// 	} face_colors[8];
		// };
		// const PixelShaderConstants cb2 =
		// {
		// 	{
		// 		{ 1.0f,1.0f,1.0f },
		// 		{ 1.0f,0.0f,0.0f },
		// 		{ 0.0f,1.0f,0.0f },
		// 		{ 1.0f,1.0f,0.0f },
		// 		{ 0.0f,0.0f,1.0f },
		// 		{ 1.0f,0.0f,1.0f },
		// 		{ 0.0f,1.0f,1.0f },
		// 		{ 0.0f,0.0f,0.0f },
		// 	}
		// };

		// struct PSLightConstants
		// {
		// 	dx::XMVECTOR pos;
		// };
		// AddStaticBind( std::make_unique<PixelConstantBuffer<PSLightConstants>>( gfx ) );
		
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}else
	{
		SetIndexFromStatic();
	}
	AddBind( std::make_unique<TransformCbuf>( gfx,*this ) );
	// 材质，多次加载
	struct PSMaterialConstant
	{
		alignas(16) dx::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[2];
	} colorConst;
	colorConst.color = material;
	AddBind( std::make_unique<PixelConstantBuffer<PSMaterialConstant>>( gfx,colorConst,1u ) );
	
	// model deformation transform (per instance, not stored as bind)
	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling( 1.0f,1.0f,bdist( rng ) )
	);
}


DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	// return DirectX::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
	// 	DirectX::XMMatrixTranslation( r,0.0f,0.0f ) *
	// 	DirectX::XMMatrixRotationRollPitchYaw( theta,phi,chi ) *
	// 	DirectX::XMMatrixTranslation( 0.0f,0.0f,20.0f );

	namespace dx = DirectX;
	return dx::XMLoadFloat3x3( &mt ) * TestObject::GetTransformXM();
}
