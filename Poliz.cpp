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
/// Poliz + 2 functions
///===================

Type_of_lex effective_type (Lex l, Ident_table ident_table) {
    if (l.get_type() != LEX_ID) {
        return l.get_type();
    }
    else {
        if (ident_table.get(l.get_value_string()).get_declared()) {
            return ident_table.get(l.get_value_string()).get_type();
        }
        else {
            throw runtime_error ("Identifier wasn't defined.");
        }
    }
}

Const effective_value(Lex l, Ident_table ident_table) {
    if (l.get_type() != LEX_ID) {
        return l.get_value_const();
    }
    else {
        if (ident_table.get(l.get_value_string()).get_assigned()) {
            return ident_table.get(l.get_value_string()).get_value_const();
        }
        else {
            throw runtime_error ("Identifier wasn't initialized.");
        }
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
        //cout << get_lex_type_string (l.get_type()) << "\n";
        contents.push_back (l);
    }

    void put(Type_of_lex t) {
        //cout << get_lex_type_string (t) << "\n";
        contents.push_back (Lex (t));
    }

    void set (int i, Lex l) {
        contents[i] = l;
        //cout << contents[i].get_value_string() << "\n";
    }

    int label_address (string name) {
        //cout << "Looking for label \"" + name + "\"\n";

        for (auto it : labels)
            if (name == it.first)
                return it.second;
        throw runtime_error ("Label \"" + name + "\" wasn't found in poliz labels map");
    }

    void add_label (string name, int i) {
        labels.insert (pair<string,int>(name, i));
    }

    void execute(Ident_table ident_table) {
        Lex to_push, operand1, operand2, label;
        Const operand1_value, operand2_value;
        for (int i = 0; i < contents.size(); i++) {
            //cout << get_lex_type_string (contents[i].get_type()) << "\n";

            switch (contents[i].get_type()) {
            case LEX_FIN:
                {
                    i = contents.size();
                }
                break;
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

                    if (!effective_value(cash.top(), ident_table).b) {
                        i = label_address(label.get_value_string());
                        i--;
                    }
                    cash.pop();
                }
                break;
            case POLIZ_READ:
                {
                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        {
                            int in;
                            cin >> in;
                            ident_table.get(cash.top().get_value_string()).set_value(in);
                            ident_table.get(cash.top().get_value_string()).set_assigned();
                        }
                        break;
                    case CONST_DOUBLE:
                        {
                            double in;
                            cin >> in;
                            ident_table.get(cash.top().get_value_string()).set_value(in);
                            ident_table.get(cash.top().get_value_string()).set_assigned();
                        }
                        break;
                    case CONST_STRING:
                        {
                            string in;
                            cin >> in;
                            ident_table.get(cash.top().get_value_string()).set_value(in);
                            ident_table.get(cash.top().get_value_string()).set_assigned();
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
                    operand1_value = effective_value(cash.top(), ident_table);
                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        cout << operand1_value.i;
                        break;
                    case CONST_DOUBLE:
                        cout << operand1_value.d;
                        break;
                    case CONST_BOOL:
                        cout << operand1_value.b;
                        break;
                    case CONST_STRING:
                        cout << operand1_value.s;
                        break;
                    }
                    cash.pop();
                }
                break;
            case LEX_NOT:
                {
                    Const operand1_value = effective_value(cash.top(), ident_table);
                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_INT, (int) !(operand1_value.i));
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_INT, (double) !(operand1_value.d));
                        break;
                    cash.pop();
                    cash.push(to_push);
                    }
                }
                break;
            case LEX_EQUALS:
                {
                    operand2 = cash.top();
                    cash.pop();
                    operand1 = cash.top();
                    cash.pop();
                    switch (effective_type(operand1, ident_table)) {
                    case CONST_INT:
                        {
                            ident_table.get(operand1.get_value_string()).set_value(operand2.get_value_int());
                            ident_table.get(operand1.get_value_string()).set_assigned();
                        }
                        break;
                    case CONST_DOUBLE:
                        {
                            ident_table.get(operand1.get_value_string()).set_value(operand2.get_value_double());
                            ident_table.get(operand1.get_value_string()).set_assigned();
                        }
                        break;
                    case CONST_BOOL:
                        {
                            ident_table.get(operand1.get_value_string()).set_value(operand2.get_value_bool());
                            ident_table.get(operand1.get_value_string()).set_assigned();
                        }
                        break;
                    case CONST_STRING:
                        {
                            ident_table.get(operand1.get_value_string()).set_value(operand2.get_value_string());
                            ident_table.get(operand1.get_value_string()).set_assigned();
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
                    operand1_value = effective_value(operand1, ident_table);
                    operand2_value = effective_value(operand2, ident_table);

                    switch (effective_type(operand1, ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_INT, operand1_value.i + operand2_value.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_DOUBLE, operand1_value.d + operand2_value.d);
                        break;
                    case CONST_STRING:
                        to_push = Lex (CONST_STRING, string (operand1_value.s) + string (operand2_value.s));
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
                    operand1_value = effective_value(operand1, ident_table);
                    operand2_value = effective_value(operand2, ident_table);

                    switch (effective_type(operand1, ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_INT, operand1_value.i - operand2_value.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_DOUBLE, operand1_value.d - operand2_value.d);
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
                    operand1_value = effective_value(operand1, ident_table);
                    operand2_value = effective_value(operand2, ident_table);

                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_INT, operand1_value.i * operand2_value.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_DOUBLE, operand1_value.d * operand2_value.d);
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
                    operand1_value = effective_value(operand1, ident_table);
                    operand2_value = effective_value(operand2, ident_table);

                    switch (effective_type(cash.top(), ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_INT, operand1_value.i / operand2_value.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_DOUBLE, operand1_value.d / operand2_value.d);
                        break;
                    }

                    cash.push(to_push);
                }
                break;
            case LEX_LESS:
                {
                    operand2 = cash.top();
                    cash.pop();
                    operand1 = cash.top();
                    cash.pop();
                    operand1_value = effective_value(operand1, ident_table);
                    operand2_value = effective_value(operand2, ident_table);

                    switch (effective_type(operand1, ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_BOOL, operand1_value.i < operand2_value.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_BOOL, operand1_value.d < operand2_value.d);
                        break;
                    case CONST_STRING:
                        to_push = Lex (CONST_BOOL, string(operand1_value.s) < string(operand2_value.s));
                        break;
                    }

                    cash.push(to_push);
                }
                break;
            case LEX_MORE:
                {
                    operand2 = cash.top();
                    cash.pop();
                    operand1 = cash.top();
                    cash.pop();
                    operand1_value = effective_value(operand1, ident_table);
                    operand2_value = effective_value(operand2, ident_table);

                    switch (effective_type(operand1, ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_BOOL, operand1_value.i > operand2_value.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_BOOL, operand1_value.d > operand2_value.d);
                        break;
                    case CONST_STRING:
                        to_push = Lex (CONST_BOOL, string(operand1_value.s) > string(operand2_value.s));
                        break;
                    }

                    cash.push(to_push);
                }
                break;
            case LEX_LESS_EQUALS:
                {
                    operand2 = cash.top();
                    cash.pop();
                    operand1 = cash.top();
                    cash.pop();
                    operand1_value = effective_value(operand1, ident_table);
                    operand2_value = effective_value(operand2, ident_table);

                    switch (effective_type(operand1, ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_BOOL, operand1_value.i <= operand2_value.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_BOOL, operand1_value.d <= operand2_value.d);
                        break;
                    case CONST_STRING:
                        to_push = Lex (CONST_BOOL, string(operand1_value.s) <= string(operand2_value.s));
                        break;
                    }

                    cash.push(to_push);
                }
                break;
            case LEX_MORE_EQUALS:
                {
                    operand2 = cash.top();
                    cash.pop();
                    operand1 = cash.top();
                    cash.pop();
                    operand1_value = effective_value(operand1, ident_table);
                    operand2_value = effective_value(operand2, ident_table);

                    switch (effective_type(operand1, ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_BOOL, operand1_value.i >= operand2_value.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_BOOL, operand1_value.d >= operand2_value.d);
                        break;
                    case CONST_STRING:
                        to_push = Lex (CONST_BOOL, string(operand1_value.s) >= string(operand2_value.s));
                        break;
                    }

                    cash.push(to_push);
                }
                break;
            case LEX_EQUALS_EQUALS:
                {
                    operand2 = cash.top();
                    cash.pop();
                    operand1 = cash.top();
                    cash.pop();
                    operand1_value = effective_value(operand1, ident_table);
                    operand2_value = effective_value(operand2, ident_table);

                    switch (effective_type(operand1, ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_BOOL, operand1_value.i == operand2_value.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_BOOL, operand1_value.d == operand2_value.d);
                        break;
                    case CONST_STRING:
                        to_push = Lex (CONST_BOOL, string(operand1_value.s) == string(operand2_value.s));
                        break;
                    }

                    cash.push(to_push);
                }
                break;
            case LEX_NOT_EQUALS:
                {
                    operand2 = cash.top();
                    cash.pop();
                    operand1 = cash.top();
                    cash.pop();
                    operand1_value = effective_value(operand1, ident_table);
                    operand2_value = effective_value(operand2, ident_table);

                    switch (effective_type(operand1, ident_table)) {
                    case CONST_INT:
                        to_push = Lex (CONST_BOOL, operand1_value.i != operand2_value.i);
                        break;
                    case CONST_DOUBLE:
                        to_push = Lex (CONST_BOOL, operand1_value.d != operand2_value.d);
                        break;
                    case CONST_STRING:
                        to_push = Lex (CONST_BOOL, string(operand1_value.s) != string(operand2_value.s));
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