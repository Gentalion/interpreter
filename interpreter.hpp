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

///===================
/// Poliz + 3 functions
///===================

Type_of_lex effective_type (Lex l, Ident_table ident_table) {
    if (l.get_type() != LEX_ID) {
        return l.get_type();
    }
    else {
        if (ident_table[l.get_value_int()].get_declared()) {
            return ident_table[l.get_value_int()].get_type();
        }
        else {
            throw runtime_error ("Identifier wasn't defined.");
        }
    }
}

const Const& effective_value(Lex l, Ident_table ident_table) {
    if (l.get_type() != LEX_ID) {
        return l.get_value_const();
    }
    else {
        if (ident_table[l.get_value_int()].get_assigned()) {
            return ident_table[l.get_value_int()].get_value_const();
        }
        else {
            throw runtime_error ("Identifier wasn't initialized.");
        }
    }
}

bool effective_bool (Lex l, Ident_table ident_table) {
    switch (effective_type(l, ident_table)) {
    case CONST_BOOL:
        return effective_value(l, ident_table).b;
        break;
    case CONST_INT:
        return (effective_value(l, ident_table).i != 0);
        break;
    case CONST_DOUBLE:
        return (effective_value(l, ident_table).d != 0);
        break;
    case CONST_STRING:
        throw runtime_error ("String can't be cast to boolean in \"if\" or \"while\".");
    default:
        throw runtime_error ("Lex of invalid type in the condition of \"if\" or \"while\"");
    }
}

class Poliz {
    vector<Lex> contents;
    map<string,int> labels;
    stack<Lex> cash;

public:
    Poliz ():
        contents(),
        labels(),
        cash()
    {}

    int current_index () {
        return contents.size();
    }

    void put(Lex l) {
        cout << "I am";
        contents.push_back (l);
        cout << "here";
    }

    void put(Type_of_lex t) {
        contents.push_back (Lex (t));
    }

    void set (int i, Lex l) {
        contents[i] = l;
    }

    int label_address (string name) {
        for (auto it : labels)
            if (name == it.first)
                return it.second;
        throw runtime_error ("Label wasn't found in poliz labels map");
    }

    void add_label (string name, int i) {
        labels.insert (pair<string,int>(name, i));
    }

    void execute(Ident_table ident_table) {
        Lex to_push, operand1, operand2, label;
        Ident ident;
        for (int i = 0; i < contents.size(); i++) {
            switch (contents[i].get_type()) {
            case POLIZ_GOTO:
                {
                    i = label_address(cash.top().get_value_string());
                    cash.pop();
                    i--;
                }
                break;
            case POLIZ_IF:
                {
                    label = cash.top();
                    cash.pop();

                    if (!effective_bool(cash.top(), ident_table)) {
                        i = label_address(label.get_value_string());
                        i--;
                    }
                    cash.pop();
                }
                break;
            case POLIZ_READ:
                {
                    ident = ident_table.get(cash.top().get_value_int());
                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        {
                            int in;
                            cin >> in;
                            ident.set_value(in);
                            ident.set_assigned();
                        }
                        break;
                    case CONST_DOUBLE:
                        {
                            double in;
                            cin >> in;
                            ident.set_value(in);
                            ident.set_assigned();
                        }
                        break;
                    case CONST_STRING:
                        {
                            string in;
                            cin >> in;
                            ident.set_value(in);
                            ident.set_assigned();
                        }
                        break;
                    default: //supposed for booleans
                        throw runtime_error ("Operator \"read\" can't take boolean as an argument.");
                    }

                    cash.pop();
                }
                break;
            case POLIZ_WRITE:
                {
                    const Const& write_value = effective_value(cash.top(), ident_table);
                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        cout << write_value.i;
                        break;
                    case CONST_DOUBLE:
                        cout << write_value.d;
                        break;
                    case CONST_BOOL:
                        cout << write_value.b;
                        break;
                    case CONST_STRING:
                        cout << write_value.s;
                        break;
                    }
                    cash.pop();
                }
                break;
            case LEX_NOT:
                {
                    const Const& not_value = effective_value(cash.top(), ident_table);
                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_INT, (int) !(not_value.i));
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_INT, (double) !(not_value.d));
                        break;
                    cash.pop();
                    cash.push(to_push);
                    }
                }
                break;
            case LEX_EQUALS:
                {
                    operand1 = cash.top();
                    ident = ident_table.get(operand1.get_value_int());
                    cash.pop();
                    operand2 = cash.top();
                    cash.pop();
                    switch (ident.get_type()) {
                    case CONST_INT:
                        {
                            ident.set_value(operand2.get_value_int());
                            ident.set_assigned();
                        }
                        break;
                    case CONST_DOUBLE:
                        {
                            ident.set_value(operand2.get_value_double());
                            ident.set_assigned();
                        }
                        break;
                    case CONST_BOOL:
                        {
                            ident.set_value(operand2.get_value_bool());
                            ident.set_assigned();
                        }
                        break;
                    case CONST_STRING:
                        {
                            ident.set_value(operand2.get_value_string());
                            ident.set_assigned();
                        }
                        break;
                    }

                    cash.push(operand1);
                }
                break;
            case LEX_PLUS:
                {
                    operand2 = cash.top();
                    cash.pop();
                    operand1 = cash.top();
                    cash.pop();
                    const Const& plus_value1 = effective_value(operand1, ident_table);
                    const Const& plus_value2 = effective_value(operand2, ident_table);

                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_INT, plus_value1.i + plus_value2.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_DOUBLE, plus_value1.d + plus_value2.d);
                        break;
                    case CONST_STRING:
                        to_push = Lex (CONST_STRING, plus_value1.s + plus_value2.s);
                        break;
                    }

                    cash.push(to_push);
                }
                break;
            case LEX_MINUS:
                {
                    operand2 = cash.top();
                    cash.pop();
                    operand1 = cash.top();
                    cash.pop();
                    const Const& minus_value1 = effective_value(operand1, ident_table);
                    const Const& minus_value2 = effective_value(operand2, ident_table);

                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_INT, minus_value1.i - minus_value2.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_DOUBLE, minus_value1.d - minus_value2.d);
                        break;
                    }

                    cash.push(to_push);
                }
                break;
            case LEX_ASTERISK:
                {
                    operand2 = cash.top();
                    cash.pop();
                    operand1 = cash.top();
                    cash.pop();
                    const Const& multuply_value1 = effective_value(operand1, ident_table);
                    const Const& multuply_value2 = effective_value(operand2, ident_table);

                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_INT, multuply_value1.i * multuply_value2.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_DOUBLE, multuply_value1.d * multuply_value2.d);
                        break;
                    }

                    cash.push(to_push);
                }
                break;
            case LEX_SLASH:
                {
                    operand2 = cash.top();
                    cash.pop();
                    operand1 = cash.top();
                    cash.pop();
                    const Const& divide_value1 = effective_value(operand1, ident_table);
                    const Const& divide_value2 = effective_value(operand2, ident_table);

                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_INT, divide_value1.i / divide_value2.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_DOUBLE, divide_value1.d / divide_value2.d);
                        break;
                    }

                    cash.push(to_push);
                }
                break;
            default:
                cash.push(contents[i]);
            }
        }
    }
};

