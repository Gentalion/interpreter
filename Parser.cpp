#include <stdlib.h>
#include <string>
#include <ctype.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <stack>
#include "Lex_analyzer.cpp"
#include "Poliz.cpp"

///===================
/// Parser
///===================

class Parser {
    Lex current_lex;
    Lex_analyzer lex_analyzer;
    Poliz * poliz;

    Lex & next_lex (bool used = true) {
        static bool next = true;
        if (next) current_lex = lex_analyzer.get_lex();
        next = used;
        return current_lex;
    }

public:

    void execute_program () {
        Program();
        cout << "...Executing...\n";
        poliz->execute(lex_analyzer.get_ident_table());
    }

    void Program() {
        if (next_lex().get_type() != LEX_PROGRAM) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"program\" is expected."));
        }
        if (next_lex().get_type() != LEX_CURLY_OPENING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"{\" is expected."));
        }
        Definitions();
        Operators();
        if (next_lex().get_type() != LEX_CURLY_CLOSING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"}\" is expected."));
        }
        poliz->put(LEX_FIN);
    }

private:
    void Definitions() {
        while (next_lex(false).get_type() == LEX_INT)
            Definition();
    }
    //===================
    void Definition() {
        Type_of_lex type_buf = Type();
        Variable(type_buf);
        while (next_lex(false).get_type() == LEX_COMMA) {
            next_lex();
            Variable(type_buf);
        }
        if (next_lex().get_type() != LEX_SEMICOLON) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \";\" is expected."));
        }
    }
    //===================
    Type_of_lex Type() {
        if (next_lex(false).get_type() == LEX_INT) {
            next_lex();
            return CONST_INT;
        }
        if (next_lex(false).get_type() == LEX_STRING) {
            next_lex();
            return CONST_STRING;
        }
        if (next_lex(false).get_type() == LEX_BOOL) {
            next_lex();
            return CONST_BOOL;
        }
        if (next_lex(false).get_type() == LEX_DOUBLE) {
            next_lex();
            return CONST_DOUBLE;
        }
        else {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Type is expected."));
        }
 
    }
    //===================
    void Variable(Type_of_lex t) {
        Lex var = next_lex();

        if (var.get_type() != LEX_ID) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Identifier is expected."));
        }
        else {
            if (lex_analyzer.get_ident_table().get(var.get_value_string()).get_declared()) {
                throw runtime_error (generate_error(lex_analyzer.get_line(), "Variable was defined twice."));
            }
            else {
                lex_analyzer.get_ident_table().get(var.get_value_string()).set_declared();
                lex_analyzer.get_ident_table().get(var.get_value_string()).set_type(t);
            }
            if (next_lex(false).get_type() == LEX_EQUALS) {
                next_lex();
                //!Важно, вызов Constant()
                lex_analyzer.get_ident_table().get(var.get_value_string()).set_value(Constant());
                lex_analyzer.get_ident_table().get(var.get_value_string()).set_assigned();
            }
        }
    }
    //===================
    Lex Constant() {
        if (next_lex(false).get_type() != CONST_STRING) {
            return Number();
        }
        else {
            return next_lex();
        }
    }
    //===================
    Lex Number() {
        Lex lex_buf = next_lex();
        if (lex_buf.get_type() != LEX_PLUS && lex_buf.get_type() != LEX_MINUS) {
            if (lex_buf.get_type() != CONST_INT && lex_buf.get_type() != CONST_DOUBLE) {
                throw runtime_error (generate_error(lex_analyzer.get_line(), "Constant is expected."));
            }
            else {
                return lex_buf;
            }
        }
        else {
            Lex sign = lex_buf;
            lex_buf = next_lex();
            if (lex_buf.get_type() != CONST_INT && lex_buf.get_type() != CONST_DOUBLE) {
                throw runtime_error (generate_error(lex_analyzer.get_line(), "Constant is expected."));
            }

            if (sign.get_type() == LEX_PLUS) {
                return lex_buf;
            }
            else { //sign.get_type() = LEX_MINUS
                if (lex_buf.get_type() == CONST_INT) {
                    return Lex (CONST_INT, lex_buf.get_value_int() * (-1));
                }
                else { //lex_buf.get_type() = CONST_DOUBLE
                    return Lex (CONST_INT, lex_buf.get_value_double() * (-1));
                }
            }
        }
    }
    //===================
    void Operators () {
        while (true) {
            if (next_lex(false).get_type() == LEX_CURLY_CLOSING_BRACKET) {
                break;
            }
            if (!Operator()) break;
        }
    }
    //===================
    bool Operator() {
        if (next_lex(false).get_type() == LEX_CURLY_OPENING_BRACKET) {
            Complex_operator();
            return true;
        }
        if (next_lex(false).get_type() == LEX_IF) {
            If ();
            return true;
        }
        if (next_lex(false).get_type() == LEX_WHILE) {
            While ();
            return true;
        }
        if (next_lex(false).get_type() == LEX_READ) {
            Read ();
            return true;
        }
        if (next_lex(false).get_type() == LEX_WRITE) {
            Write ();
            return true;
        }
        if (Expression() == LEX_FIN) return false;
        if (next_lex().get_type() != LEX_SEMICOLON) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \";\" is expected."));
        }

        return true;
    }
    //===================
    void Complex_operator() {
        if (next_lex().get_type() != LEX_CURLY_OPENING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"{\" is expected."));
        }
        Operators();
        if (next_lex().get_type() != LEX_CURLY_CLOSING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"}\" is expected."));
        }
    }
    //===================
    void If () {
        static int  index = 1;
        if (next_lex().get_type() != LEX_IF) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"if\" is expected."));
        }
        if (next_lex().get_type() != LEX_OPENING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"(\" is expected."));
        }

        Expression();
        int first_label_place = poliz->current_index();
        poliz->put(LEX_NULL);
        poliz->put(POLIZ_IF);

        if (next_lex().get_type() != LEX_CLOSING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \")\" is expected."));
        }
        Operator();

        if (next_lex().get_type() == LEX_ELSE) {
            int second_label_place = poliz->current_index();
            poliz->put(LEX_NULL);
            poliz->put(POLIZ_GOTO);

            int first_label_destination = poliz->current_index();
            Operator();
            int second_label_destination = poliz->current_index();

            poliz->add_label("IF" + to_string(index), second_label_destination);
            poliz->set(second_label_place, Lex (LEX_LABEL, "IF" + to_string(index)));

            poliz->add_label("ELSE" + to_string(index), first_label_destination);
            poliz->set(first_label_place, Lex (LEX_LABEL, "ELSE" + to_string(index)));
        }
        else {
            int first_label_destination = poliz->current_index();
            poliz->add_label("IF" + to_string(index), first_label_destination);
            poliz->set(first_label_place, Lex (POLIZ_LABEL, "IF" + to_string(index)));
        }

        index++;
    }
    //===================
    void While () {
        static int index = 1;

        if (next_lex().get_type() != LEX_WHILE) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"while\" is expected."));
        }
        if (next_lex().get_type() != LEX_OPENING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"(\" is expected."));
        }

        int second_label_destination = poliz->current_index();
        poliz->add_label("WHILE_BEGIN" + to_string(index), second_label_destination);
        Expression ();
        int first_label_place = poliz->current_index();
        poliz->put (LEX_NULL);
        poliz->put (POLIZ_IF);

        if (next_lex().get_type() != LEX_CLOSING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \")\" is expected."));
        }
        Operator();

        //second label
        poliz->put (Lex (POLIZ_LABEL, "WHILE_BEGIN" + to_string(index)));
        poliz->put (POLIZ_GOTO);

        int first_label_destination = poliz->current_index();
        poliz->add_label("WHILE_END" + to_string(index), first_label_destination);
        poliz->set(first_label_place, Lex(LEX_LABEL, "WHILE_END" + to_string(index)));

        index++;
    }
    //===================
    void Read () {
        Lex lex_buf;
        if (next_lex().get_type() != LEX_READ) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"read\" is expected."));
        }
        if (next_lex().get_type() != LEX_OPENING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"(\" is expected."));
        }
        if ((lex_buf = next_lex()).get_type() != LEX_ID) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Identifier is expected."));
        }
        if (next_lex().get_type() != LEX_CLOSING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \")\" is expected."));
        }
        if (next_lex().get_type() != LEX_SEMICOLON) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \";\" is expected."));
        }

        //semantics
        if ((effective_type(lex_buf, lex_analyzer.get_ident_table())) == CONST_BOOL) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Operator \"read\" isn't defined for booleans."));
        }

        //poliz
        poliz->put(lex_buf);
        poliz->put(POLIZ_READ);
    }
    //===================
    void Write () {
        if (next_lex().get_type() != LEX_WRITE) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"write\" is expected."));
        }
        if (next_lex().get_type() != LEX_OPENING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"(\" is expected."));
        }
        Expression ();
        poliz->put(POLIZ_WRITE);
        while (next_lex(false).get_type() == LEX_COMMA) {
            next_lex();
            Expression();
            poliz->put(POLIZ_WRITE);
        }
        poliz->put(Lex(CONST_STRING, string("\n")));
        poliz->put(POLIZ_WRITE);

        if (next_lex().get_type() != LEX_CLOSING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \")\" is expected."));
        }
        if (next_lex().get_type() != LEX_SEMICOLON) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \";\" is expected."));
        }
    }

    //===================

    Type_of_lex Expression () {
        //cout << "EXP\n";
        Lex lex_buf = next_lex();
        // операция not
        if (lex_buf.get_type() == LEX_FIN) {
            return LEX_FIN;
        }

        if (lex_buf.get_type() == LEX_NOT) {
            Expression();
            poliz->put(lex_buf);
        }
        // унарные + и -
        else if (lex_buf.get_type() == LEX_PLUS || lex_buf.get_type() == LEX_MINUS) {
            poliz->put(Lex (CONST_INT, 0));
            Expression();
            poliz->put(lex_buf);
        }
        else if (lex_buf.get_type() == LEX_OPENING_BRACKET) {
            Type_of_lex to_return = Expression();
            if (next_lex().get_type() != LEX_CLOSING_BRACKET) {
                throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \")\" is expected."));
            }
            return to_return;
        }
        else {
            poliz->put(lex_buf); // это происходит в каждой ветке при правильном выражении
            Type_of_lex lex_operation = next_lex(false).get_type();

            if (lex_operation == LEX_CLOSING_BRACKET || lex_operation == LEX_SEMICOLON || lex_operation == LEX_COMMA) {
                return effective_type(lex_buf, lex_analyzer.get_ident_table());
            }
            else {
                next_lex();
            }
            // операции * и / для int
            if (lex_operation == LEX_ASTERISK || lex_operation == LEX_SLASH) {
                if (effective_type(lex_buf, lex_analyzer.get_ident_table()) == CONST_STRING ||
                    effective_type(lex_buf, lex_analyzer.get_ident_table()) == CONST_BOOL) {
                    throw runtime_error (generate_error(lex_analyzer.get_line(), "Operator \"not\" is not defined for strings and booleans."));
                }
                else {
                    if (effective_type(lex_buf, lex_analyzer.get_ident_table()) == Expression()) {
                        poliz->put(lex_operation);
                        return effective_type(lex_buf, lex_analyzer.get_ident_table());
                    }
                    else {
                        throw runtime_error (generate_error(lex_analyzer.get_line(), "Operand types in expression don't match."));
                    }
                }
            }
            // операции + для int и string
            if (lex_operation == LEX_PLUS) {
                if (effective_type(lex_buf, lex_analyzer.get_ident_table()) == CONST_BOOL) {
                    throw runtime_error (generate_error(lex_analyzer.get_line(), "Operator \"+\" isn't defined for booleans."));
                }
                else {
                    if (effective_type(lex_buf, lex_analyzer.get_ident_table()) == Expression()) {
                        poliz->put(lex_operation);
                        return effective_type(lex_buf, lex_analyzer.get_ident_table());
                    }
                    else {
                        throw runtime_error (generate_error(lex_analyzer.get_line(), "Operand types in expression don't match."));
                    }
                }
            }
            // операция - для int
            if (lex_operation == LEX_MINUS) {
                if (effective_type(lex_buf, lex_analyzer.get_ident_table()) == CONST_BOOL ||
                    effective_type(lex_buf, lex_analyzer.get_ident_table()) == CONST_STRING) {
                    throw runtime_error (generate_error(lex_analyzer.get_line(), "Operator \"-\" isn't defined for strings and booleans."));
                }
                else {
                    if (effective_type(lex_buf, lex_analyzer.get_ident_table()) == Expression()) {
                        poliz->put(lex_operation);
                        return effective_type(lex_buf, lex_analyzer.get_ident_table());
                    }
                    else {
                        throw runtime_error (generate_error(lex_analyzer.get_line(), "Operand types in expression don't match."));
                    }
                }
            }
            // операции <, >, <=, >=, ==, != для int и string
            if (lex_operation == LEX_LESS ||
                lex_operation == LEX_MORE ||
                lex_operation == LEX_LESS_EQUALS ||
                lex_operation == LEX_MORE_EQUALS ||
                lex_operation == LEX_EQUALS_EQUALS ||
                lex_operation == LEX_NOT_EQUALS) {
                if (effective_type(lex_buf, lex_analyzer.get_ident_table()) == CONST_BOOL) {
                    throw runtime_error (generate_error(lex_analyzer.get_line(), "Operators \"<\", \">\", \"<=\", \">=\", \"==\", \"!=\" aren't defined for booleans."));
                }
                else {
                    if (effective_type(lex_buf, lex_analyzer.get_ident_table()) == Expression()) {
                        poliz->put(lex_operation);
                        return effective_type(lex_buf, lex_analyzer.get_ident_table());
                    }
                    else {
                        throw runtime_error (generate_error(lex_analyzer.get_line(), "Operand types in expression don't match."));
                    }
                }
            }
            // операция and
            if (lex_operation == LEX_AND) {
                if (effective_type(lex_buf, lex_analyzer.get_ident_table()) != CONST_BOOL) {
                    throw runtime_error (generate_error(lex_analyzer.get_line(), "Operator \"and\" isn't defined for integers and strings."));
                }
                else {
                    if (effective_type(lex_buf, lex_analyzer.get_ident_table()) == Expression()) {
                        poliz->put(lex_operation);
                        return effective_type(lex_buf, lex_analyzer.get_ident_table());
                    }
                    else {
                        throw runtime_error (generate_error(lex_analyzer.get_line(), "Operand types in expression don't match."));
                    }
                }
            }
            // операция or
            if (lex_operation == LEX_OR) {
                if (effective_type(lex_buf, lex_analyzer.get_ident_table()) != CONST_BOOL) {
                    throw runtime_error (generate_error(lex_analyzer.get_line(), "Operator \"or\" isn't defined for integers and strings."));
                }
                else {
                    if (effective_type(lex_buf, lex_analyzer.get_ident_table()) == Expression()) {
                        poliz->put(lex_operation);
                        return effective_type(lex_buf, lex_analyzer.get_ident_table());
                    }
                    else {
                        throw runtime_error (generate_error(lex_analyzer.get_line(), "Operand types in expression don't match."));
                    }
                }
            }
            // операция =
            if (lex_operation == LEX_EQUALS) {
                if (effective_type(lex_buf, lex_analyzer.get_ident_table()) == Expression()) {
                    poliz->put(lex_operation);
                    return effective_type(lex_buf, lex_analyzer.get_ident_table());
                }
                else {
                    throw runtime_error (generate_error(lex_analyzer.get_line(), "Operand types in expression don't match."));
                }
            }
        }
    }

public:
    Parser (const string& name):
        current_lex (Lex (LEX_NULL)),
        lex_analyzer (name)
    {
        poliz = new Poliz();
    }

    ~Parser () {
        delete poliz;
    }
};

///===================
/// Functions
///===================

string generate_error (int number, const string& text) {
    stringstream ss;
    ss << number;
    return ("Line " +  ss.str() + ": " + text);
}
