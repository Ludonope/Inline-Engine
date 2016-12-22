#include "TextureViewPack.hpp"


namespace inl::gxeng::nodes {


RenderTargetPack::RenderTargetPack(uint64_t width, uint32_t height, gxapi::eFormat format, GraphicsContext& context) {
	Texture2D tex = context.CreateRenderTarget2D(width, height, format, true);

	gxapi::RtvTexture2DArray rtvDesc;
	rtvDesc.activeArraySize = 1;
	rtvDesc.firstArrayElement = 0;
	rtvDesc.planeIndex = 0;
	rtvDesc.firstMipLevel = 0;

	rtv = context.CreateRtv(tex, rtvDesc);

	gxapi::SrvTexture2DArray srvDesc;
	srvDesc.activeArraySize = 1;
	srvDesc.firstArrayElement = 0;
	srvDesc.numMipLevels = 0;
	srvDesc.mipLevelClamping = 0;
	srvDesc.mostDetailedMip = 0;
	srvDesc.planeIndex = 0;

	srv = context.CreateSrv(tex, format, srvDesc);
}


DepthStencilPack::DepthStencilPack(uint64_t width, uint32_t height, gxapi::eFormat format, GraphicsContext & context) {
	Texture2D tex = context.CreateDepthStencil2D(width, height, format, true);

	gxapi::DsvTexture2DArray dsvDesc;
	dsvDesc.activeArraySize = 1;
	dsvDesc.firstArrayElement = 0;
	dsvDesc.firstMipLevel = 0;

	dsv = context.CreateDsv(tex, dsvDesc);

	gxapi::SrvTexture2DArray srvDesc;
	srvDesc.activeArraySize = 1;
	srvDesc.firstArrayElement = 0;
	srvDesc.numMipLevels = 0;
	srvDesc.mipLevelClamping = 0;
	srvDesc.mostDetailedMip = 0;
	srvDesc.planeIndex = 0;

	srv = context.CreateSrv(tex, format, srvDesc);
}


} // namespace inl::gxeng::nodes