///===================
/// Parser
///===================

class Parser {
    Lex current_lex;
    Lex_analyzer lex_analyzer;
    Poliz * poliz;

    Lex next_lex (bool used = true) {
        static bool next = true;
        if (next) current_lex = lex_analyzer.get_lex ();
        next = used;
        return current_lex;
    }

public:

    void execute_program () {
        Program();
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
        while (next_lex(false).get_type() == LEX_COMMA)
            Variable(type_buf);
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
            if (lex_analyzer.get_ident_table()[var.get_value_int()].get_declared()) {
                throw runtime_error (generate_error(lex_analyzer.get_line(), "Variable was defined twice."));
            }
            else {
                lex_analyzer.get_ident_table()[var.get_value_int()].set_declared();
                lex_analyzer.get_ident_table()[var.get_value_int()].set_type(t);
            }
            if (next_lex(false).get_type() == LEX_EQUALS) {
                next_lex();
                //!Важно, вызов Constant()
                lex_analyzer.get_ident_table()[var.get_value_int()].set_value(Constant());
                lex_analyzer.get_ident_table()[var.get_value_int()].set_assigned();
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
            Operator();
        }
    }
    //===================
    void Operator() {
        if (next_lex(false).get_type() == LEX_CURLY_OPENING_BRACKET) {
            Complex_operator();
        }
        if (next_lex(false).get_type() == LEX_IF) {
            If ();
        }
        if (next_lex(false).get_type() == LEX_WHILE) {
            While ();
        }
        if (next_lex(false).get_type() == LEX_READ) {
            Read ();
        }
        if (next_lex(false).get_type() == LEX_WRITE) {
            Write ();
        }
        Expression();
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
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \"}if\" is expected."));
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

            poliz->add_label("IF" + index, second_label_destination);
            poliz->set(second_label_place, Lex (LEX_LABEL, "IF" + index));

            poliz->add_label("ELSE" + index, first_label_destination);
            poliz->set(second_label_place, Lex (LEX_LABEL, "ELSE" + index));
        }
        else {
            int first_label_destination = poliz->current_index();
            poliz->add_label("IF" + index, first_label_destination);
            poliz->set(first_label_place, Lex (POLIZ_LABEL, "IF" + index));
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
        poliz->add_label("WHILE_BEGIN" + index, second_label_destination);
        Expression ();
        int first_label_place = poliz->current_index();
        poliz->put (LEX_NULL);
        poliz->put (POLIZ_IF);

        if (next_lex().get_type() != LEX_CLOSING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \")\" is expected."));
        }
        Operator();

        //second label
        poliz->put (Lex (POLIZ_LABEL, "WHILE_BEGIN" + index));
        poliz->put (POLIZ_GOTO);

        int first_label_destination = poliz->current_index();
        poliz->add_label("WHILE_END" + index, first_label_destination);
        poliz->set(first_label_place, Lex(LEX_LABEL, "WHILE_END" + index));

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
        while (next_lex(false).get_type() == LEX_COMMA) {
            next_lex();
            Expression();
            poliz->put(POLIZ_WRITE);
        }
        if (next_lex().get_type() != LEX_CLOSING_BRACKET) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \")\" is expected."));
        }
        if (next_lex().get_type() != LEX_SEMICOLON) {
            throw runtime_error (generate_error(lex_analyzer.get_line(), "Lexem \";\" is expected."));
        }
    }

    //===================

    Type_of_lex Expression () {
        Lex lex_buf = next_lex();
        // операция not
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
            return Expression();
        }
        else {
            poliz->put(lex_buf); // это происходит в каждой ветке при правильном выражении

            Type_of_lex lex_operation = next_lex().get_type();

            if (lex_operation == LEX_CLOSING_BRACKET || lex_operation == LEX_SEMICOLON) {
                return effective_type(lex_buf, lex_analyzer.get_ident_table());
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
    Parser (const string& name) :
        lex_analyzer (name)
    {}
};

///===================
/// Functions
///===================

string generate_error (int number, const string& text) {
    stringstream ss;
    ss << number;
    return ("Line " +  ss.str() + ": " + text);
}
