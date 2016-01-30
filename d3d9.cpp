#include "dinput8.h"
#include "d3d9.h"
#include <vector>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

typedef LPDIRECT3D9(WINAPI *tDirect3DCreate9)(UINT SDKVersion);
tDirect3DCreate9 oDirect3DCreate9;
LPDIRECT3D9 WINAPI HDirect3DCreate9(UINT SDKVersion) { return new fIDirect3D9(oDirect3DCreate9(SDKVersion)); }

bool Hooks::D3D9()
{
	if (!config.GetBool("d3d9", "enabled", false))
	{
		logFile << "D3D9: disabled" << std::endl;
		return false;
	}

	HMODULE hMod = LoadLibrary(L"d3d9.dll");
	oDirect3DCreate9 = (tDirect3DCreate9)GetProcAddress(hMod, "Direct3DCreate9");
	CreateHook("D3D9", Direct3DCreate9, &HDirect3DCreate9, &oDirect3DCreate9);
	return true;
}

std::vector<d3d9CallbackEx> onCreateDevice, onLostDevice, onResetDevice;
std::vector<d3d9Callback> onEndScene;
void Hooks::D3D9Add(d3d9CallbackEx onCreate, d3d9CallbackEx onLost, d3d9CallbackEx onReset, d3d9Callback onEnd)
{
	onCreateDevice.push_back(onCreate);
	onLostDevice.push_back(onLost);
	onResetDevice.push_back(onReset);
	onEndScene.push_back(onEnd);
}
/*---------------------------------------------------------------------------
---------------------------------------------------------------------------
---------------------------------------------------------------------------*/
HRESULT fIDirect3D9::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	IDirect3DDevice9* pDirect3DDevice9;
	HRESULT hr = pD3D9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &pDirect3DDevice9);
	*ppReturnedDeviceInterface = new fIDirect3DDevice9(pDirect3DDevice9, this);
	for (auto &h : onCreateDevice)
		h(pDirect3DDevice9, pPresentationParameters);
	return hr;
}

HRESULT fIDirect3DDevice9::EndScene()
{
	for (auto &h : onEndScene)
		h(pD3DDevice);
	return pD3DDevice->EndScene();
}

HRESULT fIDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	for (auto &h : onLostDevice)
		h(pD3DDevice, pPresentationParameters);
	HRESULT hr = pD3DDevice->Reset(pPresentationParameters);
	for (auto &h : onResetDevice)
		h(pD3DDevice, pPresentationParameters);
	return hr;
}

HRESULT fIDirect3DDevice9::GetDirect3D(IDirect3D9 **ppD3D9)
{
	*ppD3D9 = pD3D9;
	return D3D_OK;
}
/*---------------------------------------------------------------------------
---------------------------------------------------------------------------
---------------------------------------------------------------------------*/
ULONG fIDirect3D9::AddRef()
{
	return pD3D9->AddRef();
}

HRESULT fIDirect3D9::QueryInterface(const IID& riid, void **ppvObj)
{
	return pD3D9->QueryInterface(riid, ppvObj);
}

ULONG fIDirect3D9::Release()
{
	return pD3D9->Release();
}

HRESULT fIDirect3D9::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
	return pD3D9->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

UINT fIDirect3D9::GetAdapterCount()
{
	return pD3D9->GetAdapterCount();
}

HRESULT fIDirect3D9::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE *pMode)
{
	return pD3D9->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT fIDirect3D9::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier)
{
	return pD3D9->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT fIDirect3D9::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{
	return pD3D9->GetAdapterModeCount(Adapter, Format);
}

HMONITOR fIDirect3D9::GetAdapterMonitor(UINT Adapter)
{
	return pD3D9->GetAdapterMonitor(Adapter);
}

HRESULT fIDirect3D9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9 *pCaps)
{
	return pD3D9->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HRESULT fIDirect3D9::RegisterSoftwareDevice(void *pInitializeFunction)
{
	return pD3D9->RegisterSoftwareDevice(pInitializeFunction);
}

HRESULT fIDirect3D9::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	return pD3D9->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT fIDirect3D9::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	return pD3D9->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT fIDirect3D9::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
	return pD3D9->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT fIDirect3D9::CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed)
{
	return pD3D9->CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, Windowed);
}

