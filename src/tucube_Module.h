#ifndef _TUCUBE_MODULE_H
#define _TUCUBE_MODULE_H

#include <jansson.h>
#include <pthread.h>
#include <libgenc/genc_Tree.h>
#include <libgenc/genc_Generic.h>

struct tucube_Config {
    json_t* json;
};

struct tucube_Module {
    const char* name;
    void* dlHandle;
    union tucube_Generic generic;
    pthread_rwlock_t* rwLock;
    pthread_key_t* tlModuleKey;
    GENC_TREE_NODE(struct tucube_Module);
};

#define TUCUBE_MODULE_DLOPEN(config, moduleName, module)                                                      \
do {                                                                                                          \
    const char* modulePath;                                                                                   \
    if((modulePath = json_string_value(json_object_get(json_object_get(config, moduleName), "path")) == NULL) \
        errx(EXIT_FAILURE, "%s: %u: Unable to find path of module %s", __FILE__, __LINE__, moduleName);       \
    if(((module)->dlHandle = dlopen(modulePath, RTLD_LAZY | RTLD_GLOBAL)) == NULL)                            \
        errx(EXIT_FAILURE, "%s: %u: Failed to load next module", __FILE__, __LINE__);                         \
} while(0)

#define TUCUBE_MODULE_DLSYM(module, moduleType, moduleFunction)                 \
do {                                                                            \
    if((GENC_CAST((module)->generic.pointer,                                    \
         moduleType*)->moduleFunction =                                         \
              dlsym((module)->dlHandle, #moduleFunction)) == NULL) {            \
        errx(EXIT_FAILURE,                                                      \
             "%s: %u: Unable to find "#moduleFunction"()", __FILE__, __LINE__); \
    }                                                                           \
}                                                                               \
while(0)

#define TUCUBE_CONFIG_GET(config, moduleName, valueName, valueType, output, defaultValue)                                         \
do {                                                                                                                              \
    json_t* jsonOutput;                                                                                                           \
    if((jsonOutput = json_object_get(json_object_get(json_object_get((config)->json, moduleName), "config"), valueName)) != NULL) \
        *(output) = json_##valueType##_value(jsonOutput);                                                                         \
    else                                                                                                                          \
        *(output) = defaultValue;                                                                                                 \
} while(0)

#define TUCUBE_CONFIG_GET_REQUIRED(config, moduleName, valueName, valueType, output)                                                  \
do {                                                                                                                                  \
    json_t* moduleConfig;                                                                                                             \
    if((moduleConfig = json_object_get(json_object_get(json_object_get((config)->json, moduleName)), "config")) != NULL)              \
        *(output) = json_##valueType##_value(moduleConfig, valueName);                                                                \
    else                                                                                                                              \
        errx(EXIT_FAILURE, "%s: %u: In module %s, configuration argument %s is required", __FILE__, __LINE__, moduleName, valueName); \
} while(0)

#define TUCUBE_CONFIG_GET_MODULE_PATH(config, moduleName, modulePath)                                        \
do {                                                                                                         \
    *(modulePath) = json_string_value(json_object_get(json_object_get((config)->json, moduleName), "path")); \
} while(0)

#define TUCUBE_CONFIG_GET_NEXT_MODULE_COUNT(config, currentModuleName, output)                   \
do {                                                                                             \
    json_t* array = json_object_get(json_object_get((config)->json, currentModuleName), "next"); \
    if(json_is_array(array))                                                                     \
        *output = json_array_size(array);                                                        \
    else                                                                                         \
        *output = -1;                                                                            \
} while(0);

#define TUCUBE_CONFIG_FOR_EACH_NEXT_MODULE_BEGIN(config, currentModuleName)                                \
do {                                                                                                       \
    json_t* nextModuleNames = json_object_get(json_object_get((config)->json, currentModuleName), "next"); \
    if(json_is_array(nextModuleNames)) {                                                                   \
        size_t index;                                                                                      \
        json_t* nextModuleNameJson;                                                                        \
        json_array_foreach(nextModuleNames, index, nextModuleNameJson) {                                   \
            const char* nextModuleName = json_string_value(nextModuleNameJson);

#define TUCUBE_CONFIG_FOR_EACH_NEXT_MODULE_END \
        }                                      \
    }                                          \
} while(0);

#endif
