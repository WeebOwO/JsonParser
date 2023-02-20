#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

enum class JsonType : uint32_t {
    Null = 0,
    False,
    True,
    Number,
    String,
    Array,
    Object,
};

enum class ParseState : uint32_t {
    ParseOk = 0,
    ParseEexpectValue,
    ParseInvalidValue,
    ParseRootNotSingular,
    ParseMissQuotationMark
};

struct JsonNode {
    JsonType                               valueType;
    std::variant<double, std::string>      val;
    std::vector<std::unique_ptr<JsonNode>> childs;
    // we can use std::vector, but use hashmap for quick lookup
    std::unordered_map<std::string, std::unique_ptr<JsonNode>> objectDict;
};

struct JsonContext {
    std::string_view json;
};

namespace json {
ParseState ParseJson(JsonNode* node, std::string_view json);
JsonType   GetType(JsonNode* node);
double     GetValue(JsonNode* node);
} // namespace json