HRESULT fIDirect3D9::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
{
	return pD3D9->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}
/*---------------------------------------------------------------------------
  ---------------------------------------------------------------------------
  ---------------------------------------------------------------------------*/
HRESULT fIDirect3DDevice9::QueryInterface(const IID& riid, void** ppvObj)
{
	return pD3DDevice->QueryInterface(riid, ppvObj);
}

ULONG fIDirect3DDevice9::AddRef()
{
	return pD3DDevice->AddRef();
}

ULONG fIDirect3DDevice9::Release()
{
	return pD3DDevice->Release();
}

void fIDirect3DDevice9::SetCursorPosition(int X, int Y, DWORD Flags)
{
	pD3DDevice->SetCursorPosition(X, Y, Flags);
}

HRESULT fIDirect3DDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9 *pCursorBitmap)
{
	return pD3DDevice->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

BOOL fIDirect3DDevice9::ShowCursor(BOOL bShow)
{
	return pD3DDevice->ShowCursor(bShow);
}

HRESULT fIDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain)
{
	return pD3DDevice->CreateAdditionalSwapChain(pPresentationParameters, ppSwapChain);
}

HRESULT fIDirect3DDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
	return pD3DDevice->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

HRESULT fIDirect3DDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return pD3DDevice->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT fIDirect3DDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
	return pD3DDevice->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT fIDirect3DDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return pD3DDevice->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT fIDirect3DDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	return pD3DDevice->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT fIDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	return pD3DDevice->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT fIDirect3DDevice9::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
	return pD3DDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

HRESULT fIDirect3DDevice9::BeginStateBlock()
{
	return pD3DDevice->BeginStateBlock();
}

HRESULT fIDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
	return pD3DDevice->CreateStateBlock(Type, ppSB);
}

HRESULT fIDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
	return pD3DDevice->EndStateBlock(ppSB);
}

HRESULT fIDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus)
{
	return pD3DDevice->GetClipStatus(pClipStatus);
}

HRESULT fIDirect3DDevice9::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
	return pD3DDevice->GetDisplayMode(iSwapChain, pMode);
}

HRESULT fIDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue)
{
	return pD3DDevice->GetRenderState(State, pValue);
}

HRESULT fIDirect3DDevice9::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	return pD3DDevice->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

HRESULT fIDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX *pMatrix)
{
	return pD3DDevice->GetTransform(State, pMatrix);
}

HRESULT fIDirect3DDevice9::SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus)
{
	return pD3DDevice->SetClipStatus(pClipStatus);
}

HRESULT fIDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	return pD3DDevice->SetRenderState(State, Value);
}

HRESULT fIDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	return pD3DDevice->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT fIDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix)
{
	return pD3DDevice->SetTransform(State, pMatrix);
}

void fIDirect3DDevice9::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
	pD3DDevice->GetGammaRamp(iSwapChain, pRamp);
}

void fIDirect3DDevice9::SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp)
{
	pD3DDevice->SetGammaRamp(iSwapChain, Flags, pRamp);
}

HRESULT fIDirect3DDevice9::DeletePatch(UINT Handle)
{
	return pD3DDevice->DeletePatch(Handle);
}

HRESULT fIDirect3DDevice9::DrawRectPatch(UINT Handle, const float *pNumSegs, const D3DRECTPATCH_INFO *pRectPatchInfo)
{
	return pD3DDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT fIDirect3DDevice9::DrawTriPatch(UINT Handle, const float *pNumSegs, const D3DTRIPATCH_INFO *pTriPatchInfo)
{
	return pD3DDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT fIDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
	return pD3DDevice->GetIndices(ppIndexData);
}

HRESULT fIDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	return pD3DDevice->SetIndices(pIndexData);
}

UINT fIDirect3DDevice9::GetAvailableTextureMem()
{
	return pD3DDevice->GetAvailableTextureMem();
}

HRESULT fIDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	return pD3DDevice->GetCreationParameters(pParameters);
}

HRESULT fIDirect3DDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)
{
	return pD3DDevice->GetDeviceCaps(pCaps);
}

