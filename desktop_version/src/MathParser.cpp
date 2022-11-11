#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <math.h> 
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <exception>

#include "MathParser.h"
#include "Maths.h"
#include "Entity.h"
#include "UtilityClass.h"

namespace MathParser {
    std::map<std::string, double> variables;
}

class MathParser::Token
{
public:
    TOKEN_TYPE type;
    double value;
    std::string name;

    Token() {
        type = END;
        value = 0;
        name = " ";
    }

    Token(TOKEN_TYPE type, std::string name)
    {
        this->type = type;
        this->name = name;
        this->value = 0;
    }

    Token(TOKEN_TYPE type, double value)
    {
        this->type = type;
        this->value = value;
        this->name = "";
    }
};

class MathParser::Lexer
{
public:
    std::string input;
    int pos;
    char currentChar;
    bool invalid;

    Lexer() {
        input = " ";
        pos = 0;
        currentChar = ' ';
        invalid = false;
    }

    Lexer(std::string input)
    {
        this->input = input;
        this->pos = 0;
        this->invalid = false;
        if (input.empty()) invalid = true;
        this->currentChar = input[this->pos];
    }

    void advance()
    {
        this->pos++;
        if (this->pos > input.length() - 1)
        {
            currentChar = '\0';
        }
        else
        {
            currentChar = input[this->pos];
        }
    }

    void skipWhitespace()
    {
        while (currentChar != '\0' && isspace(currentChar))
        {
            advance();
        }
    }

    double number()
    {
        std::string result = "";
        while (currentChar != '\0' && isdigit(currentChar))
        {
            result += currentChar;
            advance();
        }
        if (currentChar == '.')
        {
            result += currentChar;
            advance();
            while (currentChar != '\0' && isdigit(currentChar))
             {
                result += currentChar;
                advance();
            }
        }
        return atof(result.c_str());
    }

    std::string identifier()
    {
        std::string result = "";
        while (currentChar != '\0' && isalpha(currentChar))
{
            result += currentChar;
            advance();
        }
        return result;
    }

    Token getNextToken()
    {
        if (invalid) {
            return Token(ERROR, 0);
        }
        while (currentChar != '\0')
        {

            if (isspace(currentChar))
            {
                skipWhitespace();
                continue;
            }
            if (isdigit(currentChar))
            {
                return Token(NUMBER, number());
            }
            if (isalpha(currentChar))
            {
                return Token(IDENTIFIER, identifier());
            }
            if (currentChar == '+')
            {
                advance();
                return Token(PLUS, 0);
            }
            if (currentChar == '-')
            {
                advance();
                return Token(MINUS, 0);
            }
            if (currentChar == '*')
            {
                advance();
                return Token(MULTIPLY, 0);
            }
            if (currentChar == '/')
            {
                advance();
                return Token(DIVIDE, 0);
            }
            if (currentChar == '%')
            {
                advance();
                return Token(MODULO, 0);
            }
            if (currentChar == '^')
            {
                advance();
                return Token(POWER, 0);
            }
            if (currentChar == '(')
            {
                advance();
                return Token(LPAREN, 0);
            }
            if (currentChar == ')')
            {
                advance();
                return Token(RPAREN, 0);
            }
            if (currentChar == ',')
            {
                advance();
                return Token(COMMA, 0);
            }
            if (currentChar == '=')
            {
                advance();
                return Token(ASSIGN, 0);
            }
            return Token(ERROR, 0);
        }
        return Token(END, 0);
    }
};

// Parser starts here

class MathParser::AST
{
public:
    AST_TYPE type;
};

class MathParser::Num : public AST
{
public:
    Token token;
    double value;

    Num(Token token)
    {
        type = AST_NUMBER;
        this->token = token;
        this->value = token.value;
    }
};

class MathParser::Var : public AST
{
public:
    Token token;
    std::string name;

    Var(Token token)
    {
        type = AST_IDENTIFIER;
        this->token = token;
        this->name = token.name;
    }
};


class MathParser::BinOp : public AST
{
public:
    AST* left;
    Token op;
    AST* right;

    BinOp(AST* left, Token op, AST* right)
    {
        type = AST_BINOP;
        this->left = left;
        this->op = op;
        this->right = right;
    }
};

class MathParser::UnaryOp : public AST
{
public:
    Token op;
    AST* expr;

    UnaryOp(Token op, AST* expr)
    {
        type = AST_UNARYOP;
        this->op = op;
        this->expr = expr;
    }
};

class MathParser::Function : public AST
{
public:
    Token token;
    std::string name;
    std::vector<AST*> exprs;

    Function(Token token, std::vector<AST*> exprs)
    {
        type = AST_FUNCTION;
        this->token = token;
        this->name = token.name;
        this->exprs = exprs;
    }
};

