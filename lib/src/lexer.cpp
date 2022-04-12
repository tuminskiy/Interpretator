#include "lexer.hpp"
#include "util.hpp"

#include <boost/tokenizer.hpp>

#include <unordered_set>

using namespace std::literals::string_literals;


std::vector<Token> Lexer::tokenize(std::string_view program)
{
  auto tokens = std::vector<Token>{};

  const auto separator = boost::char_separator<char>{ " \t\n", "+-/*=:!&|;" };

  auto tokenizer = boost::tokenizer{program, separator};

  for (const auto& token : tokenizer) {
    tokens.push_back(make_token(token));
  }

  return tokens;
}



Lexer::AnalyzeResult Lexer::analyze(const std::vector<Token>& tokens)
{
  AnalyzeResult result;
  result.token_it = tokens.begin();

  if (tokens.empty()) {
    result.success = false;
    result.msg = "Текст для анализа отсутствуют";

    return result;
  }

  result = analyze_langugage(result.token_it, tokens.end());

  return result;
}


Lexer::AnalyzeResult Lexer::analyze_langugage(It first, It last)
{
  AnalyzeResult result;

  const auto text_index = static_cast<int>(TokenIndex::Text);

  if (!Util::is_expected_token<text_index>(*first, "Начало")) {
    result.success = false;
    result.msg = "Язык должен начинаться со слова 'Начало'";
    result.token_it = first;
    return result;
  }

  result = analyze_definitions(++first, last);

  if (!result.success)
    return result;

  result = analyze_operations(result.token_it, last);

  if (!result.success)
    return result;

  if (Util::get_token(*result.token_it) == ":") {
    result.success = false;
    result.msg = "Перед знаком ':' должна идти метка, "
                 "либо операция должна заканчиваться переменной или целым числом";
    return result;
  }

  if (result.token_it == last || !Util::is_expected_token<text_index>(*result.token_it, "Конец")) {
    result.success = false;
    result.msg = "Язык должен заканчиватся словом 'Конец'";
    return result;
  }

  ++result.token_it;

  if (result.token_it != last) {
    result.success = false;
    result.msg = "После слова 'Конец' не должно ничего идти";
  }

  return result;
}


Lexer::AnalyzeResult Lexer::analyze_definitions(It first, It last)
{
  auto result = AnalyzeResult{};
  result.success = true;
  result.token_it = first;

  if (first == last) {
    result.success = false;
    result.msg = "После слова 'Начало' ожидается слово 'Анализ' или 'Синтез'";
    result.token_it = first;
    return result;
  }

  constexpr auto text_index = static_cast<int>(TokenIndex::Text);

  while (result.success) {
    result = analyze_definition(result.token_it, last);

    if (!result.success)
      continue;

    const bool is_semicolumn = Util::is_expected_token<text_index>(*result.token_it, ";");

    if (!is_semicolumn)
      break;

    ++result.token_it;
  }

  return result;
}

Lexer::AnalyzeResult Lexer::analyze_definition(It first, It last)
{
  AnalyzeResult result;

  if (first == last) {
    result.success = false;
    result.msg = "Определение должно начинаться со слова 'Анализ' или 'Синтез'";
    result.token_it = first;
    return result;
  }

  constexpr auto text_index = static_cast<int>(TokenIndex::Text);
  constexpr auto real_index = static_cast<int>(TokenIndex::Real);

  const auto start_of_analyze = Util::is_expected_token<text_index>(*first, "Анализ");
  const auto start_of_syntez = Util::is_expected_token<text_index>(*first, "Синтез");

  if (!(start_of_analyze || start_of_syntez)) {
    result.success = false;
    result.msg = "Определение должно начинаться со слова 'Анализ' или 'Синтез'";
    result.token_it = first;
    return result;
  }

  if (++first == last || first->index() != real_index) {
    result.success = false;
    result.msg = "После слова '"
      + (start_of_analyze ? "Анализ"s : "Синтез") + "' "
      + "ожидается вещественное число";
    result.token_it = first;
    return result;
  }

  while (first->index() == real_index) {
    ++first;

    if (first == last) {
      result.success = false;
      result.msg = "После вещественного числа ожидается 'Конец "
        + (start_of_analyze ? "анализа"s : "синтеза"s ) + "'";
      result.token_it = first;
      return result;
    }
  }

  const auto end = Util::is_expected_token<text_index>(*first, "Конец");

  if (!end) {
    result.success = false;
    result.msg = "После вещественных чисел в определении ожидается 'Конец "
      + (start_of_analyze ? "анализа"s : "синтеза"s)
      + "'";
    result.token_it = first;
    return result;
  }

  ++first;

  const auto expected_msg = "Ожидается 'Конец "
   + (start_of_analyze ? "анализа"s : "синтеза"s) + "' "
   + "так как определение начиналось со слова '"
   + (start_of_analyze ? "Анализ"s : "Синтез"s) + "'";

  if (first == last) {
    result.success = false;
    result.msg = expected_msg;
    result.token_it = first;
    return result;
  }

  const auto end_of_analyze = Util::is_expected_token<text_index>(*first, "анализа");
  const auto end_of_syntez = Util::is_expected_token<text_index>(*first, "синтеза");

  if ((start_of_analyze && !end_of_analyze)
   || (start_of_syntez && !end_of_syntez)
   || (!end_of_analyze && !end_of_syntez)) {
    result.success = false;
    result.msg = expected_msg;
    result.token_it = first;
    return result;
  }

  result.success = true;
  result.token_it = ++first;
  return result;
}


