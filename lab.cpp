#include <iostream>
#include <vector>
#include <map>
#include <cctype>
#include <string>

#include <fstream> // For file I/O
#include <sstream> // For string stream

using namespace std;

enum TokenType
{
    T_INT,
    T_ID,
    T_NUM,
    T_IF,
    T_ELSE,
    T_RETURN,
    T_ASSIGN,
    T_PLUS,
    T_MINUS,
    T_MUL,
    T_DIV,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_SEMICOLON,
    T_GT,
    T_FLT,
    T_EOF,
    T_DBL,
    T_STR,
    T_CHR,
    T_BOOL,
};

struct Token
{
    TokenType type;
    string value;
    int line; // Add line number
};

class Lexer
{
private:
    string src;
    size_t pos;
    int line; // Track the current line number

public:
    Lexer(const string &src)
    {
        this->src = src;
        this->pos = 0;
        this->line = 1; // Initialize the line number to 1
    }

    vector<Token> tokenize()
    {
        vector<Token> tokens;

        while (pos < src.size())
        {
            char current = src[pos];

            if (isspace(current))
            {
                if (current == '\n')
                    line++; // Increment line number on new line
                pos++;
                continue;
            }

            if (isdigit(current))
            {
                tokens.push_back(Token{T_NUM, consumeNumber(), line});
                continue;
            }

            if (current == '"') // Handle string literals
            {
                tokens.push_back(Token{T_STR, consumeString()});
                continue;
            }

            if (current == '\'') // Handle char literals
            {
                tokens.push_back(Token{T_CHR, consumeChar(), line});
                continue;
            }
            if (isalpha(current))
            {
                string word = consumeWord();
                if (word == "int")
                    tokens.push_back(Token{T_INT, word, line});
                else if (word == "string")
                    tokens.push_back(Token{T_STR, word, line});
                else if (word == "float")
                    tokens.push_back(Token{T_FLT, word, line});
                else if (word == "bool")
                    tokens.push_back(Token{T_BOOL, word, line});
                else if (word == "char")
                    tokens.push_back(Token{T_CHR, word, line});
                else if (word == "double")
                    tokens.push_back(Token{T_DBL, word, line});
                else if (word == "if")
                    tokens.push_back(Token{T_IF, word, line});
                else if (word == "else")
                    tokens.push_back(Token{T_ELSE, word, line});
                else if (word == "return")
                    tokens.push_back(Token{T_RETURN, word, line});
                else
                    tokens.push_back(Token{T_ID, word, line});
                continue;
            }

            switch (current)
            {
            case '=':
                tokens.push_back(Token{T_ASSIGN, "=", line});
                break;
            case '+':
                tokens.push_back(Token{T_PLUS, "+", line});
                break;
            case '-':
                tokens.push_back(Token{T_MINUS, "-", line});
                break;
            case '*':
                tokens.push_back(Token{T_MUL, "*", line});
                break;
            case '/':
                tokens.push_back(Token{T_DIV, "/", line});
                break;
            case '(':
                tokens.push_back(Token{T_LPAREN, "(", line});
                break;
            case ')':
                tokens.push_back(Token{T_RPAREN, ")", line});
                break;
            case '{':
                tokens.push_back(Token{T_LBRACE, "{", line});
                break;
            case '}':
                tokens.push_back(Token{T_RBRACE, "}", line});
                break;
            case ';':
                tokens.push_back(Token{T_SEMICOLON, ";", line});
                break;
            case '>':
                tokens.push_back(Token{T_GT, ">", line});
                break;
            default:
                cout << "Unexpected character: " << current << " at line " << line << endl;
                exit(1);
            }
            pos++;
        }

        tokens.push_back(Token{T_EOF, "", line});
        return tokens;
    }

    string consumeNumber()
    {
        size_t start = pos;
        bool hasDot = false;

        while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.'))
        {
            if (src[pos] == '.')
            {
                if (hasDot) // If there's more than one dot, it's an invalid float
                {
                    cout << "Unexpected character: . at line " << line << endl;
                    exit(1);
                }
                hasDot = true;
            }
            pos++;
        }

        return src.substr(start, pos - start);
    }

