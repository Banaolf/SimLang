#include <any>
#include <cstdint>
#include <cstring>
#include <utility>
#include <cctype>
#include <iostream>
#include <string>
#include <cstdio>
#include <string_view>
#include <cstdarg>
#include <vector>

#define adv chr++; continue;
#define advln line++; chr=1; continue;
#define addtolist(v, t, l, c) TkenList.push_back(Tken(v, t, l, c));
#define not_eof(c) (c = fgetc(f)) != EOF

//The Error Struct
struct Error {
	bool is_active = false;
	std::string msg;
	std::string type;
	int ln;
	int chr;
};
//Global Error
struct Error glberr;
//Check if there is an error active
bool errq() {return glberr.is_active;}
//Request an error
void reqerr(std::string message, std::string type, int line, int character) {
	glberr.is_active = true;
	glberr.msg = message;
	glberr.type = type;
	glberr.ln = line;
	glberr.chr = character;
}
//Mark the current error as not active
void errdl() {glberr.is_active = false;}
//Print the current error
void printerr() {
	std::cout << "[ " << std::move(glberr.type) << " ]: " << std::endl << std::move(glberr.msg) << std::endl << "At line " << std::to_string(std::move(glberr.ln)) << " Character " << std::to_string(std::move(glberr.chr)) << "." << std::endl;
	glberr.is_active = false;
}

//Token type Enum class
enum class Tkty {
	EndOfFile, Identifier, Keyword, Underscore, Boolean, Integer, Float, Equals, Operator, Parenthesis, SquareBrackets, CurlyBrackets, AngleBrackets, String, RawString,
	Ampersand, DollarSign, At, Exclamation, Pipe, Backslash, Colon, SemiColon, Comma, Dot, QuestionMark, Tab, Newline, Caret, Hashtag, Tilde, Percent
};

//Token Class
class Tken {
private:
	std::string contents;
	Tkty ty;
	int ln;
	int chr;
public:
	Tken(char c, Tkty type, int line, int chra) : contents(""), ty(type), ln(line), chr(chra) {this->contents.push_back(c);}
	Tken(std::string val, Tkty type, int line, int chra) : contents(val), ty(type), ln(line), chr(chra) {}
	~Tken() = default;

	//Compare type with token's type
	bool cmp(Tkty Ty) {return this->ty == Ty;}
	//Compare a string with token's string
	bool cmp(std::string s) {return this->contents == s;}
	//Compare a character to the token's character (if it is)
	bool cmp(char c) {return this->contents.size() == 1 && this->contents[0] == c;}

	//Get the Token's contents (copy)
	std::string getCont() {return this->contents;}
	//Get the Token's type (copy)
	Tkty getType() {return this->ty;}
	int getLine() {return this->ln;}
	int getChar() {return this->chr;}

	bool is_statement_ender() {return this->cmp(Tkty::Newline) || this->cmp(Tkty::Tilde);}
};

//Escaping characters
int getEscapedChar(FILE* fp) {
	int c = fgetc(fp);
	if (c == EOF) return EOF;

	switch (c) {//Simple
		case 'n':  return '\n';
		case 't':  return '\t';
		case 'r':  return '\r';
		case '\\': return '\\';
		case '\"': return '\"';
		case '\'': return '\'';
		case 'a':  return '\a';
		case 'b':  return '\b';
		case 'f':  return '\f';
		case 'v':  return '\v';
		//Hex
		case 'x': {
			int val = 0;
			for (int i = 0; i < 2; ++i) {
				int h = fgetc(fp);
				if (h != EOF && isxdigit(h)) {
					val = val * 16 + (isdigit(h) ? h - '0' : tolower(h) - 'a' + 10);
				} else {
					if (h != EOF) ungetc(h, fp);
					break;
				}
			}
			return val;
		}
		//Octal
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7': {
			int val = c - '0';
			for (int i = 0; i < 2; ++i) {
				int o = fgetc(fp);
				if (o != EOF && o >= '0' && o <= '7') {
					val = val * 8 + (o - '0');
				} else {
					if (o != EOF) ungetc(o, fp);
					break;
				}
			}
			return (val & 0xFF);
		}

		default: return c;
	}
}

