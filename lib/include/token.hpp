#pragma once

#include <string_view>
#include <string>
#include <variant>

class TokenBase
{
  std::string m_token;

public:
  explicit TokenBase(std::string_view token);

  virtual ~TokenBase() = default;

  TokenBase(const TokenBase&) = default;
  TokenBase(TokenBase&&) = default;

  TokenBase& operator=(const TokenBase&) = default;
  TokenBase& operator=(TokenBase&&) = default;

  virtual std::string_view name() const = 0;

  std::string_view token() const;
};


class TokenReal : public TokenBase
{
protected:
  double m_value;

public:
  explicit TokenReal(std::string_view token);

  std::string_view name() const override;

  friend bool operator== (const TokenReal& lhs, const TokenReal& rhs);
};


class TokenInteger : public TokenBase
{
protected:
  int m_value;

public:
  explicit TokenInteger(std::string_view token);

  std::string_view name() const override;

  friend bool operator== (const TokenInteger& lhs, const TokenInteger& rhs);
};


class TokenText : public TokenBase
{
public:
  explicit TokenText(std::string_view token);

  std::string_view name() const override;

  friend bool operator== (const TokenText& lhs, const TokenText& rhs);
};


using Token = std::variant<TokenReal, TokenInteger, TokenText>;

enum class TokenIndex { Real = 0, Integer = 1, Text = 2 };

std::ostream& operator<< (std::ostream& os, const Token& token);

Token make_token(std::string_view sv);
