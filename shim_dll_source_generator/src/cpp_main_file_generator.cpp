#include <cstdio> // FILE
#include <vector> // std::vector<...>
#include <wchar.h> // fwprintf_s(...)
#include "..\include\export_symbol.h"
#include "..\include\cpp_main_file_generator.h"


void GenerateCppMainFile(FILE* hFile, wchar_t* dll_name, char isX64Module)
{
   // write header (includes and global vars) to .cpp file
   //fwprintf_s(hFile, L"#include \"stdafx.h\"\n");
   fwprintf_s(hFile, L"#include <cstdlib>\n");
   fwprintf_s(hFile, L"#include <cstdio>\n");
   fwprintf_s(hFile, L"#include <cwchar>\n");
   fwprintf_s(hFile, L"#include <cstring>\n");
   fwprintf_s(hFile, L"#include <cstdint>\n");
   fwprintf_s(hFile, L"#include <initializer_list>\n");
   fwprintf_s(hFile, L"#include <utility>\n");

   fwprintf_s(hFile, L"// uncomment this to use the C version of the Windows API\n");
   fwprintf_s(hFile, L"#ifdef CINTERFACE\n");
   fwprintf_s(hFile, L"  #include <Windows.h>\n");
   fwprintf_s(hFile, L"  #include <WinCrypt.h>\n");
   fwprintf_s(hFile, L"#else\n");
   fwprintf_s(hFile, L"//  #define CINTERFACE\n");
   fwprintf_s(hFile, L"  #include <Windows.h>\n");
   fwprintf_s(hFile, L"  #include <WinCrypt.h>\n");
   fwprintf_s(hFile, L"//  #undef CINTERFACE\n");
   fwprintf_s(hFile, L"#endif\n");

   fwprintf_s(hFile, L"//#include <Hidsdi.h>\n");
   fwprintf_s(hFile, L"//#include <Hidpi.h>\n");
   fwprintf_s(hFile, L"//#include <psapi.h>\n");

   fwprintf_s(hFile, L"#include \"../include/dll_exports_list.h\"\n");

   fwprintf_s(hFile, L"// uncomment this to use MinHook library\n");
   fwprintf_s(hFile, L"//#include \"../MinHook/include/MinHook.h\"\n");
   fwprintf_s(hFile, L"// uncomment this to use S'n'R (Search and Replace) helper functions\n");
   fwprintf_s(hFile, L"//#include \"../SnR_helpers/include/SnR_helpers.h\"\n");
   fwprintf_s(hFile, L"//#include \"../resource.h\"\n");
   fwprintf_s(hFile, L"\n");

   fwprintf_s(hFile, L"#define REAL_MODULE_NAME              L\"%s\"\n", dll_name);
   fwprintf_s(hFile, L"#define PATH_AND_FILENAME_MAX_COUNT   4096\n");
   fwprintf_s(hFile, L"\n");

   fwprintf_s(hFile, L"static wchar_t realDLL_pathAndFilename[PATH_AND_FILENAME_MAX_COUNT];\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"static HMODULE realDLL_hmod      = 0;\n");
   fwprintf_s(hFile, L"static HMODULE shimDLL_hmod      = 0;\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"static char isShimDllAttached    = 0;\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"// uncomment this if using MinHook library\n");
   fwprintf_s(hFile, L"//static char isMinHookInitialized = 0;\n");
   fwprintf_s(hFile, L"\n");

   // source: https://gist.github.com/Dantali0n/9cbab11b089f96d3382fe2509e5bc668
   // disable telemetry added in Visual Studio >= 2015
   fwprintf_s(hFile, L"// source: https://gist.github.com/Dantali0n/9cbab11b089f96d3382fe2509e5bc668\n");
   fwprintf_s(hFile, L"// disable telemetry added in Visual Studio >= 2015\n");
   fwprintf_s(hFile, L"#if defined(_MSC_VER) && (_MSC_VER >= 1900)\n");
   fwprintf_s(hFile, L"extern \"C\" {\n");
   fwprintf_s(hFile, L"   void __cdecl __vcrt_initialize_telemetry_provider() { }\n");
   fwprintf_s(hFile, L"   void __cdecl __telemetry_main_invoke_trigger() { }\n");
   fwprintf_s(hFile, L"   void __cdecl __telemetry_main_return_trigger() { }\n");
   fwprintf_s(hFile, L"   void __cdecl __vcrt_uninitialize_telemetry_provider() { }\n");
   fwprintf_s(hFile, L"};\n");
   fwprintf_s(hFile, L"#endif\n");
   fwprintf_s(hFile, L"\n");

   // write a function to get real module path and filename
   fwprintf_s(hFile, L"static char GetSystemModuleRealPathAndName(wchar_t* out_buffer, size_t charsCount, wchar_t* module_name)\n");
   fwprintf_s(hFile, L"{\n");
   fwprintf_s(hFile, L"   UINT ret;\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"   ret = GetSystemDirectoryW(out_buffer, (UINT)charsCount);\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"   if (ret < charsCount)\n");
   fwprintf_s(hFile, L"   {\n");
   fwprintf_s(hFile, L"      if ( wcscat_s(out_buffer, charsCount, L\"\\\\\") ||\n");
   fwprintf_s(hFile, L"           wcscat_s(out_buffer, charsCount, module_name) )\n");
   fwprintf_s(hFile, L"      {\n");
   fwprintf_s(hFile, L"         ret = 0;\n");
   fwprintf_s(hFile, L"      }\n");
   fwprintf_s(hFile, L"   }\n");
   fwprintf_s(hFile, L"   else\n");
   fwprintf_s(hFile, L"   {\n");
   fwprintf_s(hFile, L"      ret = 0;\n");
   fwprintf_s(hFile, L"   }\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"   return (char)!!ret;\n");
   fwprintf_s(hFile, L"}\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"\n");

   // friendly reminder comment
   fwprintf_s(hFile, L"// define your functions like this:\n");
   fwprintf_s(hFile, L"// [extern \"C\"] [__declspec(dllexport)] <return_type> [__stdcall | __cdecl] myFunc(...)\n\n");

   // write an example of a typedef and a pointer to original function/export
   fwprintf_s(hFile, L"// this example shows how to create a typedef and an object for the original export\n");
   fwprintf_s(hFile, L"//typedef <return_type>(__stdcall *original_func_t)(<arg1_type>, <arg2_type>, <arg3_type>, <arg4_type>);\n");
   fwprintf_s(hFile, L"//original_func_t original_func_addr;\n");
   fwprintf_s(hFile, L"\n");

   // write the DllMain() code
   fwprintf_s(hFile, L"BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)\n");
   fwprintf_s(hFile, L"{\n");
   fwprintf_s(hFile, L"   unsigned int i; // used to index each export\n");
   fwprintf_s(hFile, L"\n");

   fwprintf_s(hFile, L"   if (reason == DLL_PROCESS_ATTACH)\n");
   fwprintf_s(hFile, L"   {\n");

   // if Shim DLL was already attached, then return
   fwprintf_s(hFile, L"      if (isShimDllAttached)\n");
   fwprintf_s(hFile, L"      {\n");
   fwprintf_s(hFile, L"         return TRUE;\n");
   fwprintf_s(hFile, L"      }\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"      shimDLL_hmod = hModule;\n");
   fwprintf_s(hFile, L"\n");

   // write a code to get path to original system dll
   fwprintf_s(hFile, L"      if (!GetSystemModuleRealPathAndName(realDLL_pathAndFilename, _countof(realDLL_pathAndFilename), REAL_MODULE_NAME))\n");
   fwprintf_s(hFile, L"      {\n");
   fwprintf_s(hFile, L"         return FALSE;\n");
   fwprintf_s(hFile, L"      }\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"      realDLL_hmod = LoadLibraryW(realDLL_pathAndFilename);\n");
   fwprintf_s(hFile, L"      if (!realDLL_hmod) // if getting handle to original module failed\n");
   fwprintf_s(hFile, L"      {\n");
   fwprintf_s(hFile, L"         return FALSE;\n");
   fwprintf_s(hFile, L"      }\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"      // we get here if we have a module handle to the real dll\n");
   fwprintf_s(hFile, L"      isShimDllAttached = 1;\n");
   fwprintf_s(hFile, L"\n");

   // write a code to get proc addr for all original exports
   fwprintf_s(hFile, L"      for (i = 0; i < REAL_MODULE_EXPORTS_COUNT; i++)\n");
   fwprintf_s(hFile, L"      {\n");
   fwprintf_s(hFile, L"         _exportOriginalAddr[i] = GetProcAddress(realDLL_hmod, _exportSymbol[i].exportName);\n");
   fwprintf_s(hFile, L"         if (!_exportOriginalAddr[i] && !_exportSymbol[i].isOrdinalOnly) // if (getting export address failed) and (original export had a name)\n");
   fwprintf_s(hFile, L"         {\n");
   fwprintf_s(hFile, L"            // try to get export address by ordinal number\n");
   fwprintf_s(hFile, L"            _exportOriginalAddr[i] = GetProcAddress(realDLL_hmod, (const char*)(uint%u_t)_exportSymbol[i].exportOrdinal);\n", (isX64Module) ? 64U : 32U);
   fwprintf_s(hFile, L"         }\n");
   fwprintf_s(hFile, L"      }\n");
   fwprintf_s(hFile, L"\n");

   fwprintf_s(hFile, L"      // do the patching here after all exports addresses were retrieved\n");
   fwprintf_s(hFile, L"      //HotPatchFunction(_exportOriginalAddr[...], <new_addr>, original_bytes, 5);\n");
   fwprintf_s(hFile, L"\n");

   fwprintf_s(hFile, L"      //if (MH_Initialize() == MH_OK)\n");
   fwprintf_s(hFile, L"      //{\n");
   fwprintf_s(hFile, L"      //   isMinHookInitialized = 1;\n");
   fwprintf_s(hFile, L"      //}\n");
   fwprintf_s(hFile, L"      //else\n");
   fwprintf_s(hFile, L"      //{\n");
   fwprintf_s(hFile, L"      //   return FALSE;\n");
   fwprintf_s(hFile, L"      //}\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"      //if ( (MH_CreateHook(_exportOriginalAddr[...], <new_addr>, (void**)(&original_func_addr)) != MH_OK) ||\n");
   fwprintf_s(hFile, L"      //     (MH_EnableHook(_exportOriginalAddr[...])                                            != MH_OK) )\n");
   fwprintf_s(hFile, L"      //{\n");
   fwprintf_s(hFile, L"      //   return FALSE;\n");
   fwprintf_s(hFile, L"      //}\n");
   fwprintf_s(hFile, L"   }\n");
   fwprintf_s(hFile, L"   else if (reason == DLL_PROCESS_DETACH)\n");
   fwprintf_s(hFile, L"   {\n");

   fwprintf_s(hFile, L"      // uncomment this if using MinHook library\n");
   fwprintf_s(hFile, L"      //if (isMinHookInitialized)\n");
   fwprintf_s(hFile, L"      //{\n");
   fwprintf_s(hFile, L"      //   MH_DisableHook(MH_ALL_HOOKS);\n");
   fwprintf_s(hFile, L"      //   MH_RemoveHook(_exportOriginalAddr[...]);\n");
   fwprintf_s(hFile, L"      //   MH_Uninitialize();\n");
   fwprintf_s(hFile, L"      //   isMinHookInitialized = 0;\n");
   fwprintf_s(hFile, L"      //}\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"      if (isShimDllAttached)\n");
   fwprintf_s(hFile, L"      {\n");
   fwprintf_s(hFile, L"         FreeLibrary(realDLL_hmod);\n");
   fwprintf_s(hFile, L"         isShimDllAttached = 0;\n");
   fwprintf_s(hFile, L"      }\n");
   fwprintf_s(hFile, L"   }\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"   return TRUE;\n");
   fwprintf_s(hFile, L"}\n");
   fwprintf_s(hFile, L"\n");
}