//The Keyword Array.
std::vector<std::string> Keywords = {
	"be"
};

//Lookup a keyword. O(n)
bool lookupKeyword(std::string_view s) {
	for (std::string child : Keywords) {
		if (s == child) return true;
	}
	return false;
}

//Global Token List
std::vector<Tken> TkenList;

//Check if a string only contains a certain character
bool only_contains(std::string_view s, char cmp) {
	for (char c : s) {
		if (c != cmp) return false;
	}
	return true;
}

//Lex a file into the global Token list
void lex(std::string path) {
	FILE* f = fopen(path.c_str(), "r");
	int c;
	int line = 1;
	int chr = 1;
	while (not_eof(c)) {
		if (c == '/') {
			if ((c = fgetc(f)) == '_') {
				//In here, there is no fgetc in the _ check since not_eof already checks and advances c.
				while (not_eof(c) && c != '_' && (c = fgetc(f)) != '/') {if (c == '\n') {advln} else adv;}
				if (c != EOF)
					c = fgetc(f); //Skip trailing /
			} else ungetc(c, f);
		}
		if (isspace(c)) {
			if (c == '\n') { addtolist('\n', Tkty::Newline, line, chr) advln }
			else if (c == '\t') { addtolist('\t', Tkty::Tab, line, chr) }
		} else if (isalpha(c)) { //IDENTIFIERS
			std::string buffer;
			while (not_eof(c) && (isalnum(c) || c == '_')) {
				buffer.push_back(c);
				adv
			}
			if (lookupKeyword(buffer)) {
				addtolist(buffer, Tkty::Keyword, line, chr)
			} else if (buffer == "true" || buffer == "false") {
				addtolist(buffer, Tkty::Boolean, line, chr)
			} else if (only_contains(buffer, '_')) {
				addtolist(buffer, Tkty::Underscore, line, chr)
			} else {
				addtolist(buffer, Tkty::Identifier, line, chr)
			}
		} else if (isdigit(c)) { //INTEGERS & FLOATS
			std::string buffer;
			bool has_dot = false;
			while (not_eof(c) && (isdigit(c) || c == '.')) {
				if (c == '.' && has_dot) {reqerr("Cannot have multiple points on a number", "SyntaxError", line, chr); return;}
				buffer.push_back(c);
				adv
			}
			if (has_dot) 
				addtolist(buffer, Tkty::Float, line, chr)
			else
				addtolist(buffer, Tkty::Integer, line, chr)
		} else if (c == '\'') { //RAW STRINGS
			std::string buffer;
			c = fgetc(f);
			while (not_eof(c) && c != '\'') {
				if (c == '\n') continue;
				buffer.push_back(c);
				adv
			}
			if (c == EOF) {reqerr("(Raw)Strings cannot end in EOF.", "SyntaxError", line, chr); return;}
			addtolist(buffer, Tkty::RawString, line, chr)
		} else if (c == '"') { //STRINGS
			std::string buffer;
			c = fgetc(f);
			while (not_eof(c) && c != '"') {
				if (c == '\n') {advln}
				if (c == '\\') {c = fgetc(f); c = getEscapedChar(f);}
				buffer.push_back(c);
				adv
			}
			if (c == EOF) {reqerr("(Raw)Strings cannot end in EOF.", "SyntaxError", line, chr); return;}
			addtolist(buffer, Tkty::String, line, chr)
		} else if (strchr("+-*/", c)) { //OPERATOR (+-*/)
			addtolist(c, Tkty::Operator, line, chr)
		} else if (c == '=') { //EQUALS
			addtolist(c, Tkty::Equals, line, chr)
		} else if (strchr("<{([])}>", c)) { //BRACKETS
			if (strchr("{}", c)) addtolist(c, Tkty::CurlyBrackets, line, chr)
			else if (strchr("()", c)) addtolist(c, Tkty::Parenthesis, line, chr)
			else if (strchr("[]", c)) addtolist(c, Tkty::SquareBrackets, line, chr)
			else addtolist(c, Tkty::AngleBrackets, line, chr)
		} else if (strchr("\\!|@#~$%&?^;:,.", c)) { //SPECIAL SIGNS
			switch(c) {
				case '\\': addtolist(c, Tkty::Backslash, line, chr); break;
				case '!': addtolist(c, Tkty::Exclamation, line, chr); break;
				case '|': addtolist(c, Tkty::Pipe, line, chr); break;
				case '@': addtolist(c, Tkty::At, line, chr); break;
				case '#': addtolist(c, Tkty::Hashtag, line, chr); break;
				case '~': addtolist(c, Tkty::Tilde, line, chr); break;
				case '$': addtolist(c, Tkty::DollarSign, line, chr); break;
				case '%': addtolist(c, Tkty::Percent, line, chr); break;
				case '&': addtolist(c, Tkty::Ampersand, line, chr); break;
				case '?': addtolist(c, Tkty::QuestionMark, line, chr); break;
				case '^': addtolist(c, Tkty::Caret, line, chr); break;
				case ';': addtolist(c, Tkty::SemiColon, line, chr); break;
				case ':': addtolist(c, Tkty::Colon, line, chr); break;
				case ',': addtolist(c, Tkty::Comma, line, chr); break;
				case '.': addtolist(c, Tkty::Dot, line, chr); break;
				default: break;
			}
		} else { //UNKNOWN
			reqerr("Unknown character", "SyntaxError", line, chr);
			return;
		}
		adv
	}
	addtolist('\0', Tkty::EndOfFile, line, chr)

	fclose(f);
}

