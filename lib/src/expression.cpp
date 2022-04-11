#include "expression.hpp"
#include "util.hpp"

#include <boost/lexical_cast.hpp>

#include <stack>

const std::unordered_map<std::string_view, int> Expression::m_operator_priority = {
  { "!", 5 },
  { "--", 4 },
  { "&", 3 }, { "|", 3 },
  { "*", 2 }, { "/", 2 },
  { "+", 1 }, { "-", 1 }
};

Expression::Expression(std::string_view variable, const std::vector<Token>& right_part)
  : m_variable{ variable }
  , m_right_part{ right_part }
{ }

void Expression::set_var_value_getter(std::function<std::optional<double>(std::string_view)> func)
{
  m_get_var_value = func;
}

auto Expression::eval() const -> double
{
  auto rpn = convert_to_rpn(m_right_part);

  std::stack<double> helper;
  constexpr auto text_index = static_cast<int>(TokenIndex::Text);

  while (!rpn.empty()) {
    const auto token = rpn.front();
    rpn.pop();

    if (Util::is_integer(token)) {
      const auto int_text = std::get<TokenInteger>(token).token();
      helper.push(boost::lexical_cast<double>(int_text));
    } else if (Util::is_operator(token) || Util::is_expected_token<text_index>(token, "--")) {
      const auto op_text = std::get<TokenText>(token).token();

      if (op_text == "!") {
        auto value = static_cast<int>(helper.top());
        helper.top() = !value;
      } else if (op_text == "--") {
        helper.top() = -helper.top();
      } else {
        auto rhs = helper.top(); helper.pop();
        auto lhs = helper.top(); helper.pop();

        auto int_lhs = static_cast<int>(lhs);
        auto int_rhs = static_cast<int>(rhs);

        if      (op_text == "+") helper.push(lhs + rhs);
        else if (op_text == "-") helper.push(lhs - rhs);
        else if (op_text == "*") helper.push(lhs * rhs);
        else if (op_text == "/") helper.push(lhs / rhs);
        else if (op_text == "&") helper.push(int_lhs && int_rhs);
        else if (op_text == "|") helper.push(int_lhs || int_rhs);
      }
    } else if (Util::is_function(token)) {
      const auto func_text = std::get<TokenText>(token).token();
      auto& value = helper.top();

      if      (func_text == "sin") value = std::sin(value);
      else if (func_text == "cos") value = std::cos(value);
      else if (func_text == "tan") value = std::tan(value);

    } else if (Util::is_variable(token)) {
      if (!m_get_var_value)
        throw std::logic_error("Не установлена функция получения значений переменных");

      const auto var = std::get<TokenText>(token).token();
      const auto value = m_get_var_value(var);

      if (!value)
        throw std::logic_error("Неизвестная переменная '" + std::string{var} + "'");

      helper.push(value.value());
    }
  }

  return helper.top();
}

auto Expression::variable() const -> std::string_view { return m_variable; }

auto Expression::right_part() const -> const std::vector<Token>& { return m_right_part; }

std::queue<Token> Expression::convert_to_rpn(const std::vector<Token>& tokens)
{
  std::queue<Token> out;
  std::stack<Token> helper;

  bool is_first = true;

  for (const auto& token : tokens) {
    if (Util::is_integer(token) || Util::is_variable(token)) {
      out.push(token);
    } else if (Util::is_function(token)) {
      helper.push(token);
    } else if (Util::is_operator(token)) {
      constexpr auto text_index = static_cast<int>(TokenIndex::Text);

      auto comp_token = token;

      if (is_first && Util::is_expected_token<text_index>(token, "-")) {
        comp_token = TokenText("--");
        is_first = false;
      }

      if (!helper.empty()) {
        auto& top_token = helper.top();
        const auto current_op = std::get<TokenText>(comp_token).token();
        const auto top_op = std::get<TokenText>(top_token).token();

        while ((m_operator_priority.at(current_op) < m_operator_priority.at(top_op))) {
          out.push(top_token);
          helper.pop();

          if (helper.empty())
            break;

          top_token = helper.top();
        }
      }

      helper.push(comp_token);
    }
  }

  while (!helper.empty()) {
    out.push(helper.top());
    helper.pop();
  }

  return out;
}


