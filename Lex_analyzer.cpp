#include <string>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <stack>

#pragma once

const int IDENT_TABLE_SIZE = 100;

using namespace std;

string generate_error (int number, const string& text);

enum Type_of_lex {
    LEX_NULL,
    LEX_AND,
    LEX_BEGIN,
    LEX_BOOL,
    LEX_DO,
    LEX_ELSE,
    LEX_END,
    LEX_IF,
    LEX_FALSE,
    LEX_INT,
    LEX_STRING,
    LEX_DOUBLE,
    LEX_NOT,
    LEX_OR,
    LEX_PROGRAM,
    LEX_READ,
    LEX_THEN,
    LEX_TRUE,
    LEX_VAR,
    LEX_WHILE,
    LEX_WRITE,
    LEX_FIN,
    LEX_SEMICOLON,
    LEX_AT,
    LEX_COMMA,
    LEX_COLON,
    LEX_OPENING_BRACKET,
    LEX_CLOSING_BRACKET,
    LEX_EQUALS,
    LEX_EQUALS_EQUALS,
    LEX_NOT_EQUALS,
    LEX_LESS,
    LEX_MORE,
    LEX_PLUS,
    LEX_MINUS,
    LEX_ASTERISK,
    LEX_SLASH,
    LEX_LESS_EQUALS,
    LEX_MORE_EQUALS,
    LEX_CURLY_OPENING_BRACKET,
    LEX_CURLY_CLOSING_BRACKET,
    CONST_BOOL,
    CONST_INT,
    CONST_STRING,
    CONST_DOUBLE,
    LEX_ID,
    LEX_NEG,
    LEX_GOTO,
    LEX_LABEL,
    POLIZ_GOTO,
    POLIZ_LABEL,
    POLIZ_IF,
    POLIZ_READ,
    POLIZ_WRITE
};

const map<Type_of_lex, string> lex_type_to_string = {
    {LEX_NULL,"LEX_NULL"},
    {LEX_AND,"LEX_AND"},
    {LEX_BEGIN,"LEX_BEGIN"},
    {LEX_BOOL,"LEX_BOOL"},
    {LEX_DO,"LEX_DO"},
    {LEX_ELSE,"LEX_ELSE"},
    {LEX_END,"LEX_END"},
    {LEX_IF,"LEX_IF"},
    {LEX_FALSE,"LEX_FALSE"},
    {LEX_INT,"LEX_INT"},
    {LEX_STRING,"LEX_STRING"},
    {LEX_DOUBLE,"LEX_DOUBLE"},
    {LEX_NOT,"LEX_NOT"},
    {LEX_OR,"LEX_OR"},
    {LEX_PROGRAM,"LEX_PROGRAM"},
    {LEX_READ,"LEX_READ"},
    {LEX_THEN,"LEX_THEN"},
    {LEX_TRUE,"LEX_TRUE"},
    {LEX_VAR,"LEX_VAR"},
    {LEX_WHILE,"LEX_WHILE"},
    {LEX_WRITE,"LEX_WRITE"},
    {LEX_FIN,"LEX_FIN"},
    {LEX_SEMICOLON,"LEX_SEMICOLON"},
    {LEX_AT,"LEX_AT"},
    {LEX_COMMA,"LEX_COMMA"},
    {LEX_COLON,"LEX_COLON"},
    {LEX_OPENING_BRACKET,"LEX_OPENING_BRACKET"},
    {LEX_CLOSING_BRACKET,"LEX_CLOSING_BRACKET"},
    {LEX_EQUALS,"LEX_EQUALS"},
    {LEX_EQUALS_EQUALS,"LEX_EQUALS_EQUALS"},
    {LEX_NOT_EQUALS,"LEX_NOT_EQUALS"},
    {LEX_LESS,"LEX_LESS"},
    {LEX_MORE,"LEX_MORE"},
    {LEX_PLUS,"LEX_PLUS"},
    {LEX_MINUS,"LEX_MINUS"},
    {LEX_ASTERISK,"LEX_ASTERISK"},
    {LEX_SLASH,"LEX_SLASH"},
    {LEX_LESS_EQUALS,"LEX_LESS_EQUALS"},
    {LEX_MORE_EQUALS,"LEX_MORE_EQUALS"},
    {LEX_CURLY_OPENING_BRACKET,"LEX_CURLY_OPENING_BRACKET"},
    {LEX_CURLY_CLOSING_BRACKET,"LEX_CURLY_CLOSING_BRACKET"},
    {CONST_BOOL,"CONST_BOOL"},
    {CONST_INT,"CONST_INT"},
    {CONST_STRING,"CONST_STRING"},
    {CONST_DOUBLE,"CONST_DOUBLE"},
    {LEX_ID,"LEX_ID"},
    {LEX_NEG,"LEX_NEG"},
    {LEX_GOTO,"LEX_GOTO"},
    {LEX_LABEL,"LEX_LABEL"},
    {POLIZ_GOTO,"POLIZ_GOTO"},
    {POLIZ_LABEL,"POLIZ_LABEL"},
    {POLIZ_IF,"POLIZ_IF"},
    {POLIZ_READ,"POLIZ_READ"},
    {POLIZ_WRITE,"POLIZ_WRITE"}
};

