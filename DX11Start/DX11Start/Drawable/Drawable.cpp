#include "Drawable.h"
#include "../Macros/GraphicsThrowMacros.h"
#include "../Bindable/IndexBuffer.h"
#include <cassert>
#include <typeinfo>

using namespace Bind;

void Drawable::Draw( Graphics& gfx ) const noexcept(!_DEBUG)
{
	for( auto& b : binds )
	{
		b->Bind( gfx );
	}
	gfx.DrawIndexed( pIndexBuffer->GetCount() );
}

void Drawable::AddBind( std::shared_ptr<Bindable> bind ) noexcept(!_DEBUG)
{
	//assert( "*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer) );
	// special case for index buffer
	if( typeid(*bind) == typeid(IndexBuffer) )
	{	
		assert( "Binding multiple index buffers not allowed" && pIndexBuffer == nullptr );
		pIndexBuffer = &static_cast<IndexBuffer&>(*bind);
	}
	binds.push_back( std::move( bind ) );
}
	