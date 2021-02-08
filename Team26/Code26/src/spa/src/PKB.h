#pragma once

#include "Table.h"
#include <unordered_set>
#include <vector>

class PKB {
public:
	PKB();
	void addVar(std::string var);
	void addStmt(int stmtNo);
	void addProc(std::string proc);

	void addIf(int stmtNo);
	void addWhile(int stmtNo);
	void addRead(int stmtNo);
	void addPrint(int stmtNo);
	void addAssign(int stmtNo);

	void addFollows(int follower, int followed);
	void addFollowsT();
	void addParent(int parent, int child);
	void addParentT();
	void addUses(int stmtNo, std::string var);
	void addUses(std::string proc, std::string var);
	void addModifies(int stmtNo, std::string var);
	void addModifies(std::string proc, std::string var);

	Table getVarTable();
	Table getStmtTable();
	Table getProcTable();
	Table getIfTable();
	Table getWhileTable();
	Table getReadTable();
	Table getPrintTable();
	Table getAssignTable();
	Table getFollowsTable();
	Table getFollowsTTable();
	Table getParentTable();
	Table getParentTTable();
	Table getUsesTable();
	Table getModifiesTable();

private:
	Table varTable;
	Table stmtTable;
	Table procTable;

	Table ifTable;
	Table whileTable;
	Table readTable;
	Table printTable;
	Table assignTable;

	Table followsTable;
	Table followsTTable;
	Table parentTable;
	Table parentTTable;
	Table usesTable;
	Table modifiesTable;
};