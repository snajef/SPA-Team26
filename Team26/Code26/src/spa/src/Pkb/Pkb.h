#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Table.h"
#include "Cfg.h"
#include "CfgBip.h"

class Pkb {
public:
  /*
   * Constructor of Pkb.
   */
  Pkb();

  /**
   * Adds the range of statement numbers that belong to a procedure.
   *
   * @param proc Procedure in question.
   * @param first Statement number of the first statement in the procedure.
   * @param last Last statement number in the procedure. Different from end statement of a procedure.
   */
  void addProcRange(const std::string proc, const int first, const int last);

  /**
   * Adds a procedure's first and last statement numbers in the control flow path.
   *
   * @param proc Procedure in question.
   * @param start Statement number of the first statement in the procedure's control flow path.
   * @param end List of statement numbers of the last statements in the procedure's control flow path.
   */
  void addProcStartEnd(const std::string proc, const int start, const std::vector<int> end);

  /**
  * Adds a directed edge into the CFGBip and populates the NextBip relation.
  *
  * @param from Statement number of the statement executed first.
  * @param to Statement number of the statement which can be executed immediately after.
  * @param label Label of the edge.
  * @param type Type of the edge.
  */
  void addCfgBipEdge(const int from, const int to, const int label, const Cfg::NodeType type);

  /**
   * Adds a directed edge into the CFG and CFGBip. Also populates Next and NextBip relation.
   *
   * @param from Statement number of the statement executed first.
   * @param to Statement number of the statement which can be executed immediately after.
   */
  void addCfgEdge(const int from, const int to);

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
   * Adds stmtNo into callTable and stmtTable.
   *
   * @param stmtNo Statement number to be inserted
   */
  void addCall(int stmtNo);

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
   * Adds the Row {proc, var} into usesPTable.
   *
   * @param proc Name of procedure which uses var
   * @param var Variable or constant name which is used by the procedure
   */
  void addUsesP(std::string proc, std::string var);

  /**
   * Adds the Row {stmtNo, var} into modifiesSTable.
   *
   * @param stmtNo Statement number of the statement which modifies var
   * @param var Variable or constant name which is modified by the statement
   */
  void addModifiesS(int stmtNo, std::string var);

  /**
   * Adds the Row {proc, var} into modifiesPTable.
   *
   * @param proc Name of procedure which modifies var
   * @param var Variable or constant name which is modified by the procedure
   */
  void addModifiesP(std::string proc, std::string var);

  /**
   * Adds the Row {caller, called} into callsTable.
   *
   * @param followed Name of the caller Procedure
   * @param follower Name of the Procedure being called
   */
  void addCalls(std::string caller, std::string called);

  /**
   * Adds the Row {caller, called} into callsTTable.
   *
   * @param followed Name of the transitive caller Procedure
   * @param follower Name of the Procedure being transitively called
   */
  void addCallsT(std::string caller, std::string called);

  /**
   * Adds the Row {prev, next} into nextTable.
   *
   * @param prev Statement number of the statement executed first
   * @param next Statement number of the statement which can be executed immediately after prev in some execution sequence
   */
  void addNext(int prev, int next);

  /**
   * Adds the Row {prev, next} into nextTTable.
   *
   * @param prev Statement number of the statement executed first
   * @param next Statement number of the statement which can be executed after prev in some execution sequence
   */
  void addNextT(int prev, int next);

  /**
   * Adds the Row {affecter, affected} into affectsTable.
   *
   * @param prev Statement number of the statement which modifies a variable used in affected
   * @param next Statement number of the statement uses a variable modified by prev
   */
  void addAffects(int affecter, int affected);

  /**
   * Adds the Row {affecter, affected} into affectsTTable.
   *
   * @param prev Statement number of the statement which modifies a variable used in affected
   * @param next Statement number of the statement uses a variable modified by prev
   */
  void addAffectsT(int affecter, int affected);

  /**
   * Adds the Row {prev, next} into nextBipTable.
   *
   * @param prev Statement number of the statement executed first
   * @param next Statement number of the statement which can be executed immediately after prev in some execution sequence
   */
  void addNextBip(int prev, int next);

  /**
   * Adds the Row {prev, next} into nextBipTTable.
   *
   * @param prev Statement number of the statement executed first
   * @param next Statement number of the statement which can be executed after prev in some execution sequence
   */
  void addNextBipT(int prev, int next);

