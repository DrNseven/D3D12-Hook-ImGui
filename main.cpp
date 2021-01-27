////////////////
// D3D12 HOOK //
////////////////

#include "main.h"

//=========================================================================================================================// D3D12 HOOKS 
// D3D12 HOOKS Example

bool InitOnce = true;
bool InitOnce2 = true;
bool InitOnce3 = true;
int countnum = -1;
bool nopants_enabled = true;


ID3D12Device *dDevice = NULL;
ID3D12GraphicsCommandList *dCommandList = NULL;


typedef long(__stdcall* Present12) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
Present12 oPresent12 = NULL;

typedef void(__stdcall *DrawInstanced)(ID3D12GraphicsCommandList *dCommandList, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation);
DrawInstanced oDrawInstanced = NULL;

typedef void(__stdcall *DrawIndexedInstanced)(ID3D12GraphicsCommandList *dCommandList, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation);
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

	
	//no fog/smoke/glow test (elemental-demo-dx12)
	//const float f[4] = { 0, 0, 0, 0 };
	//dCommandList->OMSetBlendFactor(f);

	/*
	//hold down P key until a texture disappears, press END to log values of those textures
	if (GetAsyncKeyState('O') & 1) //-
		countnum--;
	if (GetAsyncKeyState('L') & 1) //+
		countnum++;
	if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState('9') & 1) //reset, set to -1
		countnum = -1;

	if (countnum == VertexCountPerInstance)
		if (GetAsyncKeyState(VK_END) & 1) //log
			Log("VertexCountPerInstance == %d && InstanceCount == %d",
				VertexCountPerInstance, InstanceCount);

	if (countnum == VertexCountPerInstance)
		return;
	*/

	return oDrawInstanced(dCommandList, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}


//=========================================================================================================================//

void __stdcall hkDrawIndexedInstanced(ID3D12GraphicsCommandList *dCommandList, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
{
	if (InitOnce3)
	{
		Log("[d3d12]DrawIndexedInstanced called");
		InitOnce3 = false;
	}

	//cyberpunk 2077 no pants hack (low settings)
	if(nopants_enabled)
	if(IndexCountPerInstance == 10068|| //bargirl pants near
		IndexCountPerInstance == 3576) //med range
	return; //delete texture

	if (GetAsyncKeyState(VK_F12) & 1) //toggle key
		nopants_enabled = !nopants_enabled;
	

	//logger, hold down B key until a texture disappears, press END to log values of those textures
	if (GetAsyncKeyState('V') & 1) //-
		countnum--;
	if (GetAsyncKeyState('B') & 1) //+
		countnum++;
	if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState('9') & 1) //reset, set to -1
		countnum = -1;

	if (countnum == IndexCountPerInstance/100)
		if (GetAsyncKeyState(VK_END) & 1) //log
			Log("IndexCountPerInstance == %d && InstanceCount == %d",
				IndexCountPerInstance, InstanceCount);

	if (countnum == IndexCountPerInstance/100)
		return;
	
	return oDrawIndexedInstanced(dCommandList, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
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
