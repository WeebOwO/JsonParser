#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>

#include "json/json.h"

static int mainRet   = 0;
static int testCount = 0;
static int testPass  = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do { \
        testCount++; \
        if (equality) testPass++; \
        else { \
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, \
                    expect, actual); \
            mainRet = 1; \
        } \
    } while (0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

class Test {
public:
    Test()  = default;
    ~Test() = default;
    static void ParseNullTest() {
        JsonNode node;
        node.valueType = JsonType::False;
        EXPECT_EQ_INT(ParseState::ParseOk, json::ParseJson(&node, "null"));
        EXPECT_EQ_INT(JsonType::Null, json::GetType(&node));
    }

    static void ParseTrueTest() {
        JsonNode node;
        node.valueType = JsonType::Null;
        EXPECT_EQ_INT(ParseState::ParseOk, json::ParseJson(&node, "true"));
        EXPECT_EQ_INT(JsonType::True, json::GetType(&node));
    }

    static void ParseFalseTest() {
        JsonNode node;
        node.valueType = JsonType::Null;
        EXPECT_EQ_INT(ParseState::ParseOk, json::ParseJson(&node, "false"));
        EXPECT_EQ_INT(JsonType::False, json::GetType(&node));
    }

    static void ParseTest() {
        ParseNullTest();
        ParseTrueTest();
        ParseFalseTest();
    }
};

int main() {
    Test::ParseTest();
    std::cout << std::format("{}/{} ({:.2f}%) passed\n", testPass, testCount,
                             testPass * 100.0 / testCount);
    return 0;
}