string get_lex_type_string (Type_of_lex lex) {
    for (auto it : lex_type_to_string)
        if (it.first == lex) return it.second;
    return "Lex not found";
}

struct Const {
    int i;
    string s;
    bool b;
    double d;
};

class Lex {
    Type_of_lex type;
    Const value;

public:
    Lex (Type_of_lex t = LEX_NULL, int v = 0):
        type (t),
        value ()
    {
        value.i = v;
    }

    Lex (Type_of_lex t, const string& v):
        type (t)
    {
        value.s = v;
    }

    Lex (Type_of_lex t, double d):
        type (t),
        value ()
    {
        value.d = d;
    }

    Lex (Type_of_lex t, bool b):
        type (t),
        value ()
    {
        value.b = b;
    }

    Lex (const Lex& lex):
        type(lex.get_type()),
        value(lex.get_value_const())
    {}

    ~Lex () {}

    Lex operator = (const Lex& lex) {
        type = lex.get_type();
        value = lex.get_value_const();
        return *this;
    }

    void set_value (Lex lex) {
        type = lex.get_type();
        value = lex.get_value_const();
    }

    void set_type (Type_of_lex t) { type = t; }
    Type_of_lex get_type () const { return type; }
    bool get_value_bool () const { return value.b; }
    int get_value_int () const { return value.i; }
    string get_value_string () const { return string (value.s); }
    double get_value_double () const { return value.d; }
    const Const& get_value_const () const { return value; }
    void set_value (int v) { value.i = v; }
    void set_value (string v) { value.s = v; }
    void set_value (bool v) { value.b = v; }
    void set_value (double v) { value.d = v; }
};

class Ident : public Lex {
    string name;
    bool declared, assigned;

public:
    Ident (const string& new_name):
        Lex (LEX_NULL, 0),
        name (new_name)
    {
        declared = false;
        assigned = false;
    }

    string get_name () {
        return name;
    }

    void set_name (const string& new_name) {
        name = new_name;
    }

    bool get_declared () const { return declared; }
    void set_declared () { declared = true; }
    bool get_assigned () const { return assigned; }
    void set_assigned () { assigned = true; }
};

class Ident_table {
    map <string, Ident> contents;

public:
    Ident_table () {
        contents.clear();
    }

    ~Ident_table () {}

    Ident & put (const string& name) {
        if (contents.find(name) != contents.end()) return contents.find(name)->second;

        contents.insert (pair<string, Ident> (name, Ident (name)));
    }

    Ident & get (const string& name) {
        return contents.find(name)->second;

        throw runtime_error ("Identifier wasn't found in ident table.");
    }
};

///===================
/// Lex_analyzer
///===================

class Lex_analyzer {
    enum state {H, IDENTIFIER, INTEGER, REAL, STRING, DELIMITER, LONG_DELIMITER, LABEL};
public:
    static const map<string, Type_of_lex> words_labels;
    static const map<string, Type_of_lex> delimiters_labels;
    Ident_table ident_table;
private:
    state current_state;
    string program_code;
    int current_symbol, current_line;
    string buf;

    Type_of_lex look_for (const string& str, const map<string, Type_of_lex>& where) {
    for (auto it : where)
        if (str == it.first)
            return it.second;
    return LEX_NULL;
}

    char symbol () {
        return program_code[current_symbol];
    }

public:
    Lex_analyzer (const string& p):
        ident_table ()
    {
        program_code = p;
        current_symbol = -1; //= 0 after first current_symbol++
        current_line = 1;
        buf.clear();
    }

    Ident_table & get_ident_table () {
        return ident_table;
    }

    int get_line () {
        return current_line;
    }

    Type_of_lex get_id_type (Lex l) {
        if (l.get_type() != LEX_ID) {
            return LEX_NULL;
        }
        else {
            return ident_table.get(l.get_value_string()).get_type();
        }
    }

