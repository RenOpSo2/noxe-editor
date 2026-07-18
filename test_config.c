#include "src/config.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

int main() {
    printf("Running config system tests...\n");

    // Test 1: init defaults
    config_init();
    assert(config_get_number("tabsize", 4) == 4.0);
    assert(config_get_bool("mouse", 0) == 1);
    printf("Test 1 (Defaults): Passed.\n");

    // Test 2: Validation
    SchemaError err;
    assert(config_validate("tabsize", "abc", &err) == 0);
    assert(config_validate("tabsize", "4", NULL) == 1);
    assert(config_validate("mouse", "invalid", &err) == 0);
    assert(config_validate("mouse", "true", NULL) == 1);
    printf("Test 2 (Validation): Passed.\n");

    // Test 3: Load from CLI overrides
    char* test_argv[] = {"./noxe", "--tabsize", "8", "--mouse", "false", "--custom_theme", "dracula"};
    int test_argc = sizeof(test_argv) / sizeof(test_argv[0]);
    config_load(test_argc, test_argv);
    
    assert(config_get_number("tabsize", 4) == 8.0);
    assert(config_get_bool("mouse", 1) == 0);
    assert(strcmp(config_get_string("custom_theme", ""), "dracula") == 0);
    printf("Test 3 (CLI Override): Passed.\n");

    // Test 4: Set and file write
    // This will write to ./noxe.json
    config_set_number("tabsize", 2);
    config_set_bool("mouse", 1);
    
    // Read directly from file to verify JSON format
    FILE* f = fopen("./noxe.json", "r");
    assert(f != NULL);
    char buf[1024];
    size_t r = fread(buf, 1, sizeof(buf) - 1, f);
    buf[r] = '\0';
    fclose(f);
    
    assert(strstr(buf, "\"tabsize\": 2") != NULL);
    assert(strstr(buf, "\"mouse\": true") != NULL);
    printf("Test 4 (File Persistence/JSON format): Passed.\n");

    // Clean up
    unlink("./noxe.json");

    printf("All config system tests passed successfully!\n");
    return 0;
}
