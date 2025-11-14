#pragma once
#include "Bindable.h"
#include "../VertexLayout.h"

namespace Bind
{
	class InputLayout : public Bindable
	{
	public:
		InputLayout( Graphics& gfx,
			myVertex::VertexLayout layout,
			ID3DBlob* pVertexShaderBytecode );
		void Bind( Graphics& gfx ) noexcept override;
		static std::shared_ptr<Bindable> Resolve( Graphics& gfx,
		const myVertex::VertexLayout& layout,ID3DBlob* pVertexShaderBytecode );
		static std::string GenerateUID( const myVertex::VertexLayout& layout,ID3DBlob* pVertexShaderBytecode = nullptr );
		std::string GetUID() const noexcept override;
	protected:
		myVertex::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}