namespace Parser {
	//The type of ASTNodes there are
	enum class NodeType {
		VarSet, VarReassign, VarGet, Body, Identifier, Arguments, FunctionDef, FunctionCall,
		Integer, Float, String, RawString, Boolean, Operation
	};
	//The Node class itself
	class Node {
	private:
		std::string extra;
		NodeType ty;
		std::vector<Node*> Children;
	public:
		Node(NodeType __Ty) : extra(""), ty(__Ty), Children() {}
		~Node() {
			if (this->Children.size() == 0) return;
			for (auto child : this->Children) {
				delete child;
			}
		}
		//Add a child to the Children vector
		void addChild(Node* __ptr_to_node) {
			Children.push_back(__ptr_to_node);
		}
		//Add a string as "extra info"
		void addExtra(std::string s) {this->extra = s;}
		//Set a new type for the node
		void resetType(NodeType newType) {this->ty = newType;}
		//Returns if the Node is an expression node
		/*
		Get a copy of a child at x index
		@param __idx: index of where the object you want to retrieve is
		*/
		Node* getChild(int __idx) {return this->Children[__idx];}
		//Get the extra info
		std::string_view getExtra() {return this->extra;}
	};
	#define whereabouts(current) current->getLine(), current->getChar()
	// Current token index
    int cursor = 0;

    inline Tken* peek(int offset = 0) {
        if (cursor + offset < TkenList.size() && cursor + offset >= 0) {
            return &TkenList.at(cursor + offset);
        }
        return &TkenList.back(); 
    }

    inline Tken* advance() {
        if (cursor < TkenList.size()) return &TkenList[cursor++];
        return &TkenList.back();
    }
	#define addextracontents lit->addExtra(current->getCont());

