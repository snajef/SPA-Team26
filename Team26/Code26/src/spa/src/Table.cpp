#include "Table.h"
#include <algorithm>
#include <iterator>
#include <map>
#include <stack>
#include <stdexcept>

Table::Table() {
  headerRow.emplace_back("");
}

Table::Table(Row header) {
  headerRow = header;
}

void Table::setHeader(Row header) {
  headerRow = header;
}

Table::Row Table::getHeader() {
  return headerRow;
}

void Table::insertRow(Row row) {
  data.emplace(row);
}

int Table::getColumnIndex(std::string columnName) {
  for (int i = 0; i < headerRow.size(); ++i) {
    if (headerRow[i] == columnName) {
      return i;
    }
  }
  throw std::logic_error("Column with name:" + columnName + " not found");
}

std::set<std::string> Table::getColumn(std::string columnName) {
  int index = getColumnIndex(columnName);
  std::set<std::string> column;
  for (auto &dataRow : data) {
    column.emplace(dataRow[index]);
  }
  return column;
}

int Table::size() {
  return data.size();
}

bool Table::contains(Row row) {
  return data.count(row);
}

bool Table::empty() {
  return data.empty();
}

std::set<Table::Row> Table::getData() {
  return data;
}

std::set<Table::Row> Table::getDataWithColumns(Row columnNames) {
  std::vector<int> indexList;
  for (auto columnName : columnNames) {
    auto it = std::find(headerRow.begin(), headerRow.end(), columnName);
    if (it == headerRow.end()) {
      throw std::logic_error("Column: " + columnName + " does not exist");
    }
    indexList.emplace_back(std::distance(headerRow.begin(), it));
  }

  std::set<Row> result;
  for (auto dataRow : data) {
    Row curr;
    for (auto i : indexList) {
      curr.emplace_back(dataRow[i]);
    }
    result.insert(curr);
  }
  return result;
}

void Table::dropColumn(std::string toDrop) {
  int index = getColumnIndex(toDrop);
  headerRow.erase(headerRow.begin() + index);
  std::set<Row> newData;
  for (auto &dataRow : data) {
    Row curr;
    curr.reserve(dataRow.size() - 1);
    auto it = dataRow.begin();
    std::advance(it, index);
    curr.assign(dataRow.begin(), it++);
    curr.insert(curr.end(), it, dataRow.end());
    newData.emplace_hint(newData.end(), std::move(curr));
  }
  data = std::move(newData);
}

void Table::selfJoin() {
  auto it = data.begin();
  while (it != data.end()) {
    if (it->at(0) != it->at(1)) {
      it = data.erase(it);
    } else {
      ++it;
    }
  }
  dropColumn(headerRow[1]);
}

void dfs(std::string v, std::map<std::string, std::set<std::string>> adjList,
  std::map<std::string, std::set<std::string>> tMap) {
  std::set<std::string> visited;
  std::stack<std::string> stack;
  std::string curr;

  stack.push(v);

  while (!stack.empty()) {
    curr = stack.top();
    stack.pop();

    if (!(visited.find(curr) != visited.end())) {
      visited.insert(curr);
      if (adjList.find(curr) != adjList.end()) {
        for (auto value : adjList.at(curr)) {
          stack.push(value);
        }
      }
    }
  }
  tMap.insert({ v, std::move(visited) });
}

// used for transitive closure for relationships Follows* and Parent*
void Table::fillTransitiveTable(Table table) {
  std::map<std::string, std::set<std::string>> adjList;
  for (auto row : table.getData()) {
    if (!(adjList.find(row[0]) != adjList.end())) {
      adjList.insert({ row[0], std::set<std::string>({ row[1] }) });
    } else {
      adjList.at(row[0]).insert(row[1]);
    }
  }

  std::map<std::string, std::set<std::string>> tMap;
  for (auto entry : tMap) {
    dfs(entry.first, adjList, tMap);
  }

  for (auto entry : tMap) {
    for (auto value : entry.second) {
      data.insert(Row({ entry.first, value }));
    }
  }
}