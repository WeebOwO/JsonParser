#include "json.h"

#include <cassert>
#include <exception>

static void Expect(JsonContext& context, char ch) {
    assert(context.json.front() == ch);
    context.json.remove_prefix(1);
    return;
}

static void ParseWhiteSpace(JsonContext& context) {
    if (context.json.empty()) return;
    uint32_t cnt = 0;
    for (const auto& ch : context.json) {
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            cnt++;
        } else {
            break;
        }
    }
    context.json.remove_prefix(cnt);
}

static ParseState ParseNull(JsonContext& context, JsonNode* node) {
    Expect(context, 'n');
    if (context.json[0] != 'u' || context.json[1] != 'l' || context.json[2] != 'l') {
        return ParseState::ParseInvalidValue;
    }
    context.json.remove_prefix(3);
    node->valueType = JsonType::Null;
    return ParseState::ParseOk;
}

static ParseState ParseTrue(JsonContext& context, JsonNode* node) {
    Expect(context, 't');
    if (context.json[0] != 'r' || context.json[1] != 'u' || context.json[2] != 'e') {
        return ParseState::ParseInvalidValue;
    }
    context.json.remove_prefix(3);
    node->valueType = JsonType::True;
    return ParseState::ParseOk;
}

static ParseState ParseFalse(JsonContext& context, JsonNode* node) {
    Expect(context, 'f');
    if (context.json[0] != 'a' || context.json[1] != 'l' || context.json[2] != 's' ||
        context.json[3] != 'e') {
        return ParseState::ParseInvalidValue;
    }
    context.json.remove_prefix(4);
    node->valueType = JsonType::False;
    return ParseState::ParseOk;
}

static ParseState ParseNumber(JsonContext& context, JsonNode* node) {
    std::string temp(context.json.cbegin(), context.json.cend());
    size_t      idx = -1;
    double      val;
    try {
        val = std::stod(temp, &idx);
    } catch (std::exception& e) { return ParseState::ParseInvalidValue; }
    node->val = val;
    context.json.remove_prefix(idx);
    node->valueType = JsonType::Number;
    return ParseState::ParseOk;
}

static ParseState ParseString(JsonContext& context, JsonNode* node) {
    Expect(context, '"');
    std::string parsedString;
    int         cnt = 0;
    for (const auto& ch : context.json) {
        switch (ch) {
        case '\"': {
            node->val = parsedString;
            context.json.remove_prefix(cnt + 1);
            node->valueType = JsonType::String;
            return ParseState::ParseOk;
        }
        default:
            parsedString.push_back(ch);
        }
        ++cnt;
    }
    return ParseState::ParseMissQuotationMark;
}

static ParseState ParseValue(JsonContext& context, JsonNode* node);

static ParseState ParseArray(JsonContext& context, JsonNode* node) {
    Expect(context, '[');

    if (context.json.front() == ']') {
        context.json.remove_prefix(1);
        node->valueType = JsonType::Array;
        return ParseState::ParseOk;
    }

    std::vector<std::unique_ptr<JsonNode>> childs;

    while (true) {
        ParseState ret = ParseValue(context, node);
        if (ret != ParseState::ParseOk) { return ret; }
        auto child = std::make_unique<JsonNode>();

        // copy and swap
        child->val = node->val;
        child->childs.swap(node->childs);
        child->objectDict.swap(node->objectDict);
        child->valueType = node->valueType;

        childs.emplace_back(std::move(child));
        ParseWhiteSpace(context);
        if (context.json.front() == ',') {
            context.json.remove_prefix(1);
        } else if (context.json.front() == ']') {
            context.json.remove_prefix(1);
            node->childs.swap(childs);
            node->valueType = JsonType::Array;
            return ParseState::ParseOk;
        }
    }
}

static ParseState ParseObject(JsonContext& context, JsonNode* node) {
    Expect(context, '{');

    if (context.json.front() == '}') {
        node->valueType = JsonType::Object;
        context.json.remove_prefix(1);
        return ParseState::ParseOk;
    }

    std::unordered_map<std::string, std::unique_ptr<JsonNode>> dict;
    while (true) {
        ParseWhiteSpace(context);

        if (context.json.front() == '}') {
            node->valueType = JsonType::Object;
            node->objectDict.swap(dict);
            context.json.remove_prefix(1);
            return ParseState::ParseOk;
        }

        ParseState ret = ParseValue(context, node);
        if (ret != ParseState::ParseOk || node->valueType != JsonType::String) break;

        std::string key = std::get<std::string>(node->val);
        ParseWhiteSpace(context);
        if (context.json.front() == ':') { context.json.remove_prefix(1); }
        ParseWhiteSpace(context);

        ret = ParseValue(context, node);

        auto val = std::make_unique<JsonNode>();

        val->valueType = node->valueType;
        val->objectDict.swap(node->objectDict);
        val->childs.swap(node->childs);
        val->val  = node->val;
        dict[key] = std::move(val);

        if (context.json.front() == ',') context.json.remove_prefix(1);
    }

    return ParseState::ParseInvalidValue;
}

static ParseState ParseValue(JsonContext& context, JsonNode* node) {
    if (context.json.empty()) return ParseState::ParseEexpectValue;

    ParseWhiteSpace(context);
    switch (context.json.front()) {
    case 'n':
        return ParseNull(context, node);
    case 't':
        return ParseTrue(context, node);
    case 'f':
        return ParseFalse(context, node);
    case '"':
        return ParseString(context, node);
    case '[':
        return ParseArray(context, node);
    case '{':
        return ParseObject(context, node);
    default:
        return ParseNumber(context, node);
    }

    return ParseState::ParseInvalidValue;
}

namespace json {
ParseState ParseJson(JsonNode* node, std::string_view json) {
    JsonContext context;
    assert(node != nullptr);
    context.json    = json;
    node->valueType = JsonType::Null;
    ParseWhiteSpace(context);
    ParseState ret = ParseValue(context, node);
    if (ret == ParseState::ParseOk) {
        ParseWhiteSpace(context);
        if (!context.json.empty()) return ParseState::ParseRootNotSingular;
    }
    return ret;
}

JsonType GetType(JsonNode* node) {
    assert(node != nullptr);
    return node->valueType;
}

double GetValue(JsonNode* node) {
    assert(node != nullptr && node->valueType == JsonType::Number);
    return std::get<double>(node->val);
}

} // namespace json