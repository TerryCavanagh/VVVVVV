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

    class InvalidExpression;
    class FunctionNotFound;
    class VariableNotFound;

    class AST;
    class Num;
    class Var;
    class BinOp;
    class UnaryOp;
    class Function;
    class Assign;

    class NodeVisitor;
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
        END
    };

    enum AST_TYPE
    {
        AST_NUMBER,
        AST_IDENTIFIER,
        AST_BINOP,
        AST_UNARYOP,
        AST_FUNCTION,
        AST_ASSIGN
    };

    extern std::map<std::string, double> variables;

    void ResetEnvironment();

    extern void SetVariable(std::string name, double value);

    extern ExpressionOutput ParseExpression(std::string expression);
}

#endif /* MATHPARSER_H */
