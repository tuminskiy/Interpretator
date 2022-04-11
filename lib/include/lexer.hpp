#pragma once

#include <vector>
#include "token.hpp"

class Lexer final
{
public:
  using It = std::vector<Token>::const_iterator;

  static std::vector<Token> tokenize(std::string_view program);

  struct AnalyzeResult
  {
    std::string msg;
    bool success;
    It token_it;
    bool from_func = false;
  };

  static AnalyzeResult analyze(const std::vector<Token>& tokens);

protected:
  static AnalyzeResult analyze_langugage(It first, It last);

  static AnalyzeResult analyze_definitions(It first, It last);

  static AnalyzeResult analyze_definition(It first, It last);

  static AnalyzeResult analyze_operations(It first, It last);

  static AnalyzeResult analyze_operation(It first, It last);

  static AnalyzeResult analyze_variable(It first, It last);

  static AnalyzeResult analyze_right_part(It first, It last);

  static AnalyzeResult analyze_additional(It first, It last);

  static AnalyzeResult analyze_multi(It first, It last);

  static AnalyzeResult analyze_logic(It first, It last);

  static AnalyzeResult analyze_func(It first, It last);
};
