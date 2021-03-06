#include "PqlParser.h"

#include <assert.h>

#include <list>
#include <string>

#include "ExprParser.h"
#include "PqlQuery.h"
#include "SpaException.h"
#include "Token.h"

// Helper methods
namespace {
  /**
   * Checks whether the given token is a valid design entity.
   * design-entity : 'stmt' | 'read' | 'print' | 'call' | 'while' | 'if' | 'assign' | 'variable' |
   *                 'constant' | 'procedure' | 'proc_line'
   *
   * @param token Token to verify.
   * @return True if the token is valid. Otherwise, false.
   */
  bool isValidDesignEntity(const Token& token) {
    return Pql::tokenToDesignEntityTypeMapper.count(token) == 1;
  }

  /**
   * Checks whether the given token is a relation that can have a transitive version. e.g. Follows and Parent.
   *
   * @param relationToken Token to verify.
   * @return True if the token is a relation that can have a transitive version. Otherwise, false.
   */
  bool canBeTransitive(const Token& relationToken) {
    return Pql::transitiveRelationTokens.count(relationToken) == 1;
  }

  /**
   * Checks whether the given token is an attribute name token. e.g. procName and varName.
   *
   * @param token Token to verify.
   * @return True if the token is a relation that can have a transitive version. Otherwise, false.
   */
  bool isAttributeName(const Token& token) {
    return Pql::tokenToAttributeRefTypeMapper.count(token) == 1;
  }

  /**
   * Checks whether the given synonym type and attribute reference type pair is valid.
   * attributeRefType cannot be of type NONE.
   *
   * @param synonymType Synonym type to verify. Cannot be of type NONE.
   * @param attributeRefType Attribute reference type to verify.
   * @return True if the pair is valid. Otherwise, false.
   */
  bool isSemanticallyValidAttributeName(const Pql::EntityType& synonymType, const Pql::AttributeRefType& attributeRefType) {
    return Pql::semanticallyValidAttributeReferences.count({ synonymType, attributeRefType }) == 1;
  }

  /**
   * Checks whether the given entity type is a synonym that refers to a statement.
   *
   * @param entityType Entity type to verify.
   * @return True if the entity type refers to a statement. Otherwise, false.
   */
  bool isStmtRef(const Pql::EntityType& entityType) {
    return Pql::synonymStmtEntityTypes.count(entityType) == 1;
  }

  /**
   * Checks whether the given entity type is a synonym that refers to a variable.
   *
   * @param entityType Entity type to verify.
   * @return True if the entity type refers to a variable. Otherwise, false.
   */
  bool isVarRef(const Pql::EntityType& entityType) {
    return entityType == Pql::EntityType::VARIABLE;
  }

  /**
   * Checks whether the given entity type is a synonym that refers to a procedure.
   *
   * @param entityType Entity type to verify.
   * @return True if the entity type refers to a procedure. Otherwise, false.
   */
  bool isProcRef(const Pql::EntityType& entityType) {
    return entityType == Pql::EntityType::PROCEDURE;
  }

  /**
   * Checks whether the given with clause parameter entity is a number.
   *
   * @param entity Entity to verify.
   * @return True if the entity is a number type param. Otherwise, false.
   */
  bool isWithClauseParamNumber(const Pql::Entity& entity) {
    return Pql::numberReferences.count({ entity.getType(), entity.getAttributeRefType() }) == 1;
  }

  /**
   * Checks whether the given with-clause params are of the same type (both numbers
   * or both names).
   *
   * @param lhs Left-hand-side parameter entity.
   * @param rhs Right-hand-side parameter entity.
   * @return True if both lhs and rhs are of the same type. Otherwise, false.
  */
  bool areWithClauseParamsSameType(const Pql::Entity& lhs, const Pql::Entity& rhs) {
    const bool isLhsNumber = isWithClauseParamNumber(lhs);
    const bool isRhsNumber = isWithClauseParamNumber(rhs);
    return isLhsNumber && isRhsNumber || // both are numbers 
      !isLhsNumber && !isRhsNumber;      // both are names (guaranteed)
  }

  /**
   * Converts infix expression tokens into postfix expression as a whitespace delimited string.
   *
   * @param infixExpressionTokens List of infix expression tokens.
   * @return Postfix expression string.
   */
  std::string infixToPostfixExpression(std::list<Token>& infixExpressionTokens) {
    ExprProcessor::AssignExprParser exprParser(infixExpressionTokens);
    exprParser.parse();
    return exprParser.getPostfixExprString();
  }

