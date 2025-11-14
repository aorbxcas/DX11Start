#include "VertexBuffer.h"

namespace Bind
{
	void VertexBuffer::Bind( Graphics& gfx ) noexcept
	{
		const UINT offset = 0u;
		GetContext( gfx )->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	}
	std::shared_ptr<Bindable> VertexBuffer::Resolve( Graphics& gfx,const std::string& tag,
	const myVertex::VertexBuffer& vbuf )
	{
		return Codex::Resolve<VertexBuffer>( gfx,tag,vbuf );
	}
	std::string VertexBuffer::GenerateUID_( const std::string& tag )
	{
		using namespace std::string_literals;
		return typeid(VertexBuffer).name() + "#"s + tag;
	}
	std::string VertexBuffer::GetUID() const noexcept
	{
		return GenerateUID( tag );
	}
}
