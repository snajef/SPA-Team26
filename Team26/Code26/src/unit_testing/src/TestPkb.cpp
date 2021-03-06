#include "catch.hpp"

#include <string>
#include <unordered_set>
#include <vector>

#include "Pkb.h"
#include "Table.h"

TEST_CASE("[TestPkb] varTable Insertion") {
  Pkb pkb;
  pkb.addVar("x");
  RowSet dataCopy = pkb.getVarTable().getData();
  REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("x") }) == 1);
}

TEST_CASE("[TestPkb] stmtTable Insertion") {
  Pkb pkb;
  pkb.addStmt(2);
  auto dataCopy = pkb.getStmtTable().getData();
  REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(2) }) == 1);
}

TEST_CASE("[TestPkb] procTable Insertion") {
  Pkb pkb;
  pkb.addProc("main");
  auto dataCopy = pkb.getProcTable().getData();
  REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("main") }) == 1);
}

TEST_CASE("[TestPkb] constTable Insertion") {
  Pkb pkb;
  pkb.addConst("2");
  auto dataCopy = pkb.getConstTable().getData();
  REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("2") }) == 1);
}

TEST_CASE("[TestPkb] ifTable Insertion") {
  Pkb pkb;
  pkb.addIf(12);
  auto dataCopy = pkb.getIfTable().getData();
  REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(12) }) == 1);
  auto stmtDataCopy = pkb.getStmtTable().getData();
  REQUIRE(stmtDataCopy.count({ pkb.getIntRefFromStmtNum(12) }) == 1);
}

TEST_CASE("[TestPkb] whileTable Insertion") {
  Pkb pkb;
  pkb.addWhile(15);
  auto dataCopy = pkb.getWhileTable().getData();
  REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(15) }) == 1);
  auto stmtDataCopy = pkb.getStmtTable().getData();
  REQUIRE(stmtDataCopy.count({ pkb.getIntRefFromStmtNum(15) }) == 1);
}

TEST_CASE("[TestPkb] readTable Insertion") {
  Pkb pkb;
  pkb.addRead(15);
  auto dataCopy = pkb.getReadTable().getData();
  REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(15) }) == 1);
  auto stmtDataCopy = pkb.getStmtTable().getData();
  REQUIRE(stmtDataCopy.count({ pkb.getIntRefFromStmtNum(15) }) == 1);
}

TEST_CASE("[TestPkb] printTable Insertion") {
  Pkb pkb;
  pkb.addPrint(526);
  auto dataCopy = pkb.getPrintTable().getData();
  REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(526) }) == 1);
  auto stmtDataCopy = pkb.getStmtTable().getData();
  REQUIRE(stmtDataCopy.count({ pkb.getIntRefFromStmtNum(526) }) == 1);
}

TEST_CASE("[TestPkb] assignTable Insertion") {
  Pkb pkb;
  pkb.addAssign(32);
  auto dataCopy = pkb.getAssignTable().getData();
  REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(32) }) == 1);
  auto stmtDataCopy = pkb.getStmtTable().getData();
  REQUIRE(stmtDataCopy.count({ pkb.getIntRefFromStmtNum(32) }) == 1);
}

TEST_CASE("[TestPkb] addFollows") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addFollows(5, 6);
    auto dataCopy = pkb.getFollowsTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(5), pkb.getIntRefFromStmtNum(6) }) == 1);
  }
}

TEST_CASE("[TestPkb] addFollowsT(int, int)") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addFollowsT(5, 6);
    auto dataCopy = pkb.getFollowsTTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(5), pkb.getIntRefFromStmtNum(6) }) == 1);
  }
}

TEST_CASE("[TestPkb] addParent") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addParent(5, 6);
    auto dataCopy = pkb.getParentTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(5), pkb.getIntRefFromStmtNum(6) }) == 1);
  }
}

TEST_CASE("[TestPkb] addParentT(int, int)") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addParentT(5, 6);
    auto dataCopy = pkb.getParentTTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(5), pkb.getIntRefFromStmtNum(6) }) == 1);
  }
}

