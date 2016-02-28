// taken from CheatEngine table by Cielos
// http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841

#include "stdafx.h"
#include "Misc.h"

LPBYTE pCharCustomization, oCharCustomization;
void __declspec(naked) HCharCustomization()
{
	__asm
	{
		cmp     dword ptr[ebx + 284h], 10;
		jne		getBack;
		mov		dword ptr[ebx + 284h], 2;
	getBack:
		jmp		oCharCustomization;
	}
}

const float plus = 89.9f, minus = -89.9f;
LPBYTE pExtendVerticalCam1, pExtendVerticalCam2, pExtendVerticalBow1, pExtendVerticalBow2;
LPVOID oExtendVerticalCam1, oExtendVerticalCam2, oExtendVerticalBow1, oExtendVerticalBow2;
void __declspec(naked) HExtendVerticalCam1()
{
	__asm	movss	xmm0, plus
	__asm	jmp		oExtendVerticalCam1;
}

void __declspec(naked) HExtendVerticalCam2()
{
	__asm	movss	xmm0, minus
	__asm	jmp		oExtendVerticalCam2;
}

void __declspec(naked) HExtendVerticalBow1()
{
	__asm	movss	xmm0, plus
	__asm	jmp		oExtendVerticalBow1;
}

void __declspec(naked) HExtendVerticalBow2()
{
	__asm	movss	xmm0, minus
	__asm	jmp		oExtendVerticalBow2;
}

LPBYTE pAutoCamV, pAutoCamH, oAutoCamV, oAutoCamH;
void __declspec(naked) HAutoCamV()
{
	__asm	cmp		esi, 0;
	__asm	jmp		oAutoCamV;
}

void __declspec(naked) HAutoCamH()
{
	__asm	cmp		edx, 0;
	__asm	jmp		oAutoCamH;
}

LPVOID oWeather;
void __declspec(naked) HWeather()
{
	__asm
	{
		cmp		byte ptr[ecx + 0xB33A8], 0;
		je		flagZero;
		cmp		word ptr[ecx + 0x34], 0xDC;
		jmp		oWeather;

	flagZero:
		cmp		word ptr[ecx + 0x34], 0xE6;
		je		unsetZero;
		cmp		ecx, ecx;
		jmp		oWeather;

	unsetZero:
		cmp		ecx, 0;
		jmp		oWeather;
	}
}

bool jumpMod;
float jumpMods[6] = { 0.0f, 19.00f, 13.95f, -2.00f, 0.30f, 4.25f };
float jumpModsRun[6] = { 0.0f, 15.25f, 22.90f, -1.25f, 0.30f, 4.25f };
LPBYTE pJumpMod1, pJumpMod2, oJumpMod1, oJumpMod2;
void __declspec(naked) HJumpMod1()
{
	__asm	mov		edx, ecx;
	__asm	jmp		oJumpMod1;
}

void __declspec(naked) HJumpMod2()
{
	__asm
	{
		mov		ecx, [edi + 0x3DEC];
		mov		ecx, [ecx + 8];
		test	ecx, ecx;
		jnz		getBack;

		cmp		edx, 9;
		jl		loadNormal;
		cmp		edx, 18;
		jl		loadRun;
		jmp		getBack;

	loadNormal:
		lea		esi, jumpMods;
		jmp		getBack;

	loadRun:
		lea		esi, jumpModsRun;

	getBack:
		jmp		oJumpMod2;
	}
}

float fallHeight;
LPBYTE pFallHeight, oFallHeight;
void __declspec(naked) HFallHeight()
{
	__asm
	{
		mov		ecx, [esi + 0x3DEC];
		test	ecx, ecx;
		jz		getBack;

		mov		ecx, [ecx + 8];
		cmp		ecx, -1;
		je		getBack;

		fstp	st(0);
		fld		fallHeight;

	getBack:
		jmp		oFallHeight;
	}
}

