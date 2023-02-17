#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

enum class JsonType : uint32_t {
    Null = 0,
    False,
    True,
    Number,
    String,
    Array,
    Obejct,
};

enum class ParseState : uint32_t {
    ParseOk = 0,
    ParseEexpectValue,
    ParseInvalidValue,
    ParseRootNotSingular
};

struct JsonNode {
    JsonType valueType;
};

struct JsonContext {
    std::string_view json;
};

namespace json {
ParseState ParseJson(JsonNode* node, std::string_view json);
JsonType   GetType(JsonNode* node);
} // namespace json
