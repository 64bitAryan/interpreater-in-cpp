#include "../token/token.cpp"
#include <iostream>
#include <string>

class Lexer {
    private:
        bool isLetter(char c) {
            return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_';
        }

        bool isDigit(char c) { return '0' <= c && c <= '9'; }

        Token newToken(TokenType tokenType, char ch) {
            return Token(tokenType, std::string(1, ch));
        }

    public:
        std::string input;
        int position;
        int readPosition;
        char ch;

        Lexer(const std::string input)
            : input(input), position(0), readPosition(0), ch(0) {
                readChar();
            }

        void readChar() {
            if (readPosition >= input.length()) {
                ch = 0;             
            } else {
                ch = input[readPosition];
                position = readPosition;
                readPosition++;
            }
        }

        void skipeWhiteSpace() {
            while (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
                readChar();
            }
        }

        char peekCharacter() {
            if (readPosition >= input.length()) {
                return 0;
            } else {
                return input[readPosition];
            }
        }

        std::string readIdentifier() {
            int pos = position;
            while (isLetter(ch)) {
                readChar();
            }
            return input.substr(pos, position - pos);
        }

        std::string readNumber() {
            int pos = position;
            while (isDigit(ch)) {
                readChar();
            }
            return input.substr(pos, position - pos);
        }

        Token NextToken() {
            Token tok;
            skipeWhiteSpace();
            switch (ch) {
                case '=':
                    if (peekCharacter() == '=') {
                        char c = ch;
                        readChar();
                        std::string _literal = std::to_string(c) + std::to_string(ch);
                        tok = Token(EQ, _literal);
                    } else {
                        tok = newToken(ASSIGN, ch);
                    }
                    break;
                case '+':
                    tok = newToken(PLUS, ch);
                    break;
                case '-':
                    tok = newToken(MINUS, ch);
                    break;
                case '!':
                    if (peekCharacter() == '=') {
                        char c = ch;
                        readChar();
                        std::string _literal = std::to_string(c) + std::to_string(ch);
                        tok = Token(NOT_EQ, _literal);
                    } else {
                        tok = newToken(BANG, ch);
                    }
                    break;
                case '/':
                    tok = newToken(SLASH, ch);
                    break;
                case '*':
                    tok = newToken(ASTERISK, ch);
                    break;
                case '<':
                    tok = newToken(LT, ch);
                    break;
                case '>':
                    tok = newToken(GT, ch);
                    break;
                case ';':
                    tok = newToken(SEMICOLON, ch);
                    break;
                case ',':
                    tok = newToken(COMMA, ch);
                    break;
                case '{':
                    tok = newToken(LBRACE, ch);
                    break;
                case '}':
                    tok = newToken(RBRACE, ch);
                    break;
                case '(':
                    tok = newToken(LPAREN, ch);
                    break;
                case ')':
                    tok = newToken(RPAREN, ch);
                    break;
                case EOF:
                    tok.Literals = "";
                    tok.Type = _EOF;
                default:
                    if (isLetter(ch)) {
                        tok.Literals = readIdentifier();
                        tok.Type = LookupIdent(tok.Literals);
                        return tok;
                    } else if (isDigit(ch)) {
                        tok.Literals = readNumber();
                        tok.Type = INT;
                        return tok;
                    } else {
                        tok = newToken(ILLEGAL, ch);
                    }
                    break;
            }
            readChar();
            return tok;
        }
};

Lexer *New(std::string input) {
    Lexer *l = new Lexer(input);
    return l;
}
