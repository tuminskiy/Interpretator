#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <queue>
#include <functional>

#include "token.hpp"

class Expression final
{
  static const std::unordered_map<std::string_view, int> m_operator_priority;

  std::string m_variable;
  std::vector<Token> m_right_part;

  std::function<std::optional<double>(std::string_view)> m_get_var_value;

public:
  explicit Expression(std::string_view variable, const std::vector<Token>& right_part);

  void set_var_value_getter(std::function<std::optional<double>(std::string_view)> func);

  auto eval() const -> double;

  auto variable() const -> std::string_view;

  auto right_part() const -> const std::vector<Token>&;

private:
  static std::queue<Token> convert_to_rpn(const std::vector<Token>& tokens);
};

