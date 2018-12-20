#include "catch.hpp"
#include "feature.h"

using Catch::Equals;

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof(*(arr)))

static int32_t find_index(uint32_t count, const char** array, const char* value) {
  for (uint32_t index = 0; index < count; ++index) {
    if (0 == strcmp(array[index], value)) {
      return (int32_t)index;
    }
  }
  return -1;
}

// init/shutdown helper if an exception gets thrown
struct init_t {
  init_t(const feature_lib_config_t* config) {
    feature_lib_config_t config_default;
    if (!config) {
      feature_lib_config_init(&config_default);
      config = &config_default;
    }
    feature_lib_init(config);
  }
  init_t(uint32_t count, const char** names, const bool* values) {
    feature_lib_config_t config;
    feature_lib_config_init(&config);
    config.feature_override_count = count;
    config.feature_override_names = names;
    config.feature_override_values = values;
    feature_lib_init(&config);
  }
  ~init_t() {
    feature_lib_shutdown();
  }
};

TEST_CASE("feature") {
  feature_t with_cats = {
    "with-cats",
    true,
  };
  feature_t with_dogs = {
    "with-dogs",
    false,
  };

  SECTION("it uses the default values") {
    init_t init(nullptr);

    uint32_t override_count;
    const char** override_names;
    const bool* override_values;
    feature_get_overrides(&override_count, &override_names, &override_values);
    CHECK(override_count == 0);
    CHECK(feature_is_enabled(&with_cats));
    CHECK(!feature_is_enabled(&with_dogs));
  }

  SECTION("it allows an override to the same value") {
    const char* names[] = {
      "with-cats",
      "with-dogs",
    };
    const bool values[] = {
      true,
      false,
    };
    init_t init(ARRAY_COUNT(names), names, values);
    uint32_t override_count;
    const char** override_names;
    const bool* override_values;
    feature_get_overrides(&override_count, &override_names, &override_values);
    const int32_t index_cats = find_index(override_count, override_names, "with-cats");
    const int32_t index_dogs = find_index(override_count, override_names, "with-dogs");
    CHECK(override_count == 2);
    CHECK_THAT(override_names[index_cats], Equals("with-cats"));
    CHECK(override_values[index_cats] == true);
    CHECK_THAT(override_names[index_dogs], Equals("with-dogs"));
    CHECK(override_values[index_dogs] == false);
    CHECK(feature_is_enabled(&with_cats));
    CHECK(!feature_is_enabled(&with_dogs));
  }

  SECTION("it allows an override to the opposite value") {
    const char* names[] = {
      "with-cats",
      "with-dogs",
    };
    const bool values[] = {
      false,
      true,
    };
    init_t init(ARRAY_COUNT(names), names, values);
    uint32_t override_count;
    const char** override_names;
    const bool* override_values;
    feature_get_overrides(&override_count, &override_names, &override_values);
    const int32_t index_cats = find_index(override_count, override_names, "with-cats");
    const int32_t index_dogs = find_index(override_count, override_names, "with-dogs");
    CHECK(override_count == 2);
    CHECK_THAT(override_names[index_cats], Equals("with-cats"));
    CHECK(override_values[index_cats] == false);
    CHECK_THAT(override_names[index_dogs], Equals("with-dogs"));
    CHECK(override_values[index_dogs] == true);
    CHECK(!feature_is_enabled(&with_cats));
    CHECK(feature_is_enabled(&with_dogs));
  }
}