  /**
   * Removes and returns the given number in string format with leading zeros removed.
   *
   * @param numberString Given number in string format. Must provide a valid integer number.
   * @return Number in string format with leading zeros removed.
   */
  std::string removeLeadingZerosFromNumber(const std::string& numberString) {
    std::string removedZeroesNumberString;
    bool isFirstNonZeroReached = false;
    for (const char& digit : numberString) {
      if (isFirstNonZeroReached) {
        removedZeroesNumberString.push_back(digit);
      } else {
        if (digit != '0') {
          isFirstNonZeroReached = true;
          removedZeroesNumberString.push_back(digit);
        }
      }
    }

    if (removedZeroesNumberString.empty()) {
      return "0";
    }

    return removedZeroesNumberString;
  }
}

namespace Pql {
  PqlParser::PqlParser(std::list<Token>& tokens)
    : tokens(tokens) {
  }

  Query PqlParser::parseQuery() {
    Query pqlQuery; // Mutating pqlOuery object directly to avoid unnecessary copying.

    consumeFrontWhitespaceTokens(); // Handles possible whitespace before declarations

    parseDeclarations();

    parseBody(pqlQuery);

    // Check for unexpected tokens at the end of the query
    if (!tokens.empty()) {
      throw SyntaxError(
        ErrorMessage::SYNTAX_ERROR_ADDITIONAL_TOKENS +
        ErrorMessage::APPEND_TOKEN_RECEIVED +
        getFrontToken().value
      );
    }

    pqlQuery.setSemanticErrorMessage(semanticErrorMessage);

    return pqlQuery;
  }

  void PqlParser::addSemanticErrorMessage(const std::string& message) {
    semanticErrorMessage.append(message).append("\n");
  }

  void PqlParser::parseDeclarations() {
    while (getFrontToken() != SELECT) {
      parseDeclaration();
    }
  }

  // declaration: design-entity synonym (',' synonym)* ';'
  void PqlParser::parseDeclaration() {
    // Parses design-entity - Do not consume whitespaces yet (For prog_line)
    const Token& designEntityToken = validateAndGet(IDENTIFIER, false);

    if (!isValidDesignEntity(designEntityToken)) {
      throw SyntaxError(
        ErrorMessage::SYNTAX_ERROR_INVALID_DESIGN_ENTITY +
        ErrorMessage::APPEND_TOKEN_RECEIVED +
        designEntityToken.value
      );
    }

    // Check for prog_line
    if (designEntityToken == PROG) {
      validateAndGet(UNDERSCORE, false);
      validateAndGet(LINE, false);
    }

    consumeFrontWhitespaceTokens(); // Consume whitespaces at the end

    const EntityType& designEntityType = tokenToDesignEntityTypeMapper.at(designEntityToken);

    // Parses first synonym
    parseDeclarationSynonym(designEntityType);

    // Parses additional synonyms
    while (getFrontToken() != SEMICOLON) {
      validateAndGet(COMMA);

      parseDeclarationSynonym(designEntityType);
    }

    validateAndGet(SEMICOLON);
  }

  void PqlParser::parseDeclarationSynonym(const EntityType& designEntityType) {
    const Token& synonymToken = validateAndGet(IDENTIFIER);

    // Disallow synonym to be named 'BOOLEAN' to avoid confusion
    if (synonymToken == BOOLEAN) {
      addSemanticErrorMessage(ErrorMessage::SEMANTIC_ERROR_INVALID_DECLARATION_NAME_BOOLEAN);
      return;
    }

    // Check if synonym has been declared
    if (isSynonymDeclared(synonymToken.value)) {
      addSemanticErrorMessage(
        ErrorMessage::SEMANTIC_ERROR_DUPLICATE_SYNONYM_DECLARATION +
        ErrorMessage::APPEND_TOKEN_RECEIVED +
        synonymToken.value
      );
      return;
    }

    // Store synonym
    declaredSynonyms[synonymToken.value] = designEntityType;
  }

  void PqlParser::parseBody(Query& queryUnderConstruction) {
    validateAndGet(SELECT);

    parseSelectTargets(queryUnderConstruction);

    parseClauses(queryUnderConstruction);

  }

  void PqlParser::parseSelectTargets(Query& queryUnderConstruction) {
    const Token frontToken = getFrontToken();
    if (frontToken == BOOLEAN) {                   // BOOLEAN query
      validateAndGet(BOOLEAN);
    } else if (frontToken == LEFT_ANGLE_BRACKET) { // Tuple select
      validateAndGet(LEFT_ANGLE_BRACKET);

      // Parse first select target in tuple
      parseSelectTarget(queryUnderConstruction);

      // Check for additional select targets in tuple
      while (getFrontToken() == COMMA) {
        validateAndGet(COMMA);
        parseSelectTarget(queryUnderConstruction);
      }

      validateAndGet(RIGHT_ANGLE_BRACKET);
    } else {                                       // Single select
      parseSelectTarget(queryUnderConstruction);
    }
  }

