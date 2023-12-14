#include <cstdio> // FILE
#include <vector> // std::vector<...>
#include <wchar.h> // fwprintf_s(...)
#include "..\include\export_symbol.h"
#include "..\include\cpp_exports_list_file_generator.h"


void GenerateCppExportsListFile(FILE* hFile)
{
   fwprintf_s(hFile, L"#include <Windows.h>\n");
   fwprintf_s(hFile, L"#include \"../include/dll_exports_list.h\"\n");
   fwprintf_s(hFile, L"\n");

   // write forward declaration and definition of functions pointers
   fwprintf_s(hFile, L"// forward declaration and definition of functions pointers list to original exports\n");
   fwprintf_s(hFile, L"FARPROC _exportOriginalAddr[REAL_MODULE_EXPORTS_COUNT] = { 0 };\n");
   fwprintf_s(hFile, L"\n");

   // write a list of exports details to the .cpp file
   fwprintf_s(hFile, L"// list of exports details\n");
   fwprintf_s(hFile, L"const ExportSymbol_t _exportSymbol[REAL_MODULE_EXPORTS_COUNT] = {\n");
   for (unsigned int i = 0; i < exportsList.size(); i++)
   {
      if (exportsList[i].isUnnamed)
      {
         fwprintf_s(hFile, L"   { (const char*)%uU, %uU, 1 },", exportsList[i].ordinal, exportsList[i].ordinal);
      }
      else
      {
         fwprintf_s(hFile, L"   { \"%s\", %uU, 0 },", exportsList[i].name, exportsList[i].ordinal);
      }

      // write a helper comment, in case array index is needed
      fwprintf_s(hFile, L" // array index [%u]\n", i);
   }
   fwprintf_s(hFile, L"};\n");
   fwprintf_s(hFile, L"\n");
}

