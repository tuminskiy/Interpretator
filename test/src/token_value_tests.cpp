#include <boost/test/unit_test.hpp>

#include <string_view>

#include "token.hpp"

using namespace std::literals::string_view_literals;

BOOST_AUTO_TEST_CASE(token_value_valid_construct_test)
{
  auto token = TokenReal("2.123");

  BOOST_REQUIRE_EQUAL(token.name(), "real token"sv);
  BOOST_REQUIRE_EQUAL(token.token(), "2.123");

  token = TokenReal("42");

  BOOST_REQUIRE_EQUAL(token.name(), "real token"sv);
  BOOST_REQUIRE_EQUAL(token.token(), "42");

  token = TokenReal("-1.42");

  BOOST_REQUIRE_EQUAL(token.name(), "real token"sv);
  BOOST_REQUIRE_EQUAL(token.token(), "-1.42");

  token = TokenReal(".42");

  BOOST_REQUIRE_EQUAL(token.name(), "real token"sv);
  BOOST_REQUIRE_EQUAL(token.token(), ".42");
}

BOOST_AUTO_TEST_CASE(token_value_not_valid_construct_test)
{
  try {
    auto token = TokenReal("abc");
    BOOST_FAIL("TokenValue constructed from invalid value: 'abc'");
  } catch (const std::invalid_argument& e) {
    const auto msg = e.what();
    BOOST_REQUIRE_EQUAL(msg, "Token is not value: abc");
  } catch (const std::exception&) {
    BOOST_FAIL("Throwed exception is not 'std::invalid_argument'");
  }

  try {
    auto token = TokenReal(".42.42");
    BOOST_FAIL("TokenValue constructed from invalid value: '.42.42'");
  } catch (const std::invalid_argument& e) {
    const auto msg = e.what();
    BOOST_REQUIRE_EQUAL(msg, "Token is not value: .42.42");
  } catch (const std::exception&) {
    BOOST_FAIL("Throwed exception is not 'std::invalid_argument'");
  }
}
