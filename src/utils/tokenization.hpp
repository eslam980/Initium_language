#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cassert>

enum class TokenType {
    _exit,
    _int_lit,
    _semi_col,
    _open_paren,
    _close_paren,
    _ident,
    _let,
    _equal,
    _plus,
    _minus,
    _star,
    _fslash,
    _open_curly,
    _close_curly,
    _if,
    _elif,
    _else,
};
// converting the TokenType from enum to string
inline std::string convert_str(const TokenType type)
{
    switch (type) {
    case TokenType::_exit:
        return "leave";
    case TokenType::_int_lit:
        return "int literal";
    case TokenType::_semi_col:
        return ";";
    case TokenType::_open_paren:
        return "(";
    case TokenType::_close_paren:
        return ")";
    case TokenType::_ident:
        return "identifier";
    case TokenType::_let:
        return "var";
    case TokenType::_equal:
        return "=";
    case TokenType::_plus:
        return "+";
    case TokenType::_star:
        return "*";
    case TokenType::_minus:
        return "-";
    case TokenType::_fslash:
        return "/";
    case TokenType::_open_curly:
        return "{";
    case TokenType::_close_curly:
        return "}";
    case TokenType::_if:
        return "if";
    case TokenType::_elif:
        return "elif";
    case TokenType::_else:
        return "else";
    }
    assert(false);
}

inline std::optional<int> bin_prec(const TokenType type)
{
    switch (type) {
    case TokenType::_minus:
    case TokenType::_plus:
        return 0;
    case TokenType::_fslash:
    case TokenType::_star:
        return 1;
    default:
        return {};
    }
}

struct Token {
    TokenType type;
    int line;
    std::optional<std::string> value {};
};

class Tokenizer {
public:
    explicit Tokenizer(std::string src)
        : m_src(std::move(src))
    {
    }

    std::vector<Token> tokenize()
    {
        std::vector<Token> tokens;
        std::string buf;
        int line_count = 1;
        while (peek().has_value()) {
            if (std::isalpha(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isalnum(peek().value())) {
                    buf.push_back(consume());
                }
                if (buf == convert_str(TokenType::_exit)) {
                    tokens.push_back({ TokenType::_exit, line_count });
                    buf.clear();
                }
                else if (buf == convert_str(TokenType::_let)) {
                    tokens.push_back({ TokenType::_let, line_count });
                    buf.clear();
                }
                else if (buf == convert_str(TokenType::_if)) {
                    tokens.push_back({ TokenType::_if, line_count });
                    buf.clear();
                }
                else if (buf == convert_str(TokenType::_elif)) {
                    tokens.push_back({ TokenType::_elif, line_count });
                    buf.clear();
                }
                else if (buf == convert_str(TokenType::_else)) {
                    tokens.push_back({ TokenType::_else, line_count });
                    buf.clear();
                }
                else {
                    tokens.push_back({ TokenType::_ident, line_count, buf });
                    buf.clear();
                }
            }
            else if (std::isdigit(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buf.push_back(consume());
                }
                tokens.push_back({ TokenType::_int_lit, line_count, buf });
                buf.clear();
            }
            else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '/') {
                consume();
                consume();
                while (peek().has_value() && peek().value() != '\n') {
                    consume();
                }
            }
            else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '*') {
                consume();
                consume();
                while (peek().has_value()) {
                    if (peek().value() == '*' && peek(1).has_value() && peek(1).value() == '/') {
                        break;
                    }
                    consume();
                }
                if (peek().has_value()) {
                    consume();
                }
                if (peek().has_value()) {
                    consume();
                }
            }
            else if (peek().value() == '(') {
                consume();
                tokens.push_back({ TokenType::_open_paren, line_count });
            }
            else if (peek().value() == ')') {
                consume();
                tokens.push_back({ TokenType::_close_paren, line_count });
            }
            else if (peek().value() == ';') {
                consume();
                tokens.push_back({ TokenType::_semi_col, line_count });
            }
            else if (peek().value() == '=') {
                consume();
                tokens.push_back({ TokenType::_equal, line_count });
            }
            else if (peek().value() == '+') {
                consume();
                tokens.push_back({ TokenType::_plus, line_count });
            }
            else if (peek().value() == '*') {
                consume();
                tokens.push_back({ TokenType::_star, line_count });
            }
            else if (peek().value() == '-') {
                consume();
                tokens.push_back({ TokenType::_minus, line_count });
            }
            else if (peek().value() == '/') {
                consume();
                tokens.push_back({ TokenType::_fslash, line_count });
            }
            else if (peek().value() == '{') {
                consume();
                tokens.push_back({ TokenType::_open_curly, line_count });
            }
            else if (peek().value() == '}') {
                consume();
                tokens.push_back({ TokenType::_close_curly, line_count });
            }
            else if (peek().value() == '\n') {
                consume();
                line_count++;
            }
            else if (std::isspace(peek().value())) {
                consume();
            }
            else {
                std::cerr << "Invalid token" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        m_index = 0;
        return tokens;
    }

private:
    [[nodiscard]] std::optional<char> peek(const size_t offset = 0) const
    {
        if (m_index + offset >= m_src.length()) {
            return {};
        }
        return m_src.at(m_index + offset);
    }

    char consume()
    {
        return m_src.at(m_index++);
    }

    const std::string m_src;
    size_t m_index = 0;
};