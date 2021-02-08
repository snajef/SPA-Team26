#include <catch.hpp>

#include "PKB.h"

TEST_CASE("[PKB.cpp] 1st Test") { 
	REQUIRE(1 == 1);
}

TEST_CASE("[PKB.cpp] varTable Insertion") {
	PKB pkb;
	pkb.addVar("x");
	auto dataCopy = pkb.getVarTable().getData();
	REQUIRE(dataCopy.find({ "x" }) != dataCopy.end());
}

TEST_CASE("[PKB.cpp] stmtTable Insertion") {
	PKB pkb;
	pkb.addStmt(2);
	auto dataCopy = pkb.getStmtTable().getData();
	REQUIRE(dataCopy.find({ "2" }) != dataCopy.end());
}

TEST_CASE("[PKB.cpp] procTable Insertion") {
	PKB pkb;
	pkb.addProc("main");
	auto dataCopy = pkb.getProcTable().getData();
	REQUIRE(dataCopy.find({ "main" }) != dataCopy.end());
}

TEST_CASE("[PKB.cpp] ifTable Insertion") {
	PKB pkb;
	pkb.addIf(12);
	auto dataCopy = pkb.getIfTable().getData();
	REQUIRE(dataCopy.find({ "12" }) != dataCopy.end());
}

TEST_CASE("[PKB.cpp] whileTable Insertion") {
	PKB pkb;
	pkb.addWhile(15);
	auto dataCopy = pkb.getWhileTable().getData();
	REQUIRE(dataCopy.find({ "15" }) != dataCopy.end());
}

TEST_CASE("[PKB.cpp] readTable Insertion") {
	PKB pkb;
	pkb.addRead(15);
	auto dataCopy = pkb.getReadTable().getData();
	REQUIRE(dataCopy.find({ "15" }) != dataCopy.end());
}

TEST_CASE("[PKB.cpp] printTable Insertion") {
	PKB pkb;
	pkb.addPrint(526);
	auto dataCopy = pkb.getPrintTable().getData();
	REQUIRE(dataCopy.find({ "526" }) != dataCopy.end());
}

TEST_CASE("[PKB.cpp] assignTable Insertion") {
	PKB pkb;
	pkb.addAssign(32);
	auto dataCopy = pkb.getAssignTable().getData();
	REQUIRE(dataCopy.find({ "32" }) != dataCopy.end());
}

TEST_CASE("[PKB.cpp] addFollows") {
	PKB pkb;

	SECTION("Check valid insertion") {
		pkb.addFollows(5, 6);
		auto dataCopy = pkb.getFollowsTable().getData();
		REQUIRE(dataCopy.count({ "5", "6" }) == 1);
	}

	SECTION("Check invalid insertion") {
		REQUIRE_THROWS_AS(pkb.addFollows(5, 4), std::logic_error);
	}
}

TEST_CASE("[PKB.cpp] addFollowsT") {
	PKB pkb;
	pkb.addFollows(5, 6);
	pkb.addFollows(6, 7);
	pkb.addFollowsT();
	auto dataCopy = pkb.getFollowsTTable().getData();
	REQUIRE(dataCopy.count({ "5", "6" }) == 1);
	REQUIRE(dataCopy.count({ "6", "7" }) == 1);
	REQUIRE(dataCopy.count({ "5", "7" }) == 1);
}

TEST_CASE("[PKB.cpp] addParent") {
	PKB pkb;

	SECTION("Check valid insertion") {
		pkb.addParent(5, 6);
		auto dataCopy = pkb.getParentTable().getData();
		REQUIRE(dataCopy.count({ "5", "6" }) == 1);
	}

	SECTION("Check invalid insertion") {
		REQUIRE_THROWS_AS(pkb.addParent(5, 4), std::logic_error);
	}
}

TEST_CASE("[PKB.cpp] addParentT") {
	PKB pkb;
	pkb.addParent(5, 6);
	pkb.addParent(6, 7);
	pkb.addParentT();
	auto dataCopy = pkb.getParentTTable().getData();
	REQUIRE(dataCopy.count({ "5", "6" }) == 1);
	REQUIRE(dataCopy.count({ "6", "7" }) == 1);
	REQUIRE(dataCopy.count({ "5", "7" }) == 1);
}