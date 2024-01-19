#include <iostream>
#include <vector>
#include <sstream>
#include "../token/token.cpp"

/* Forward declarations */
class Node;
class Statement;
class Expression;
class Program;
class Identifier;
class LetStatement;
class ReturnStatement;
class ExpressionStatement;

/* Interfaces */
class Node {
public:
    virtual ~Node() = default;
    virtual std::string TokenLiteral() const = 0;
    virtual std::string String() const = 0;
};

class Statement : public Node {
public:
    Node *Node;
    Statement(): Node(nullptr) {}
    virtual ~Statement() = default;
    virtual void statementNode() const = 0;
};

class Expression : public Node {
public:
    Node *Node;
    Expression(): Node(nullptr) {}
    virtual ~Expression() = default;
    virtual void expressionNode() const = 0;
};

/* Program class */
class Program : public Node {
public:
    std::vector<Statement*> Statements;

    ~Program() {
        for (auto statement : Statements) {
            delete statement;
        }
    }

    std::string TokenLiteral() const override {
        if (!Statements.empty()) {
            return Statements[0]->TokenLiteral();
        } else {
            return "";
        }
    }

    std::string String() const override {
        std::ostringstream out;
        for (const auto& statement : Statements) {
            out << statement->String();
        }
        return out.str();
    }
};

/* Identifier class */
class Identifier : public Expression {
public:
    Token _Token;
    std::string Value;

    Identifier(const Token& token, const std::string& value)
        : _Token(token), Value(value) {}

    void expressionNode() const override {}
    std::string TokenLiteral() const override { return _Token.Literals; }
    std::string String() const override { return Value; }
};

/* LetStatement class */
class LetStatement : public Statement {
public:
    Token _Token;
    Identifier* Name;
    Expression* Value;

    LetStatement(const Token& token)
        : _Token(token) {}

    ~LetStatement() {
        delete Name;
        delete Value;
    }

    void statementNode() const override {}
    std::string TokenLiteral() const override { return _Token.Literals; }

    std::string String() const override {
        std::ostringstream out;
        out << TokenLiteral() << " " << Name->String() << "=";
        if (Value != nullptr) {
            out << Value->String();
        }
        out << ";";
        return out.str();
    }
};

/* ReturnStatement class */
class ReturnStatement : public Statement {
public:
    Token _Token;
    Expression* ReturnValue;

    ReturnStatement(const Token& token)
        : _Token(token){}

    ~ReturnStatement() {
        delete ReturnValue;
    }

    void statementNode() const override {}
    std::string TokenLiteral() const override { return _Token.Literals; }

    std::string String() const override {
        std::ostringstream out;
        out << TokenLiteral() << " ";
        if (ReturnValue != nullptr) {
            out << ReturnValue->String();
        }
        out << ";";
        return out.str();
    }
};

/* ExpressionStatement class */
class ExpressionStatement : public Statement {
public:
    Token _Token;
    Expression* _Expression;

    ExpressionStatement(const Token& token)
        : _Token(token){}

    ~ExpressionStatement() {
        delete _Expression;
    }

    void statementNode() const override {}
    std::string TokenLiteral() const override { return _Token.Literals; }

    std::string String() const override {
        if (_Expression != nullptr) {
            return _Expression->String();
        }
        return "";
    }
};

class IntegerLiteral : public Expression {
    public:
        Token _Token;
        int Value;
        
        IntegerLiteral(const Token& token) : 
        _Token(token){}

        void expressionNode() const override {}
        std::string TokenLiteral() const override { return _Token.Literals; }
        std::string String() const override { return _Token.Literals; }
};

class PrefixExpression : public Expression {
    public:
        Token _Token;
        std::string Operator;
        Expression* Right;

        PrefixExpression(const Token& token, const std::string& op) :
      _Token(token), Operator(op) {}

        ~PrefixExpression() {
            delete Right;
        }

        void expressionNode() const override {}
        std::string TokenLiteral() const override { return _Token.Literals; }
        std::string String() const override {
            std::ostringstream out;
            out << "(" << Operator <<Right -> String() << ")";
            return out.str();
        }
};

class InfixExpression : public Expression {
    public:
        Token _Token;
        std::string Operator;
        Expression* Left;
        Expression* Right;


        InfixExpression(const Token& token, const std::string& op, Expression* left) :
        _Token(token), Operator(op), Left(left){}

        ~InfixExpression() {
            delete Left;
            delete Right;
        }

        void expressionNode() const override {}

        std::string TokenLiteral() const override { return _Token.Literals; }

        std::string String() const override {
            std::ostringstream out;
            out << "(" << Left->String() << " " << Operator << " " << Right->String() << ")";
            return out.str();
        }
};