  void PqlParser::parseSelectTarget(Query& queryUnderConstruction) {
    const Token& synonymToken = validateAndGet(IDENTIFIER);
    const EntityType& synonymType = getSynonymType(synonymToken.value);
    AttributeRefType attributeRefType = AttributeRefType::NONE;

    // Check if select target is an attrRef
    if (!tokens.empty() && getFrontToken() == DOT) {
      validateAndGet(DOT);
      const Token& attributeNameToken = validateAndGet(IDENTIFIER, false); // Don't consume whitespaces yet due to 'stmt#'
      if (!isAttributeName(attributeNameToken)) {
        throw SyntaxError(
          ErrorMessage::SYNTAX_ERROR_INVALID_ATTRIBUTE_NAME +
          ErrorMessage::APPEND_TOKEN_RECEIVED +
          attributeNameToken.value
        );
      }

      if (attributeNameToken == STMT) {
        validateAndGet(NUMBER_SIGN, false);
      }
      consumeFrontWhitespaceTokens(); // Consume whitespaces at the end

      attributeRefType = tokenToAttributeRefTypeMapper.at(attributeNameToken); // Update attributeRefType

      // Checks if synonym has the attribute name
      if (!isSemanticallyValidAttributeName(synonymType, attributeRefType)) {
        addSemanticErrorMessage(
          ErrorMessage::SEMANTIC_ERROR_INVALID_ATTRIBUTE_NAME +
          ErrorMessage::APPEND_TOKEN_RECEIVED +
          attributeNameToken.value
        );
      }

    }

    queryUnderConstruction.addTarget(Entity(synonymType, synonymToken.value, attributeRefType));
  }

  void PqlParser::parseClauses(Query& queryUnderConstruction) {
    ParsingClauseType currentClauseType = ParsingClauseType::UNDEFINED;
    while (!tokens.empty()) {
      const Token frontToken = getFrontToken();

      // NOTE: Cannot use switch-case due to Token being a struct and not enum
      if (frontToken == SUCH) { // such that clause
        // Consume 'such that'
        validateAndGet(SUCH, false);
        validateAndGet(SPACE, false);
        validateAndGet(THAT);

        parseSuchThatClause(queryUnderConstruction);
        currentClauseType = ParsingClauseType::SUCH_THAT;
      } else if (frontToken == PATTERN) { // pattern clause
        // Consume 'pattern'
        validateAndGet(PATTERN);

        parsePatternClause(queryUnderConstruction);
        currentClauseType = ParsingClauseType::PATTERN;
      } else if (frontToken == WITH) { // with clause
        // Consume 'with'
        validateAndGet(WITH);

        parseWithClause(queryUnderConstruction);
        currentClauseType = ParsingClauseType::WITH;
      } else if (frontToken == AND) {
        validateAndGet(AND);

        switch (currentClauseType) {
        case ParsingClauseType::SUCH_THAT:
          parseSuchThatClause(queryUnderConstruction);
          break;
        case ParsingClauseType::PATTERN:
          parsePatternClause(queryUnderConstruction);
          break;
        case ParsingClauseType::WITH:
          parseWithClause(queryUnderConstruction);
          break;
        default:
          throw SyntaxError(
            ErrorMessage::SYNTAX_ERROR_WRONG_TOKEN_VALUE +
            ErrorMessage::APPEND_TOKEN_EXPECTED + SUCH.value + "/" + PATTERN.value + "/" + WITH.value +
            ErrorMessage::APPEND_TOKEN_RECEIVED + frontToken.value
          );
          break;
        }
      } else {
        break; // Additional tokens will be caught by parseQuery()
      }
    }
  }

