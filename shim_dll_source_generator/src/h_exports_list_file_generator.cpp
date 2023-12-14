#include <cstdio> // FILE
#include <vector> // std::vector<...>
#include <wchar.h> // fwprintf_s(...)
#include "..\include\export_symbol.h"
#include "..\include\h_exports_list_file_generator.h"


void GenerateHExportsListFile(FILE* hFile)
{
   fwprintf_s(hFile, L"#ifndef DLL_EXPORTS_LIST_H\n");
   fwprintf_s(hFile, L"#define DLL_EXPORTS_LIST_H\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"#include <Windows.h>\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"#define REAL_MODULE_EXPORTS_COUNT %uU\n", exportsList.size());
   fwprintf_s(hFile, L"\n");

   // write a type that represents each export
   fwprintf_s(hFile, L"// a type representing each export\n");
   fwprintf_s(hFile, L"typedef struct\n");
   fwprintf_s(hFile, L"{\n");
   fwprintf_s(hFile, L"   const char* exportName;     // export name\n");
   fwprintf_s(hFile, L"   unsigned int exportOrdinal; // export ordinal number\n");
   fwprintf_s(hFile, L"   char isOrdinalOnly : 1;     // 1 if export has no name, 0 otherwise\n");
   fwprintf_s(hFile, L"} ExportSymbol_t;\n");
   fwprintf_s(hFile, L"\n");

   // write forward declarations of asm hooks
   fwprintf_s(hFile, L"// forward declarations of asm hooks\n");
   fwprintf_s(hFile, L"extern \"C\"\n");
   fwprintf_s(hFile, L"{\n");
   for (unsigned int i = 0; i < exportsList.size(); i++)
   {
      fwprintf_s(hFile, L"   void __stdcall _ShimAsmHook_%u(); // ", exportsList[i].ordinal);

      // write a helper comment
      if (!exportsList[i].isUnnamed) // if export had a name
      {
         fwprintf_s(hFile, L"export name = %s, ", exportsList[i].name);
      }
      fwprintf_s(hFile, L"export ordinal = %u\n", exportsList[i].ordinal);
   }
   fwprintf_s(hFile, L"}\n");
   fwprintf_s(hFile, L"\n");

   fwprintf_s(hFile, L"// extern these arrays since they're used in the main .cpp file\n");
   fwprintf_s(hFile, L"extern \"C\" FARPROC _exportOriginalAddr[REAL_MODULE_EXPORTS_COUNT];\n");
   fwprintf_s(hFile, L"extern const ExportSymbol_t _exportSymbol[REAL_MODULE_EXPORTS_COUNT];\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"#endif /* DLL_EXPORTS_LIST_H */\n");
   fwprintf_s(hFile, L"\n");
}

