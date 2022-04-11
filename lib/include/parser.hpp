#pragma once

#include <vector>
#include <optional>

#include "token.hpp"
#include "expression.hpp"


class Parser
{
  using Expressions = std::vector<Expression>;

  using It = typename std::vector<Token>::const_iterator;

public:
  static auto parse(const std::vector<Token>& tokens) -> Expressions;

private:
  static auto get_expression(It first, It last) -> Expression;
};

