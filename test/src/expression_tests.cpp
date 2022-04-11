#include <boost/test/unit_test.hpp>

#include <string_view>

#include "expression.hpp"

BOOST_AUTO_TEST_CASE(expression_test_1)
{
  const std::vector<Token> tokens {
    TokenInteger("123"), TokenText("+"), TokenInteger("42")
  };

  const auto expr = Expression{ "qw111", tokens };

  const auto value = expr.eval();

  BOOST_REQUIRE_EQUAL(value, 165);
}

BOOST_AUTO_TEST_CASE(expression_test_2)
{
  const std::vector<Token> tokens {
    TokenText("-"), TokenInteger("123"), TokenText("+"), TokenInteger("42")
  };

  const auto expr = Expression{ "qw111", tokens };

  const auto value = expr.eval();

  BOOST_REQUIRE_EQUAL(value, -81);
}

BOOST_AUTO_TEST_CASE(expression_test_3)
{
  const std::vector<Token> tokens {
    TokenText("-"), TokenText("!"), TokenInteger("0")
  };

  const auto expr = Expression{ "qw111", tokens };

  const auto value = expr.eval();

  BOOST_REQUIRE_EQUAL(value, -1);
}

BOOST_AUTO_TEST_CASE(expression_test_4)
{
  const std::vector<Token> tokens {
    TokenText("-"), TokenInteger("1"), TokenText("+"), TokenInteger("3")
  };

  const auto expr = Expression{ "qw111", tokens };

  const auto value = expr.eval();

  BOOST_REQUIRE_EQUAL(value, 2);
}

BOOST_AUTO_TEST_CASE(expression_test_5)
{
  const std::vector<Token> tokens {
    TokenText("-"), TokenInteger("1"), TokenText("-"), TokenInteger("3")
  };

  const auto expr = Expression{ "qw111", tokens };

  const auto value = expr.eval();

  BOOST_REQUIRE_EQUAL(value, -4);
}