	Node* parseExpression();
	Node* parsePrimary() {
		Tken* current = peek();
		Node* lit = nullptr;
		
		if (current->cmp(Tkty::Integer)) { //INTEGER (temporally no operations)
			lit = new Node(NodeType::Integer);
			addextracontents
			cursor++;
		} else if (current->cmp(Tkty::String)) { //STRING
			lit = new Node(NodeType::String);
			addextracontents
			cursor++;
		} else if (current->cmp(Tkty::RawString)) {
			lit = new Node(NodeType::RawString); //RAW STRING
			addextracontents
			cursor++;
		} else if (current->cmp(Tkty::Float)) { //FLOAT
			lit = new Node(NodeType::Float);
			addextracontents
			cursor++;
		} else if (current->cmp(Tkty::Boolean)) { //BOOLEAN
			lit = new Node(NodeType::Boolean);
			addextracontents
			cursor++;
		} else if (current->cmp(Tkty::Identifier)) { //IDENTIFIER
			if (current->cmp('(')) { //FUNCTION CALL
				lit = new Node(NodeType::FunctionCall);
				Node* id = new Node(NodeType::Identifier); 
				id->addExtra(current->getCont());
				lit->addChild(id);
				current = advance();
                Node* args = new Node(NodeType::Arguments);
				while (!peek()->cmp(')') && !peek()->cmp(Tkty::EndOfFile)) {
					if (peek()->cmp(Tkty::Newline)) { cursor++; continue; }
                    
                    Node* expr = parseExpression(); //Recursion at its finest
                    if (!expr) { delete lit; delete args; return nullptr; }
                    args->addChild(expr);
				}
				lit->addChild(args);
				if (peek()->cmp(Tkty::EndOfFile)) {
					delete lit;
					reqerr("Argument list cannot end in End Of File", "SyntaxError", whereabouts(current));
					lit = nullptr;
				}
				current = advance(); //Ignore the trailing )
			} else { //VAR GET
				lit = new Node(NodeType::VarGet);
				addextracontents
				cursor++;
			}//Future: method and attribute access with @
		} else
			reqerr("Unexpected expression", "SyntaxError", whereabouts(current));
        return lit; 
	}

	Node* parseTerm() {
		Node* left = parsePrimary();
		if (!left) return left;
		Tken* current = peek();

		while (peek()->cmp('*') || peek()->cmp('/')) {
			std::string oper = advance()->getCont();
			Node* right = parsePrimary();
			if (!right) return nullptr;

			Node* op = new Node(NodeType::Operation);
			op->addExtra(oper);
			op->addChild(left);
			op->addChild(right);
			left = op;
		}

		return left;
	}

	Node* parseAddSub() {
		Node* left = parseTerm();
		if (!left) return left;
		Tken* current = peek();

		while (peek()->cmp('+') || peek()->cmp('-')) {
			std::string oper = advance()->getCont();
			Node* right = parseTerm(); 
			if (!right) return nullptr;

			Node* op = new Node(NodeType::Operation);
			op->addExtra(oper);
			op->addChild(left);
			op->addChild(right);
			left = op;
		}

		return left;
	}

    Node* parseExpression() {return parseAddSub();}

