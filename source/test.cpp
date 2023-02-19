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

#define TEST_NUMBER(expect, str) \
    do { \
        JsonNode v; \
        EXPECT_EQ_INT(ParseState::ParseOk, json::ParseJson(&v, str)); \
        EXPECT_EQ_INT(JsonType::Number, json::GetType(&v)); \
    } while (0)

#define TEST_ERROR(error, str) \
    do { \
        JsonNode v; \
        v.valueType = JsonType::False; \
        EXPECT_EQ_INT(error, json::ParseJson(&v, str)); \
        EXPECT_EQ_INT(JsonType::Null, json::GetType(&v)); \
    } while (0)

class Test {
public:
    Test()  = default;
    ~Test() = default;

    static void ParseNumberTest() {
        TEST_NUMBER(0.0, "0");
        TEST_NUMBER(0.0, "-0");
        TEST_NUMBER(0.0, "-0.0");
        TEST_NUMBER(1.0, "1");
        TEST_NUMBER(-1.0, "-1");
        TEST_NUMBER(1.5, "1.5");
        TEST_NUMBER(-1.5, "-1.5");
        TEST_NUMBER(3.1416, "3.1416");
        TEST_NUMBER(1E10, "1E10");
        TEST_NUMBER(1e10, "1e10");
        TEST_NUMBER(1E+10, "1E+10");
        TEST_NUMBER(1E-10, "1E-10");
        TEST_NUMBER(-1E10, "-1E10");
        TEST_NUMBER(-1e10, "-1e10");
        TEST_NUMBER(-1E+10, "-1E+10");
        TEST_NUMBER(-1E-10, "-1E-10");
        TEST_NUMBER(1.234E+10, "1.234E+10");
        TEST_NUMBER(1.234E-10, "1.234E-10");
    }

    static void ParseExpectValueTest() {
        TEST_ERROR(ParseState::ParseEexpectValue, "");
        TEST_ERROR(ParseState::ParseEexpectValue, " ");
    }

    static void ParseInvalidTest() {
        TEST_ERROR(ParseState::ParseInvalidValue, "+asd");
        TEST_ERROR(ParseState::ParseInvalidValue, "-eeed2");
        TEST_ERROR(ParseState::ParseInvalidValue, "-----2");
    }

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

    static void ParseStringTest() {
        JsonNode node;
        node.valueType = JsonType::Null;
        EXPECT_EQ_INT(ParseState::ParseOk, json::ParseJson(&node, "\"Hello\""));
        EXPECT_EQ_INT(JsonType::String, json::GetType(&node));
    }

    static void ParseMissQuotationTest() {
        TEST_ERROR(ParseState::ParseMissQuotationMark, "\"");
        TEST_ERROR(ParseState::ParseMissQuotationMark, "\"abc");
    }

    static void ParseNotSingularTest() {
        TEST_ERROR(ParseState::ParseRootNotSingular, "null x");
    }

    static void ParseArrayTest() {
        JsonNode node;
        node.valueType = JsonType::Null;
        EXPECT_EQ_INT(ParseState::ParseOk, json::ParseJson(&node, "[\"abc\", [1, 2], 3]"));
        EXPECT_EQ_INT(JsonType::Array, json::GetType(&node));
    }
    
    static void ParseTest() {
        ParseNullTest();
        ParseTrueTest();
        ParseFalseTest();
        ParseNumberTest();
        ParseExpectValueTest();
        ParseInvalidTest();
        ParseNotSingularTest();
        ParseStringTest();
        ParseMissQuotationTest();
        ParseArrayTest();
    }
};

int main() {
    Test::ParseTest();
    std::cout << std::format("{}/{} ({:.2f}%) passed\n", testPass, testCount,
                             testPass * 100.0 / testCount);
    
    return 0;
}