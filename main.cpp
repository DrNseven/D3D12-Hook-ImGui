////////////////
// D3D12 HOOK //
////////////////

#include "main.h"

//=========================================================================================================================// D3D12 HOOKS 
// D3D12 HOOKS Example

bool InitOnce = true;
bool InitOnce2 = true;
bool InitOnce3 = true;

ID3D12Device *dDevice = NULL;
ID3D12GraphicsCommandList *dCommandList = NULL;


typedef long(__stdcall* Present12) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
Present12 oPresent12 = NULL;

typedef void(__stdcall *DrawInstanced)(ID3D12GraphicsCommandList *dCommandList, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation);
DrawInstanced oDrawInstanced = NULL;

typedef void(__stdcall *DrawIndexedInstanced)(ID3D12GraphicsCommandList *dCommandList, UINT IndexCount, UINT InstanceCount, UINT StartIndex, INT BaseVertex);
DrawIndexedInstanced oDrawIndexedInstanced = NULL;


//=========================================================================================================================//

long __stdcall hkPresent12(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (InitOnce)
	{
		Log("[d3d12]Present called");
		InitOnce = false;

		//get device
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D12Device), (void **)&dDevice)))
		{
			pSwapChain->GetDevice(__uuidof(dDevice), (void**)&dDevice);
		}
	}


	return oPresent12(pSwapChain, SyncInterval, Flags);
}


//=========================================================================================================================//

void __stdcall hkDrawInstanced(ID3D12GraphicsCommandList *dCommandList, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
{
	if (InitOnce2)
	{
		Log("[d3d12]DrawInstanced called");
		InitOnce2 = false;
	}

	//screen fuqup test (elemental-demo-dx12)
	const float f[4] = { 0, 0, 0, 0 };
	dCommandList->OMSetBlendFactor(f);


	return oDrawIndexedInstanced(dCommandList, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}


//=========================================================================================================================//

void __stdcall hkDrawIndexedInstanced(ID3D12GraphicsCommandList *dCommandList, UINT IndexCount, UINT InstanceCount, UINT StartIndex, INT BaseVertex)
{
	if (InitOnce3)
	{
		Log("[d3d12]DrawIndexedInstanced called");
		InitOnce3 = false;
	}


	return oDrawIndexedInstanced(dCommandList, IndexCount, InstanceCount, StartIndex, BaseVertex);
}


//=========================================================================================================================//

int dx12Thread()
{
	if (dx12::init(dx12::RenderType::D3D12) == dx12::Status::Success)
	{
		MH_Initialize();
		MH_CreateHook((LPVOID)dx12::getMethodsTable()[140], hkPresent12, (LPVOID*)&oPresent12);
		MH_CreateHook((LPVOID)dx12::getMethodsTable()[84], hkDrawInstanced, (LPVOID*)&oDrawInstanced);
		MH_CreateHook((LPVOID)dx12::getMethodsTable()[85], hkDrawIndexedInstanced, (LPVOID*)&oDrawIndexedInstanced);
		MH_EnableHook((LPVOID)dx12::getMethodsTable()[140]);
		MH_EnableHook((LPVOID)dx12::getMethodsTable()[84]);
		MH_EnableHook((LPVOID)dx12::getMethodsTable()[85]);
	}

	return 0;
}


//=========================================================================================================================//

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID)
{
	DisableThreadLibraryCalls(hInstance);

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hInstance);
		GetModuleFileNameA(hInstance, dlldir, 512);
		for (size_t i = strlen(dlldir); i > 0; i--) { if (dlldir[i] == '\\') { dlldir[i + 1] = 0; break; } }
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)dx12Thread, NULL, 0, NULL);
		break;
	}

	return TRUE;
}