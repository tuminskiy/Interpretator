#include "util.hpp"

#include <regex>
#include <locale>
#include <codecvt>

bool Util::is_variable(const Token& token)
{
  constexpr auto text_index = static_cast<int>(TokenIndex::Text);

  return token.index() == text_index && is_valid_variable(std::get<text_index>(token).token());
}


bool Util::is_valid_variable(std::string_view variable)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring wide = converter.from_bytes(variable.data());

  std::wregex wrg(L"^([а-яёА-ЯЁ]{2}([0-9]+)?)$");
  std::wsmatch wmatch;
  return std::regex_match(wide, wmatch, wrg);
}

bool Util::is_integer(const Token& token)
{
  constexpr auto int_index = static_cast<int>(TokenIndex::Integer);

  return token.index() == int_index;
}

bool Util::is_function(const Token& token)
{
  constexpr auto text_index = static_cast<int>(TokenIndex::Text);

  return is_expected_token<text_index>(token, "sin")
      || is_expected_token<text_index>(token, "cos")
      || is_expected_token<text_index>(token, "tan");
}

bool Util::is_operator(const Token& token)
{
  constexpr auto text_index = static_cast<int>(TokenIndex::Text);

  return is_expected_token<text_index>(token, "+")
      || is_expected_token<text_index>(token, "-")
      || is_expected_token<text_index>(token, "*")
      || is_expected_token<text_index>(token, "/")
      || is_expected_token<text_index>(token, "&")
      || is_expected_token<text_index>(token, "|")
      || is_expected_token<text_index>(token, "!");
}

std::string_view Util::get_token(const Token& token)
{
  std::string_view text;
  std::visit([&text](const TokenBase& t) { text = t.token();}, token);
  return text;
}
