#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "context.hpp"
#include "sass_functions.h"

extern "C" {
  using namespace std;

  // Struct to hold custom function callback
  struct Sass_C_Function_Descriptor {
    const char*     signature;
    Sass_C_Function function;
    void*           cookie;
  };

  Sass_C_Function_List sass_make_function_list(size_t length)
  {
    return (Sass_C_Function_List) calloc(length + 1, sizeof(Sass_C_Function_Callback));
  }

  Sass_C_Function_Callback sass_make_function(const char* signature, Sass_C_Function function, void* cookie)
  {
    Sass_C_Function_Callback cb = (Sass_C_Function_Callback) calloc(1, sizeof(Sass_C_Function_Descriptor));
    if (cb == 0) return 0;
    cb->signature = signature;
    cb->function = function;
    cb->cookie = cookie;
    return cb;
  }

  // Setters and getters for callbacks on function lists
  Sass_C_Function_Callback sass_function_get_list_entry(Sass_C_Function_List* list, size_t pos) { return *list[pos]; }
  void sass_function_set_list_entry(Sass_C_Function_List* list, Sass_C_Function_Callback cb, size_t pos) { *list[pos] = cb; }

  const char* sass_function_get_signature(Sass_C_Function_Callback fn) { return fn->signature; }
  Sass_C_Function sass_function_get_function(Sass_C_Function_Callback fn) { return fn->function; }
  void* sass_function_get_cookie(Sass_C_Function_Callback fn) { return fn->cookie; }

  // External import entry
  struct Sass_Import {
    char* path;
    char* source;
    char* srcmap;
  };

  // Struct to hold importer callback
  struct Sass_C_Import_Descriptor {
    Sass_C_Import_Fn function;
    void*            cookie;
  };

  Sass_C_Import_Callback sass_make_importer(Sass_C_Import_Fn function, void* cookie)
  {
    Sass_C_Import_Callback cb = (Sass_C_Import_Callback) calloc(1, sizeof(Sass_C_Import_Descriptor));
    if (cb == 0) return 0;
    cb->function = function;
    cb->cookie = cookie;
    return cb;
  }

  Sass_C_Import_Fn sass_import_get_function(Sass_C_Import_Callback fn) { return fn->function; }
  void* sass_import_get_cookie(Sass_C_Import_Callback fn) { return fn->cookie; }

  // Creator for sass custom importer return argument list
  struct Sass_Import** sass_make_import_list(size_t length)
  {
    return (Sass_Import**) calloc(length + 1, sizeof(Sass_Import*));
  }

  // Creator for a single import entry returned by the custom importer inside the list
  // We take ownership of the memory for source and srcmap (freed when context is destroyd)
  struct Sass_Import* sass_make_import_entry(const char* path, char* source, char* srcmap)
  {
    Sass_Import* v = (Sass_Import*) calloc(1, sizeof(Sass_Import));
    if (v == 0) return 0;
    v->path = strdup(path);
    v->source = source;
    v->srcmap = srcmap;
    return v;
  }

  // Setters and getters for entries on the import list
  void sass_import_set_list_entry(struct Sass_Import** list, size_t idx, struct Sass_Import* entry) { list[idx] = entry; }
  struct Sass_Import* sass_import_get_list_entry(struct Sass_Import** list, size_t idx) { return list[idx]; }

  // Deallocator for the allocated memory
  void sass_delete_import_list(struct Sass_Import** list)
  {
    struct Sass_Import** it = list;
    if (list == 0) return;
    while(*list) {
      free((*list)->path);
      free((*list)->source);
      free((*list)->srcmap);
      free(*list);
      ++list;
    }
    free(it);
  }

  // Getter for import entry
  const char* sass_import_get_path(struct Sass_Import* entry) { return entry->path; }
  const char* sass_import_get_source(struct Sass_Import* entry) { return entry->source; }
  const char* sass_import_get_srcmap(struct Sass_Import* entry) { return entry->srcmap; }

  // Explicit functions to take ownership of the memory
  // Resets our own property since we do not know if it is still alive
  char* sass_import_take_source(struct Sass_Import* entry) { char* ptr = entry->source; entry->source = 0; return ptr; }
  char* sass_import_take_srcmap(struct Sass_Import* entry) { char* ptr = entry->srcmap; entry->srcmap = 0; return ptr; }

}
