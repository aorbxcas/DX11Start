#pragma once
#include "Bindable.h"
#include "../Macros/GraphicsThrowMacros.h"
#include "../VertexLayout.h"
#include "BindableCodex.h"
namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		VertexBuffer( Graphics& gfx,const myVertex::VertexBuffer& vbuf )
		:
		VertexBuffer( gfx,"?",vbuf )
		{}
		
		VertexBuffer( Graphics& gfx,const std::string& tag,const myVertex::VertexBuffer& vbuf )
			:
			stride( (UINT)vbuf.GetLayout().Size() ),
			tag( tag )
		{
			INFOMAN( gfx );

			D3D11_BUFFER_DESC bd = {};
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = UINT( vbuf.SizeBytes() );
			bd.StructureByteStride = stride;
			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = vbuf.GetData();
			GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &bd,&sd,&pVertexBuffer ) );
		}
		void Bind( Graphics& gfx ) noexcept override;
		static std::shared_ptr<Bindable> Resolve( Graphics& gfx,const std::string& tag,
		const myVertex::VertexBuffer& vbuf );
		template<typename...Ignore>
		static std::string GenerateUID( const std::string& tag,Ignore&&...ignore )
		{
			return GenerateUID_( tag );
		}
		std::string GetUID() const noexcept override;
	protected:
		UINT stride;
		std::string tag;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	private:
		static std::string GenerateUID_( const std::string& tag );
	};

}
