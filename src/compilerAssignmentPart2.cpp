//============================================================================
// Name        : compilerAssignmentPart2.cpp
// Author      : Kyle Welsh
// Version     : N/A
// Copyright   : N/A
// Description : The syntax analyzer portion of the compiler assignment
//============================================================================

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

class SyntaxAnalyzer{
    private:
        vector<string> lexemes;
        vector<string> tokens;
        vector<string>::iterator lexitr;
        vector<string>::iterator tokitr;

        // other private methods
        bool vdec();
        int vars();
        bool stmtlist();
        int stmt();
        bool ifstmt();
        bool elsepart();
        bool whilestmt();
        bool assignstmt();
        bool inputstmt();
        bool outputstmt();
        bool expr();
        bool simpleexpr();
        bool term();
        bool logicop();
        bool arithop();
        bool relop();
        std::istream& getline_safe(std::istream& input, std::string& output);
    public:
        SyntaxAnalyzer(istream& infile);
        // pre: 1st parameter consists of an open file containing a source code's
        //	valid scanner output.  This data must be in the form
        //			token : lexeme
        // post: the vectors have been populated

        bool parse();
        // pre: none
        // post: The lexemes/tokens have been parsed.
        // If an error occurs, a message prints indicating the token/lexeme pair
        // that caused the error.  If no error, data is loaded in vectors
};
SyntaxAnalyzer::SyntaxAnalyzer(istream& infile){
    string line, tok, lex;
    int pos;
    getline_safe(infile, line);
    bool valid = true;
    while(!infile.eof() && (valid)){
        pos = line.find(":");
        tok = line.substr(0, pos);
        lex = line.substr(pos+1, line.length());
        cout << pos << " " << tok << " " << lex << endl;
        tokens.push_back(tok);
        lexemes.push_back(lex);
        getline_safe(infile, line);
    }
    tokitr = tokens.begin();
    lexitr = lexemes.begin();
}

bool SyntaxAnalyzer::parse(){
    if (vdec()){
        if (tokitr!=tokens.end() && *tokitr=="t_main"){
            tokitr++; lexitr++;
            if (tokitr!=tokens.end() && stmtlist()){
            	if (tokitr!=tokens.end()) // should be at end token
                	if (*tokitr == "t_end"){
                		tokitr++; lexitr++;
                		if (tokitr==tokens.end()){  // end was last thing in file
                			cout << "Valid source code file" << endl;
                			return true;
                		}
                		else{
                			cout << "end came too early" << endl;
                		}
                	}
                	else{
                		cout << "invalid statement ending code" << endl;
                }
                else{
                	cout << "no end" << endl;
                }
            }
            else{
            	cout << "bad/no stmtlist" << endl;
            }
        }
        else{
        	cout << "no main" << endl;
        }
    }
    else{
    	cout << "bad var list" << endl;
    }
    return false;

}

bool SyntaxAnalyzer::vdec(){

    if (tokitr!=tokens.end() && *tokitr != "t_var")	// KYLE WELSH (Adding end check)
        return true;
    else{
        tokitr++; lexitr++;
        int result = 0;   // 0 - valid, 1 - done, 2 - error
        result = vars();
        if (result == 2)
            return false;
        while (result == 0){
            if (tokitr!=tokens.end())
                result = vars(); // parse vars
        }

        if (result == 1)
            return true;
        else
            return false;
    }
}

int SyntaxAnalyzer::vars(){
    int result = 0;  // 0 - valid, 1 - done, 2 - error
    string temp;
    if (tokitr!=tokens.end() && *tokitr == "t_integer"){ // KYLE WELSH (Adding end check)
        temp = "t_integer";
        tokitr++; lexitr++;
    }
    else if (tokitr!=tokens.end() && *tokitr == "t_string"){ // KYLE WELSH (Adding end check)
        temp = "t_string";
        tokitr++; lexitr++;
    }
    else
        return 1;	// LOOK INTO THIS MORE / END CHECK?
    bool semihit = false;
    while (tokitr != tokens.end() && result == 0 && !semihit){
        if (*tokitr == "t_id"){
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_comma"){
                tokitr++; lexitr++;
            }
            else if (tokitr != tokens.end() && *tokitr == "s_semi"){
                semihit = true;
                tokitr++; lexitr++;
            }
            else
                result = 2;
        }
        else{
            result = 2;
        }
    }
    return result;
}

bool SyntaxAnalyzer::stmtlist(){
    int result = stmt();

    while (result == 1){
    	result = stmt();
    }
    if (result == 0)
        return false;
    else
        return true;
}
int SyntaxAnalyzer::stmt(){  // returns 1 or 2 if valid, 0 if invalid
	if (tokitr!=tokens.end() && *tokitr == "t_if"){	// KYLE WELSH (Adding end check)
        tokitr++; lexitr++;
        if (ifstmt()) return 1;
        else return 0;
    }	// WONT GO INTO ANY STATEMENTS IF ITS AT THE END AND THEN RETURNS 2 ??
    else if (tokitr!=tokens.end() && *tokitr == "t_while"){	// KYLE WELSH
        tokitr++; lexitr++;
        if (whilestmt()) return 1;
        else return 0;
    }
    else if (tokitr!=tokens.end() && *tokitr == "t_id"){  // assignment starts with identifier // KYLE WELSH
        tokitr++; lexitr++;
        cout << "t_id" << endl;
        if (assignstmt()) return 1;
        else return 0;
    }
    else if (tokitr!=tokens.end() && *tokitr == "t_input"){	// KYLE WELSH
        tokitr++; lexitr++;
        if (inputstmt()) return 1;
        else return 0;
    }
    else if (tokitr!=tokens.end() && *tokitr == "t_output"){ // KYLE WELSH
        tokitr++; lexitr++;
        cout << "t_output" << endl;
        if (outputstmt()) return 1;
        else return 0;
    }
    return 2;  //stmtlist can be null
}