  /**
   * Adds the Row {affecter, affected} into affectsBipTable.
   *
   * @param prev Statement number of the statement which modifies a variable used in affected
   * @param next Statement number of the statement uses a variable modified by prev
   */
  void addAffectsBip(int affecter, int affected);

  /**
   * Adds the Row {affecter, affected} into affectsBipTTable.
   *
   * @param prev Statement number of the statement which modifies a variable used in affected
   * @param next Statement number of the statement uses a variable modified by prev
   */
  void addAffectsBipT(int affecter, int affected);

  /**
   * Adds the Row {stmtNo, lhs, rhs} into patternAssignTable.
   *
   * @param stmtNo Statement number of the assign statement
   * @param lhs String of the left hand side of the assign statement
   * @param rhs String of the postfix form of the right hand side of the
   * assign statement
   */
  void addPatternAssign(int stmtNo, std::string lhs, std::string rhs);

  /**
   * Adds the Row {stmtNo, var} int patternIfTable.
   *
   * @param stmtNo Statement number of the if statement
   * @param var Name of variable within the condition of the if statement
   */
  void addPatternIf(int stmtNum, std::string var);

  /**
   * Adds the Row {stmtNo, var} into patternWhileTable.
   *
   * @param stmtNo Statement number of the while statement
   * @param var Name of variable within the condition of the while statement
   */
  void addPatternWhile(int stmtNum, std::string var);

  /**
  * Adds the Row {stmtNo, proc} into callProcTable.
  *
  * @param stmtNo Statement number of the call statement
  * @param var Name of procedure being called by stmtNo
  */
  void addCallProc(int stmtNo, std::string proc);

  /**
  * Adds the Row {stmtNo, var} into readVarTable.
  *
  * @param stmtNo Statement number of the read statement
  * @param var Name of variable being read
  */
  void addReadVar(int stmtNo, std::string var);

  /**
  * Adds the Row {stmtNo, var} into printVarTable.
  *
  * @param stmtNo Statement number of the print statement
  * @param var Name of variable being printed
  */
  void addPrintVar(int stmtNo, std::string var);

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
   * @return patternAssignTable
   */
  Table getPatternAssignTable() const;

  /**
   * @return callTable
   */
  Table getCallTable() const;

  /**
   * @return callsTable
   */
  Table getCallsTable() const;

  /**
   * @return callsTTable
   */
  Table getCallsTTable() const;

  /**
   * @return nextTable
   */
  Table getNextTable() const;

  /**
   * @return nextTTable
   */
  Table getNextTTable() const;

  /**
   * @return affectsTable
   */
  Table getAffectsTable() const;

  /**
   * @return affectsTTable
   */
  Table getAffectsTTable() const;

  /**
 * @return nextBipTable
 */
  Table getNextBipTable() const;

  /**
   * @return nextBipTTable
   */
  Table getNextBipTTable() const;

  /**
   * @return affectsBipTable
   */
  Table getAffectsBipTable() const;

  /**
   * @return affectsBipTTable
   */
  Table getAffectsBipTTable() const;

  /**
   * @return usesPTable
   */
  Table getUsesPTable() const;

  /**
   * @return modifiesPTable
   */
  Table getModifiesPTable() const;

  /**
   * @return callProcTable
   */
  Table getCallProcTable() const;

  /**
   * @return readVarTable
   */
  Table getReadVarTable() const;

  /**
   * @return printVarTable
   */
  Table getPrintVarTable() const;

  /**
   * @return patternIfTable
   */
  Table getPatternIfTable() const;

  /**
   * @return patternWhileTable
   */
  Table getPatternWhileTable() const;

  /**
   * Finds and returns the assign statement numbers that are uses the given variable. If no
   * assign statement uses the variable, returns an empty result. This function must be
   * called after the Design Extractor extracts all the design abstractions.
   *
   * @param varName Variable name to check.
   * @return Set of assign statement numbers that uses the variable.
   */
  std::unordered_set<int> getAssignUses(const std::string& varName) const;

