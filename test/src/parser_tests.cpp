#include <boost/test/unit_test.hpp>

#include <string_view>

#include "parser.hpp"

BOOST_AUTO_TEST_CASE(parser_test)
{
  const std::vector<Token> tokens {
    TokenInteger("0"), TokenText(":"), TokenText("ab111"), TokenText("="), TokenInteger("123"), TokenText("+"), TokenInteger("42"),
    TokenInteger("1"), TokenText(":"), TokenText("cc1"), TokenText("="), TokenInteger("1"), TokenText("&"), TokenInteger("0"),
    TokenInteger("17"), TokenText(":"), TokenText("cc2"), TokenText("="), TokenText("sin"), TokenText("ab111"),
    TokenText("Конец")
  };

  const auto expressions = Parser::parse(tokens);

  BOOST_REQUIRE_EQUAL(expressions.size(), 3);

  BOOST_REQUIRE_EQUAL(expressions[0].variable(), "ab111");
  BOOST_REQUIRE_EQUAL(expressions[1].variable(), "cc1");
  BOOST_REQUIRE_EQUAL(expressions[2].variable(), "cc2");

  std::vector<Token> right_part = expressions[0].right_part();

  BOOST_REQUIRE_EQUAL(right_part.size(), 3);
  BOOST_REQUIRE_EQUAL(right_part[0], Token{ TokenInteger("123") });
  BOOST_REQUIRE_EQUAL(right_part[1], Token{ TokenText("+") });
  BOOST_REQUIRE_EQUAL(right_part[2], Token{ TokenInteger("42") });

  right_part = expressions[1].right_part();

  BOOST_REQUIRE_EQUAL(right_part.size(), 3);
  BOOST_REQUIRE_EQUAL(right_part[0], Token{ TokenInteger("1") });
  BOOST_REQUIRE_EQUAL(right_part[1], Token{ TokenText("&") });
  BOOST_REQUIRE_EQUAL(right_part[2], Token{ TokenInteger("0") });

  right_part = expressions[2].right_part();

  BOOST_REQUIRE_EQUAL(right_part.size(), 2);
  BOOST_REQUIRE_EQUAL(right_part[0], Token{ TokenText("sin") });
  BOOST_REQUIRE_EQUAL(right_part[1], Token{ TokenText("ab111") });
}
