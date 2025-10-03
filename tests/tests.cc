#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif

#include "atm.hpp"
#include "catch.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Helper Definitions //
/////////////////////////////////////////////////////////////////////////////////////////////

bool CompareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1);
  std::ifstream f2(p2);

  if (f1.fail() || f2.fail()) {
    return false;  // file problem
  }

  std::string f1_read;
  std::string f2_read;
  while (f1.good() || f2.good()) {
    f1 >> f1_read;
    f2 >> f2_read;
    if (f1_read != f2_read || (f1.good() && !f2.good()) ||
        (!f1.good() && f2.good()))
      return false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Test Cases
/////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Example: Create a new account", "[ex-1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto accounts = atm.GetAccounts();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);

  Account sam_account = accounts[{12345678, 1234}];
  REQUIRE(sam_account.owner_name == "Sam Sepiol");
  REQUIRE(sam_account.balance == 300.30);

  auto transactions = atm.GetTransactions();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);
  std::vector<std::string> empty;
  REQUIRE(transactions[{12345678, 1234}] == empty);
}

TEST_CASE("Example: Simple widthdraw", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  atm.WithdrawCash(12345678, 1234, 20);
  auto accounts = atm.GetAccounts();
  Account sam_account = accounts[{12345678, 1234}];

  REQUIRE(sam_account.balance == 280.30);
}

TEST_CASE("Example: Print Prompt Ledger", "[ex-3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto& transactions = atm.GetTransactions();
  transactions[{12345678, 1234}].push_back(
      "Withdrawal - Amount: $200.40, Updated Balance: $99.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $40000.00, Updated Balance: $40099.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $32000.00, Updated Balance: $72099.90");
  atm.PrintLedger("./prompt.txt", 12345678, 1234);
  REQUIRE(CompareFiles("./ex-1.txt", "./prompt.txt"));
}

TEST_CASE("Duplicate fail", "[ex-4]") {

  Atm atm;
  atm.RegisterAccount(10000001, 1001, "A", 10.0);

  REQUIRE_THROWS_AS(atm.RegisterAccount(10000001, 1001, "A", 99.0), std::invalid_argument);
}

TEST_CASE("WithdrawCash negative and overdraft", "[ex-5]") {
  
  Atm atm;
  atm.RegisterAccount(10000002, 1002, "B", 50.0);

  REQUIRE_THROWS_AS(atm.WithdrawCash(10000002, 1002, -5.0), std::invalid_argument);
  REQUIRE_THROWS_AS(atm.WithdrawCash(10000002, 1002, 60.0), std::runtime_error);

  auto accounts = atm.GetAccounts();

  REQUIRE(accounts[{10000002, 1002}].balance == 50.0);
}

TEST_CASE("DepositCash negative", "[ex-6]") {

  Atm atm;

  atm.RegisterAccount(10000003, 1003, "C", 0.0);
  REQUIRE_THROWS_AS(atm.DepositCash(10000003, 1003, -0.01), std::invalid_argument);
}

TEST_CASE("Invalid credentials deposit/withdraw", "[ex-7]") {

  Atm atm;

  REQUIRE_THROWS_AS(atm.DepositCash(99999999, 9999, 5.0), std::invalid_argument);
  REQUIRE_THROWS_AS(atm.WithdrawCash(99999999, 9999, 5.0), std::invalid_argument);
}

TEST_CASE("PrintLedger NE account", "[ex-8]") {
  Atm atm;

  REQUIRE_THROWS_AS(atm.PrintLedger("./nope.txt", 42424242, 4242), std::invalid_argument);
}

TEST_CASE("Transactions logged", "[ex-9]") {

  Atm atm;
  atm.RegisterAccount(10000004, 1004, "D", 100.0);

  atm.DepositCash(10000004, 1004, 25.0);
  atm.WithdrawCash(10000004, 1004, 10.0);

  auto& tx = atm.GetTransactions();
  REQUIRE(tx.count({10000004, 1004}) == 1);
  REQUIRE(tx[{10000004, 1004}].size() == 2);

}

TEST_CASE("DepositCash adds", "[ex-10]") {

  Atm atm;
  atm.RegisterAccount(77777777, 7777, "Grace", 10.0);

  atm.DepositCash(77777777, 7777, 25.0); 

  auto accounts = atm.GetAccounts();
  REQUIRE(accounts[{77777777, 7777}].balance == Approx(35.0));

}