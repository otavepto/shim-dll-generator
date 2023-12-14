#include <cstdio> // FILE
#include <vector> // std::vector<...>
#include <wchar.h> // fwprintf_s(...)
#include "..\include\export_symbol.h"
#include "..\include\asm_file_generator.h"


void GenerateAsmFile(FILE* hFile, char isX64Module)
{

   if (!isX64Module)
   {
      // write processor instructions type and assembler model to the .asm file since it's required in x86
      // .686 Enables assembly of nonprivileged instructions for the Pentium Pro processor (latest processor model in MASM)
      // .MODEL flat, C : flat means size of code and data pointers are 32-bit,
      //                  C means/is the calling and naming convention for procedures and public symbols
      fwprintf_s(hFile, L".686\n");
      fwprintf_s(hFile, L".MODEL flat, STDCALL\n\n");
   }

   // write PUBLIC statements to the .asm file to make hook functions visible to .cpp file
   fwprintf_s(hFile, L"; make these assembly hooks public and visible to other files\n");
   for (unsigned int i = 0; i < exportsList.size(); i++)
   {
      fwprintf_s(hFile, L"   PUBLIC _ShimAsmHook_%u\n", exportsList[i].ordinal);
   }
   fwprintf_s(hFile, L"\n");

   // write an EXTERN statement in the .asm file to use original export addrs list defined in .cpp file
   fwprintf_s(hFile, L"; extern/borrow original exports addresses list defined in the C/C++ file\n");
   // variable type (primitive: {BYTE, WORD, DWORD, QWORD, PROC}, pointers: PTR [primitive])
   fwprintf_s(hFile, L"   EXTERN _exportOriginalAddr : PTR\n");
   fwprintf_s(hFile, L"\n");

   fwprintf_s(hFile, L"   ; custom detours/hooks defined in the C/C++ file can be declarated here\n");
   fwprintf_s(hFile, L"   ;x86: <function name> PROTO <C|STDCALL> [:<param1 type>] [, :<param2 type>]...\n");
   fwprintf_s(hFile, L"   ;x64: <function name> PROTO [C|STDCALL] [:<param1 type>] [, :<param2 type>]...\n\n");
   fwprintf_s(hFile, L"\n");

   // write the definition/body of the asm hooks in the .text section
   fwprintf_s(hFile, L"_text SEGMENT\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"; definitions of asm hooks\n");
   fwprintf_s(hFile, L"\n");
   for (unsigned int i = 0; i < exportsList.size(); i++)
   {
      // write a helper comment
      fwprintf_s(hFile, L"   ; ");
      if (!exportsList[i].isUnnamed) // if export had a name
      {
         fwprintf_s(hFile, L"export name = %s, ", exportsList[i].name);
      }
      fwprintf_s(hFile, L"export ordinal = %u\n", exportsList[i].ordinal);

      fwprintf_s(hFile, L"   _ShimAsmHook_%u PROC\n", exportsList[i].ordinal);
      fwprintf_s(hFile, L"      jmp [_exportOriginalAddr + %u]\n", (isX64Module) ? i * sizeof(uint64_t) : i * sizeof(uint32_t));
      fwprintf_s(hFile, L"      ; leave some space for hot-patching\n");
      fwprintf_s(hFile, L"      db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\n");
      fwprintf_s(hFile, L"   _ShimAsmHook_%u ENDP\n", exportsList[i].ordinal);
      fwprintf_s(hFile, L"\n");
   }
   fwprintf_s(hFile, L"_text ENDS\n");
   fwprintf_s(hFile, L"\n");
   fwprintf_s(hFile, L"END\n"); // .asm file end
   fwprintf_s(hFile, L"\n");
}

