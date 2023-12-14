#include <cstdio> // FILE
#include <cwchar> // wcsrchr(...)
#include <vector> // std::vector<...>
#include <wchar.h> // fwprintf_s(...)
#include "..\include\export_symbol.h"
#include "..\include\def_file_generator.h"


void GenerateDefFile(FILE* hFile, wchar_t* dll_name)
{
   // write "LIBRARY" statement to .def file, this tells LINKER the name of the dll, it triggers a warning if project name is different
   // find last dot char and null it out
   wchar_t* dotChar = wcsrchr(dll_name, L'.');
   *dotChar = L'\0';
   fwprintf_s(hFile, L"LIBRARY \"%s\"\n", dll_name);
   // restore dot char
   *dotChar = L'.';
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"EXPORTS\n");

   // write all exports to .def file (original  name = internal function name @ ordinal number)
   for (unsigned int i = 0; i < exportsList.size(); i++)
   {
      if (exportsList[i].isUnnamed)
      {
         fwprintf_s(hFile, L"   ordinal%u = _ShimAsmHook_%u @%u NONAME\n", exportsList[i].ordinal, exportsList[i].ordinal, exportsList[i].ordinal);
      }
      else
      {
         fwprintf_s(hFile, L"   %s = _ShimAsmHook_%u @%u\n", exportsList[i].name, exportsList[i].ordinal, exportsList[i].ordinal);
      }
   }
   fwprintf_s(hFile, L"\n");
}

