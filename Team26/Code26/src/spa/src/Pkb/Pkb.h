#pragma once

#include "Table.h"

#include <map>

class Pkb {

public:
  /*
   * Constructor of Pkb.
   */
  Pkb();

  /**
   * Adds a variable name into varTable.
   *
   * @param var Variable name to be inserted
   */
  void addVar(std::string var);

  /**
   * Adds a statement number into stmtTable.
   *
   * @param stmtNo Statement number to be inserted
   */
  void addStmt(int stmtNo);

  /**
   * Adds a procedure name into procTable.
   *
   * @param proc Procedure name to be inserted
   */
  void addProc(std::string proc);

  /**
   * Adds a constant into constTable.
   *
   * @param constant Constant String to be inserted
   */
  void addConst(std::string constant);

  /**
   * Adds stmtNo into ifTable and stmtTable.
   *
   * @param stmtNo Statement number to be inserted
   */
  void addIf(int stmtNo);

  /**
   * Adds stmtNo into whileTable and stmtTable.
   *
   * @param stmtNo Statement number to be inserted
   */
  void addWhile(int stmtNo);

  /**
   * Adds stmtNo into readTable and stmtTable.
   *
   * @param stmtNo Statement number to be inserted
   */
  void addRead(int stmtNo);

  /**
   * Adds stmtNo into printTable and stmtTable.
   *
   * @param stmtNo Statement number to be inserted
   */
  void addPrint(int stmtNo);

  /**
   * Adds stmtNo into assignTable and stmtTable.
   *
   * @param stmtNo Statement number to be inserted
   */
  void addAssign(int stmtNo);

  /**
   * Adds the Row {followed, follower} into followsTable.
   *
   * @param followed Statement number of the preceding statement
   * @param follower Statement number of the following statement
   */
  void addFollows(int followed, int follower);

  /**
   * Adds the Row {followed, follower} into followsTTable.
   *
   * @param followed Statement number of the preceding statement
   * @param follower Statement number of the following statement
   */
  void addFollowsT(int followed, int follower);

  /**
   * Adds the Row {parent, child} into parentTable.
   *
   * @param parent Statement number of the container statement
   * @param child Statement number of the child statement
   */
  void addParent(int parent, int child);

  /**
   * Adds the Row {parent, child} into parentTTable.
   *
   * @param parent Statement number of the container statement
   * @param child Statement number of the child statement
   */
  void addParentT(int parent, int child);

  /**
   * Adds the Row {stmtNo, var} into usesSTable.
   *
   * @param stmtNo Statement number of the statement which uses var
   * @param var Variable or constant name which is used by the statement
   */
  void addUsesS(int stmtNo, std::string var);

  /**
   * Adds the Row {stmtNo, var} into modifiesSTable.
   *
   * @param stmtNo Statement number of the statement which modifies var
   * @param var Variable or constant name which is modified by the statement
   */
  void addModifiesS(int stmtNo, std::string var);

  /**
   * Adds the Row {stmtNo, lhs, rhs} int patterTable.
   *
   * @param stmtNo Statement number of the assign statement
   * @param lhs String of the left hand side of the assign statement
   * @param rhs String of the postfix form of the right hand side of the
   * assign statement
   */
  void addPatternAssign(int stmtNo, std::string lhs, std::string rhs);

  /**
   * @return varTable
   */
  Table getVarTable() const;

  /**
   * @return stmtTable
   */
  Table getStmtTable() const;

  /**
   * @return procTable
   */
  Table getProcTable() const;

  /**
   * @return constTable
   */
  Table getConstTable() const;

  /**
   * @return ifTable
   */
  Table getIfTable() const;

  /**
   * @return whileTable
   */
  Table getWhileTable() const;

  /**
   * @return readTable
   */
  Table getReadTable() const;

  /**
   * @return printTable
   */
  Table getPrintTable() const;

  /**
   * @return assignTable
   */
  Table getAssignTable() const;

  /**
   * @return followsTable
   */
  Table getFollowsTable() const;

  /**
   * @return followsTTable
   */
  Table getFollowsTTable() const;

  /**
   * @return parentTable
   */
  Table getParentTable() const;

  /**
   * @return parentTTable
   */
  Table getParentTTable() const;

  /**
   * @return usesSTable
   */
  Table getUsesSTable() const;

  /**
   * @return modifiesSTable
   */
  Table getModifiesSTable() const;

