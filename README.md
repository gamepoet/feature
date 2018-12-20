# feature

Feature is a simple API for a feature flags concept. You can define features that can be enabled at library initialization and then easily query later to see if that feature is enabled.

The basic architecture is that at library initialization time you specify any overrides to the enabled status of any features. At this point the overrides from the feature default enabled states are committed and cannot be changed. This allows the querying of whether the feature is enabled to be trivially thread-safe since all the data at that point is read-only.

## Usage

```c
#include <stdlib.h>
#include <feature.h>

static feature_t s_my_feature = { "my-feature", false };

void func() {
  // These values could come from command line arguments or some config file.
  const char* override_names[] = {
    "my-feature",
  };
  const bool override_values[] = {
    true,
  };

  // Initialize the library with specific feature overrides (if any).
  feature_lib_config_t config;
  feature_lib_config_init(&config);
  config.feature_override_count = 1;
  config.feature_override_names = override_names;
  config.feature_override_values = override_values;
  feature_lib_init(&config);

  // Test if a feature is enabled
  if (feature_is_enabled(&s_my_feature)) {
    printf("my feature is enabled!\n");
  }
  else {
    printf("my feature is *NOT* enabled!\n");
  }

  // Teardown the library when all done
  feature_lib_shutdown();
}
```

## Compiling

This project uses CMake for compilation. Make sure it's installed and in your path.

```bash
$ ./s/setup
$ ./s/build
```
