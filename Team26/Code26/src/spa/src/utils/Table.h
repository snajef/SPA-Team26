#pragma once

#include <exception>
#include <string>
#include <unordered_set>
#include <vector>

typedef std::vector<std::string> Header;
typedef std::vector<int> Row;

// Hash function from boost::hash_combine
// Source: https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
struct RowHash {
  std::size_t operator()(Row const& elements) const {
    std::size_t seed = elements.size();
    for (const size_t& i : elements) {
      seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

typedef std::unordered_set<Row, RowHash> RowSet;

class Table {
private:
  Header header;
  RowSet data;

public:
  /**
   * Constructor for Table.
   */
  Table();

  /**
   * Constructor for Table with specified number of columns.
   *
   * @param n The number of columns.
   */
  explicit Table(size_t n);

  /**
   * Constructor for Table with specified headers.
   *
   * @param newHeader A vector of strings corresponding to the header titles.
   */
  explicit Table(const Header& header);

  /**
   * Replaces the current headers with new headers.
   *
   * @param newHeader A vector of strings corresponding to the header titles.
   */
  void setHeader(const Header& newHeader);

  /**
   * Inserts a new row into Table.
   *
   * @param row The new row of data to be inserted.
   */
  void insertRow(const Row& row);

  /**
   * @return The headers of the Table.
   */
  Header getHeader() const;

  /**
   * @return The data of the table.
   */
  RowSet getData() const;

  /**
   * Returns the column index of the Table under the specified header.
   * Returns -1 if header is not found.
   *
   * @param headerTitle The specified header.
   * @return The index of the column under the header if found. Otherwise, return -1.
   */
  size_t getColumnIndex(const std::string& headerTitle) const;

  /**
   * Deletes a column from the Table at the specified index if the index is not out of bound.
   * This method also deletes the header, reducing the number of columns of Table by 1.
   *
   * @param index The column index of the Table.
   * @return boolean Whether column has been dropped.
   */
  bool dropColumn(const size_t index);

  /**
   * Deletes a column from the Table at the index corresponding to the specified
   * header title if the title is found.
   * This method also deletes the header, reducing the number of columns of Table by 1.
   *
   * @param headerTitle The specified column header.
   * @return boolean Whether column has been dropped.
   */
  bool dropColumn(const std::string& headerTitle);

  /**
   * Filter table with specified headers to keep. Must contain at least one 
   * existing header to keep. All other columns will be dropped.
   *
   * @param headerTitles The specified headers to keep.
   */
  void Table::filterHeaders(const std::unordered_set<std::string>& headersToKeep);

  /**
   * Filter the table rows based on the values for a particular column.
   * If the values from the column at the specified index in the table matches any of the
   * specified values, that particular row would be preserved.
   * Otherwise, if there is no match, that row would be deleted.
   *
   * @param index The column index to be based on for filtering the Table.
   * @param values A set of values to be checked upon when filtering the Table.
   */
  void filterColumn(const size_t index, const std::unordered_set<int>& values);

  /**
   * Concatenates two tables with the same header size.
   * The data from the other table is appended into the original table.
   * The other table remains unaltered.
   *
   * @param otherTable The other table.
   */
  void concatenate(Table& otherTable);

  /**
   * Joins two tables using natural join.
   *
   * The data from the other table would be added to the original table.
   * The other table remains unaltered.
   *
   * @param otherTable The other table.
   */
  void naturalJoin(const Table& otherTable);

  /**
   * Cross joins two tables using cross join.
   *
   * The data from the other table would be added to the original table.
   * The other table remains unaltered.
   *
   * @param otherTable The other table.
   */
  void crossJoin(const Table& otherTable);

  /**
   * Joins two tables using inner join based on the specified index pairs.
   * The index pairs must contain the index of the first table and then the second table.
   *
   * The data from the other table would be added to the original table.
   * The other table remains unaltered.
   *
   * @param otherTable The other table.
   * @param indexPairs The index pairs representing the column index for each table.
   */
  void innerJoin(const Table& otherTable, 
    const std::vector<std::pair<size_t, size_t>>& indexPairs);

  /**
   * Joins two tables using inner join based on the specified indices.
   *
   * The data from the other table would be added to the original table.
   * The other table remains unaltered.
   *
   * @param otherTable The other table.
   * @param thisTableIndex The index of the original table.
   * @param otherTableIndex The index of the other table.
   */
  void innerJoin(const Table& otherTable, size_t thisTableIndex, size_t otherTableIndex);

  /**
   * Joins two tables using inner join based on the specified common header.
   * If the specified header is not common to the two Tables, an error is thrown.
   *
   * The data from the other table would be added to the original table.
   * The other table remains unaltered.
   *
   * @param otherTable The other table.
   * @param commonHeader The common header for both Tables.
   */
  void innerJoin(const Table& otherTable, const std::string& commonHeader);

  /**
   * Deletes a row from the Table.
   *
   * @param row The specified row.
   * @return True if a row is deleted. Otherwise, false
   */
  bool deleteRow(const Row& row);

  /**
   * @return Returns the number of rows of the Table data.
   */
  size_t size() const;

  /**
   * @param row The specified row.
   * @return Returns true if the Table data contains the specified row. Otherwise, returns false.
   */
  bool contains(const Row& row) const;

  /**
   * @return Returns true if the Table data is empty. Otherwise, returns false.
   */
  bool empty() const;

private:
  /**
   * Returns a list of pairs which have the same header titles.
   * The pair contains two integers. The first integer refers to the column index
   * of the first table. The second integer refers to that of the second table.
   *
   * @param otherTable The second table to be compared with the first table.
   * @return A list of integer pairs corresponding to the column index of the tables.
   */
  std::vector<std::pair<size_t, size_t>> getColumnIndexPairs(const Table& otherTable) const;
};