TEST_CASE("[TestPkb] addUsesS") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addUsesS(5, "x");
    pkb.addUsesS(7, "y");
    auto dataCopy = pkb.getUsesSTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(5), pkb.getIntRefFromEntity("x") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(7), pkb.getIntRefFromEntity("y") }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] addUsesP") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addUsesP("foo", "x");
    pkb.addUsesP("bar", "y");
    auto dataCopy = pkb.getUsesPTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("foo"), pkb.getIntRefFromEntity("x") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("bar"), pkb.getIntRefFromEntity("y") }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] addModifiesS") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addModifiesS(5, "x");
    pkb.addModifiesS(7, "y");
    auto dataCopy = pkb.getModifiesSTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(5), pkb.getIntRefFromEntity("x") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(7), pkb.getIntRefFromEntity("y") }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] addModifiesP") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addModifiesP("foo", "x");
    pkb.addModifiesP("bar", "y");
    auto dataCopy = pkb.getModifiesPTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("foo"), pkb.getIntRefFromEntity("x") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("bar"), pkb.getIntRefFromEntity("y") }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] addCalls") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addCalls("main", "foo");
    pkb.addCalls("foo", "bar");
    auto dataCopy = pkb.getCallsTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("main"), pkb.getIntRefFromEntity("foo") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("foo"), pkb.getIntRefFromEntity("bar") }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] addCallsT") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addCallsT("main", "foo");
    pkb.addCallsT("foo", "bar");
    pkb.addCallsT("main", "bar");
    auto dataCopy = pkb.getCallsTTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("main"), pkb.getIntRefFromEntity("foo") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("foo"), pkb.getIntRefFromEntity("bar") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromEntity("main"), pkb.getIntRefFromEntity("bar") }) == 1);
    REQUIRE(dataCopy.size() == 3);
  }
}

TEST_CASE("[TestPkb] addNext") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addNext(3, 4);
    pkb.addNext(4, 5);
    auto dataCopy = pkb.getNextTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(3), pkb.getIntRefFromStmtNum(4) }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(4), pkb.getIntRefFromStmtNum(5) }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] addNextT") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addNextT(3, 4);
    pkb.addNextT(4, 5);
    pkb.addNextT(3, 5);
    auto dataCopy = pkb.getNextTTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(3), pkb.getIntRefFromStmtNum(4) }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(4), pkb.getIntRefFromStmtNum(5) }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(3), pkb.getIntRefFromStmtNum(5) }) == 1);
    REQUIRE(dataCopy.size() == 3);
  }
}

TEST_CASE("[TestPkb] addAffects") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addAffects(3, 4);
    pkb.addAffects(4, 7);
    auto dataCopy = pkb.getAffectsTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(3), pkb.getIntRefFromStmtNum(4) }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(4), pkb.getIntRefFromStmtNum(7) }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] addAffectsT") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addAffectsT(3, 4);
    pkb.addAffectsT(4, 7);
    pkb.addAffectsT(3, 7);
    auto dataCopy = pkb.getAffectsTTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(3), pkb.getIntRefFromStmtNum(4) }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(4), pkb.getIntRefFromStmtNum(7) }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(3), pkb.getIntRefFromStmtNum(7) }) == 1);
    REQUIRE(dataCopy.size() == 3);
  }
}

TEST_CASE("[TestPkb] addPatternAssign") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addPatternAssign(5, "x", " x y * ");
    pkb.addPatternAssign(7, "y", " b c * a + ");
    auto dataCopy = pkb.getPatternAssignTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(5), pkb.getIntRefFromEntity("x"), pkb.getIntRefFromEntity(" x y * ") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(7), pkb.getIntRefFromEntity("y"), pkb.getIntRefFromEntity(" b c * a + ") }) == 1);
  }
}

TEST_CASE("[TestPkb] addPatternIf") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addPatternIf(1, "count");
    pkb.addPatternIf(70, "a");
    auto dataCopy = pkb.getPatternIfTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(1), pkb.getIntRefFromEntity("count") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(70), pkb.getIntRefFromEntity("a") }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] addPatternWhile") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addPatternWhile(4, "count");
    pkb.addPatternWhile(100, "i");
    auto dataCopy = pkb.getPatternWhileTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(4), pkb.getIntRefFromEntity("count") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(100), pkb.getIntRefFromEntity("i") }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] addCallProc") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addCallProc(4, "proc1");
    pkb.addCallProc(20, "proc2");
    auto dataCopy = pkb.getCallProcTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(4), pkb.getIntRefFromEntity("proc1") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(20), pkb.getIntRefFromEntity("proc2") }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] addReadVar") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addReadVar(1, "x");
    pkb.addReadVar(3, "y");
    auto dataCopy = pkb.getReadVarTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(1), pkb.getIntRefFromEntity("x") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(3), pkb.getIntRefFromEntity("y") }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] addPrintVar") {
  Pkb pkb;

  SECTION("Check valid insertion") {
    pkb.addPrintVar(1, "x");
    pkb.addPrintVar(3, "y");
    auto dataCopy = pkb.getPrintVarTable().getData();
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(1), pkb.getIntRefFromEntity("x") }) == 1);
    REQUIRE(dataCopy.count({ pkb.getIntRefFromStmtNum(3), pkb.getIntRefFromEntity("y") }) == 1);
    REQUIRE(dataCopy.size() == 2);
  }
}