    Lex get_lex () {
        current_state = H;
        Type_of_lex type_buf;
        buf.clear();
        while (true) {
            current_symbol++;
            if (current_symbol > program_code.size())
                return Lex (LEX_FIN);

            switch (current_state) {
            case H:
                if (symbol() != ' ' && symbol() != '\r' && symbol() != '\t') {
                    if (symbol () == '\n') {
                        current_line++;
                    }
                    else if (isalpha(program_code[current_symbol])) {
                        buf.push_back(symbol());
                        current_state = IDENTIFIER;
                    }
                    else if (isdigit(program_code[current_symbol])) {
                        buf.push_back(symbol());
                        current_state = INTEGER;
                    }
                    else if (symbol() == '"') {
                        current_state = STRING;
                    }
                    else if (symbol() == '@') {
                        current_state = LABEL;
                    }
                    else {
                        buf.push_back(symbol());
                        current_state = DELIMITER;
                    }
                }
                break;
            case IDENTIFIER:
                if (isalnum(symbol())) {
                    buf.push_back(symbol());
                }
                else {
                    current_symbol--;
                    if ((type_buf = look_for (buf, words_labels)) != LEX_NULL) {
                        return Lex (type_buf);
                    }
                    else {
                        ident_table.put(buf);
                        return Lex (LEX_ID, buf);
                    }
                }
                break;
            case INTEGER:
                if (isdigit (symbol())) {
                    buf.push_back(symbol());
                }
                else if (symbol() == '.') {
                    buf.push_back(symbol());
                    current_state = REAL;
                }
                else {
                    current_symbol--;
                    return Lex (CONST_INT, atoi(buf.c_str()));
                }
                break;
            case REAL:
                if (isdigit (symbol())) {
                    buf.push_back(symbol());
                }
                else {
                    current_symbol--;
                    return Lex (CONST_DOUBLE, atof(buf.c_str()));
                }
                break;
            case STRING:
                if (symbol() == '\0') {
                    throw runtime_error (generate_error(get_line(), "Missing closing quotation mark."));
                }
                else if (symbol() == '"'){
                    return Lex (CONST_STRING, buf);
                }
                else {
                    buf.push_back(symbol());
                }
                break;
            case DELIMITER:
                if (symbol() != '=') {
                    current_symbol--;
                    if ((type_buf = look_for (buf, delimiters_labels)) != LEX_NULL) {
                        return type_buf;
                    }
                    else {
                        cout << "1" << buf << "\n";
                        throw runtime_error (generate_error(get_line(), "Invalid delimiter."));
                    }
                }
                else {
                    buf.push_back(symbol());
                    if ((type_buf = look_for (buf, delimiters_labels)) != LEX_NULL) {
                        return type_buf;
                    }
                    else {
                        cout << "2" << buf << "\n";
                        throw runtime_error (generate_error(get_line(), "Invalid delimiter."));
                    }
                }
                break;
            case LABEL:
                if (isalnum(symbol())) {
                    buf.push_back(symbol());
                }
                else {
                    current_symbol--;
                    return Lex (LEX_LABEL, buf);
                }
                break;
            }
        }

        return LEX_NULL;
    }
};

const map<string, Type_of_lex> Lex_analyzer::words_labels = {
    {"and", LEX_AND},
    {"begin", LEX_BEGIN},
    {"bool", LEX_BOOL},
    {"do", LEX_DO},
    {"else", LEX_ELSE},
    {"end", LEX_END},
    {"if", LEX_IF},
    {"false", LEX_FALSE},
    {"int", LEX_INT},
    {"not", LEX_NOT},
    {"or", LEX_OR},
    {"program", LEX_PROGRAM},
    {"read", LEX_READ},
    {"then", LEX_THEN},
    {"true", LEX_TRUE},
    {"var", LEX_VAR},
    {"while", LEX_WHILE},
    {"write", LEX_WRITE},
};

const map<string, Type_of_lex> Lex_analyzer::delimiters_labels = {
    {";", LEX_SEMICOLON},
    {"@", LEX_AT},
    {",", LEX_COMMA},
    {":", LEX_COLON},
    {"=", LEX_EQUALS},
    {"(", LEX_OPENING_BRACKET},
    {")", LEX_CLOSING_BRACKET},
    {"==", LEX_EQUALS_EQUALS},
    {"!=", LEX_NOT_EQUALS},
    {"<", LEX_LESS},
    {">", LEX_MORE},
    {"+", LEX_PLUS},
    {"-", LEX_MINUS},
    {"*", LEX_ASTERISK},
    {"/", LEX_SLASH},
    {"<=", LEX_LESS_EQUALS},
    {">=", LEX_MORE_EQUALS},
    {"{", LEX_CURLY_OPENING_BRACKET},
    {"}", LEX_CURLY_CLOSING_BRACKET},
};