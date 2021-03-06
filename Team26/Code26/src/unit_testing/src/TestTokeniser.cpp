#include "catch.hpp"

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

#include "Token.h"
#include "Tokeniser.h"

namespace {
  std::stringstream strToStream(std::string str) {
    std::stringstream stream;
    stream << str << "\n";
    return stream;
  }

  std::string tokenTypeToStr(TokenType type) {
    switch (type) {
    case TokenType::DELIMITER:
      return "Delimiter";
    case TokenType::IDENTIFIER:
      return "Identifier";
    case TokenType::NUMBER:
      return "Number";
    case TokenType::OPERATOR:
      return "Operator";
    default:
      throw std::invalid_argument("TokenType passed is not one of the defined types.");
    }
  }

  Tokeniser tokeniser;
}

TEST_CASE("[TestTokeniser] Delimiter, single") {
  std::stringstream stream = strToStream("{");
  std::list<Token> tokens = tokeniser.tokenise(stream);

  Token token = tokens.front();
  REQUIRE(token.type == TokenType::DELIMITER);
  REQUIRE(token.value == "{");
}


TEST_CASE("[TestTokeniser] Delimiter, multiple") {
  std::stringstream stream = strToStream("{}();_\",");
  std::list<Token> tokens = tokeniser.tokenise(stream);
  std::list<std::string> expectedValues = { "{", "}", "(", ")", ";", "_", "\"", "," };
  std::list<std::string>::const_iterator expectedValuesItr = expectedValues.begin();

  for (const Token token : tokens) {
    TokenType expectedType = TokenType::DELIMITER;
    std::string expectedValue = *expectedValuesItr;

    REQUIRE(token.type == expectedType);
    REQUIRE(token.value == expectedValue);

    ++expectedValuesItr;
  }
}

TEST_CASE("[TestTokeniser] Identifier, positive") {
  std::stringstream stream = strToStream("aName anotherName aName2");
  std::list<Token> tokens = tokeniser.tokenise(stream);
  std::list<std::string> expectedValues = { "aName", "anotherName", "aName2" };
  std::list<std::string>::const_iterator expectedValuesItr = expectedValues.begin();

  for (const Token token : tokens) {
    TokenType expectedType = TokenType::IDENTIFIER;
    std::string expectedValue = *expectedValuesItr;

    REQUIRE(token.type == expectedType);
    REQUIRE(token.value == expectedValue);

    ++expectedValuesItr;
  }
}

TEST_CASE("[TestTokeniser] Identifier, negative") {
  std::stringstream stream = strToStream("3name");
  REQUIRE_THROWS(tokeniser.tokenise(stream));
}

TEST_CASE("[TestTokeniser] Operator, single-char, single") {
  std::stringstream stream = strToStream("!");
  std::list<Token> tokens = tokeniser.tokenise(stream);

  Token token = tokens.front();
  REQUIRE(token.type == TokenType::OPERATOR);
  REQUIRE(token.value == "!");
}

TEST_CASE("[TestTokeniser] Operator, two-char, single, positive") {
  std::stringstream stream = strToStream("<=");
  std::list<Token> tokens = tokeniser.tokenise(stream);

  Token token = tokens.front();
  REQUIRE(token.type == TokenType::OPERATOR);
  REQUIRE(token.value == "<=");
}

TEST_CASE("[TestTokeniser] Operator, two-char, multiple, positive") {
  std::stringstream stream = strToStream("<= >= == != && ||");
  std::list<Token> tokens = tokeniser.tokenise(stream);
  std::list<std::string> expectedValues = { "<=", ">=", "==", "!=", "&&", "||" };
  std::list<std::string>::const_iterator expectedValuesItr = expectedValues.begin();

  for (const Token token : tokens) {
    REQUIRE(token.type == TokenType::OPERATOR);
    REQUIRE(token.value == *expectedValuesItr);

    ++expectedValuesItr;
  }
}

TEST_CASE("[TestTokeniser] Operator, two-char, single, negative") {
  std::stringstream stream = strToStream("&");
  REQUIRE_THROWS(tokeniser.tokenise(stream));
}

TEST_CASE("[TestTokeniser] Number, leading zero, single-digit, positive") {
  Tokeniser allowLeadingZeroTokeniser;

  std::stringstream stream = strToStream("0");
  std::list<Token> tokens = allowLeadingZeroTokeniser.allowingLeadingZeroes().tokenise(stream);
  Token token = tokens.front();

  REQUIRE(token.type == TokenType::NUMBER);
  REQUIRE(token.value == "0");
}

TEST_CASE("[TestTokeniser] Number, multiple-digit, positive") {
  std::stringstream stream = strToStream("123");
  std::list<Token> tokens = tokeniser.tokenise(stream);
  Token token = tokens.front();

  REQUIRE(token.type == TokenType::NUMBER);
  REQUIRE(token.value == "123");
}

TEST_CASE("[TestTokeniser] Number, multiple-digit, negative") {
  Tokeniser notAllowLeadingZeroTokeniser;

  std::stringstream stream = strToStream("0123");
  REQUIRE_THROWS(notAllowLeadingZeroTokeniser.notAllowingLeadingZeroes().tokenise(stream));
}

TEST_CASE("[TestTokeniser] Number, leading zero, multiple-digit, positive") {
  Tokeniser allowLeadingZeroTokeniser;

  std::stringstream stream = strToStream("0001");
  std::list<Token> tokens = allowLeadingZeroTokeniser.allowingLeadingZeroes().tokenise(stream);
  Token token = tokens.front();

  REQUIRE(token.type == TokenType::NUMBER);
  REQUIRE(token.value == "0001");
}

TEST_CASE("[TestTokeniser] Whitespace not consumed") {
  std::stringstream stream = strToStream("Follows *");

  std::list<Token> tokens = tokeniser.notConsumingWhitespace().tokenise(stream);
  std::list<std::string> expectedValues = { "Follows", " ", "*", "\n" };
  std::list<std::string>::const_iterator expectedValuesItr = expectedValues.begin();

  for (Token token : tokens) {
    REQUIRE(token.value == *expectedValuesItr);

    ++expectedValuesItr;
  }
}

TEST_CASE("[TestTokeniser] Whitespace character preserved") {
  std::stringstream stream = strToStream(" \t\r\v\f\n");

  std::list<Token> tokens = tokeniser.notConsumingWhitespace().tokenise(stream);
  std::list<std::string> expectedValues = { " ", "\t", "\r", "\v", "\f", "\n", "\n" };
  std::list<std::string>::const_iterator expectedValuesItr = expectedValues.begin();

  for (Token token : tokens) {
    REQUIRE(token.value == *expectedValuesItr);

    ++expectedValuesItr;
  }
}