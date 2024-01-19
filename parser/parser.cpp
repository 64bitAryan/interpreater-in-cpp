#include <iostream>
#include <vector>
#include <map>
#include "../token/token.cpp"
#include "../lexer/lexer.cpp"
#include "../ast/ast.cpp"

enum {
    _,
    LOWEST,
    EQUALS,
    LESSGREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL
};

std::map<TokenType, int> precedences = {
    {EQ, EQUALS}, {NOT_EQ, EQUALS}, {LT, LESSGREATER}, {GT, LESSGREATER},
    {PLUS, SUM}, {MINUS, SUM}, {ASTERISK, PRODUCT}, {SLASH, PRODUCT},
    {LPAREN, CALL}
};

using prefixParseFn = Expression* (*)(Parser*);
using infixParseFn = Expression* (*)(Parser*, Expression*);

class Parser {
    private: 
        Lexer *l;
        Token curToken;
        Token peekToken;
        std::vector<std::string> errors;
        std::map<TokenType, prefixParseFn>prefixParseFns;
        std::map<TokenType, infixParseFn>infixParseFns;

    public:
        // Constructore
        Parser(Lexer* lexer) : l(lexer), errors() {
            prefixParseFns = std::map<TokenType, prefixParseFn>();
            infixParseFns = std::map<TokenType, infixParseFn>();
        }

        LetStatement* parseLetStatement() {
            LetStatement* stmt = new LetStatement(curToken);

            if(!expectPeek(IDENT)) {
                return nullptr;
            }

            stmt->Name = new Identifier(curToken, curToken.Literals);

            if(!expectPeek(ASSIGN)) {
                return nullptr;
            }

            while (!curTokenIs(SEMICOLON)){
                nextToken();
            }
            
            return stmt;
        }

        ReturnStatement* parseReturnStatement() {
            ReturnStatement* stmt = new ReturnStatement(curToken);
            nextToken();

            while(!curTokenIs(SEMICOLON)) {
                nextToken();
            }
            return stmt;
        }

        Expression *parseExpression(int precedence) {
            prefixParseFn prefix = prefixParseFns[curToken.Type];
            if (prefix == nullptr)
            {
                noPrefixParseFnError(curToken.Type);
                return nullptr;
            }
            Expression *leftExp = prefix(this);
            while (!peekTokenIs(SEMICOLON) && precedence < peekPrecedence()) {
                infixParseFn infix = infixParseFns[peekToken.Type];
                if (infix == nullptr) 
                    return leftExp;
                nextToken();
                leftExp = infix(this, leftExp);
            }
            return leftExp;
        }

        Expression* parseInfixExpression(Expression* leftExp) {
            InfixExpression* expression = new InfixExpression(curToken, curToken.Literals, leftExp);
            int precedence = curPrecedence();
            nextToken();
            expression->Right = parseExpression(precedence);
            return expression;
        }

        ExpressionStatement* parseExpressionStatement() {
            ExpressionStatement* stmt = new ExpressionStatement(curToken);
            stmt->_Expression = parseExpression(LOWEST);
            if(!peekTokenIs(SEMICOLON)) {
                nextToken();
            }
            return stmt;
        }
        
        Statement* parseStatement() {
            TokenType curTokenType = curToken.Type;
            if (curTokenType == LET) {
                return parseLetStatement();
            } else if(curTokenType == RETURN) {
                return parseReturnStatement();
            } else {
                return parseExpressionStatement();
            }
        }

        Program* ParseProgram() {
            Program* program = new Program();
            program->Statements = std::vector<Statement*>();
            
            while (!curTokenIs(_EOF)) {
                Statement* statement = parseStatement();
                if (statement!= nullptr) {
                    program->Statements.push_back(statement);
                }
                nextToken();
            }
            return program;
        }

        void nextToken() {
            curToken = peekToken;
             peekToken = l->NextToken();
        }

        Expression* parseIdentifier() {
            return new Identifier(curToken, curToken.Literals);
        }

        Expression* parseIntegerLiteral() {
            IntegerLiteral* literal = new IntegerLiteral(curToken);
            int value = INT64_MAX;
            try {
                value = std::stoi(curToken.Literals);
            }catch (const std::invalid_argument& e) {
                std::string msg = "could not parse " + curToken.Literals + " as integer";
                errors.push_back(msg);
                return nullptr;
            }
            literal->Value = value;
            return literal;
        }

        Expression* parsePrefixExpression() {
            PrefixExpression* expression = new PrefixExpression(curToken, curToken.Literals);
            nextToken();
            expression->Right = parseExpression(PREFIX);
            return expression;
        }

        /* Helper Functions*/
        std::vector<std::string> Errors() {
            return errors;
        }

        void peekError(TokenType t) {
            std::string msg = "expected next token to be " + t + ", got " + peekToken.Type;
            errors.push_back(msg);
        }

        bool curTokenIs(TokenType t) {
            return curToken.Type == t;
        }
        
        bool peekTokenIs(TokenType t) {
            return peekToken.Type == t;
        }

        bool expectPeek(TokenType t) {
            if (peekTokenIs(t)) {
                nextToken();
                return true;
            } else {
                peekError(t);
                return false;
            }
        }

        // helper to register prefix and infix functions in parser mapping
        void registerPrefix(TokenType t, prefixParseFn fn) {
            prefixParseFns[t] = fn;
        }

        void registerInfix(TokenType t, infixParseFn fn) {
            infixParseFns[t] = fn;
        }

        void noPrefixParseFnError(TokenType t) {
            std::string msg = "no prefix parse function for " + t + " found";
            errors.push_back(msg);
        }

        int peekPrecedence() {
            if (precedences[peekToken.Type] != 0) {
                return precedences[peekToken.Type];
            }
            return LOWEST;
        }

        int curPrecedence() {
            if (precedences[curToken.Type] != 0) {
                return precedences[curToken.Type];
            }
            return LOWEST;
        }
};

Parser* New(Lexer* lexer) {
    Parser* p = new Parser(lexer);
    p->registerPrefix(IDENT, &Parser::parseIdentifier);
    p->registerPrefix(INT, &Parser::parseIntegerLiteral);
    p->registerPrefix(BANG, &Parser::parsePrefixExpression);
    p->registerPrefix(MINUS, &Parser::parsePrefixExpression);

    p->registerInfix(PLUS, &Parser::parseInfixExpression);
    p->registerInfix(MINUS, &Parser::parseInfixExpression);
    p->registerInfix(ASTERISK, &Parser::parseInfixExpression);
    p->registerInfix(SLASH, &Parser::parseInfixExpression);
    p->registerInfix(EQ, &Parser::parseInfixExpression);
    p->registerInfix(NOT_EQ, &Parser::parseInfixExpression);
    p->registerInfix(LT, &Parser::parseInfixExpression);
    p->registerInfix(GT, &Parser::parseInfixExpression);
}