  /**
   * Finds and returns the variables that are modified by the given statement number. If no
   * variable is modified, returns an empty result. This function must be called after the
   * Design Extractor extracts all the design abstractions.
   *
   * @param stmtNo Statement number to check.
   * @return Set of variables modified.
   */
  std::unordered_set<std::string> getModifiedBy(const int stmtNo) const;

  /**
   * If the stmtNo is of a call statement, will return the name of the procedure
   * being called by stmtNo. Otherwise, returns empty String.
   *
   * @param stmtNo Statement number of a call statement
   * @return Name of procedure being called by stmtNo
   */
  std::string getProcNameFromCallStmt(const int stmtNo) const;

  /**
   * If the stmtNo is of a read statement, will return the name of the variable
   * being read by stmtNo. Otherwise, returns empty String.
   *
   * @param stmtNo Statement number of a read statement
   * @return Name of var being read by stmtNo
   */
  std::string getVarNameFromReadStmt(const int stmtNo) const;

  /**
   * If the stmtNo is of a print statement, will return the name of the variable
   * being printed by stmtNo. Otherwise, returns empty String.
   *
   * @param stmtNo Statement number of a print statement
   * @return Name of var being printed by stmtNo
   */
  std::string getVarNameFromPrintStmt(const int stmtNo) const;

  /**
   * Finds the set of stmts that can be directly executed after the given stmt number in the CFG.
   * If there are no nodes following, an empty set is returned.
   *
   * @param stmtNo Statement number of interest.
   * @return std::vector<int> List of nodes.
   */
  std::vector<int> getNextStmtsFromCfg(const int stmtNo) const;

  /**
   * Finds the list of stmts that can be directly executed after the given stmt number in the CFGBip.
   * If there are no nodes following, an empty list is returned.
   *
   * @param stmtNo Statement number of interest.
   * @return std::vector<BipNode> List of nodes.
   */
  std::vector<Cfg::BipNode> getNextStmtsFromCfgBip(const int stmtNo) const;

  /**
   * Finds the first statement of a given procedure.
   *
   * @param proc Procedure in question.
   * @return int Statement number of the first statement in the procedure.
   */
  int getStartStmtFromProc(const std::string proc) const;

  /**
   * Finds the list of last statements of a given procedure.
   *
   * @param proc Procedure in question.
   * @return std::vector<int> List of statement number of the last statements in the procedure.
   */
  std::vector<int> getEndStmtsFromProc(const std::string proc) const;

  /**
   * Finds the procedure corresponding to the target statement number.
   *
   * @param stmt Statement in question.
   * @return std::string Procedure that the statement belongs to.
   */
  std::string getProcFromStmt(const int stmt) const;

private:
  Cfg::Cfg cfg;
  Cfg::CfgBip cfgBip;

  Table varTable{ 1 };
  Table stmtTable{ 1 };
  Table procTable{ 1 };
  Table constTable{ 1 };

  Table ifTable{ 1 };
  Table whileTable{ 1 };
  Table readTable{ 1 };
  Table printTable{ 1 };
  Table assignTable{ 1 };
  Table callTable{ 1 };

  Table followsTable{ 2 };
  Table followsTTable{ 2 };
  Table parentTable{ 2 };
  Table parentTTable{ 2 };
  Table usesSTable{ 2 };
  Table usesPTable{ 2 };
  Table modifiesSTable{ 2 };
  Table modifiesPTable{ 2 };
  Table callsTable{ 2 };
  Table callsTTable{ 2 };
  Table nextTable{ 2 };
  Table nextTTable{ 2 };
  Table affectsTable{ 2 };
  Table affectsTTable{ 2 };

  Table nextBipTable{ 2 };
  Table nextBipTTable{ 2 };
  Table affectsBipTable{ 2 };
  Table affectsBipTTable{ 2 };

  Table callProcTable{ 2 };
  Table readVarTable{ 2 };
  Table printVarTable{ 2 };

  Table patternAssignTable{ 3 };
  Table patternIfTable{ 2 };
  Table patternWhileTable{ 2 };

  std::unordered_map<int, std::string> callProcMapper;
  std::unordered_map<int, std::string> readVarMapper;
  std::unordered_map<int, std::string> printVarMapper;
  std::unordered_map<std::string, int> procStartMapper;
  std::unordered_map<std::string, std::vector<int>> procEndMapper;
  std::unordered_map<int, std::string> stmtProcMapper;
};