Lexer::AnalyzeResult Lexer::analyze_operations(It first, It last)
{
  AnalyzeResult result;
  result.success = true;
  result.token_it = first;

  if (first == last) {
    result.success = false;
    result.msg = "После определения ожидается оператор";
    return result;
  }

  constexpr auto integer_index = static_cast<int>(TokenIndex::Integer);

  while (result.success) {
    result = analyze_operation(result.token_it, last);

    if (!result.success || result.token_it == last)
      break;

    const auto is_integer = result.token_it->index() == integer_index;

    if (!is_integer)
      break;
  }

  return result;
}

Lexer::AnalyzeResult Lexer::analyze_operation(It first, It last)
{
  AnalyzeResult result;

  constexpr auto text_index = static_cast<int>(TokenIndex::Text);
  constexpr auto integer_index = static_cast<int>(TokenIndex::Integer);

  if (first->index() != integer_index) {
    result.success = false;
    result.msg = "После 'Конец " + std::string{ Util::get_token(*std::prev(first)) } + "' ожидается метка";
    result.token_it = first;
    return result;
  }

  ++first;

  if (first == last) {
    result.success = false;
    result.msg = "После метки ожидается ':'";
    result.token_it = first;
    return result;
  }

  while (first->index() == integer_index) {
    ++first;

    if (first == last) {
      result.success = false;
      result.msg = "После метки ожидается ':'";
      result.token_it = first;
      return result;
    }
  }

  if (!Util::is_expected_token<text_index>(*first, ":")) {
    result.success = false;
    result.msg = "После метки ожидается ':'";
    result.token_it = first;
    return result;
  }

  result = analyze_variable(++first, last);

  if (!result.success)
    return result;

  return analyze_right_part(result.token_it, last);
}


Lexer::AnalyzeResult Lexer::analyze_variable(It first, It last)
{
  AnalyzeResult result;
  result.success = true;
  result.token_it = first;

  constexpr auto text_index = static_cast<int>(TokenIndex::Text);

  if (first == last || first->index() != text_index) {
    result.success = false;
    result.msg = "После ':' должна идти переменная";
    return result;
  }

  const auto variable = std::get<text_index>(*first).token();

  if (!Util::is_valid_variable(variable)) {
    result.success = false;
    result.msg = "Неверный формат переменной";
    return result;
  }

  ++result.token_it;

  return result;
}

Lexer::AnalyzeResult Lexer::analyze_right_part(It first, It last)
{
  AnalyzeResult result;
  result.success = true;

  constexpr auto text_index = static_cast<int>(TokenIndex::Text);

  if (first == last) {
    result.success = false;
    result.msg = "После переменной должен идти знак '='";
    result.token_it = first;
    return result;
  }

  if (!Util::is_expected_token<text_index>(*first, "=")) {
    result.success = false;
    result.msg = "После переменной должен идти знак '='";
    result.token_it = first;
    return result;
  }

  ++first;

  if (first == last) {
    result.success = false;
    result.msg = "После знака '=' должно идти либо переменная, "
                 "либо целое число, либо знак '-' , "
                 "либо знак '!' , либо функция";
    result.token_it = first;
    return result;
  }

  result = analyze_additional(first, last);

  if (!result.success && result.from_func) {
    auto prev_token = *std::prev(result.token_it);


    if (Util::is_operator(prev_token) && Util::is_operator(*result.token_it)) {
      result.msg = "Ошибка: встречено два оператора подряд "
      "'" + std::string{Util::get_token(prev_token)} + "' и "
      "'" + std::string{Util::get_token(*result.token_it)} + "'";
    }
  }

  return result;
}


