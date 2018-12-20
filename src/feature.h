#pragma once
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct feature_t {
  // The name of the feature; used as a unique id
  const char* name;

  // Is this feature enabled or disabled by default.
  bool enabled_by_default;
} feature_t;

typedef struct feature_lib_config_t {
  // The number of features for which to override values.
  uint32_t feature_override_count;

  // The list of names for each feature to override. Count should be `feature_override_count`.
  const char** feature_override_names;

  // The list of boolean enabled values for each overridden feature. Count should be `feature_override_count`.
  const bool* feature_override_values;

  // The function to use when an assertion fails
  void (*assert)(const char* file, int line, const char* func, const char* expression, const char* message);

  // The function to use to allocate memory
  void* (*alloc)(size_t size, void* user_data, const char* file, int line, const char* func);

  // The function to use to free memory
  void (*free)(void* ptr, void* user_data, const char* file, int line, const char* func);

  // Opaque data passed to the alloc and free functions.
  void* alloc_user_data;
} feature_lib_config_t;

// Initializes the given config struct to fill it in with the default values.
void feature_lib_config_init(feature_lib_config_t* config);

// Initializes this library.
void feature_lib_init(const feature_lib_config_t* config);

// Tears down this library and frees all allocations.
void feature_lib_shutdown();

// Tests if the given feature is enabled.
bool feature_is_enabled(const feature_t* feature);

// Gets a list of all the features that are overridden.
void feature_get_overrides(uint32_t* override_count, const char*** override_names, const bool** override_values);

#ifdef __cplusplus
}
#endif
