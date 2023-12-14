#ifndef EXPORT_SYMBOL_H
#define EXPORT_SYMBOL_H


#include <vector>


// struct that represents each export
typedef struct
{
   unsigned int ordinal; // ordinal number
   wchar_t name[256];    // export name
   char isUnnamed;       // does the export have a name, or only ordinal number
} Symbol_t;

// extern the exports list for other files
extern std::vector<Symbol_t> exportsList;


#endif /* EXPORT_SYMBOL_H */

