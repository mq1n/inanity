#ifndef ___INANITY_INANITY_DX_HPP___
#define ___INANITY_INANITY_DX_HPP___

/* Подсистема DX.
 */

#include "Object.hpp"

// Если это не Windows, то капец.
#ifndef ___INANITY_WINDOWS
#error DirectX works only on Windows.
#endif

#include "graphics/D3D10BlobFile.hpp"
#include "graphics/DxAttributeLayout.hpp"
#include "graphics/DxBlendState.hpp"
#include "graphics/DxContext.hpp"
#include "graphics/DxDepthStencilBuffer.hpp"
#include "graphics/DxDevice.hpp"
#include "graphics/DxIndexBuffer.hpp"
#include "graphics/DxPixelShader.hpp"
#include "graphics/DxPresenter.hpp"
#include "graphics/DxRenderBuffer.hpp"
#include "graphics/DxSamplerState.hpp"
#include "graphics/DxShaderCompiler.hpp"
#include "graphics/DxSystem.hpp"
#include "graphics/DxTexture.hpp"
#include "graphics/DxUniformBuffer.hpp"
#include "graphics/DxVertexBuffer.hpp"
#include "graphics/DxVertexShader.hpp"
#include "graphics/HlslSource.hpp"

#endif
