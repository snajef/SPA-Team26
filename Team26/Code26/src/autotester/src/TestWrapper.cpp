#include "TestWrapper.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() : spa(Spa()) {}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
  spa.parseSourceFile(filename);
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
  // call your evaluator to evaluate the query here
  spa.evaluateQuery(query, results);
  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.
}
