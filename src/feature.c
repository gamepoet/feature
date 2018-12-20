#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "feature.h"

#define feature_assert(expr, message) ((expr) ? true : (s_config.assert(__FILE__, __LINE__, __func__, #expr, message), false))
#define feature_alloc(size) s_config.alloc(size, s_config.alloc_user_data, __FILE__, __LINE__, __func__)
#define feature_free(ptr) s_config.free(ptr, s_config.alloc_user_data, __FILE__, __LINE__, __func__)

// the configuration used to initialize this library
static feature_lib_config_t s_config;

static void default_assert(const char* file, int line, const char* func, const char* expression, const char* message) {
  fprintf(stderr, "ASSERT FAILURE: %s\n%s\nfile: %s\nline: %d\nfunc: %s\n", message, expression, file, line, func);
  exit(EXIT_FAILURE);
}

static void* default_alloc(size_t size, void* user_data, const char* file, int line, const char* func) {
  return malloc(size);
}

static void default_free(void* ptr, void* user_data, const char* file, int line, const char* func) {
  free(ptr);
}

void feature_lib_config_init(feature_lib_config_t* config) {
  if (config == NULL) {
    return;
  }

  config->feature_override_count = 0;
  config->feature_override_names = NULL;
  config->feature_override_values = NULL;
  config->assert = &default_assert;
  config->alloc = &default_alloc;
  config->free = &default_free;
  config->alloc_user_data = NULL;
}

void feature_lib_init(const feature_lib_config_t* config) {
  if (config != NULL) {
    s_config = *config;
  }
  else {
    feature_lib_config_init(&s_config);
  }

  // make a copy of the feature list data
  if (s_config.feature_override_count > 0) {
    char** names = (char**)feature_alloc(s_config.feature_override_count * sizeof(char*));
    bool* values = (bool*)feature_alloc(s_config.feature_override_count * sizeof(bool));
    for (uint32_t index = 0; index < s_config.feature_override_count; ++index) {
      // copy the name
      const uint32_t name_size_bytes = strlen(s_config.feature_override_names[index]) + 1;
      names[index] = (char*)feature_alloc(name_size_bytes);
      memmove(names[index], s_config.feature_override_names[index], name_size_bytes);

      // copy the value
      values[index] = s_config.feature_override_values[index];
    }

    // overwrite the pointers in our local copy of the config object
    s_config.feature_override_names = (const char**)names;
    s_config.feature_override_values = values;
  }
}

void feature_lib_shutdown() {
  for (uint32_t index = 0; index < s_config.feature_override_count; ++index) {
    feature_free((char*)s_config.feature_override_names[index]);
  }
  feature_free((char**)s_config.feature_override_names);
  feature_free((bool*)s_config.feature_override_values);
  memset(&s_config, 0, sizeof(s_config));
}

bool feature_is_enabled(const feature_t* feature) {
  feature_assert(feature != NULL, "feature cannot be NULL");

  // check if this feature was overridden
  // TODO: can we hash the names to avoid all the string compares?
  for (uint32_t index = 0; index < s_config.feature_override_count; ++index) {
    const char* entry_name = s_config.feature_override_names[index];
    if (0 == strcmp(entry_name, feature->name)) {
      return s_config.feature_override_values[index];
    }
  }

  return feature->enabled_by_default;
}

void feature_get_overrides(uint32_t* count, const char*** names, const bool** values) {
  feature_assert(count != NULL, "count cannot be NULL");
  feature_assert(names != NULL, "names cannot be NULL");
  feature_assert(values != NULL, "values cannot be NULL");

  *count = s_config.feature_override_count;
  *names = s_config.feature_override_names;
  *values = s_config.feature_override_values;
}
