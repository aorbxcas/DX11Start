#include "Graphics.h"
#include <d3dcompiler.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib");
namespace wrl = Microsoft::WRL;

// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

Graphics::Graphics( HWND hWnd )
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    UINT swapCreateFlags = 0u;
#ifndef NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    HRESULT hr;
    
    GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
    nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    swapCreateFlags,
    nullptr,
    0,
    D3D11_SDK_VERSION,
    &sd,
    &pSwap,
    &pDevice,
    nullptr,
    &pContext
    ));
    
    wrl::ComPtr<ID3D11Resource> pBackBuffer;
    GFX_THROW_INFO(pSwap->GetBuffer( 0,__uuidof(ID3D11Resource),&pBackBuffer ));
    GFX_THROW_INFO(pDevice->CreateRenderTargetView( pBackBuffer.Get(),nullptr,&pTarget ));
}

Graphics::~Graphics()
{
}

void Graphics::EndFrame()
{
    HRESULT hr;
#ifndef NDEBUG
    infoManager.Set();
#endif
    if( FAILED( hr = pSwap->Present( 1u,0u ) ) )
    {
        if( hr == DXGI_ERROR_DEVICE_REMOVED )
        {
            throw GFX_DEVICE_REMOVED_EXCEPT( pDevice->GetDeviceRemovedReason() );
        }
        else
        {
            throw GFX_EXCEPT( hr );
        }
    }
}
void Graphics::ClearBuffer( float red,float green,float blue ) noexcept
{
    const float color[] = { red,green,blue,1.0f };
    pContext->ClearRenderTargetView( pTarget.Get(),color );
}

