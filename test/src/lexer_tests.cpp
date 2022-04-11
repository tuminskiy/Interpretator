#include <boost/test/unit_test.hpp>

#include <string_view>

#include "lexer.hpp"

using namespace std::literals::string_view_literals;

template <class T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& v) {
  os << "{\n";

  for (const auto& i : v) {
    os << "\t" << i << "\n";
  }

  return os << "}";
}

BOOST_AUTO_TEST_CASE(lexer_test_tokenize)
{
  const std::string_view program =
    "Анализ 1.42 2.53 \t\t 42.1 Конец анализа\n"
    "1: ПР1 = 1 - 2\n"
    "2 1 3:ПР2=ПР1 / 42 & 0 | !5";

  const std::vector<Token> expected = {
    TokenText("Анализ"),
    TokenReal("1.42"),
    TokenReal("2.53"),
    TokenReal("42.1"),
    TokenText("Конец"),
    TokenText("анализа"),
    TokenInteger("1"),
    TokenText(":"),
    TokenText("ПР1"),
    TokenText("="),
    TokenInteger("1"),
    TokenText("-"),
    TokenInteger("2"),
    TokenInteger("2"),
    TokenInteger("1"),
    TokenInteger("3"),
    TokenText(":"),
    TokenText("ПР2"),
    TokenText("="),
    TokenText("ПР1"),
    TokenText("/"),
    TokenInteger("42"),
    TokenText("&"),
    TokenInteger("0"),
    TokenText("|"),
    TokenText("!"),
    TokenInteger("5"),
  };

  const auto actual = Lexer::tokenize(program);

  BOOST_REQUIRE_EQUAL(actual, expected);

  BOOST_REQUIRE_EQUAL(Lexer::tokenize(""), std::vector<Token>{});
}


BOOST_AUTO_TEST_CASE(lexer_test_analyze_valid_1)
{
  const std::string_view program =
    "Начало \n"
    "Анализ 1.42 6.12 Конец анализа;"
    "Синтез 0.11 Конец синтеза "
    "1: те1 = 1 - 2 "
    "\tКонец";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, std::string{});
  BOOST_REQUIRE(result.token_it == tokens.end());
  BOOST_REQUIRE_EQUAL(result.success, true);
}

BOOST_AUTO_TEST_CASE(lexer_test_analyze_valid_2)
{
  const std::string_view program =
    "Начало \n"
    "Анализ 1.42 6.12 Конец анализа;"
    "Синтез 0.11 Конец синтеза "
    "1: те1 = 1 | 2 "
    "\tКонец";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, std::string{});
  BOOST_REQUIRE(result.token_it == tokens.end());
  BOOST_REQUIRE_EQUAL(result.success, true);
}

BOOST_AUTO_TEST_CASE(lexer_test_analyze_not_valid_1)
{
  const std::string_view program =
    "Начало \n"
    "Анализ 1.42 6.12 Конец анализа;"
    "Синтез 0.11 Конец синтеза "
    "1 2 :  бб123 = аа111 & 1 "
    "\t";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, "Язык должен заканчиватся словом 'Конец'");
  BOOST_REQUIRE_EQUAL(result.success, false);
}


BOOST_AUTO_TEST_CASE(lexer_test_analyze_not_valid_2)
{
  const std::string_view program =
    "Начало \n"
    "Анализ 1.42 6.12 Конец анализа;"
    "Синтез 0.11 Конец синтеза "
    "6 : аб1 = !1 / sin 2 "
    "1 : бб2 = 7 * cos аб1 "
    "Конец конеец\t";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, "После слова 'Конец' не должно ничего идти");
  BOOST_REQUIRE(result.token_it != tokens.end());
  BOOST_REQUIRE_EQUAL(result.success, false);
}


BOOST_AUTO_TEST_CASE(lexer_test_analyze_not_valid_3)
{
  const std::string_view program =
    "\n"
    "Анализ 1.42 6.12 Конец анализа;"
    "Синтез 0.11 Конец синтеза "
    "\tКонец";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, "Язык должен начинаться со слова 'Начало'");
  BOOST_REQUIRE_EQUAL(result.success, false);
  BOOST_REQUIRE(result.token_it != tokens.end());
}


BOOST_AUTO_TEST_CASE(lexer_test_analyze_not_valid_4)
{
  const std::string_view program =
    "Начало начало\n"
    "Анализ 1.42 6.12 Конец анализа;"
    "Синтез 0.11 Конец синтеза "
    "\tКонец";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, "Определение должно начинаться со слова 'Анализ' или 'Синтез'");
  BOOST_REQUIRE_EQUAL(result.success, false);
  BOOST_REQUIRE(result.token_it != tokens.end());
}


BOOST_AUTO_TEST_CASE(lexer_test_analyze_not_valid_5)
{
  const std::string_view program =
    "Начало\n"
    "Анализ анализ 1.42 6.12 Конец анализа;"
    "Синтез 0.11 Конец синтеза "
    "\tКонец";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, "После слова 'Анализ' ожидается вещественное число");
  BOOST_REQUIRE_EQUAL(result.success, false);
  BOOST_REQUIRE(result.token_it != tokens.end());
}


BOOST_AUTO_TEST_CASE(lexer_test_analyze_not_valid_6)
{
  const std::string_view program =
    "Начало\n"
    "Анализ 1.42 6.12 Конец анализа;"
    "Синтез 0.11 Конец анализа "
    "\tКонец";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, "Ожидается 'Конец синтеза' так как определение начиналось со слова 'Синтез'");
  BOOST_REQUIRE_EQUAL(result.success, false);
  BOOST_REQUIRE(result.token_it != tokens.end());
}


BOOST_AUTO_TEST_CASE(lexer_test_analyze_not_valid_7)
{
  const std::string_view program =
    "Начало\n"
    "Анализ 1.42 6.12 Конец анализа;"
    "0.11 Конец cинтеза "
    "\tКонец";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, "Определение должно начинаться со слова 'Анализ' или 'Синтез'");
  BOOST_REQUIRE_EQUAL(result.success, false);
  BOOST_REQUIRE(result.token_it != tokens.end());
}

BOOST_AUTO_TEST_CASE(lexer_test_analyze_not_valid_8)
{
  const std::string_view program =
    "Начало\n";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, "После слова 'Начало' ожидается слово 'Анализ' или 'Синтез'");
  BOOST_REQUIRE_EQUAL(result.success, false);
  BOOST_REQUIRE(result.token_it == tokens.end());
}


BOOST_AUTO_TEST_CASE(lexer_test_analyze_not_valid_10)
{
  const std::string_view program =
    "Начало\n"
    "Анализ 1.42 6.12 "
    "Синтез 0.11 Конец cинтеза "
    "\tКонец";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, "После вещественных чисел в определении ожидается 'Конец анализа'");
  BOOST_REQUIRE_EQUAL(result.success, false);
  BOOST_REQUIRE(result.token_it != tokens.end());
}


BOOST_AUTO_TEST_CASE(lexer_test_analyze_not_valid_11)
{
  const std::string_view program =
    "Начало\n"
    "Анализ 1 6.12 Конец анализа; "
    "Синтез 0.11 Конец cинтеза "
    "\tКонец";

  const auto tokens = Lexer::tokenize(program);

  const auto result = Lexer::analyze(tokens);

  BOOST_REQUIRE_EQUAL(result.msg, "После слова 'Анализ' ожидается вещественное число");
  BOOST_REQUIRE_EQUAL(result.success, false);
  BOOST_REQUIRE(result.token_it != tokens.end());
}
