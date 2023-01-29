#ifndef MATHPARSER_H
#define MATHPARSER_H

#include <string>
#include <vector>

namespace MathParser
{
    struct ExpressionOutput
    {
        double value;
        bool success;
        std::string error;
    };

    class AST;
    class Num;
    class Var;
    class BinOp;
    class UnaryOp;
    class Function;
    class Assign;
    class Compound;
    class NoOp;
    class IfStatement;

    class Token;

    class Lexer;
    class Parser;
    class Interpreter;

    enum TOKEN_TYPE
    {
        NUMBER,
        IDENTIFIER,
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        MODULO,
        POWER,
        LPAREN,
        RPAREN,
        COMMA,
        ASSIGN,
        GREATER_THAN_OR_EQUAL_TO,
        LESS_THAN_OR_EQUAL_TO,
        GREATER_THAN,
        LESS_THAN,
        EQUAL_TO,
        NOT_EQUAL_TO,
        AND,
        OR,
        NOT,
        SEMICOLON,
        NEWLINE,
        END,
        BEGIN,
        IF,
        ELSE,
        EOL,
        ERROR
    };

    enum AST_TYPE
    {
        AST_NUMBER,
        AST_IDENTIFIER,
        AST_BINOP,
        AST_UNARYOP,
        AST_FUNCTION,
        AST_ASSIGN,
        AST_NOOP,
        AST_COMPOUND,
        AST_IF
    };

    extern std::map<std::string, double> variables;

    void ResetEnvironment(void);

    extern void SetVariable(std::string name, double value);

    extern ExpressionOutput ParseExpression(std::string expression);
}

#endif /* MATHPARSER_H */
