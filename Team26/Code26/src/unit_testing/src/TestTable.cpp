#include "catch.hpp"

#include <unordered_set>
#include <vector>

#include "Table.h"

TEST_CASE("[TestTable] New Table") {

  SECTION("new empty table") {
    Table table;
    REQUIRE(table.getHeader() == std::vector<std::string>{ "" });
    REQUIRE(table.empty());
  }

  SECTION("new table with empty header") {
    Table tableWithEmptyHeader(2);
    REQUIRE(tableWithEmptyHeader.getHeader().size() == 2);
    REQUIRE(tableWithEmptyHeader.getHeader() == std::vector<std::string>{ "", "" });
    REQUIRE(tableWithEmptyHeader.getColumnIndex("1") == -1);
    REQUIRE(tableWithEmptyHeader.getColumnIndex("a") == -1);
  }

  SECTION("new table with given header") {
    Table tableWithHeader({ "0", "1" });
    REQUIRE(tableWithHeader.getHeader().size() == 2);
    REQUIRE(tableWithHeader.getHeader() == std::vector<std::string>{ "0", "1" });
    REQUIRE(tableWithHeader.getColumnIndex("0") == 0);
    REQUIRE(tableWithHeader.getColumnIndex("1") == 1);
  }

}

TEST_CASE("[TestTable] Set Header") {
  SECTION("valid header") {
    Table table(2);
    table.setHeader({ "0", "1" });
    std::vector<std::string> expectedHeader{ "0", "1" };
    REQUIRE(table.getHeader() == expectedHeader);
  }

  SECTION("valid header with empty string") {
    Table table(2);
    table.setHeader({ "", "a" });
    std::vector<std::string> expectedHeader{ "", "a" };
    REQUIRE(table.getHeader() == expectedHeader);
  }

  SECTION("valid header with duplicated empty string") {
    Table table(2);
    table.setHeader({ "", "" });
    std::vector<std::string> expectedHeader{ "", "" };
    REQUIRE(table.getHeader() == expectedHeader);
  }
}

TEST_CASE("[TestTable] Insert Data") {

  SECTION("valid insertion") {
    Table table;
    table.insertRow({ 1 });
    table.insertRow({ 2 });
    table.insertRow({ 3 });
    REQUIRE(!table.empty());
    REQUIRE(table.size() == 3);
    REQUIRE(table.contains({ 2 }));
  }
}

TEST_CASE("[TestTable] Get Data") {

  SECTION("one column") {
    Table table;
    table.insertRow({ 1 });
    table.insertRow({ 2 });
    table.insertRow({ 3 });
    REQUIRE(table.size() == 3);
    REQUIRE(table.contains({ 1 }));
  }

  SECTION("two columns") {
    Table table({ "0", "1" });
    table.insertRow({ 1, 11 });
    table.insertRow({ 2, 22 });
    table.insertRow({ 3, 33 });
    REQUIRE(table.size() == 3);
    REQUIRE(table.contains({ 1, 11 }));
  }

  SECTION("Empty table") {
    Table table(2);
    REQUIRE(table.size() == 0);
    REQUIRE(table.empty());
  }

}

TEST_CASE("[TestTable] Drop Column") {
  Table table({ "a", "b" });
  table.insertRow({ 1, 11 });
  table.insertRow({ 2, 22 });
  REQUIRE(table.dropColumn("a") == true);
  REQUIRE(table.getHeader() == std::vector<std::string>{"b"});
  REQUIRE(!table.contains({ 1, 11 }));
  REQUIRE(table.contains({ 11 }));
  REQUIRE(table.dropColumn("b") == false);
  REQUIRE(table.contains({ 11 }));
  REQUIRE(table.dropColumn("a") == false);
  REQUIRE(table.contains({ 11 }));
}

TEST_CASE("[TestTable] Concatenate") {
  SECTION("valid concatenation") {
    Table table1(2);
    table1.insertRow({ 1, 11 });
    table1.insertRow({ 2, 22 });
    Table table2(2);
    table2.insertRow({ 3, 33 });
    table1.concatenate(table2);
    REQUIRE(table1.contains({ 3, 33 }));
    REQUIRE(table1.size() == 3);
  }
}

TEST_CASE("[TestTable] Filter Column") {
  SECTION("valid filtration") {
    Table table({ "a", "b" });
    table.insertRow({ 1, 11 });
    table.insertRow({ 2, 22 });
    table.filterColumn(1, { 11 });
    REQUIRE(!table.contains({ 2, 22 }));
    REQUIRE(table.contains({ 1, 11 }));
  }

  SECTION("valid filtration with empty filter values") {
    Table table({ "a", "b" });
    table.insertRow({ 1, 11 });
    table.insertRow({ 2, 22 });
    table.filterColumn(1, {});
    REQUIRE(table.empty());
  }

  SECTION("valid filtration with non-existent filter values") {
    Table table({ "a", "b" });
    table.insertRow({ 1, 11 });
    table.insertRow({ 2, 22 });
    table.filterColumn(0, { 3 });
    REQUIRE(table.empty());
  }
}