  void PqlParser::parseSuchThatClause(Query& queryUnderConstruction) {
    // 'such that' OR 'and' already consumed

    const Token& relationToken = validateAndGet(IDENTIFIER, false);
    // Do not consume space yet
    // Ensure no space between relation and *
    bool isTransitiveRelation = false;
    const bool isNextTokenStar = getFrontToken() == STAR;
    if (isNextTokenStar) {
      if (!canBeTransitive(relationToken)) {
        throw SyntaxError(
          ErrorMessage::SYNTAX_ERROR_WRONG_TOKEN_TYPE +
          ErrorMessage::APPEND_TOKEN_EXPECTED +
          LEFT_PARENTHESIS.value +
          ErrorMessage::APPEND_TOKEN_RECEIVED +
          STAR.value
        );
      }
      validateAndGet(STAR, false);
      isTransitiveRelation = true;
    }
    consumeFrontWhitespaceTokens();

    // Construct the clause
    // NOTE: Cannot use switch-case due to Token being a struct and not enum
    Clause clauseUnderConstruction;
    if (relationToken == FOLLOWS) {
      isTransitiveRelation
        ? parseFollowsTClause(clauseUnderConstruction)
        : parseFollowsClause(clauseUnderConstruction);
    } else if (relationToken == PARENT) {
      isTransitiveRelation
        ? parseParentTClause(clauseUnderConstruction)
        : parseParentClause(clauseUnderConstruction);
    } else if (relationToken == CALLS) {
      isTransitiveRelation
        ? parseCallsTClause(clauseUnderConstruction)
        : parseCallsClause(clauseUnderConstruction);
    } else if (relationToken == NEXT) {
      isTransitiveRelation
        ? parseNextTClause(clauseUnderConstruction)
        : parseNextClause(clauseUnderConstruction);
    } else if (relationToken == AFFECTS) {
      isTransitiveRelation
        ? parseAffectsTClause(clauseUnderConstruction)
        : parseAffectsClause(clauseUnderConstruction);
    } else if (relationToken == NEXT_BIP) {
      isTransitiveRelation
        ? parseNextBipTClause(clauseUnderConstruction)
        : parseNextBipClause(clauseUnderConstruction);
    } else if (relationToken == AFFECTS_BIP) {
      isTransitiveRelation
        ? parseAffectsBipTClause(clauseUnderConstruction)
        : parseAffectsBipClause(clauseUnderConstruction);
    } else if (relationToken == USES) {
      parseUsesClause(clauseUnderConstruction);
    } else if (relationToken == MODIFIES) {
      parseModifiesClause(clauseUnderConstruction);
    } else {
      throw SyntaxError(
        ErrorMessage::SYNTAX_ERROR_INVALID_RELATION +
        ErrorMessage::APPEND_TOKEN_RECEIVED +
        relationToken.value
      );
    }

    queryUnderConstruction.addClause(clauseUnderConstruction);
  }

  void PqlParser::parsePatternClause(Query& queryUnderConstruction) {
    // 'pattern' OR 'and' already consumed
    const Token& synonymToken = validateAndGet(IDENTIFIER);
    const EntityType& synonymType = getSynonymType(synonymToken.value);

    Clause clauseUnderConstruction;
    switch (synonymType) {
    case Pql::EntityType::WHILE:
      parsePatternWhileClause(clauseUnderConstruction, synonymToken.value);
      break;
    case Pql::EntityType::IF:
      parsePatternIfClause(clauseUnderConstruction, synonymToken.value);
      break;
    case Pql::EntityType::ASSIGN:
      parsePatternAssignClause(clauseUnderConstruction, synonymToken.value);
      break;
    default:
      addSemanticErrorMessage(
        ErrorMessage::SEMANTIC_ERROR_NON_PATTERN_CLAUSE_SYNONYM +
        ErrorMessage::APPEND_TOKEN_RECEIVED +
        synonymToken.value
      );
      parsePatternInvalidClause(clauseUnderConstruction, synonymToken.value, synonymType);
      break;
    }

    queryUnderConstruction.addClause(clauseUnderConstruction);
  }

  void PqlParser::parseWithClause(Query& queryUnderConstruction) {
    // 'with' OR 'and' already consumed
    Clause clauseUnderConstruction;
    clauseUnderConstruction.setType(ClauseType::WITH);
    parseRef(clauseUnderConstruction);
    validateAndGet(EQUAL);
    parseRef(clauseUnderConstruction);
    // Verify LHS and RHS are the same type
    const std::vector<Entity>& params = clauseUnderConstruction.getParams();
    const Entity& lhsEntity = params[0];
    const Entity& rhsEntity = params[1];
    if (!areWithClauseParamsSameType(lhsEntity, rhsEntity)) {
      addSemanticErrorMessage(ErrorMessage::SEMANTIC_ERROR_INVALID_WITH_CLAUSE);
    }
    queryUnderConstruction.addClause(clauseUnderConstruction);
  }

