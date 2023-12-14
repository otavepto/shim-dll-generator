// source: http://www.hulver.com/scoop/story/2006/2/18/125521/185
// originally from: https://github.com/Miyuki333/ShimmySham
// someone shared a shimmed kernel33.dll file to be used with unity games to block dinput to prevent controller ghosting
// which used the previous github repo to generate a custom dll for each machine


#include <stdio.h> // feof(...), fclose(...)
#include <ctime> // time(...), gmtime(...)
#include <wchar.h> // _wfopen_s(...), swprintf_s(...), wscanf_s(...), _wsystem(...), fgetws(...), swscanf_s(...)
#include <vector> // std::vector<...>
#include <utility> // std::move(...)
#include <Windows.h> // GetFullPathNameW(...), GetModuleFileNameW(...), CreateDirectoryW(...), DeleteFileW(...)
#include "..\include\export_symbol.h"
#include "..\include\def_file_generator.h"
#include "..\include\asm_file_generator.h"
#include "..\include\cpp_main_file_generator.h"
#include "..\include\h_exports_list_file_generator.h"
#include "..\include\cpp_exports_list_file_generator.h"

// this controls the output of the generated code in many places
static char isX64Module                        = 0;

wchar_t originalDLL[MAX_PATH]                  = { 0 }; // full path and name of the original dll
errno_t fileOpenErr                            = 1;     // error object

time_t rawTime                                 = 0;
struct tm utcTime                              = { 0 };

wchar_t cloneDllProjectFolder[MAX_PATH]        = { 0 };
wchar_t cloneDllProjectIncludeFolder[MAX_PATH] = { 0 };
wchar_t cloneDllProjectSrcFolder[MAX_PATH]     = { 0 };

wchar_t exportsTempFile[MAX_PATH]              = { 0 };
wchar_t dumpbinCMD[2048]                       = { 0 };

wchar_t cloneDLL_def[MAX_PATH]                 = { 0 };
wchar_t cloneDLL_asm[MAX_PATH]                 = { 0 };
wchar_t cloneDLL_cppMain[MAX_PATH]             = { 0 };
wchar_t cloneDLL_hExportsList[MAX_PATH]        = { 0 };
wchar_t cloneDLL_cppExportsList[MAX_PATH]      = { 0 };

FILE* hFileExportsTemp                         = NULL;
FILE* hFileDef                                 = NULL;
FILE* hFileAsm                                 = NULL;
FILE* hFileCppMain                             = NULL;
FILE* hFileHExportsList                        = NULL;
FILE* hFileCppExportsList                      = NULL;


// true if all chars are hex
char IsHexNumStrW(wchar_t* str)
{
   if (*str == NULL)
   {
      return 0;
   }

   while (*str)
   {
      if (!((*str >= '0') && (*str <= '9')) && // not numeric
         !((*str >= 'a') && (*str <= 'f')) && // and not hex lowercase
         !((*str >= 'A') && (*str <= 'F')))   // and not hex uppercase
      {
         return 0;
      }
      str++;
   }
   return 1;
}