void Graphics::DrawTriangle(float angle)
{
    namespace wrl = Microsoft::WRL;
    HRESULT hr;

    // ==================== 顶点数据定义 ====================
    struct Vertex
    {
        struct
        {
            float x;
            float y;
        }pos;
        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        }color;

    };

    // 创建顶点数据（1个2D三角形）
    const Vertex vertices[] =
    {
        { 0.0f, 0.5f ,255,0,0,255},    // 顶点0: 三角形顶部
        { 0.5f, -0.5f,0,255,0,255 },   // 顶点1: 右下角
        { -0.5f, -0.5f ,0,0,255,255}   // 顶点2: 左下角
    };

    // ==================== 创建顶点缓冲区 ====================
    wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;        // 缓冲区用途：顶点缓冲区
    bd.Usage = D3D11_USAGE_DEFAULT;                 // 使用方式：GPU读取，不频繁更新
    bd.CPUAccessFlags = 0u;                         // CPU访问权限：无
    bd.MiscFlags = 0u;                              // 杂项标志：无
    bd.ByteWidth = sizeof(vertices);                // 缓冲区大小
    bd.StructureByteStride = sizeof(Vertex);        // 每个顶点的大小
    
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;                          // 指向顶点数据
    
    GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

    // ==================== 加载并创建着色器 ====================
    wrl::ComPtr<ID3DBlob> pBlob;
    // 加载并创建像素着色器
    wrl::ComPtr<ID3D11PixelShader> pPixelShader;
    GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
    GFX_THROW_INFO(pDevice->CreatePixelShader(
        pBlob->GetBufferPointer(),     // 着色器字节码指针
        pBlob->GetBufferSize(),        // 着色器字节码大小
        nullptr,                       // 类链接接口（不使用）
        &pPixelShader                  // 输出：创建的像素着色器
    ));
    pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);   // 设置像素着色器
    // 加载并创建顶点着色器
    wrl::ComPtr<ID3D11VertexShader> pVertexShader;
    GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
    GFX_THROW_INFO(pDevice->CreateVertexShader(
        pBlob->GetBufferPointer(),     // 着色器字节码指针
        pBlob->GetBufferSize(),        // 着色器字节码大小
        nullptr,                       // 类链接接口（不使用）
        &pVertexShader                 // 输出：创建的顶点着色器
    ));
    pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);  // 设置顶点着色器

    // ==================== 创建输入布局 ====================
    wrl::ComPtr<ID3D11InputLayout> pInputLayout;
    // 定义顶点输入元素描述
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { 
            "Position",                    // 语义名称：与HLSL着色器匹配
            0,                             // 语义索引
            DXGI_FORMAT_R32G32_FLOAT,      // 数据格式：两个32位浮点数
            0,                             // 输入槽：使用槽0
            0,                             // 字节偏移：第一个元素偏移为0
            D3D11_INPUT_PER_VERTEX_DATA,   // 输入分类：每个顶点数据
            0                              // 实例数据步进率：0（不使用实例化）
        },
        { 
            "Color",                    // 语义名称：与HLSL着色器匹配
            0,                             // 语义索引
            DXGI_FORMAT_R8G8B8A8_UNORM,      // 数据格式：两个32位浮点数
            0,                             // 输入槽：使用槽0
            8u,                             // 字节偏移：第一个元素偏移为0
            D3D11_INPUT_PER_VERTEX_DATA,   // 输入分类：每个顶点数据
            0                              // 实例数据步进率：0（不使用实例化）
        }
    };
    
    // 基于顶点着色器创建输入布局
    GFX_THROW_INFO(pDevice->CreateInputLayout(
        ied,                           // 输入元素描述数组
        (UINT)std::size(ied),          // 数组元素数量
        pBlob->GetBufferPointer(),     // 顶点着色器字节码（用于验证兼容性）
        pBlob->GetBufferSize(),        // 字节码大小
        &pInputLayout                  // 输出：创建的输入布局
    ));

    // ==================== 设置渲染管线状态 ====================
    
    // 设置输入布局
    pContext->IASetInputLayout(pInputLayout.Get());
    
    // 设置顶点缓冲区
    const UINT stride = sizeof(Vertex);    // 每个顶点的字节大小
    const UINT offset = 0u;                // 缓冲区起始偏移
    pContext->IASetVertexBuffers(
        0u,                                // 起始输入槽
        1u,                                // 缓冲区数量
        pVertexBuffer.GetAddressOf(),      // 顶点缓冲区数组
        &stride,                           // 步长数组
        &offset                            // 偏移数组
    );
    
    // 设置图元拓扑（绘制三角形列表）
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 设置着色器常量缓存(当前为z轴旋转矩阵)
    struct ConstantBuffer
    {
        struct
        {
            float element[4][4];
        } transformation;
    };
    const ConstantBuffer cb =
    {
        {
            (3.0f/4.0f)*std::cos( angle ),	std::sin( angle ),	0.0f,	0.0f,
            (3.0f/4.0f)*-std::sin( angle ),	std::cos( angle ),	0.0f,	0.0f,
            0.0f,				0.0f,				1.0f,	0.0f,
            0.0f,				0.0f,				0.0f,	1.0f,
        }
    };
    wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof( cb );
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;
    GFX_THROW_INFO( pDevice->CreateBuffer( &cbd,&csd,&pConstantBuffer ) );
    pContext->VSSetConstantBuffers(
        0u,
        1u,
        pConstantBuffer.GetAddressOf()
        );
    
    // 设置视口
    D3D11_VIEWPORT vp;
    vp.Width = 800;                        // 视口宽度
    vp.Height = 600;                       // 视口高度
    vp.MinDepth = 0;                       // 最小深度值
    vp.MaxDepth = 1;                       // 最大深度值
    vp.TopLeftX = 0;                       // 左上角X坐标
    vp.TopLeftY = 0;                       // 左上角Y坐标
    pContext->RSSetViewports(1u, &vp);     // 设置视口
    
    // 设置渲染目标
    pContext->OMSetRenderTargets(
        1u,                                // 渲染目标数量
        pTarget.GetAddressOf(),            // 渲染目标视图数组
        nullptr                            // 深度模板视图（不使用）
    );

    // ==================== 执行绘制命令 ====================
    GFX_THROW_INFO_ONLY(pContext->Draw(
        (UINT)std::size(vertices),         // 顶点数量
        0u                                 // 起始顶点索引
    ));
}


// Graphics exception stuff
Graphics::HrException::HrException( int line,const char * file,HRESULT hr,std::vector<std::string> infoMsgs ) noexcept
    :
    Exception( line,file ),
    hr( hr )
{
    // join all info messages with newlines into single string
    for( const auto& m : infoMsgs )
    {
        info += m;
        info.push_back( '\n' );
    }
    // remove final newline if exists
    if( !info.empty() )
    {
        info.pop_back();
    }
}

const char* Graphics::HrException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
        << std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
        << "[Error String] " << GetErrorString() << std::endl
        << "[Description] " << GetErrorDescription() << std::endl;
    if( !info.empty() )
    {
        oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    }
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
    return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
    return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
    return DXGetErrorString( hr );
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
    char buf[512];
    DXGetErrorDescription( hr,buf,sizeof( buf ) );
    return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
    return info;
}


const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
    return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException( int line,const char * file,std::vector<std::string> infoMsgs ) noexcept
    :
    Exception( line,file )
{
    // join all info messages with newlines into single string
    for( const auto& m : infoMsgs )
    {
        info += m;
        info.push_back( '\n' );
    }
    // remove final newline if exists
    if( !info.empty() )
    {
        info.pop_back();
    }
}


const char* Graphics::InfoException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
    return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
    return info;
}