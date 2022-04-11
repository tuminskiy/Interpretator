#include <boost/test/unit_test.hpp>

#include <string_view>

#include "token.hpp"

using namespace std::literals::string_view_literals;

BOOST_AUTO_TEST_CASE(token_text_construct_test)
{
  auto token = TokenText("Начало");

  BOOST_REQUIRE_EQUAL(token.name(), "text token");
  BOOST_REQUIRE_EQUAL(token.token(), "Начало"sv);

  try {
    const auto _ = token.evalute();
    BOOST_FAIL("TokenText::evalute() must throw exception");
  } catch (const std::logic_error& e) {
    const auto msg = e.what();
    BOOST_REQUIRE_EQUAL(msg, "Could not evalute text token");
  } catch (const std::exception&) {
    BOOST_FAIL("Throwed exception is not 'std::logic_error'");
  }
}