int __cdecl wmain(int argc, wchar_t* argv[])
{
   // file handle to the original dll, used just for checking
   FILE * originalDLL_file = NULL;

   // if parameters are provided
   if (argc >= 2)
   {
      // dll file
      swprintf_s(originalDLL, _countof(originalDLL), L"%s", argv[1]); // read the 1st arg
      fileOpenErr = _wfopen_s(&originalDLL_file, originalDLL, L"r");  // try to open the file with read access
      if (fileOpenErr) // if an error occurred
      {
         return 1;
      }

      fclose(originalDLL_file);  // close the file handle (we're not interested in it)

      // if a 2nd arg is provided
      if (argc >= 3)
      {
          // whether we should generate code for x86 (0) or x64 (1)
          if ((*argv[3] == L'1') || (*argv[3] == L'y') || (*argv[3] == L'Y'))
          {
              isX64Module = 1;
          }
          else
          {
              isX64Module = 0;
          }
      }

   }
   else // if no input parameters are provided
   {
      // get module name (with full path or relative)
      do
      {
         printf_s("DLL full path and name (with extension):\n");
         wscanf_s(L"%[^\r\n]", originalDLL, _countof(originalDLL) - 1);
         // discard \n
         wscanf_s(L"%*c");
         fileOpenErr = _wfopen_s(&originalDLL_file, originalDLL, L"r");
         if (fileOpenErr) // if an error occurred
         {
            printf_s("  ERROR: couldn't open file, error code = %d\n\n", fileOpenErr);
         }
         else // if it succeeded
         {
            fclose(originalDLL_file); // close the file handle (we're not interested in it)
         }
      }
      while (fileOpenErr);

      printf_s("\n");

      wchar_t choice[2] = { 0 };

      // ask whether to generate x86 or x64 code
      do
      {
         printf_s("Generate code for x64 module ? [1|0|Y|N]\n");
         wscanf_s(L"%1s%*[^\n]", choice, _countof(choice));
         if ((*choice == L'1') || (*choice == L'y') || (*choice == L'Y'))
         {
            isX64Module = 1;
         }
         else if ((*choice == L'0') || (*choice == L'n') || (*choice == L'N'))
         {
            isX64Module = 0;
         }
         else
         {
            printf_s("  ERROR: invalid choice\n\n");
         }
      }
      while ( (*choice != L'1') && (*choice != L'0') && 
              (*choice != L'y') && (*choice != L'Y') && 
              (*choice != L'n') && (*choice != L'N') );
   }

   // get time in UTC standard
   time(&rawTime); // get time
   gmtime_s(&utcTime, &rawTime); // convert time to UTC

   // get just the DLL name from the full path
   wchar_t* dll_name = NULL;
   wchar_t dll_full_path_filename[MAX_PATH] = { 0 };
   GetFullPathNameW(originalDLL, _countof(dll_full_path_filename), dll_full_path_filename, &dll_name);

   // get full path of current running process
   wchar_t current_app_path[MAX_PATH];
   DWORD chars_read = GetModuleFileNameW(NULL, current_app_path, _countof(current_app_path));
   // walk backwards until 1st backslash or beginning of string
   for (wchar_t* current_app_path_end = current_app_path + chars_read;
        current_app_path_end != current_app_path;
        current_app_path_end--)
   {
      // if we stopped at a backslash, then it's the end of the path
      if (*current_app_path_end == L'\\')
      {
         *current_app_path_end = NULL;
         break;
      }
   }

   // create a new dir for the output
   swprintf_s(cloneDllProjectFolder, _countof(cloneDllProjectFolder), 
              L"%s\\%s-%s-%d_%d_%d", 
              current_app_path,
              dll_name,
              (isX64Module) ? L"x64" : L"x86", 
              utcTime.tm_hour, 
              utcTime.tm_min, 
              utcTime.tm_sec);
   
   // subdir for the include folder
   swprintf_s(cloneDllProjectIncludeFolder, _countof(cloneDllProjectIncludeFolder),
              L"%s\\%s-%s-%d_%d_%d\\include", 
              current_app_path,
              dll_name,
              (isX64Module) ? L"x64" : L"x86", 
              utcTime.tm_hour, 
              utcTime.tm_min, 
              utcTime.tm_sec);
   
   // subdir for the src folder
   swprintf_s(cloneDllProjectSrcFolder, _countof(cloneDllProjectSrcFolder),
              L"%s\\%s-%s-%d_%d_%d\\src", 
              current_app_path,
              dll_name,
              (isX64Module) ? L"x64" : L"x86", 
              utcTime.tm_hour, 
              utcTime.tm_min, 
              utcTime.tm_sec);

   if (!CreateDirectoryW(cloneDllProjectFolder, NULL))
   {
      return 1;
   }

   if (!CreateDirectoryW(cloneDllProjectIncludeFolder, NULL))
   {
      return 1;
   }

   if (!CreateDirectoryW(cloneDllProjectSrcFolder, NULL))
   {
      return 1;
   }



   // create exports file
   swprintf_s(exportsTempFile, _countof(exportsTempFile), L"%s\\_temp_exports.txt", cloneDllProjectFolder);

   // create the cmd line for dumpbin.exe tool and invoke it
   swprintf_s(dumpbinCMD, _countof(dumpbinCMD), L"cmd.exe /c \"\"%s\\dumpbin_tool\\dumpbin.exe\" /exports \"%s\" /out:\"%s\" >nul 2>&1\"", 
                                                  current_app_path, 
                                                  originalDLL, 
                                                  exportsTempFile);
   _wsystem(dumpbinCMD);

   // create .cpp, .def, and .asm files, then open them for read/write
   swprintf_s(cloneDLL_def, _countof(cloneDLL_def), L"%s\\dll_exports.def", cloneDllProjectFolder);
   swprintf_s(cloneDLL_asm, _countof(cloneDLL_asm), L"%s\\dll_hooks.asm", cloneDllProjectSrcFolder);
   swprintf_s(cloneDLL_cppMain, _countof(cloneDLL_cppMain), L"%s\\dll_main.cpp", cloneDllProjectSrcFolder);
   swprintf_s(cloneDLL_hExportsList, _countof(cloneDLL_hExportsList), L"%s\\dll_exports_list.h", cloneDllProjectIncludeFolder);
   swprintf_s(cloneDLL_cppExportsList, _countof(cloneDLL_cppExportsList), L"%s\\dll_exports_list.cpp", cloneDllProjectSrcFolder);

   fileOpenErr = _wfopen_s(&hFileExportsTemp, exportsTempFile, L"r");
   if (fileOpenErr)
   {
      return 1;
   }

   fileOpenErr = _wfopen_s(&hFileDef, cloneDLL_def, L"w");
   if (fileOpenErr)
   {
      return 1;
   }

   fileOpenErr = _wfopen_s(&hFileAsm, cloneDLL_asm, L"w");
   if (fileOpenErr)
   {
      return 1;
   }

   fileOpenErr = _wfopen_s(&hFileCppMain, cloneDLL_cppMain, L"w");
   if (fileOpenErr)
   {
      return 1;
   }

   fileOpenErr = _wfopen_s(&hFileHExportsList, cloneDLL_hExportsList, L"w");
   if (fileOpenErr)
   {
      return 1;
   }

   fileOpenErr = _wfopen_s(&hFileCppExportsList, cloneDLL_cppExportsList, L"w");
   if (fileOpenErr)
   {
      return 1;
   }

   // parse exports/symbols from file to a list
   while (!feof(hFileExportsTemp))
   {
      wchar_t line[1024] = { 0 };

      if (fgetws(line, _countof(line), hFileExportsTemp)) // if reading a complete line from the exports file succeeded
      {
         wchar_t ordinal_str[10] = { 0 };
         wchar_t hint_str[256]   = { 0 };
         wchar_t rva_str[256]    = { 0 };
         wchar_t name_str[256]   = { 0 };

         swscanf_s(line, L"%s%s%s%s", 
                         ordinal_str, _countof(ordinal_str),
                         hint_str,    _countof(hint_str),
                         rva_str,     _countof(rva_str),
                         name_str,    _countof(name_str));

         if (IsHexNumStrW(hint_str)) // this eliminates/skips the header dumped by the tool
         {
            Symbol_t newSymbol = { 0 };

            // conver the ordinal number from string to int
            swscanf_s(ordinal_str, L"%u", &newSymbol.ordinal);

            if (IsHexNumStrW(rva_str))  // proper format
            {
               // read the export/symbol name
               swscanf_s(name_str, L"%s", newSymbol.name, _countof(newSymbol.name));
            }
            else // no hint or no RVA
            {
               // read the export/symbol name (it will be stored in the rva buffer)
               swscanf_s(rva_str, L"%s", newSymbol.name, _countof(newSymbol.name));
            }

            // if no symbol name (by ordinal only)
            if (0 == _wcsicmp(newSymbol.name, L"[NONAME]"))
            {
               newSymbol.isUnnamed = 1;
            }

            // put this symbol/export to the list
            exportsList.push_back(std::move(newSymbol));
         }
      }
   }

   // generate .def file
   GenerateDefFile(hFileDef, dll_name);

   // generate .asm file
   GenerateAsmFile(hFileAsm, isX64Module);

   // generate .cpp main file
   GenerateCppMainFile(hFileCppMain, dll_name, isX64Module);

   // generate .h exports list file
   GenerateHExportsListFile(hFileHExportsList);

   // generate .cpp exports list file
   GenerateCppExportsListFile(hFileCppExportsList);

   // close all files handles
   fclose(hFileExportsTemp);
   fclose(hFileDef);
   fclose(hFileAsm);
   fclose(hFileCppMain);
   fclose(hFileHExportsList);
   fclose(hFileCppExportsList);

   DeleteFileW(exportsTempFile);

   return 0;
}