float gatheringSpeed;
bool charCustomization, extendVerticalCam, disableAutoCam;
void renderMiscUI()
{
	if (ImGui::CollapsingHeader("Main"))
	{
		static bool borderlessFullscreen = config.getBool("main", "borderlessFullscreen", false);
		if (ImGui::Checkbox("Borderless fullscreen", &borderlessFullscreen))
			config.setBool("main", "borderlessFullscreen", borderlessFullscreen);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("requires game restart");

		if (ImGui::Checkbox("Char customization", &charCustomization))
		{
			config.setBool("main", "charCustomization", charCustomization);
			Hooks::SwitchHook("CharCustomization", pCharCustomization, charCustomization);
		}

		if (ImGui::Checkbox("Camera - extend vertical", &extendVerticalCam))
		{
			config.setBool("main", "extendVerticalCam", extendVerticalCam);
			Hooks::SwitchHook("ExtendVerticalCam1", pExtendVerticalCam1, extendVerticalCam);
			Hooks::SwitchHook("ExtendVerticalCam2", pExtendVerticalCam2, extendVerticalCam);
			Hooks::SwitchHook("ExtendVerticalCamBow1", pExtendVerticalBow1, extendVerticalCam);
			Hooks::SwitchHook("ExtendVerticalCamBow2", pExtendVerticalBow2, extendVerticalCam);
		}

		if (ImGui::Checkbox("Camera - disable autocorrection", &disableAutoCam))
		{
			config.setBool("main", "disableAutoCam", disableAutoCam);
			Hooks::SwitchHook("DisableAutoCamV", pAutoCamV, disableAutoCam);
			Hooks::SwitchHook("DisableAutoCamH", pAutoCamH, disableAutoCam);
		}

		std::pair<int, const char*> weather[]{ { 0, "Clear sky" }, { 1, "Cloudy" }, { 2, "Foggy" }, { 3, "Vulcanic" } };
		ImGui::RadioButtons(GetBasePtr(0xB8780), weather);
		ImGui::Checkbox("Weather - post game", GetBasePtr<bool>(0xB33A8));

		ImGui::PushItemWidth(150.0f);
		if (ImGui::InputFloatEx("Gathering/Mining speed", &gatheringSpeed, 0.1f, 1.0f, 100.0f, 1))
			config.setFloat("main", "gatheringSpeed", gatheringSpeed);

		bool prevState = fallHeight < 0;
		if (ImGui::InputFloatEx("Fall height", &fallHeight, 10.0f, -FLT_MAX, 0.0f))
		{
			config.setFloat("main", "fallHeight", fallHeight);
			if (prevState != fallHeight < 0)
				Hooks::SwitchHook("FallHeight", pFallHeight, fallHeight < 0);
		}
		ImGui::PopItemWidth();

		if (ImGui::TreeNode("Jump mod"))
		{
			if (ImGui::Checkbox("Enabled", &jumpMod))
			{
				config.setBool("main", "jumpMod", jumpMod);
				Hooks::SwitchHook("JumpMod", pJumpMod1, jumpMod);
				Hooks::SwitchHook("JumpMod", pJumpMod2, jumpMod);
			}

			ImGui::TextUnformatted("Walking Jump");
			ImGui::TextUnformatted("Running Jump", 190.0f);
			ImGui::PushItemWidth(150.0f);
			int i = 1;
			for (auto str : { "Height", "Length", "Gravity", "Damping", "MoveSpeed" })
			{
				if (ImGui::InputFloatEx(string("##1").append(str).c_str(), jumpMods + i, 0.1f))
					config.setFloats("main", "jumpMods", std::vector<float>(jumpMods + 1, jumpMods + 6));
				ImGui::SameLine();
				if (ImGui::InputFloatEx(str, jumpModsRun + i, 0.1f))
					config.setFloats("main", "jumpModsRun", std::vector<float>(jumpModsRun + 1, jumpModsRun + 6));
				i++;
			}
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}
	}
}