class MathParser::Assign : public AST
{
public:
    Token token;
    std::string name;
    AST* expr;

    Assign(Token token, AST* expr)
    {
        type = AST_ASSIGN;
        this->token = token;
        this->name = token.name;
        this->expr = expr;
    }
};

class MathParser::Parser
{
public:
    Lexer lexer;
    Token currentToken;
    bool invalid;
    std::string errorMessage;

    Parser(Lexer lexer)
    {
        this->invalid = false;
        this->lexer = lexer;
        this->currentToken = this->lexer.getNextToken();
        if (currentToken.type == ERROR)
        {
            error("Invalid token");
        }
    }

    void error(std::string what)
    {
        this->invalid = true;
        this->errorMessage = what;
    }

    void eat(TOKEN_TYPE tokenType)
    {
        if (currentToken.type == tokenType)
        {
            this->currentToken = lexer.getNextToken();
            if (currentToken.type == ERROR)
            {
                error("Invalid token");
            }
        }
        else
        {
            error("Unexpected token");
        }
    }

    AST* expr_4()
    {
        Token token = currentToken;
        if (token.type == COMMA)
        {
            error("Unexpected comma");
        }
        if (token.type == PLUS)
        {
            eat(PLUS);
            return new UnaryOp(token, expr_4());
        }
        if (token.type == MINUS)
        {
            eat(MINUS);
            return new UnaryOp(token, expr_4());
        }
        if (token.type == NUMBER)
        {
            eat(NUMBER);
            return new Num(token);
        }
        if (token.type == IDENTIFIER)
        {
            eat(IDENTIFIER);
            if (currentToken.type == LPAREN)
            {
                eat(LPAREN);
                std::vector<AST*> exprs;
                while (currentToken.type != RPAREN)
                {
                    exprs.push_back(expr_1());
                    if (currentToken.type == COMMA)
                    {
                        eat(COMMA);
                    }
                    else
                    {
                        break;
                    }
                }
                eat(RPAREN);
                return new Function(token, exprs);
            }
            if (currentToken.type == ASSIGN)
            {
                eat(ASSIGN);
                return new Assign(token, expr_1());
            }
            return new Var(token);
        }
        if (token.type == LPAREN)
        {
            eat(LPAREN);
            AST* node = expr_1();
            eat(RPAREN);
            return node;
        }
        return NULL;
    }

    AST* expr_3()
    {
        AST* node = expr_4();
        while (currentToken.type == POWER)
        {
            Token token = currentToken;
            eat(POWER);
            node = new BinOp(node, token, expr_4());
        }
        return node;
    }

    AST* expr_2()
    {
        AST* node = expr_3();
        while (currentToken.type == MULTIPLY || currentToken.type == DIVIDE || currentToken.type == MODULO)
        {
            Token token = currentToken;
            if (token.type == MULTIPLY)
            {
                eat(MULTIPLY);
            }
            if (token.type == DIVIDE)
            {
                eat(DIVIDE);
            }
            if (token.type == MODULO)
            {
                eat(MODULO);
            }
            node = new BinOp(node, token, expr_3());
        }
        return node;
    }

    AST* expr_1()
    {
        AST* node = expr_2();
        while (currentToken.type == PLUS || currentToken.type == MINUS)
        {
            Token token = currentToken;
            if (token.type == PLUS)
            {
                eat(PLUS);
            }
            if (token.type == MINUS)
            {
                eat(MINUS);
            }
            node = new BinOp(node, token, expr_2());
        }
        return node;
    }

    AST* parse()
    {
        return expr_1();
    }
};

// Interpreter starts here

class MathParser::Interpreter {
public:

    std::string errorMessage;
    bool invalid;

    Interpreter()
    {
        this->invalid = false;
        this->errorMessage = "";
    }

    void error(std::string message)
    {
        this->invalid = true;
        this->errorMessage = message;
    }

