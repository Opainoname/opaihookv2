#include "sdk.h"
#include "MaterialHelper.h"
#include "xor.h"
CMaterialHelper* g_MaterialHelper = nullptr;

typedef void(__thiscall* orgGameFunc_InitKeyValues)(void* thisptr, const char* name);
orgGameFunc_InitKeyValues pInitKeyValues;
void CMaterialHelper::GameFunc_InitKeyValues(KeyValues* pKeyValues, const char* name)
{
	/*Mid function Pattern -> sub 0x45*/
	/*8B 0E 33 4D FC 81 E1 ? ? ? ? 31 0E 88 46 03 C1 F8 08 66 89 46 12 8B C6*/


	//Add Pattern Scan
	//	static DWORD funcAddr =Utils::xFindPattern(client_dll, (PBYTE)"\x8B\x0E\x33\x4D\xFC\x81\xE1\x00\x00\x00\x00\x31\x0E\x88\x46\x03\xC1\xF8\x08\x66\x89\x46\x12\x8B\xC6", "xxxxxxx????xxxxxxxxxxxxxx") - 0x45;

	if (!pInitKeyValues)
		pInitKeyValues = reinterpret_cast<orgGameFunc_InitKeyValues>(offys.dwInitKeyValues);

	pInitKeyValues(pKeyValues, name);

}
typedef void(__thiscall* orgGameFunc_LoadFromBuffer)(void* thisptr, const char* resourceName, const char* pBuffer, /*IBaseFileSystem**/void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc);
orgGameFunc_LoadFromBuffer pLoadFromBuffer;
void CMaterialHelper::GameFunc_LoadFromBuffer(KeyValues* pKeyValues, const char* resourceName, const char* pBuffer, /*IBaseFileSystem**/void* pFileSystem , const char* pPathID, void* pfnEvaluateSymbolProc)
{
	/*55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04*/
	//Add Pattern Scan
	//static DWORD funcAddr = Utils::xFindPattern(client_dll, (PBYTE)"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x34\x53\x8B\x5D\x0C\x89\x4C\x24\x04", "xxxxxxxxxxxxxxxxx");


	if (!pLoadFromBuffer)
		pLoadFromBuffer = reinterpret_cast<orgGameFunc_LoadFromBuffer>(offys.dwLoadFromBuffer);

	pLoadFromBuffer(pKeyValues, resourceName, pBuffer, pFileSystem, pPathID, pfnEvaluateSymbolProc);


}
IMaterial* CMaterialHelper::CreateMaterial(bool shouldIgnoreZ, bool isLit, bool isWireframe) //credits to ph0ne
{
	static int iCreated = 0;

	static const char szTmp[] =
	{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$receiveflashlight\" \"1\"\
		\n\t\"$singlepassflashlight\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
		\n}\n"
	};

	char szMaterial[512];
	char szBaseType[64];

	if (isLit)
	{
		static const char pszBaseType[] = "UnlitGeneric";
		sprintf_s(szMaterial, sizeof(szMaterial), szTmp, pszBaseType, (shouldIgnoreZ) ? 1 : 0, (isWireframe) ? 1 : 0);
		strcpy_s(szBaseType, pszBaseType);
	}
	else
	{
		static const char pszBaseType[] = "VertexLitGeneric";
		sprintf_s(szMaterial, sizeof(szMaterial), szTmp, pszBaseType, (shouldIgnoreZ) ? 1 : 0, (isWireframe) ? 1 : 0);
		strcpy_s(szBaseType, pszBaseType);
	}

	char szName[512];
	sprintf_s(szName, sizeof(szName), "custom_material_%i.vmt", iCreated);

	++iCreated;

	KeyValues* keyValues = new KeyValues();
	GameFunc_InitKeyValues(keyValues, szBaseType);
	GameFunc_LoadFromBuffer(keyValues, szName, szMaterial, nullptr, NULL, nullptr);

	IMaterial *createdMaterial = g_pMaterialSystem->CreateMaterial(szName, keyValues);
	createdMaterial->IncrementReferenceCount();

	return createdMaterial;
}

void CMaterialHelper::ForceMaterial(Color color, IMaterial* material, bool useColor, bool forceMaterial)
{
	if (useColor)
	{
		float temp[3] =
		{
			color.r(),
			color.g(),
			color.b()
		};

		temp[0] /= 255.f;
		temp[1] /= 255.f;
		temp[2] /= 255.f;


		float alpha = color.a();

		g_pRenderView->SetBlend(1.0f);
		g_pRenderView->SetColorModulation(temp);
	}

	if (forceMaterial)
		g_pModelRender->ForcedMaterialOverride(material);
	else
		g_pModelRender->ForcedMaterialOverride(NULL);

}
CMaterialHelper::CMaterialHelper()
{
	CoveredLit = CreateMaterial(true);
	OpenLit = CreateMaterial(false);
	CoveredFlat = CreateMaterial(true, false);
	OpenFlat = CreateMaterial(false, false);
	Wire = CreateMaterial(true, true, true);
	AddWire = CreateMaterial(false, true, true);
}