void Hooks::Misc()
{
	BYTE sigChar[] = { 0x83, 0xBB, 0x84, 0x02, 0x00, 0x00, 0x0B };	//cmp	dword ptr [ebx+284h], 0Bh
	if (FindSignature("CharCustomization", sigChar, &pCharCustomization))
	{
		charCustomization = config.getBool("main", "charCustomization", false);
		CreateHook("CharCustomization", pCharCustomization, &HCharCustomization, &oCharCustomization, charCustomization);
	}

	BYTE sigCam[] = { 0xF3, 0x0F, 0x10, 0x80, 0x8C, 0x00, 0x00, 0x00,	//movss	xmm0, dword ptr [eax+8Ch]
					0xF3, 0x0F, 0x10, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC,		//movss	xmm1
					0xF3, 0x0F, 0x10, 0x11 };							//movss	xmm2, dword ptr [ecx]

	BYTE sigBow[] = { 0xF3, 0x0F, 0x10, 0x81, 0x8C, 0x00, 0x00, 0x00,	//movss	xmm0, dword ptr [ecx+8Ch]
					0xF3, 0x0F, 0x10, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC,		//movss	xmm1
					0xF3, 0x0F, 0x10, 0x17 };							//movss	xmm2, dword ptr [edi]

	if (FindSignature("ExtendVerticalCam", sigCam, &pExtendVerticalCam1) &&
		FindSignature("ExtendVerticalCamBow", sigBow, &pExtendVerticalBow1))
	{
		pExtendVerticalCam1 += 8;
		pExtendVerticalCam2 = pExtendVerticalCam1 + 0x21;
		pExtendVerticalBow1 += 8;
		pExtendVerticalBow2 = pExtendVerticalBow1 + 0x27;

		extendVerticalCam = config.getBool("main", "extendVerticalCam", false);
		CreateHook("ExtendVerticalCam1", pExtendVerticalCam1, &HExtendVerticalCam1, &oExtendVerticalCam1, extendVerticalCam);
		CreateHook("ExtendVerticalCam2", pExtendVerticalCam2, &HExtendVerticalCam2, &oExtendVerticalCam2, extendVerticalCam);
		CreateHook("ExtendVerticalCamBow1", pExtendVerticalBow1, &HExtendVerticalBow1, &oExtendVerticalBow1, extendVerticalCam);
		CreateHook("ExtendVerticalCamBow2", pExtendVerticalBow2, &HExtendVerticalBow2, &oExtendVerticalBow2, extendVerticalCam);
	}

	BYTE sigV[] = { 0x80, 0xBE, 0xF0, 0x02, 0x00, 0x00, 0x00,	//cmp	byte ptr [esi+2F0h], 0
					0x0F, 0x85, 0xCC, 0xCC, 0x00, 0x00 };		//jnz
	BYTE sigH[] = { 0x80, 0xBA, 0xF1, 0x02, 0x00, 0x00, 0x00,	//cmp	byte ptr [edx+2F1h], 0
					0x0F, 0x85, 0xCC, 0xCC, 0x00, 0x00 };		//jnz
	if (FindSignature("DisableAutoCamV", sigV, &pAutoCamV) &&
		FindSignature("DisableAutoCamH", sigH, &pAutoCamH))
	{
		disableAutoCam = config.getBool("main", "disableAutoCam", false);
		CreateHook("DisableAutoCamV", pAutoCamV, &HAutoCamV, nullptr, disableAutoCam);
		CreateHook("DisableAutoCamH", pAutoCamH, &HAutoCamH, nullptr, disableAutoCam);
		oAutoCamV = pAutoCamV + 7;
		oAutoCamH = pAutoCamH + 7;
	}

	BYTE *pOffset;
	BYTE sigWeather[] = { 0x80, 0xB9, 0xA8, 0x33, 0x0B, 0x00, 0x00, 0x74, 0x15 };
	if (FindSignature("Weather", sigWeather, &pOffset))
	{
		CreateHook("Weather", pOffset, &HWeather, nullptr);
		oWeather = pOffset + 7;
	}

	gatheringSpeed = config.getFloat("main", "gatheringSpeed", 1.0f);
	BYTE sigGathering[] = { 0xBA, 0x68, 0x00, 0x00, 0x00, 0x8B, 0xCE, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x84, 0xC0, 0x74 };
	if (FindSignature("Gathering", sigGathering, &pOffset))
	{
		pOffset += pOffset[sizeof sigGathering] + sizeof sigGathering + 5;
		Set<float*>((float**)pOffset, { &gatheringSpeed });
		for (int i = 0; i < 3; i++)
		{
			if (!Find("Gathering", pOffset, pOffset + 0x10000, sigGathering, &pOffset))
				break;
			pOffset += pOffset[sizeof sigGathering] + sizeof sigGathering + 5;
			Set<float*>((float**)pOffset, { &gatheringSpeed });
		}
	}

	fallHeight = config.getFloat("main", "fallHeight", -1000.0f);
	BYTE sigFall[] = { 0xD9, 0x5C, 0x24, 0x0C, 0x80, 0xBE, 0xD0, 0x1E, 0x00, 0x00, 0x00 };
	if (FindSignature("FallHeight", sigFall, &pFallHeight))
		CreateHook("FallHeight", pFallHeight, &HFallHeight, &oFallHeight, fallHeight < 0);

	BYTE sigJump[] = { 0x8B, 0x40, 0x70, 0x56, 0x8B, 0x34, 0x88, 0x8B, 0x16, 0x8B, 0x42, 0x10 };
	if (FindSignature("JumpMod", sigJump, &pJumpMod1))
	{
		pJumpMod1 += sizeof sigJump;
		BYTE sigJump2[] = { 0xD9, 0x46, 0x04, 0xD9, 0x9F, 0xD8, 0x38, 0x00, 0x00 };
		if (Find("JumpMod", pJumpMod1, pJumpMod1 + 0x100, sigJump2, &pJumpMod2))
		{
			jumpMod = config.getBool("main", "jumpMod", false);
			auto mods = config.getFloats("main", "jumpMods");
			if (mods.size() == 5)
				copy(mods.begin(), mods.end(), jumpMods + 1);
			mods = config.getFloats("main", "jumpModsRun");
			if (mods.size() == 5)
				copy(mods.begin(), mods.end(), jumpModsRun + 1);
			CreateHook("JumpMod", pJumpMod1, &HJumpMod1, &oJumpMod1, jumpMod);
			CreateHook("JumpMod", pJumpMod2, &HJumpMod2, &oJumpMod2, jumpMod);
		}
	}

	InGameUIAdd(renderMiscUI);
}