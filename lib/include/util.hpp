#pragma once

#include "token.hpp"

class Util final
{
public:
  static bool is_variable(const Token& token);

  static bool is_valid_variable(std::string_view variable);

  static bool is_integer(const Token& token);

  static bool is_function(const Token& token);

  static bool is_operator(const Token& token);

  static std::string_view get_token(const Token& token);

  template <int Index>
  static bool is_expected_token(const Token& token, std::string_view expected) {
    return token.index() == Index && std::get<Index>(token).token() == expected;
  }
};