  void PqlParser::parseFollowsClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::FOLLOWS);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  void PqlParser::parseFollowsTClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::FOLLOWS_T);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  void PqlParser::parseParentClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::PARENT);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  void PqlParser::parseParentTClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::PARENT_T);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  void PqlParser::parseCallsClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::CALLS);
    parseProcAndProcArgs(clauseUnderConstruction);
  }

  void PqlParser::parseCallsTClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::CALLS_T);
    parseProcAndProcArgs(clauseUnderConstruction);
  }

  void PqlParser::parseNextClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::NEXT);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  void PqlParser::parseNextTClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::NEXT_T);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  void PqlParser::parseAffectsClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::AFFECTS);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  void PqlParser::parseAffectsTClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::AFFECTS_T);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  void PqlParser::parseNextBipClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::NEXT_BIP);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  void PqlParser::parseNextBipTClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::NEXT_BIP_T);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  void PqlParser::parseAffectsBipClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::AFFECTS_BIP);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  void PqlParser::parseAffectsBipTClause(Clause& clauseUnderConstruction) {
    clauseUnderConstruction.setType(ClauseType::AFFECTS_BIP_T);
    parseStmtAndStmtArgs(clauseUnderConstruction);
  }

  // 'Uses' '(' stmtRef ',' entRef ')' OR 'Uses' '(' entRef ',' entRef ')'
  void PqlParser::parseUsesClause(Clause& clauseUnderConstruction) {
    parseUsesModifiesClause(clauseUnderConstruction, ClauseType::USES_P, ClauseType::USES_S);
  }

  // 'Modifies' '(' stmtRef ',' entRef ')' OR 'Modifies' '(' entRef ',' entRef ')'
  void PqlParser::parseModifiesClause(Clause& clauseUnderConstruction) {
    parseUsesModifiesClause(clauseUnderConstruction, ClauseType::MODIFIES_P, ClauseType::MODIFIES_S);
  }

  // Helper method for Uses and Modifies clause
  void PqlParser::parseUsesModifiesClause(Clause& clauseUnderConstruction, const ClauseType& procedureType, const ClauseType& stmtType) {
    // 'Uses'/'Modifies' already consumed
    validateAndGet(LEFT_PARENTHESIS);
    const Token frontToken = getFrontToken();

    // NOTE: Cannot use switch-case due to Token being a struct and not enum
    if (frontToken == UNDERSCORE) {                            // wildcard
      validateAndGet(UNDERSCORE);
      addSemanticErrorMessage(ErrorMessage::SEMANTIC_ERROR_INVALID_WILDCARD);
      clauseUnderConstruction.setType(procedureType);
      clauseUnderConstruction.addParam(Pql::Entity(Pql::EntityType::WILDCARD, "_"));
    } else if (frontToken.type == TokenType::NUMBER) {         // constant as stmtRef
      parseStmtRef(clauseUnderConstruction);
      clauseUnderConstruction.setType(stmtType);
    } else if (frontToken == QUOTE) {                          // name as procName
      parseProcRef(clauseUnderConstruction);
      clauseUnderConstruction.setType(procedureType);
    } else if (frontToken.type == TokenType::IDENTIFIER) {     // synonym
      const EntityType synonymType = getSynonymType(frontToken.value);
      if (isStmtRef(synonymType)) {
        parseStmtRef(clauseUnderConstruction);
        clauseUnderConstruction.setType(stmtType);
      } else {
        parseProcRef(clauseUnderConstruction);
        clauseUnderConstruction.setType(procedureType);
      }
    } else {
      throw SyntaxError(
        ErrorMessage::SYNTAX_ERROR_WRONG_TOKEN_TYPE +
        ErrorMessage::APPEND_TOKEN_RECEIVED +
        frontToken.value
      );
    }

    validateAndGet(COMMA);

    parseVarRef(clauseUnderConstruction);

    validateAndGet(RIGHT_PARENTHESIS);
  }

  // assign : 'pattern' syn-assign '('entRef ',' expression-spec ')'
  void PqlParser::parsePatternAssignClause(Pql::Clause& clauseUnderConstruction, const std::string& synonymName) {
    clauseUnderConstruction.setType(ClauseType::PATTERN_ASSIGN);
    clauseUnderConstruction.addParam(Entity(EntityType::ASSIGN, synonymName));

    validateAndGet(LEFT_PARENTHESIS);

    parseVarRef(clauseUnderConstruction);

    validateAndGet(COMMA);

    parseExprSpec(clauseUnderConstruction);

    validateAndGet(RIGHT_PARENTHESIS);
  }

  // if : syn-if '(' entRef ',' '_' ',' '_' ')'
  void PqlParser::parsePatternIfClause(Pql::Clause& clauseUnderConstruction, const std::string& synonymName) {
    clauseUnderConstruction.setType(ClauseType::PATTERN_IF);
    clauseUnderConstruction.addParam(Entity(EntityType::IF, synonymName));

    validateAndGet(LEFT_PARENTHESIS);

    parseVarRef(clauseUnderConstruction);

    validateAndGet(COMMA);

    validateAndGet(UNDERSCORE);

    validateAndGet(COMMA);

    validateAndGet(UNDERSCORE);

    validateAndGet(RIGHT_PARENTHESIS);
  }

  // while : syn-while '(' entRef ',' '_' ')'
  void PqlParser::parsePatternWhileClause(Pql::Clause& clauseUnderConstruction, const std::string& synonymName) {
    clauseUnderConstruction.setType(ClauseType::PATTERN_WHILE);
    clauseUnderConstruction.addParam(Entity(EntityType::WHILE, synonymName));

    validateAndGet(LEFT_PARENTHESIS);

    parseVarRef(clauseUnderConstruction);

    validateAndGet(COMMA);

    validateAndGet(UNDERSCORE);

    validateAndGet(RIGHT_PARENTHESIS);
  }

  // invalid: syn-invalid '(' entRef ',' expression-spec ')' | syn-invalid '(' entRef ',' '_' ',' '_' ')'
  void PqlParser::parsePatternInvalidClause(
    Clause& clauseUnderConstruction, const std::string& synonymName, const EntityType& synonymType) {
    Clause tempClause;
    tempClause.addParam(Entity(synonymType, synonymName));
    validateAndGet(LEFT_PARENTHESIS);

    parseVarRef(tempClause);

    validateAndGet(COMMA);

    parseExprSpec(tempClause);

    // Check whether the invalid pattern clause is 2 params or 3 params.
    const Token frontToken = getFrontToken();
    if (frontToken == RIGHT_PARENTHESIS) { // 2 params
      validateAndGet(RIGHT_PARENTHESIS);

      tempClause.setType(ClauseType::PATTERN_ASSIGN);
      clauseUnderConstruction = std::move(tempClause);
    } else {                               // 3 params
      validateAndGet(COMMA);
      validateAndGet(UNDERSCORE);
      validateAndGet(RIGHT_PARENTHESIS);

      const std::vector<Entity>& tempClauseParams = tempClause.getParams();
      const Entity& secondParam = tempClauseParams[2]; // Second param is index 2
      const bool isSecondParamWildCard = secondParam.getType() == EntityType::WILDCARD;
      if (!isSecondParamWildCard) {
        throw SyntaxError(
          ErrorMessage::SYNTAX_ERROR_WRONG_TOKEN_TYPE +
          ErrorMessage::APPEND_TOKEN_EXPECTED + "_" +
          ErrorMessage::APPEND_TOKEN_RECEIVED + secondParam.getValue()
        );
      }

      clauseUnderConstruction.setType(ClauseType::PATTERN_IF);
      clauseUnderConstruction.addParam(Entity(synonymType, synonymName));
      clauseUnderConstruction.addParam(tempClause.getParams()[1]); // Add first param (varRef)
    }
  }

  // '(' stmtRef ',' stmtRef ')'
  void PqlParser::parseStmtAndStmtArgs(Clause& clauseUnderConstruction) {
    validateAndGet(LEFT_PARENTHESIS);

    parseStmtRef(clauseUnderConstruction);

    validateAndGet(COMMA);

    parseStmtRef(clauseUnderConstruction);

    validateAndGet(RIGHT_PARENTHESIS);
  }

  // '(' procRef ',' procRef ')'
  void PqlParser::parseProcAndProcArgs(Clause& clauseUnderConstruction) {
    validateAndGet(LEFT_PARENTHESIS);

    parseProcRef(clauseUnderConstruction);

    validateAndGet(COMMA);

    parseProcRef(clauseUnderConstruction);

    validateAndGet(RIGHT_PARENTHESIS);
  }

  void PqlParser::parseRef(Clause& clauseUnderConstruction) {
    const Token frontToken = getFrontToken();

    // NOTE: Cannot use switch-case due to Token being a struct and not enum
    if (frontToken == QUOTE) {                             // '"' IDENT '"'
      validateAndGet(QUOTE);

      const Token& nameToken = validateAndGet(IDENTIFIER);

      validateAndGet(QUOTE);

      clauseUnderConstruction.addParam(Entity(EntityType::NAME, nameToken.value));
    } else if (frontToken.type == TokenType::NUMBER) {     // INTEGER
      const Token& numberToken = validateAndGet(NUMBER);
      clauseUnderConstruction.addParam(Entity(EntityType::NUMBER, removeLeadingZerosFromNumber(numberToken.value)));
    } else if (frontToken.type == TokenType::IDENTIFIER) { // prog_line synoynm OR attrRef
      const Token& synonymToken = validateAndGet(IDENTIFIER);
      const EntityType& synonymType = getSynonymType(synonymToken.value);
      if (synonymType == EntityType::PROG_LINE) { // prog_line synonym
        clauseUnderConstruction.addParam(Entity(synonymType, synonymToken.value));
      } else {                                    // attrRef
        if (getFrontToken() != DOT) {
          addSemanticErrorMessage(
            ErrorMessage::SEMANTIC_ERROR_NON_ATTR_REF +
            ErrorMessage::APPEND_SYNONYM_WITH_MISSING_ATTR_REF + synonymToken.value
          );
          return;
        }
        validateAndGet(DOT);
        const Token& attributeNameToken = validateAndGet(IDENTIFIER, false); // Don't consume whitespaces yet due to 'stmt#'
        if (!isAttributeName(attributeNameToken)) {
          throw SyntaxError(
            ErrorMessage::SYNTAX_ERROR_INVALID_ATTRIBUTE_NAME +
            ErrorMessage::APPEND_TOKEN_RECEIVED +
            attributeNameToken.value
          );
        }

        if (attributeNameToken == STMT) {
          validateAndGet(NUMBER_SIGN, false);
        }
        consumeFrontWhitespaceTokens(); // Consume whitespaces at the end

        AttributeRefType attributeRefType = tokenToAttributeRefTypeMapper.at(attributeNameToken); // Update attributeRefType
        // Checks if synonym has the attribute name
        if (!isSemanticallyValidAttributeName(synonymType, attributeRefType)) {
          addSemanticErrorMessage(
            ErrorMessage::SEMANTIC_ERROR_INVALID_ATTRIBUTE_NAME +
            ErrorMessage::APPEND_TOKEN_RECEIVED +
            attributeNameToken.value
          );
        }

        clauseUnderConstruction.addParam(Entity(synonymType, synonymToken.value, attributeRefType));
      }
    } else {
      throw SyntaxError(
        ErrorMessage::SYNTAX_ERROR_WRONG_TOKEN_TYPE +
        ErrorMessage::APPEND_TOKEN_RECEIVED +
        frontToken.value
      );
    }
  }

  // stmtRef: synonym | '_' | INTEGER
  void PqlParser::parseStmtRef(Clause& clauseUnderConstruction) {
    const Token frontToken = getFrontToken();

    // NOTE: Cannot use switch-case due to Token being a struct and not enum
    if (frontToken.type == TokenType::IDENTIFIER) {     // synonym
      validateAndGet(IDENTIFIER);
      const EntityType& entityType = getSynonymType(frontToken.value);
      if (!isStmtRef(entityType)) {
        addSemanticErrorMessage(
          ErrorMessage::SEMANTIC_ERROR_NON_STMT_REF +
          ErrorMessage::APPEND_TOKEN_RECEIVED +
          frontToken.value
        );
      }

      clauseUnderConstruction.addParam(Entity(entityType, frontToken.value));
    } else if (frontToken == UNDERSCORE) {              // wildcard
      validateAndGet(UNDERSCORE);

      clauseUnderConstruction.addParam(Entity(EntityType::WILDCARD, frontToken.value));
    } else if (frontToken.type == TokenType::NUMBER) {  // number
      validateAndGet(NUMBER);

      std::string removedLeadingZerosNumber = removeLeadingZerosFromNumber(frontToken.value);
      const bool isStmtNumberZero = removedLeadingZerosNumber == "0";
      if (isStmtNumberZero) {
        addSemanticErrorMessage(ErrorMessage::SEMANTIC_ERROR_ZERO_STMT_NUMBER);
      }

      clauseUnderConstruction.addParam(Entity(EntityType::NUMBER, removedLeadingZerosNumber));
    } else {
      throw SyntaxError(
        ErrorMessage::SYNTAX_ERROR_WRONG_TOKEN_TYPE +
        ErrorMessage::APPEND_TOKEN_RECEIVED +
        frontToken.value
      );
    }
  }

  // var-synonym | '_' | '"' IDENT '"'
  void PqlParser::parseVarRef(Clause& clauseUnderConstruction) {
    parseEntRef(clauseUnderConstruction, isVarRef);
  }

  // procedure-synonym | '_' | '"' IDENT '"'
  void PqlParser::parseProcRef(Clause& clauseUnderConstruction) {
    parseEntRef(clauseUnderConstruction, isProcRef);
  }

  void PqlParser::parseEntRef(Clause& clauseUnderConstruction, bool refTypeCheck(const Pql::EntityType&)) {
    const Token frontToken = getFrontToken();

    // NOTE: Cannot use switch-case due to Token being a struct and not enum
    if (frontToken.type == TokenType::IDENTIFIER) {     // synonym
      validateAndGet(IDENTIFIER);
      const EntityType& entityType = getSynonymType(frontToken.value);
      if (!refTypeCheck(entityType)) {
        addSemanticErrorMessage(
          ErrorMessage::SEMANTIC_ERROR_NON_ENT_REF +
          ErrorMessage::APPEND_TOKEN_RECEIVED +
          frontToken.value
        );
      }

      clauseUnderConstruction.addParam(Entity(entityType, frontToken.value));
    } else if (frontToken == UNDERSCORE) {              // wildcard
      validateAndGet(UNDERSCORE);

      clauseUnderConstruction.addParam(Entity(EntityType::WILDCARD, frontToken.value));
    } else if (frontToken == QUOTE) {                   // name
      validateAndGet(QUOTE);

      const Token& variableNameToken = validateAndGet(IDENTIFIER);

      validateAndGet(QUOTE);

      clauseUnderConstruction.addParam(Entity(EntityType::NAME, variableNameToken.value));
    } else {
      throw SyntaxError(
        ErrorMessage::SYNTAX_ERROR_WRONG_TOKEN_TYPE +
        ErrorMessage::APPEND_TOKEN_RECEIVED +
        frontToken.value
      );
    }
  }

  // expression-spec: '_' '"' factor '"' '_' | '_'
  void PqlParser::parseExprSpec(Clause& clauseUnderConstruction) {
    const Token frontToken = getFrontToken();

    // Check for '_' OR '"'
    if (frontToken == QUOTE) {
      parseExpression(clauseUnderConstruction, true);
    } else {
      validateAndGet(UNDERSCORE);

      if (getFrontToken() == QUOTE) {
        parseExpression(clauseUnderConstruction, false);
        validateAndGet(UNDERSCORE, false);
      } else {
        clauseUnderConstruction.addParam(Entity(EntityType::WILDCARD, "_"));
      }
      consumeFrontWhitespaceTokens();
    }
  }

  void PqlParser::parseExpression(Clause& clauseUnderConstruction, const bool isExactMatch) {
    validateAndGet(QUOTE);

    // Create a list of infix expression tokens
    std::list<Token> infixExpressionTokens;
    while (getFrontToken() != QUOTE) {
      const Token currentToken = getFrontToken();
      if (currentToken.type == TokenType::NUMBER) {
        // Remove leading zero for numbers
        const Token& noLeadingZeroConstToken = { currentToken.type, removeLeadingZerosFromNumber(currentToken.value) };
        infixExpressionTokens.emplace_back(noLeadingZeroConstToken);
      } else {
        infixExpressionTokens.emplace_back(currentToken);
      }
      tokens.pop_front();
      consumeFrontWhitespaceTokens();
    }
    validateAndGet(QUOTE);

    try {
      clauseUnderConstruction.addParam(
        Entity(
          isExactMatch ? EntityType::EXPRESSION : EntityType::SUB_EXPRESSION,
          infixToPostfixExpression(infixExpressionTokens)
        )
      );
    } catch (const ExprProcessor::SyntaxError& e) {
      throw SyntaxError(e.what());
    }
  }

  Token PqlParser::validateAndGet(const Token& validationToken) {
    return validateAndGet(validationToken, true);
  }

  Token PqlParser::validateAndGet(const Token& validationToken, const bool shouldConsumeWhitespaces) {
    const Token frontToken = getFrontToken();
    const bool isCheckTokenType = validationToken.value.empty();

    if (isCheckTokenType) {
      // Check token type
      if (frontToken.type != validationToken.type) {
        throw SyntaxError(
          ErrorMessage::SYNTAX_ERROR_WRONG_TOKEN_TYPE +
          ErrorMessage::APPEND_TOKEN_RECEIVED +
          frontToken.value
        );
      }
    } else {
      // Check exact match otherwise
      if (frontToken != validationToken) {
        throw SyntaxError(
          ErrorMessage::SYNTAX_ERROR_WRONG_TOKEN_VALUE +
          ErrorMessage::APPEND_TOKEN_EXPECTED +
          validationToken.value +
          ErrorMessage::APPEND_TOKEN_RECEIVED +
          frontToken.value
        );
      }
    }

    // Validation success
    tokens.pop_front();

    if (shouldConsumeWhitespaces) {
      consumeFrontWhitespaceTokens();
    }

    return frontToken;
  }

  void PqlParser::consumeFrontWhitespaceTokens() {
    while (!tokens.empty()) {
      const bool isFrontTokenWhitespace = tokens.front().type == TokenType::WHITESPACE;

      if (isFrontTokenWhitespace) {
        tokens.pop_front();
      } else {
        break;
      }
    }
  }

  Token PqlParser::getFrontToken() {
    if (tokens.empty()) {
      throw SyntaxError(ErrorMessage::SYNTAX_ERROR_NOT_ENOUGH_TOKENS);
    }

    return tokens.front();
  }

  EntityType PqlParser::getSynonymType(const std::string& synonymName) {
    const bool isDeclared = isSynonymDeclared(synonymName);
    if (!isDeclared) {
      addSemanticErrorMessage(
        ErrorMessage::SEMANTIC_ERROR_UNDECLARED_SYNONYM +
        ErrorMessage::APPEND_TOKEN_RECEIVED +
        synonymName
      );
    }

    return isDeclared
      ? declaredSynonyms[synonymName]
      : Pql::EntityType::UNDEFINED;
  }

  bool PqlParser::isSynonymDeclared(const std::string& synonymName) const {
    return declaredSynonyms.count(synonymName) == 1;
  }
}