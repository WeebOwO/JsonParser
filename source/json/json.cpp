#include "json.h"

#include <cassert>

static void Expect(JsonContext& context, char ch) {
    assert(context.json.front() == ch);
    context.json.remove_prefix(1);
    return;
}

static void ParseWhiteSpace(JsonContext& context) {
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

static ParseState ParseNull(JsonContext context, JsonNode* node) {
    Expect(context, 'n');
    if (context.json[0] != 'u' || context.json[1] != 'l' || context.json[2] != 'l') {
        return ParseState::ParseInvalidValue;
    }
    context.json.remove_prefix(3);
    node->valueType = JsonType::Null;
    return ParseState::ParseOk;
}

static ParseState ParseTrue(JsonContext context, JsonNode* node) {
    Expect(context, 't');
    if (context.json[0] != 'r' || context.json[1] != 'u' || context.json[2] != 'e') {
        return ParseState::ParseInvalidValue;
    }
    context.json.remove_prefix(3);
    node->valueType = JsonType::True;
    return ParseState::ParseOk;
}

static ParseState ParseFalse(JsonContext context, JsonNode* node) {
    Expect(context, 'f');
    if (context.json[0] != 'a' || context.json[1] != 'l' || context.json[2] != 's' ||
        context.json[3] != 'e') {
        return ParseState::ParseInvalidValue;
    }
    context.json.remove_prefix(4);
    node->valueType = JsonType::False;
    return ParseState::ParseOk;
}

static ParseState ParseValue(JsonContext context, JsonNode* node) {
    if (context.json.empty()) return ParseState::ParseOk;

    switch (context.json.front()) {
    case 'n':
        return ParseNull(context, node);
    case 't':
        return ParseTrue(context, node);
    case 'f':
        return ParseFalse(context, node);
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
    return ParseValue(context, node);
}

JsonType GetType(JsonNode* node) { return node->valueType; }

} // namespace json