bool SyntaxAnalyzer::ifstmt(){ // CLASS WRITTEN
	if (tokitr == tokens.end())
		return false;
	if (*tokitr != "s_lparen")
		return false;
	tokitr++; lexitr++;
	if (!expr())
		return false;
	if (tokitr == tokens.end())
		return false;
	if (*tokitr != "s_rparen")
		return false;
	tokitr++; lexitr++;
	if (tokitr == tokens.end())
		return false;
	if (*tokitr != "t_then")
		return false;
	tokitr++; lexitr++;
	if (!stmtlist())
		return false;
	if (!elsepart())
		return false;
	if (tokitr == tokens.end())
		return false;
	if (*tokitr != "t_end")
		return false;
	tokitr++; lexitr++;
	if (tokitr == tokens.end())
		return false;
	if (*tokitr != "t_if")
		return false;
	tokitr++; lexitr++;
	return true;
}

bool SyntaxAnalyzer::elsepart(){
    if (*tokitr == "t_else"){
        tokitr++; lexitr++;
        if (stmtlist())
            return true;
        else
            return false;
    }
    return true;   // elsepart can be null
}

bool SyntaxAnalyzer::whilestmt(){ // KYLE WELSH
	if (tokitr == tokens.end() || *tokitr != "s_lparen")
		return false;
	tokitr++; lexitr++;
	if (!expr())
		return false;
	tokitr++; lexitr++;
	if (tokitr == tokens.end() || *tokitr != "s_rparen")
		return false;
	tokitr++; lexitr++;
	if (tokitr == tokens.end() || *tokitr != "t_loop")
		return false;
	tokitr++; lexitr++;
	if(!stmtlist())
		return false;
	tokitr++; lexitr++;
	if (tokitr == tokens.end() || *tokitr != "t_end")
			return false;
	tokitr++; lexitr++;
	if (tokitr == tokens.end() || *tokitr != "t_loop")
		return false;
	tokitr++; lexitr++;
	return true;
}

bool SyntaxAnalyzer::assignstmt(){ // KYLE WELSH
	if (tokitr == tokens.end() || *tokitr != "s_assign")
		return false;
	tokitr++; lexitr++;
	if(!expr())
		return false;
	tokitr++; lexitr++;
	if (tokitr == tokens.end() || *tokitr != "s_semi")
		return false;
	tokitr++; lexitr++;
	return true;
}
bool SyntaxAnalyzer::inputstmt(){
    if (*tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if (*tokitr == "t_id"){
            tokitr++; lexitr++;
            if (*tokitr == "s_rparen"){
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    return false;
}

bool SyntaxAnalyzer::outputstmt(){ // KYLE WELSH
	if (tokitr == tokens.end() || *tokitr != "s_lparen")
		return false;
	tokitr++; lexitr++;
	if (tokitr == tokens.end() || (!expr() && *tokitr != "t_string"))
		return false;
	tokitr++; lexitr++;
	if (tokitr == tokens.end() || *tokitr != "s_rparen")
		return false;
	tokitr++; lexitr++;
	return true;
}

bool SyntaxAnalyzer::expr(){
    if (simpleexpr()){
	if (logicop()){
		if (simpleexpr())
			return true;
		else
			return false;
	}
	else
		return true;
    }
    else{
	return false;
    }
}

bool SyntaxAnalyzer::simpleexpr(){ // KYLE WELSH
	if (!term())
		return false;
	tokitr++; lexitr++;
	if (arithop() || relop()){
		tokitr++; lexitr++;
		if(!term())
			return false;
	}
	else if (tokitr == tokens.end())
		return false;
	else
		tokitr++; lexitr++;
		return true;
}

bool SyntaxAnalyzer::term(){
    if ((*tokitr == "t_int")
	|| (*tokitr == "t_str")
	|| (*tokitr == "t_id")){
    	tokitr++; lexitr++;
    	return true;
    }
    else
        if (*tokitr == "s_lparen"){
            tokitr++; lexitr++;
            if (expr())
                if (*tokitr == "s_rparen"){
                    tokitr++; lexitr++;
                    return true;
                }
        }
    return false;
}

bool SyntaxAnalyzer::logicop(){
    if ((*tokitr == "s_and") || (*tokitr == "s_or")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}

bool SyntaxAnalyzer::arithop(){
    if ((*tokitr == "s_mult") || (*tokitr == "s_plus") || (*tokitr == "s_minus")
        || (*tokitr == "s_div")	|| (*tokitr == "s_mod")){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}

bool SyntaxAnalyzer::relop(){
    if ((*tokitr == "s_lt") || (*tokitr == "s_gt") || (*tokitr == "s_ge")
        || (*tokitr == "s_eq") || (*tokitr == "s_ne") || (*tokitr == "s_le")){
        tokitr++; lexitr++;
        return true;
    }
    else
    	return false;
}
std::istream& SyntaxAnalyzer::getline_safe(std::istream& input, std::string& output)
{
    char c;
    output.clear();

    input.get(c);
    while (input && c != '\n')
    {
        if (c != '\r' || input.peek() != '\n')
        {
            output += c;
        }
        input.get(c);
    }

    return input;
}

int main(){
    ifstream infile("codelexemes.txt");
    if (!infile){
    	cout << "error opening lexemes.txt file" << endl;
        exit(-1);
    }
    SyntaxAnalyzer sa(infile);
    sa.parse();
    return 1;
}