HRESULT fIDirect3DDevice9::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
	return pD3DDevice->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT fIDirect3DDevice9::GetLight(DWORD Index, D3DLIGHT9 *pLight)
{
	return pD3DDevice->GetLight(Index, pLight);
}

HRESULT fIDirect3DDevice9::GetLightEnable(DWORD Index, BOOL *pEnable)
{
	return pD3DDevice->GetLightEnable(Index, pEnable);
}

HRESULT fIDirect3DDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{
	return pD3DDevice->GetMaterial(pMaterial);
}

HRESULT fIDirect3DDevice9::LightEnable(DWORD LightIndex, BOOL bEnable)
{
	return pD3DDevice->LightEnable(LightIndex, bEnable);
}

HRESULT fIDirect3DDevice9::SetLight(DWORD Index, const D3DLIGHT9 *pLight)
{

	return pD3DDevice->SetLight(Index, pLight);
}

HRESULT fIDirect3DDevice9::SetMaterial(const D3DMATERIAL9 *pMaterial)
{
	return pD3DDevice->SetMaterial(pMaterial);
}

HRESULT fIDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix)
{
	return pD3DDevice->MultiplyTransform(State, pMatrix);
}

HRESULT fIDirect3DDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
	return pD3DDevice->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT fIDirect3DDevice9::TestCooperativeLevel()
{
	return pD3DDevice->TestCooperativeLevel();
}

HRESULT fIDirect3DDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber)
{
	return pD3DDevice->GetCurrentTexturePalette(pPaletteNumber);
}

HRESULT fIDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY *pEntries)
{
	return pD3DDevice->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT fIDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
	return pD3DDevice->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT fIDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY *pEntries)
{
	return pD3DDevice->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT fIDirect3DDevice9::CreatePixelShader(const DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
	return pD3DDevice->CreatePixelShader(pFunction, ppShader);
}

HRESULT fIDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
	return pD3DDevice->GetPixelShader(ppShader);
}

HRESULT fIDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9* pShader)
{
	return pD3DDevice->SetPixelShader(pShader);
}

HRESULT fIDirect3DDevice9::Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
	return pD3DDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT fIDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	return pD3DDevice->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT fIDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return pD3DDevice->DrawIndexedPrimitiveUP(PrimitiveType, MinIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT fIDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	return pD3DDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT fIDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return pD3DDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT fIDirect3DDevice9::BeginScene()
{
	return pD3DDevice->BeginScene();
}

HRESULT fIDirect3DDevice9::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride)
{
	return pD3DDevice->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride);
}

HRESULT fIDirect3DDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	return pD3DDevice->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

HRESULT fIDirect3DDevice9::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
	return pD3DDevice->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT fIDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
{
	return pD3DDevice->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT fIDirect3DDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9 **ppTexture)
{
	return pD3DDevice->GetTexture(Stage, ppTexture);
}

HRESULT fIDirect3DDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue)
{
	return pD3DDevice->GetTextureStageState(Stage, Type, pValue);
}

HRESULT fIDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9 *pTexture)
{
	return pD3DDevice->SetTexture(Stage, pTexture);
}

HRESULT fIDirect3DDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	return pD3DDevice->SetTextureStageState(Stage, Type, Value);
}

HRESULT fIDirect3DDevice9::UpdateTexture(IDirect3DBaseTexture9 *pSourceTexture, IDirect3DBaseTexture9 *pDestinationTexture)
{
	return pD3DDevice->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT fIDirect3DDevice9::ValidateDevice(DWORD *pNumPasses)
{
	return pD3DDevice->ValidateDevice(pNumPasses);
}

HRESULT fIDirect3DDevice9::GetClipPlane(DWORD Index, float *pPlane)
{
	return pD3DDevice->GetClipPlane(Index, pPlane);
}

HRESULT fIDirect3DDevice9::SetClipPlane(DWORD Index, const float *pPlane)
{
	return pD3DDevice->SetClipPlane(Index, pPlane);
}

HRESULT fIDirect3DDevice9::Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	return pD3DDevice->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT fIDirect3DDevice9::GetViewport(D3DVIEWPORT9 *pViewport)
{
	return pD3DDevice->GetViewport(pViewport);
}

HRESULT fIDirect3DDevice9::SetViewport(const D3DVIEWPORT9* pViewport)
{
	return pD3DDevice->SetViewport(pViewport);
}

HRESULT fIDirect3DDevice9::CreateVertexShader(const DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
	return pD3DDevice->CreateVertexShader(pFunction, ppShader);
}

HRESULT fIDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
	return pD3DDevice->GetVertexShader(ppShader);
}