    string consumeWord()
    {
        size_t start = pos;
        while (pos < src.size() && isalnum(src[pos]))
            pos++;
        return src.substr(start, pos - start);
    }

    string consumeString()
    {
        pos++; // Skip the opening double quote
        size_t start = pos;
        while (pos < src.size() && src[pos] != '"')
        {
            pos++;
        }
        if (pos >= src.size())
        {
            cout << "Unterminated string literal" << endl;
            exit(1);
        }
        string result = src.substr(start, pos - start);
        pos++; // Skip the closing double quote
        return result;
    }

    string consumeChar()
    {
        pos++; // Skip the opening single quote
        if (pos + 1 < src.size() && src[pos + 1] == '\'')
        {
            char result = src[pos];
            pos += 2; // Skip the char and closing quote
            return string(1, result);
        }
        else
        {
            cout << "Unexpected char literal format at line " << line << endl;
            exit(1);
        }
    }
};

class Parser
{

public:
    Parser(const vector<Token> &tokens)
    {
        this->tokens = tokens;
        this->pos = 0;
    }

    void parseProgram()
    {
        while (tokens[pos].type != T_EOF)
        {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;

    void parseStatement()
    {
        if ((tokens[pos].type == T_INT) || (tokens[pos].type == T_STR) || (tokens[pos].type == T_FLT) || (tokens[pos].type == T_BOOL) || (tokens[pos].type == T_CHR) || (tokens[pos].type == T_DBL))
        {
            parseDeclaration();
        }
        else if (tokens[pos].type == T_ID)
        {
            parseAssignment();
        }
        else if (tokens[pos].type == T_IF)
        {
            parseIfStatement();
        }
        else if (tokens[pos].type == T_RETURN)
        {
            parseReturnStatement();
        }
        else if (tokens[pos].type == T_LBRACE)
        {
            parseBlock();
        }
        else
        {
            cout << "Syntax error: unexpected token '" << tokens[pos].value
                 << "' at line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void parseBlock()
    {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF)
        {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void parseDeclaration()
    {
        if ((tokens[pos].type == T_INT) || (tokens[pos].type == T_STR) || (tokens[pos].type == T_FLT) || (tokens[pos].type == T_BOOL) || (tokens[pos].type == T_CHR) || (tokens[pos].type == T_DBL))
        {
            pos++;
        }

        expect(T_ID);
        expect(T_SEMICOLON);
    }

    void parseAssignment()
    {
        expect(T_ID);
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseIfStatement()
    {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();
        if (tokens[pos].type == T_ELSE)
        {
            expect(T_ELSE);
            parseStatement();
        }
    }

    void parseReturnStatement()
    {
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseExpression()
    {
        parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS)
        {
            pos++;
            parseTerm();
        }
        if (tokens[pos].type == T_GT)
        {
            pos++;
            parseExpression(); // After relational operator, parse the next expression
        }
    }

    void parseTerm()
    {
        parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV)
        {
            pos++;
            parseFactor();
        }
    }

    void parseFactor()
    {
        if (tokens[pos].type == T_NUM || tokens[pos].type == T_ID || tokens[pos].type == T_STR || tokens[pos].type == T_CHR)
        {
            pos++;
        }
        else if (tokens[pos].type == T_LPAREN)
        {
            expect(T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
        }
        else
        {
            cout << "Syntax error: unexpected token '" << tokens[pos].value
                 << "' at line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    void expect(TokenType type)
    {
        if (tokens[pos].type == type)
        {
            pos++;
        }
        else
        {
            cout << "Syntax error: expected " << type << " but found '"
                 << tokens[pos].value << "' at line " << tokens[pos].line << endl;
            exit(1);
        }
    }
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " <source_file>" << endl;
        return 1;
    }

    // Open the file specified in the command line
    ifstream file(argv[1]);
    if (!file.is_open())
    {
        cout << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }

    // Read the entire file content into a string
    stringstream buffer;
    buffer << file.rdbuf();
    string input = buffer.str();

    // Close the file
    file.close();

    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}