Lexer::AnalyzeResult Lexer::analyze_additional(It first, It last)
{
  AnalyzeResult result;

  constexpr auto text_index = static_cast<int>(TokenIndex::Text);
  bool has_minus = false;
  if (Util::is_expected_token<text_index>(*first, "-")) {
    ++first;
    has_minus = true;

    if (first == last) {
      result.success = false;
      result.token_it = first;
      result.msg = "После знака '-' должно идти либо переменная, "
                   "либо целое число, либо знак '!', либо функция";
      return result;
    }
  }

  if (Util::is_operator(*first) && std::get<TokenText>(*first).token() != "!") {
    result.success = false;
    result.token_it = first;
    result.msg = "Ошибка: встреченно два оператора подряд "
      "'" + std::string{Util::get_token(*std::prev(first))} + "' и "
      "'" + std::string{Util::get_token(*first)} + "'";

    return result;
  }

  result.success = true;

  while (result.success) {
    result = analyze_multi(first, last);
    first = result.token_it;

    if (!result.success || first == last || Util::is_integer(*first))
      break;

    if (Util::is_expected_token<text_index>(*first, "+") || Util::is_expected_token<text_index>(*first, "-")) {
      ++first;

      if (first == last) {
        result.success = false;
        result.token_it = first;
        result.msg = "После аддитивной операции должно идти либо переменная, "
                     "либо целое число, либо функция, либо знак '!'";
      }
    } else {
      break;
    }
  }

  return result;
}

Lexer::AnalyzeResult Lexer::analyze_multi(It first, It last)
{
  AnalyzeResult result;
  result.success = true;

  constexpr auto text_index = static_cast<int>(TokenIndex::Text);

  while (result.success) {
    result = analyze_logic(first, last);
    first = result.token_it;

    if (!result.success || first == last || Util::is_integer(*first))
      break;

    if (Util::is_expected_token<text_index>(*first, "*") || Util::is_expected_token<text_index>(*first, "/")) {
      ++first;

      if (first == last) {
        result.success = false;
        result.token_it = first;
        result.msg = "После мультипликативной операции должно идти либо переменная, "
                     "либо целое число, либо функция, либо знак '!'";
      }
    } else {
      break;
    }
  }

  return result;
}

Lexer::AnalyzeResult Lexer::analyze_logic(It first, It last)
{
  AnalyzeResult result;
  result.success = true;

  constexpr auto text_index = static_cast<int>(TokenIndex::Text);

  if (Util::is_expected_token<text_index>(*first, "!"))
    first++;

  if (first == last) {
    result.success = false;
    result.token_it = first;
    result.msg = "После знака '!' должно идти либо функция, "
                 "либо переменна, либо целое число";
    return result;
  }

  if (Util::get_token(*first) == "!") {
    result.success = false;
    result.token_it = first;
    result.msg = "После знака '" + std::string{Util::get_token(*std::prev(first))} + "' должно идти либо переменная, "
                "либо целое число, либо функция";


    return result;
  }

  while (result.success) {
    if (Util::is_expected_token<text_index>(*first, "!"))
      first++;

    if (first == last) {
      result.success = false;
      result.token_it = first;
      result.msg = "После знака '!' должно идти либо функция, "
                 "либо переменна, либо целое число";
      return result;
    }

    result = analyze_func(first, last);
    first = result.token_it;

    if (!result.success || first == last || Util::is_integer(*first))
      break;

    if (Util::is_expected_token<text_index>(*first, "|") || Util::is_expected_token<text_index>(*first, "&")
     || Util::is_expected_token<text_index>(*first, "!")) {
      ++first;

      if (first == last) {
        result.success = false;
        result.token_it = first;
        result.msg = "После логической операции должно идти либо переменная, "
                     "либо целое число, либо функция";
      }
    } else {
      break;
    }
  }

  return result;
}

Lexer::AnalyzeResult Lexer::analyze_func(It first, It last)
{
  AnalyzeResult result;
  result.success = true;

  bool func_exists = false;

  while (first != last && Util::is_function(*first)) {
    func_exists = true;
    ++first;
  }

  result.success = first != last && (Util::is_variable(*first) || Util::is_integer(*first));
  result.token_it = first;

  if (result.success) {
    ++result.token_it;
  } else if (func_exists) {
    result.msg = "После функции должна идти либо переменная, либо целое число";
  } else {
    result.from_func = true;
  }

  return result;
}