TEST_CASE("[TestTable] naturalJoin table") {
  SECTION("cross product naturalJoin") {
    Table table1({ "a", "b" });
    table1.insertRow({ 1, 11 });
    table1.insertRow({ 2, 22 });
    Table table2({ "c", "d" });
    table2.insertRow({ 3, 33 });
    table2.insertRow({ 4, 44 });
    table1.naturalJoin(table2);
    REQUIRE(table1.size() == 4);
    REQUIRE(table1.getHeader() == std::vector<std::string> {"a", "b", "c", "d"});
    REQUIRE(table1.contains({ 1, 11, 3, 33 }));
    REQUIRE(table1.contains({ 1, 11, 4, 44 }));
    REQUIRE(table1.contains({ 2, 22, 3, 33 }));
    REQUIRE(table1.contains({ 2, 22, 4, 44 }));
  }

  SECTION("valid cross product naturalJoin with empty tables") {
    Table table1({ "a", "b" });
    Table table2({ "c", "d" });
    table1.naturalJoin(table2);
    REQUIRE(table1.size() == 0);
    REQUIRE(table1.getHeader() == std::vector<std::string> {"a", "b", "c", "d"});
  }

  SECTION("natural naturalJoin one overlapping column") {
    Table table1({ "a", "b" });
    table1.insertRow({ 1, 11 });
    table1.insertRow({ 2, 22 });
    Table table2({ "a", "c" });
    table2.insertRow({ 1, 33 });
    table2.insertRow({ 2, 44 });
    table1.naturalJoin(table2);
    REQUIRE(table1.size() == 2);
    REQUIRE(table1.getHeader() == std::vector<std::string> {"a", "b", "c"});
    REQUIRE(table1.contains({ 1, 11, 33 }));
    REQUIRE(table1.contains({ 2, 22, 44 }));
  }

  SECTION("natural naturalJoin one overlapping empty string column name") {
    Table table1({ "", "b" });
    table1.insertRow({ 1, 11 });
    table1.insertRow({ 2, 22 });
    Table table2({ "", "c" });
    table2.insertRow({ 1, 33 });
    table2.insertRow({ 2, 44 });
    table1.naturalJoin(table2);
    REQUIRE(table1.size() == 4);
    REQUIRE(table1.getHeader() == std::vector<std::string> {"", "b", "", "c"});
    REQUIRE(table1.contains({ 1, 11, 1, 33 }));
    REQUIRE(table1.contains({ 1, 11, 2, 44 }));
    REQUIRE(table1.contains({ 2, 22, 1, 33 }));
    REQUIRE(table1.contains({ 2, 22, 2, 44 }));
  }

  SECTION("natural naturalJoin two overlapping column") {
    Table table1({ "a", "b", "c" });
    table1.insertRow({ 1, 11, 33 });
    table1.insertRow({ 2, 22, 43 });
    Table table2({ "a", "c" });
    table2.insertRow({ 1, 33 });
    table2.insertRow({ 2, 44 });
    table1.naturalJoin(table2);
    REQUIRE(table1.size() == 1);
    REQUIRE(table1.getHeader() == std::vector<std::string>{"a", "b", "c"});
    REQUIRE(table1.contains({ 1, 11, 33 }));
    REQUIRE(!table1.contains({ 2, 22, 43 }));
  }

  SECTION("valid natural naturalJoin with empty tables") {
    Table table1({ "a", "b" });
    Table table2({ "a", "c" });
    table1.naturalJoin(table2);
    REQUIRE(table1.size() == 0);
    REQUIRE(table1.getHeader() == std::vector<std::string> {"a", "b", "c"});
  }
}

TEST_CASE("[TestTable] inner naturalJoin with indexes") {
  SECTION("inner naturalJoin with a common header specified") {
    Table table1({ "a", "b" });
    table1.insertRow({ 1, 11 });
    table1.insertRow({ 2, 22 });
    Table table2({ "a", "c" });
    table2.insertRow({ 1, 33 });
    table2.insertRow({ 2, 44 });
    table1.innerJoin(table2, 0, 0);
    REQUIRE(table1.size() == 2);
    REQUIRE(table1.getHeader() == std::vector<std::string> {"a", "b", "c"});
    REQUIRE(table1.contains({ 1, 11, 33 }));
    REQUIRE(table1.contains({ 2, 22, 44 }));
  }
}

TEST_CASE("[TestTable] inner naturalJoin with column name") {
  SECTION("inner naturalJoin with a common header specified") {
    Table table1({ "a", "b" });
    table1.insertRow({ 1, 11 });
    table1.insertRow({ 2, 22 });
    Table table2({ "a", "c" });
    table2.insertRow({ 1, 33 });
    table2.insertRow({ 2, 44 });
    table1.innerJoin(table2, "a");
    REQUIRE(table1.size() == 2);
    REQUIRE(table1.getHeader() == std::vector<std::string> {"a", "b", "c"});
    REQUIRE(table1.contains({ 1, 11, 33 }));
    REQUIRE(table1.contains({ 2, 22, 44 }));
  }
}

TEST_CASE("[TestTable] delete row") {
  SECTION("delete rows that exist") {
    Table table1({ "a", "b" });
    table1.insertRow({ 1, 11 });
    table1.insertRow({ 2, 22 });
    table1.insertRow({ 3, 33 });
    REQUIRE(table1.deleteRow({ 2, 22 }) == true);
    REQUIRE(table1.deleteRow({ 1, 11 }) == true);
    REQUIRE(table1.size() == 1);
  }

  SECTION("delete row that does not exist") {
    Table table1({ "a", "b" });
    table1.insertRow({ 1, 11 });
    table1.insertRow({ 2, 22 });
    REQUIRE(table1.deleteRow({ 3, 33 }) == false);
  }

  SECTION("delete row for empty table") {
    Table table1({ "a", "b" });
    REQUIRE(table1.deleteRow({ 3, 33 }) == false);
  }
}