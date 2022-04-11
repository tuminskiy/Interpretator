#include "parser.hpp"
#include "util.hpp"

#include <algorithm>

auto Parser::parse(const std::vector<Token>& tokens) -> Expressions
{
  Expressions exprs;

  const auto by_text = [](std::string_view text) {
    return [text](const Token& token) {
      constexpr auto text_index = static_cast<int>(TokenIndex::Text);

      return token.index() == text_index
          && std::get<text_index>(token).token() == text;
    };
  };

  const auto is_colon = by_text(":");

  auto first = std::next(std::find_if(tokens.cbegin(), tokens.cend(), is_colon));
  const auto last = std::find_if(first, tokens.cend(), by_text("Конец"));

  while (first != last) {
    exprs.push_back(get_expression(first, last));

    first = std::find_if(first, last, is_colon);

    if (first != last)
      ++first;
  }

  return exprs;
}

auto Parser::get_expression(It first, It last) -> Expression
{
  constexpr auto text_index = static_cast<int>(TokenIndex::Text);

  const auto variable = std::get<text_index>(*first).token();

  std::advance(first, 2); // skip '='

  auto expr_end = std::adjacent_find(first, last,
    [](const Token& lhs, const Token& rhs) {
      const auto lhs_is_variable = Util::is_variable(lhs);
      const auto lhs_is_integer = Util::is_integer(lhs);
      const auto rhs_is_text = rhs.index() == text_index && !Util::is_function(rhs) && !Util::is_operator(rhs);
      const auto rhs_is_integer = Util::is_integer(rhs);

      return (lhs_is_variable || lhs_is_integer)
          && (rhs_is_text || rhs_is_integer);
    }
  );


  if (expr_end != last)
    ++expr_end;

  return Expression{variable, std::vector<Token>(first, expr_end)};
}
