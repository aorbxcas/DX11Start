#pragma once
#include "../Graphics.h"
#include <DirectXMath.h>

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}
class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable( const Drawable& ) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw( Graphics& gfx ) const noexcept(!_DEBUG);
	virtual void Update( float dt ) noexcept{};
	void AddBind( std::unique_ptr<Bind::Bindable> bind ) noexcept(!_DEBUG);
	void AddIndexBuffer( std::unique_ptr<class Bind::IndexBuffer> ibuf ) noexcept(!_DEBUG);
	virtual ~Drawable() = default;
private:
	virtual const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept = 0;
	const Bind::IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bind::Bindable>> binds;
};