    double visit(AST* node)
    {
        if (node == NULL)
        {
            error("Unexpected end of nodes");
            return -1;
        }
        switch (node->type)
        {
        case AST_NUMBER:
            return ((Num*)node)->value;
        case AST_IDENTIFIER:
            if (variables.count(((Var*)node)->name))
            {
                return variables[((Var*)node)->name];
            }
            error("Variable not found");
            return -1;
        case AST_BINOP:
            switch (((BinOp*)node)->op.type)
            {
            case PLUS:
                return visit(((BinOp*)node)->left) + visit(((BinOp*)node)->right);
            case MINUS:
                return visit(((BinOp*)node)->left) - visit(((BinOp*)node)->right);
            case MULTIPLY:
                return visit(((BinOp*)node)->left) * visit(((BinOp*)node)->right);
            case DIVIDE:
                return visit(((BinOp*)node)->left) / visit(((BinOp*)node)->right);
            case MODULO:
                return fmod(visit(((BinOp*)node)->left), visit(((BinOp*)node)->right));
            case POWER:
                return pow(visit(((BinOp*)node)->left), visit(((BinOp*)node)->right));
            case ASSIGN:
                variables[((Var*)((BinOp*)node)->left)->name] = visit(((BinOp*)node)->right);
                return variables[((Var*)((BinOp*)node)->left)->name];
            default:
                error("Unexpected operator type");
                return -1;
            }
        case AST_UNARYOP:
            switch (((UnaryOp*)node)->op.type)
            {
            case PLUS:
                return visit(((UnaryOp*)node)->expr);
            case MINUS:
                return -visit(((UnaryOp*)node)->expr);
            default:
                error("Unexpected operator type");
                return -1;
            }
        case AST_FUNCTION:
            {
                std::string name = ((Function*)node)->name;
                std::transform(name.begin(), name.end(), name.begin(), ::tolower);

                std::vector<AST*> args = ((Function*)node)->exprs;
                if (args.size() == 0)
                {
                    if (name == "random") return fRandom();
                }
                else if (args.size() == 1)
                {
                    if (name == "sin") return sin(visit(args[0]));
                    else if (name == "cos") return cos(visit(args[0]));
                    else if (name == "tan") return tan(visit(args[0]));
                    else if (name == "asin") return asin(visit(args[0]));
                    else if (name == "acos") return acos(visit(args[0]));
                    else if (name == "atan") return atan(visit(args[0]));
                    else if (name == "sinh") return sinh(visit(args[0]));
                    else if (name == "cosh") return cosh(visit(args[0]));
                    else if (name == "tanh") return tanh(visit(args[0]));
                    else if (name == "asinh") return asinh(visit(args[0]));
                    else if (name == "acosh") return acosh(visit(args[0]));
                    else if (name == "atanh") return atanh(visit(args[0]));
                    else if (name == "sqrt") return sqrt(visit(args[0]));
                    else if (name == "log") return log(visit(args[0]));
                    else if (name == "ln") return log(visit(args[0]));
                    else if (name == "log2") return log2(visit(args[0]));
                    else if (name == "log10") return log10(visit(args[0]));
                    else if (name == "exp") return exp(visit(args[0]));
                    else if (name == "abs") return abs(visit(args[0]));
                    else if (name == "floor") return floor(visit(args[0]));
                    else if (name == "ceil") return ceil(visit(args[0]));
                    else if (name == "round") return round(visit(args[0]));
                    else if (name == "cbrt") return cbrt(visit(args[0]));
                    else if (name == "int") return int(visit(args[0]));
                    else if (name == "flag") {
                        int value = (int) visit(args[0]);
                        return (int)(INBOUNDS_ARR(value, obj.flags) && obj.flags[value]);
                    }
                }
                else if (args.size() == 2)
                {
                    if (name == "min") return std::min(visit(args[0]), visit(args[1]));
                    if (name == "max") return std::max(visit(args[0]), visit(args[1]));
                    if (name == "pow") return pow(visit(args[0]), visit(args[1]));
                    if (name == "mod") return fmod(visit(args[0]), visit(args[1]));
                    if (name == "log") return log(visit(args[0])) / log(visit(args[1]));
                }
                else if (args.size() == 3)
                {
                    if (name == "clamp") return std::max(visit(args[1]), std::min(visit(args[0]), visit(args[2])));
                }

                error("Function not found or incorrect usage");
                return -1;
            }
        case AST_ASSIGN:
            {
                std::string name = ((Assign*)node)->name;
                AST* expr = ((Assign*)node)->expr;
                variables[name] = visit(expr);
                return variables[name];
            }
        }

        error("Unknown error while visiting nodes");
        return -1;
    }
};

void MathParser::ResetEnvironment()
{
    variables.clear();
    variables["pi"] = 3.14159265358979323846;
    variables["e"] = 2.71828182845904523536;
}

void MathParser::SetVariable(std::string name, double value)
{
    variables[name] = value;
}

MathParser::ExpressionOutput MathParser::ParseExpression(std::string expression)
{
    ExpressionOutput returnValue;

    returnValue.value = 0;
    returnValue.success = true;
    returnValue.error = "";

    Lexer* lexer = new Lexer(expression);
    Parser* parser = new Parser(*lexer);
    AST* parsed = (*parser).parse();

    if (parsed == NULL)
    {
        returnValue.success = false;
        returnValue.error = parser->errorMessage;
    }
    else
    {
        Interpreter interpreter;
        double value = interpreter.visit(parsed);
        returnValue.success = !interpreter.invalid;
        if (interpreter.invalid)
        {
            returnValue.error = interpreter.errorMessage;
        }
        else
        {
            returnValue.value = value;
        }
    }

    return returnValue;
}