TEST_CASE("[TestPkb] getAssignUses") {
  Pkb pkb;
  pkb.addAssign(1);
  pkb.addAssign(2);
  pkb.addUsesS(1, "x");
  pkb.addUsesS(2, "y");
  pkb.addUsesS(3, "x");
  std::unordered_set<int> stmtNumbers = pkb.getAssignUses("x");
  REQUIRE(stmtNumbers.count(1) == 1);
  REQUIRE(stmtNumbers.count(3) == 0);
  REQUIRE(stmtNumbers.size() == 1);
  pkb.addUsesS(2, "x");
  stmtNumbers = pkb.getAssignUses("x");
  REQUIRE(stmtNumbers.count(1) == 1);
  REQUIRE(stmtNumbers.count(2) == 1);
  REQUIRE(stmtNumbers.size() == 2);
}

TEST_CASE("[TestPkb] getModifiedBy") {
  Pkb pkb;
  pkb.addModifiesS(1, "x");
  pkb.addModifiesS(2, "y");
  pkb.addModifiesS(2, "x");

  SECTION("modified by statement") {
    std::unordered_set<std::string> variables = pkb.getModifiedBy(1);
    REQUIRE(variables.count("x") == 1);
    REQUIRE(variables.size() == 1);
    variables = pkb.getModifiedBy(2);
    REQUIRE(variables.count("x") == 1);
    REQUIRE(variables.count("y") == 1);
    REQUIRE(variables.size() == 2);
  }
}

TEST_CASE("[TestPkb] CFG edges/Next") {
  Pkb pkb;
  pkb.addCfgEdge(1, 2);
  pkb.addCfgEdge(1, 3);
  pkb.addCfgEdge(1, 4);
  pkb.addCfgEdge(4, 5);

  SECTION("Next relations") {
    Table nextTable = pkb.getNextTable();
    REQUIRE(nextTable.contains({ pkb.getIntRefFromStmtNum(1), pkb.getIntRefFromStmtNum(2) }));
    REQUIRE(nextTable.contains({ pkb.getIntRefFromStmtNum(1), pkb.getIntRefFromStmtNum(3) }));
    REQUIRE(nextTable.contains({ pkb.getIntRefFromStmtNum(1), pkb.getIntRefFromStmtNum(4) }));
    REQUIRE(nextTable.contains({ pkb.getIntRefFromStmtNum(4), pkb.getIntRefFromStmtNum(5) }));
    REQUIRE(nextTable.size() == 4);
  }

  SECTION("getNextStmtsFromCfg") {
    std::vector<int> neighbours1 = pkb.getNextStmtsFromCfg(1);
    std::vector<int> neighbours4 = pkb.getNextStmtsFromCfg(4);
    REQUIRE(std::find(neighbours1.begin(), neighbours1.end(), 2) != neighbours1.end());
    REQUIRE(std::find(neighbours1.begin(), neighbours1.end(), 3) != neighbours1.end());
    REQUIRE(std::find(neighbours1.begin(), neighbours1.end(), 4) != neighbours1.end());
    REQUIRE(neighbours1.size() == 3);
    REQUIRE(pkb.getNextStmtsFromCfg(2).size() == 0);
    REQUIRE(pkb.getNextStmtsFromCfg(3).size() == 0);
    REQUIRE(std::find(neighbours4.begin(), neighbours4.end(), 5) != neighbours4.end());
    REQUIRE(neighbours4.size() == 1);
    REQUIRE(pkb.getNextStmtsFromCfg(5).size() == 0);
    REQUIRE(pkb.getNextStmtsFromCfg(6).size() == 0);
  }
}
