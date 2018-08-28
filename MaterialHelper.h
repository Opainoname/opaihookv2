#pragma once
class CMaterialHelper
{
public:
	IMaterial* CoveredLit = nullptr;
	IMaterial* OpenLit = nullptr;
	IMaterial* CoveredFlat = nullptr;
	IMaterial* OpenFlat = nullptr;
	IMaterial* Wire = nullptr;
	IMaterial* AddWire = nullptr;
	CMaterialHelper();
	void ForceMaterial(Color color, IMaterial* material, bool useColor = true, bool forceMaterial = true);
	IMaterial *CreateMaterial(bool shouldIgnoreZ, bool isLit = true, bool isWireframe = false);
	void GameFunc_InitKeyValues(KeyValues* pKeyValues, const char* name);
	void GameFunc_LoadFromBuffer(KeyValues* pKeyValues, const char* resourceName, const char* pBuffer, /*IBaseFileSystem**/void* pFileSystem = nullptr, const char* pPathID = NULL, void* pfnEvaluateSymbolProc = nullptr);
};
extern CMaterialHelper* g_MaterialHelper;