HRESULT fIDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	return pD3DDevice->SetVertexShader(pShader);
}

HRESULT fIDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
	return pD3DDevice->CreateQuery(Type, ppQuery);
}

HRESULT fIDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT  BoolCount)
{
	return pD3DDevice->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT fIDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return pD3DDevice->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT fIDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount)
{
	return pD3DDevice->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT fIDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return pD3DDevice->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT fIDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount)
{
	return pD3DDevice->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT fIDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return pD3DDevice->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT fIDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT Divider)
{
	return pD3DDevice->SetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT fIDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT* Divider)
{
	return pD3DDevice->GetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT fIDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT  BoolCount)
{
	return pD3DDevice->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT fIDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return pD3DDevice->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT fIDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount)
{
	return pD3DDevice->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT fIDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return pD3DDevice->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT fIDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount)
{
	return pD3DDevice->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT fIDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return pD3DDevice->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT fIDirect3DDevice9::SetFVF(DWORD FVF)
{
	return pD3DDevice->SetFVF(FVF);
}

HRESULT fIDirect3DDevice9::GetFVF(DWORD* pFVF)
{
	return pD3DDevice->GetFVF(pFVF);
}

HRESULT fIDirect3DDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
	return pD3DDevice->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT fIDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	return pD3DDevice->SetVertexDeclaration(pDecl);
}

HRESULT fIDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
	return pD3DDevice->GetVertexDeclaration(ppDecl);
}

HRESULT fIDirect3DDevice9::SetNPatchMode(float nSegments)
{
	return pD3DDevice->SetNPatchMode(nSegments);
}

float fIDirect3DDevice9::GetNPatchMode(THIS)
{
	return pD3DDevice->GetNPatchMode();
}

int fIDirect3DDevice9::GetSoftwareVertexProcessing(THIS)
{
	return pD3DDevice->GetSoftwareVertexProcessing();
}

unsigned int fIDirect3DDevice9::GetNumberOfSwapChains(THIS)
{
	return pD3DDevice->GetNumberOfSwapChains();
}

HRESULT fIDirect3DDevice9::EvictManagedResources(THIS)
{
	return pD3DDevice->EvictManagedResources();
}

HRESULT fIDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	return pD3DDevice->SetSoftwareVertexProcessing(bSoftware);
}

HRESULT fIDirect3DDevice9::SetScissorRect(const RECT* pRect)
{
	return pD3DDevice->SetScissorRect(pRect);
}

HRESULT fIDirect3DDevice9::GetScissorRect(RECT* pRect)
{
	return pD3DDevice->GetScissorRect(pRect);
}

HRESULT fIDirect3DDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
	return pD3DDevice->GetSamplerState(Sampler, Type, pValue);
}

HRESULT fIDirect3DDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	return pD3DDevice->SetSamplerState(Sampler, Type, Value);
}

HRESULT fIDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
	return pD3DDevice->SetDepthStencilSurface(pNewZStencil);
}

HRESULT fIDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return pD3DDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT fIDirect3DDevice9::ColorFill(IDirect3DSurface9* pSurface, const RECT* pRect, D3DCOLOR color)
{
	return pD3DDevice->ColorFill(pSurface, pRect, color);
}

HRESULT fIDirect3DDevice9::StretchRect(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	return pD3DDevice->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT fIDirect3DDevice9::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
	return pD3DDevice->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT fIDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
	return pD3DDevice->GetRenderTargetData(pRenderTarget, pDestSurface);
}

HRESULT fIDirect3DDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, const POINT* pDestPoint)
{
	return pD3DDevice->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT fIDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
	return pD3DDevice->SetDialogBoxMode(bEnableDialogs);
}

HRESULT fIDirect3DDevice9::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
{
	return pD3DDevice->GetSwapChain(iSwapChain, pSwapChain);
}