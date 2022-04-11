#include "token.hpp"

#include <boost/lexical_cast.hpp>
#include <exception>
#include <unordered_set>
#include <cmath>
#include <numeric>

using namespace std::literals::string_literals;

TokenBase::TokenBase(std::string_view token) : m_token{ token } {}

std::string_view TokenBase::token() const { return m_token; }



TokenReal::TokenReal(std::string_view token)
  : TokenBase{ token }
  , m_value{ }
{
  try {
    m_value = boost::lexical_cast<double>(token);
  } catch (const boost::bad_lexical_cast&) {
    throw std::invalid_argument("Token is not value: " + std::string{ token });
  }
}

std::string_view TokenReal::name() const { return "real token"; }

bool operator== (const TokenReal& lhs, const TokenReal& rhs)
{
  return std::abs(lhs.m_value - rhs.m_value) < std::numeric_limits<double>::epsilon();
}



TokenInteger::TokenInteger(std::string_view token)
  : TokenBase{ token }
  , m_value{ }
{
  try {
    m_value = boost::lexical_cast<int>(token);
  } catch (const boost::bad_lexical_cast&) {
    throw std::invalid_argument("Token is not value: " + std::string{ token });
  }
}

std::string_view TokenInteger::name() const { return "integer token"; }

bool operator== (const TokenInteger& lhs, const TokenInteger& rhs)
{
  return lhs.m_value == rhs.m_value;
}



TokenText::TokenText(std::string_view token)
  : TokenBase{ token }
{ }

std::string_view TokenText::name() const { return "text token"; }

bool operator== (const TokenText& lhs, const TokenText& rhs)
{
  return lhs.token() == rhs.token();
}



std::ostream& operator<< (std::ostream& os, const Token& token)
{
  const auto visitor = [&](const TokenBase& token) { os << token.token() << "\t[" << token.name() << "]"; };

  std::visit(visitor, token);

  return os;
}



Token make_token(std::string_view sv)
{
  try {
    return TokenInteger(sv);
  } catch (const std::invalid_argument&) {
    try {
      return TokenReal(sv);
    } catch (const std::invalid_argument&) {
      return TokenText(sv);
    }
  }
}