  /**
   * @return patterAssignTable
   */
  Table getPatternAssignTable() const;

  /**
   * Finds statement number which directly follow {@param stmtNo} and returns it as
   * a Table with one column.
   *
   * @param stmtNo Statement number for which we will get the follower.
   * @return Table containing the follower.
   */
  Table getFollower(int stmtNo) const;

  /**
   * Finds the statement number which directly precede {@param stmtNo} and returns it as
   * a Table with one column.
   *
   * @param stmtNo Statement number for which we will get the preceding statement number.
   * @return Table containing the preceding statement number.
   */
  Table getFollowedBy(int stmtNo) const;

  /**
   * Finds all statement number that directly or indirectly follow {@param stmtNo} and returns
   * them as a Table with one column.
   *
   * @param stmtNo Statement number for which we will get the followers.
   * @return Table containing the followers.
   */
  Table getFollowerT(int stmtNo) const;

  /**
   * Finds all statement number which directly precede {@param stmtNo} and returns them as
   * a Table with one column.
   *
   * @param stmtNo Statement number for which we will get the preceding statement numbers.
   * @return Table containing the preceding statement numbers.
   */
  Table getFollowedByT(int stmtNo) const;

  /**
   * Finds statement number which is the direct parent of  {@param stmtNo} and returns
   * it as a Table with one column.
   *
   * @param stmtNo Statement number for which we will get the parent.
   * @return Table containing the parent statement number.
   */
  Table getParent(int stmtNo) const;

  /**
   * Finds statement number which is the direct child of  {@param stmtNo} and returns
   * it as a Table with one column.
   *
   * @param stmtNo Statement number for which we will get the child.
   * @return Table containing the child statement number.
   */
  Table getChild(int stmtNo) const;

  /**
   * Finds all statement number which are the ancestor of  {@param stmtNo} and returns
   * them as a Table with one column.
   *
   * @param stmtNo Statement number for which we will get the ancestors.
   * @return Table containing the ancestor statement numbers.
   */
  Table getParentT(int stmtNo) const;

  /**
  * Finds all statement number which are the descendant of  {@param stmtNo} and returns
  * them as a Table with one column.
  *
  * @param stmtNo Statement number for which we will get the descendants.
  * @return Table containing the descendant statement numbers.
  */
  Table getChildT(int stmtNo) const;

  /**
   * Finds statement numbers and procedure names that use varName and returns them as a
   * Table with one column. The function addIndirectUses() must be called before calling
   * this function in order for this function to also return the indirect uses relations.
   *
   * @param varName Variable name for which we get the users.
   * @return Table containing the line number or procedure name of users.
   */
  Table getUses(std::string varName) const;

  /**
   * Finds all variables that are used by the stmtNo and returns them as a Table with
   * one column. The function addIndirectUses() must be called before calling
   * this function in order for this function to also return the indirect uses relations.
   *
   * @param stmtNo Statement number for which we will get the variables used.
   * @return Table containing the variable names that is used by the statement.
   */
  Table getUsedBy(int stmtNo) const;

  /**
   * Finds statement numbers and procedure names that modify varName and returns them as a
   * Table with one column. The function addIndirectModify() must be called before calling
   * this function in order for this function to also return the indirect modify relations.
   *
   * @param varName Variable name for which we get the modifiers.
   * @return Table containing the line number or procedure name of modifiers.
   */
  Table getModifies(std::string varName) const;

  /**
   * Finds all variables that are modified by the stmtNo and returns them as a Table with
   * one column. The function addIndirectModify() must be called before calling
   * this function in order for this function to also return the indirect modify relations.
   *
   * @param stmtNo Statement number for which we will get the variables modified.
   * @return Table containing the variable names that is modified by the statement.
   */
  Table getModifiedBy(int stmtNo) const;

private:
  Table varTable{ 1 };
  Table stmtTable{ 1 };
  Table procTable{ 1 };
  Table constTable{ 1 };

  Table ifTable{ 1 };
  Table whileTable{ 1 };
  Table readTable{ 1 };
  Table printTable{ 1 };
  Table assignTable{ 1 };

  Table followsTable{ 2 };
  Table followsTTable{ 2 };
  Table parentTable{ 2 };
  Table parentTTable{ 2 };
  Table usesSTable{ 2 };
  Table modifiesSTable{ 2 };
  Table patternAssignTable{ 3 };
};