    Node* parseStatement() {
        if (errq()) return nullptr;
        
        Tken* current = peek();
        Node* retnode = new Node(NodeType::Body);

        if (current->cmp(Tkty::Identifier)) {
            Node* idnm = new Node(NodeType::Identifier);
            idnm->addExtra(advance()->getCont()); 
            retnode->addChild(idnm);

            // Case: x = 5
            if (peek()->cmp(Tkty::Equals)) {
                retnode->resetType(NodeType::VarReassign);
                advance(); // consume '='
                Node* expr = parseExpression();
                if (!expr) { delete retnode; return nullptr; }
                retnode->addChild(expr);
            } 
            // Case: x(args)
            else if (peek()->cmp(Tkty::Parenthesis) && peek()->cmp('(')) {
                retnode->resetType(NodeType::FunctionCall);
                advance(); // consume '('
                Node* args = new Node(NodeType::Arguments);
                while (!peek()->cmp(")") && !peek()->cmp(Tkty::EndOfFile)) {
                    if (peek()->cmp(Tkty::Newline)) { cursor++; continue; }
                    
                    Node* expr = parseExpression();
                    if (!expr) { delete retnode; delete args; return nullptr; }
                    args->addChild(expr);
                }
                retnode->addChild(args);
                advance(); // consume ')'
            }
        } 
        else if (current->cmp(Tkty::Keyword)) {
            if (current->getCont() == "be") {
                advance(); // consume 'be'
                
                if (!peek()->cmp(Tkty::Identifier)) {
                    delete retnode; 
                    reqerr("Expected name after 'be'", "SyntaxError", whereabouts(current));
                    return nullptr;
                }

                Node* idnt = new Node(NodeType::Identifier);
                idnt->addExtra(advance()->getCont());
                retnode->addChild(idnt);

                if (peek()->cmp(Tkty::Equals)) {
                    retnode->resetType(NodeType::VarSet);
                    advance(); // consume '='
                    Node* expr = parseExpression();
                    if (!expr) { delete retnode; return nullptr; }
                    retnode->addChild(expr);
                } 
                else if (peek()->cmp(Tkty::Parenthesis) && peek()->cmp('(')) {
                    retnode->resetType(NodeType::FunctionDef);
                    advance();
                    advance();
                }
            }
        }

        if (!peek()->is_statement_ender()) {
            delete retnode;
            reqerr("Missing statement ender", "SyntaxError", whereabouts(peek()));
            return nullptr;
        }
        advance();
        return retnode;
    }

    Node* parseTokenList() {
        Node* head = new Node(NodeType::Body);
        while (cursor < TkenList.size() && !peek()->cmp(Tkty::EndOfFile)) {
            // Fast-skip redundant newlines or comments
            if (peek()->cmp(Tkty::Newline) || peek()->cmp(Tkty::Hashtag)) {
                cursor++;
                continue;
            }
            Node* ret = parseStatement();
            if (!ret) return nullptr;
            head->addChild(ret);
        }
        return head;
    }
}

/* Flaglist */

std::string logged;
void log(std::string __tolog) {
	logged.push_back('\n');
	logged.append(__tolog);
}

/*
The main compiling From the AST.
@parameter head: The head(root) of the AST
@return Compiler exit code.
*/
enum class ValueType {

};
class Value {
private:
	std::any mainval;
};

//uint32_t flags = 0;
std::string target = "out/main.simb";
//Compile the program into Sim Bytecode! (AKA: .simb, .smb)
int compile(Parser::Node* head) {
	if (errq()) return 1;
	FILE* f = fopen(target.c_str(), "w");
	if (!f) return 2;

	return 0;
}

#define VERSION "S1"
int main(int argc, char* argv[]) {
	int ret = 0;
	//Example: simc input.sim -o input (flags)
	if (argc <= 1) {std::cout << "Usage: simc -i input.sim -o output.simb (flags)" << std::endl; ret = 4;}
	if (strcmp(argv[1], "-v")) {std::cout << VERSION << std::endl;ret = 0;}
	else if (strcmp(argv[1], "--flags")) {std::cout << "None yet." << std::endl;ret = 0;}
	else {
		std::string path;
		for (int i = 1; i < argc; i++) {
			char* current = argv[i];
			if (strcmp(current, "-i")) {
				if (i+1 >= argc) {std::cout << "Usage for -i: -i input.sim" << std::endl; ret = 4;}
				path = argv[i+1];
				i++;
			} else if (strcmp(current, "-o")) {
				if (i+1 >= argc) {std::cout << "Usage for -i: -o input.o" << std::endl; ret = 4;}
				target = argv[i+1];
				i++;
			} else {
				std::cout << "Invalid flag" << std::endl;
				ret = 4;
			}
		}
		lex(path);
		Parser::Node* head = Parser::parseTokenList();
		ret = compile(head);
	}
	if (logged.size() > 0) {
		FILE* logging = fopen("latest.log", "w");
		fprintf(logging, "%s", logged.c_str());
		fclose(logging);
	}
	return ret;
}