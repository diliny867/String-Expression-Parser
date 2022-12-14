#include "ExprStrParser.h"

#define pi_str "3.14159265358979323846"
#define e_str  "2.71828182845904523536"

const std::map<std::string, std::string> math_consts {{"pi", pi_str}, {"e", e_str}};

void Node::print(const std::string& prefix, const bool isLeft) const {
	std::cout << prefix;
	std::cout << (isLeft ? "|-- " : "L-- ");
	//std::cout << this << std::endl;
	std::cout << value << std::endl;

	if (left!=nullptr) {
		left->print(prefix + (isLeft ? "|   " : "    "), true);
	}
	if (right!=nullptr) {
		right->print(prefix + (isLeft ? "|   " : "    "), false);
	}
}

void Tree::print() const {
	head->print("", false);
}


void Expression::calc_func(const Tree* tree) {
	expr = [=]() {return calc_nodes(tree->head); };//this is wrong how can i optimize it (make it as already calculated function)? TODO: optimize it
}

/*
std::function<float()>* Expression::calc_nodes(const Node* node) {
	const token curr_token = node->value;

	switch (curr_token.symb) {
	case NUM://NUM and STR cover the case when both left and right nodes are nullptr
		return new std::function<float()>([=]() {return std::stof(curr_token.val); });
		//return[=]() {return std::stof(curr_token.val); };
	case STR:
		if(curr_token.val == "x") {
			return new std::function<float()>([=]() {return x; });
			//return [=]() {return x; };
		} else {
			return new std::function<float()>([=]() {return func_args[curr_token.val]; });
			//return [=]() {return func_args[curr_token.val]; };
		}
	case OP:
		if (node->left == nullptr) {
			return new std::function<float()>([=]() {return (*calc_nodes(node->right))(); });
			//return [=]() {return calc_nodes(right_node)(); };
		}
		if (curr_token.val == "+") {
			return new std::function<float()>([=]() {return (*calc_nodes(node->left))()+(*calc_nodes(node->right))(); });
			//return [=]() {return calc_nodes(left_node)()+calc_nodes(right_node)(); };
		}
		if (curr_token.val == "-") {
			return new std::function<float()>([=]() {return (*calc_nodes(node->left))()-(*calc_nodes(node->right))(); });
			//return [=]() {return calc_nodes(left_node)()-calc_nodes(right_node)(); };
		}
		if (curr_token.val == "*") {
			return new std::function<float()>([=]() {return (*calc_nodes(node->left))()*(*calc_nodes(node->right))(); });
			//return [=]() {return calc_nodes(left_node)()*(calc_nodes(right_node))(); };
		}
		if (curr_token.val == "/") {
			return new std::function<float()>([=]() {return (*calc_nodes(node->left))()/(*calc_nodes(node->right))(); });
			//return [=]() {return calc_nodes(left_node)()/calc_nodes(right_node)(); };
		}
		break;
	case COP: 
		//TODO: this
		break;
	case NUL:
		std::cout<<"NUL"<<std::endl;
		break;
	}

	return {};
}
*/
/*
std::function<float()> Expression::calc_nodes(const Node* node) {//the same but returns not func pointers, but funcs
	const token curr_token = node->value;

	switch (curr_token.symb) {
	case NUM://NUM and STR cover the case when both left and right nodes are nullptr
		//return new std::function<float()>([=]() {return std::stof(curr_token.val); });
		return[=]() {return std::stof(curr_token.val); };
	case STR:
		if(curr_token.val == "x"){
			//return new std::function<float()>([=]() {return x; });
			return[=]() {return x; };
		} else {
			//return new std::function<float()>([=]() {return func_args[curr_token.val]; });
			return [=]() {return func_args[curr_token.val]; };
		}
	case OP:
		if (node->left == nullptr) {
			//return new std::function<float()>([=]() {return (*calc_nodes(node->right))(); });
			return [=]() {return calc_nodes(node->right)(); };
		}
		if (curr_token.val == "+") {
			//return new std::function<float()>([=]() {return (*calc_nodes(node->left))()+(*calc_nodes(node->right))(); });
			return [=]() {return calc_nodes(node->left)()+calc_nodes(node->right)(); };
		}
		if (curr_token.val == "-") {
			//return new std::function<float()>([=]() {return (*calc_nodes(node->left))()-(*calc_nodes(node->right))(); });
			return [=]() {return calc_nodes(node->left)()-calc_nodes(node->right)(); };
		}
		if (curr_token.val == "*") {
			//return new std::function<float()>([=]() {return (*calc_nodes(node->left))()*(*calc_nodes(node->right))(); });
			return [=]() {return calc_nodes(node->left)()*(calc_nodes(node->right))(); };
		}
		if (curr_token.val == "/") {
			//return new std::function<float()>([=]() {return (*calc_nodes(node->left))()/(*calc_nodes(node->right))(); });
			return [=]() {return calc_nodes(node->left)()/calc_nodes(node->right)(); };
		}
		break;
	case COP:
		//TODO: this
		break;
	case NUL:
		std::cout<<"NUL"<<std::endl;
		break;
	}

	return {};
}
*/
float Expression::calc_nodes(const Node* node) {//the same but returns not funcs or pointers, but floats
	const token curr_token = node->value;

	switch (curr_token.symb) {
	case NUM://NUM and STR cover the case when both left and right nodes are nullptr
		return std::stof(curr_token.val);
	case STR:
		if (curr_token.val == "x") {
			return x;
		} else {
			return func_args.at(curr_token.val);
		}
	case OP:
		if (curr_token.val == "+") {
			return calc_nodes(node->left)+calc_nodes(node->right);
		}
		if (curr_token.val == "-") {
			return calc_nodes(node->left)-calc_nodes(node->right);
		}
		if (curr_token.val == "*") {
			return calc_nodes(node->left)*(calc_nodes(node->right));
		}
		if (curr_token.val == "/") {
			return calc_nodes(node->left)/calc_nodes(node->right);
		}
		if(curr_token.val == "^") {
			return pow(calc_nodes(node->left), calc_nodes(node->right));
		}
		break;
	case COP:
		if(curr_token.val == "log") {
			return std::log(calc_nodes(node->right));
		}
		if(curr_token.val == "sin") {
			return sin(calc_nodes(node->right));
		}
		if(curr_token.val == "cos") {
			return cos(calc_nodes(node->right));
		}
		if(curr_token.val == "tan") {
			return tan(calc_nodes(node->right));
		}
		if(curr_token.val == "sqrt") {
			return sqrt(calc_nodes(node->right));
		}
		break;
	case NUL:
		std::cout<<"NUL"<<std::endl;
		break;
	}

	return {};
}


void ExprStrParser::tokenize(std::string& str){// !TODO: parse things like xlog to x and log, not x l o g
	std::stringstream num_ss;
	std::stringstream str_ss;
	float num;
	std::string str_;

	for (auto it = str.begin(); it != str.end(); ++it) {
		if (*it==' ') { continue; }

		//if((lastSymbol != NUM) && *it == '-'){
		//	tokens.push_back(0.0f);
		//	tokens.push_back('-');
		//	lastSymbol = OP;
		//	continue;
		//}`

		if (isdigit(*it) || *it == '.') {
			
			num_ss<<*it;
			if (!str_ss.str().empty()) {
				str_ss>>str_;
				if (!tokens.empty()) {
					if (tokens.back().symb == NUM || tokens.back().symb == STR || tokens.back().val == ")") {
						tokens.push_back(token("*", OP));
					}
				}
				if (cop_set.count(str_)) {
					tokens.push_back(token(str_, COP));
				} else if (math_consts.count(str_)) {
					tokens.push_back(token(math_consts.at(str_), NUM));
				} else {
					for (const auto& ch : str_) { //converts abc to a*b*c
						tokens.push_back(token(std::string(1, ch), STR));
						tokens.push_back(token("*", OP));
					}
					tokens.pop_back();
					//tokens.push_back(token(str_, STR));
				}
				str_ss = std::stringstream();
			}
		} else {
			if (!num_ss.str().empty()) {
				num_ss>>num;
				tokens.push_back(token(std::to_string(num),NUM));
				num_ss = std::stringstream();
				//if (tokens[tokens.size()-2].symb != OP || tokens[tokens.size()-2].val == ")") {
				//	token _token = tokens.back();
				//	tokens.back() = token("*", OP);
				//	tokens.push_back(_token);
				//}
			}
			if(ispunct(*it)){
				if (!str_ss.str().empty()) {
					str_ss>>str_;
					if(!tokens.empty()) {
						if (tokens.back().symb == NUM || tokens.back().symb == STR || tokens.back().val == ")") {
							tokens.push_back(token("*", OP));
						}
					}
					if (cop_set.count(str_)) {
						tokens.push_back(token(str_, COP));
					} else if (math_consts.count(str_)) {
						tokens.push_back(token(math_consts.at(str_), NUM));
					} else {
						std::cout<<str_<<std::endl;
						for (const auto& ch : str_) { //converts abc to a*b*c
							tokens.push_back(token(std::string(1, ch), STR));
							tokens.push_back(token("*", OP));
						}
						tokens.pop_back();
						//tokens.push_back(token(str_, STR));
					}
					str_ss = std::stringstream();
				}
				if (*it == '-') {
					if(tokens.empty() || tokens.back().val == "(") {
						tokens.push_back(token("0", NUM));
					}
				}
				if (*it == '(') {
					if (!tokens.empty()) {
						if (tokens.back().symb == NUM || tokens.back().symb == STR || tokens.back().val == ")") {
							tokens.push_back(token("*", OP));
						}
					}
				}
				tokens.push_back(token(std::string(1, *it), OP));
			} else {
				str_ss<<*it;
				if(math_consts.count(str_ss.str())) {
					str_ss>>str_;
					if (!tokens.empty()) {
						if (tokens.back().symb == NUM || tokens.back().symb == STR || tokens.back().val == ")") {
							tokens.push_back(token("*", OP));
						}
					}
					tokens.push_back(token(math_consts.at(str_ss.str()), NUM));
					str_ss = std::stringstream();
				}
			}
		}
	}
	if (!num_ss.str().empty()) {//last element
		num_ss>>num;
		tokens.push_back(token(std::to_string(num), NUM));
		num_ss = std::stringstream();
	}
	if (!str_ss.str().empty()) {//last element
		str_ss>>str_;
		if (tokens.empty()) {
			tokens.push_back(token(str_, STR));
		} else {
			if (tokens.back().symb == NUM || tokens.back().symb == STR || tokens.back().val == ")") {
				tokens.push_back(token("*", OP));
			}
			if (math_consts.count(str_)) {
				tokens.push_back(token(math_consts.at(str_), NUM));
			} else {
				for (const auto& ch : str_) { //converts abc to a*b*c
					tokens.push_back(token(std::string(1, ch), STR));
					tokens.push_back(token("*", OP));
				}
				tokens.pop_back();
				//tokens.push_back(token(str_, STR));
			}
		}
	}
}

void ExprStrParser::buildTokenTree() {
	//tree.head = calcNode(tokens.begin(), tokens.end());
	tree.head = rcalcNode(tokens.rbegin(), tokens.rend());
}

Node* ExprStrParser::rcalcNode(const std::vector<token>::reverse_iterator& rit_begin, const std::vector<token>::reverse_iterator& rit_end) {//somewhere is a bug with right node creation
	//std::cout<<std::endl;
	if (rit_end-rit_begin == 1) {//leaf (symb == NUM or STR)
		//std::cout<<*it_begin<<std::endl;
		return new Node(*rit_begin);
	}

	//for (auto rit = rit_begin; rit < rit_end; ++rit) {
	//	std::cout << rit->val << " ";
	//}
	//std::cout << std::endl;

	if (rit_begin->val == ")" && (rit_end-1)->val == "(") {//strips expression of side brackets
		int level = 0;
		for (auto rit = rit_begin+1; rit<rit_end-1; ++rit) {//temporary solution(its slow and needs to be checked in next two loops)
			if (rit->val == "(") {
				--level;
				if (level < 0) {
					break;
				}
				continue;
			}
			if (rit->val == ")") {
				++level;
				continue;
			}
		}
		if (level == 0) {
			return rcalcNode(rit_begin + 1, rit_end - 1);
		}
	}

	Node* curr_node;

	if ((rit_end-1)->symb == COP) {//process COP
		if (rit_begin->val == ")" && (rit_end-2)->val == "(") {
			int level = 0;
			for (auto rit = rit_begin+1; rit<rit_end-2; ++rit) {
				if (rit->val == "(") {
					--level;
					if (level < 0) {
						break;
					}
					continue;
				}
				if (rit->val == ")") {
					++level;
					continue;
				}
			}
			if (level == 0) {
				curr_node = new Node(*(rit_end-1));
				curr_node->right = rcalcNode(rit_begin, rit_end-1);
				return curr_node;
			}
		}
	}

	int level = 0;
	for (auto rit = rit_begin; rit<rit_end; ++rit) {
		if (rit->symb == OP) {
			if (rit->val == ")") {
				++level;
				continue;
			}
			if (rit->val == "(") {
				--level;
				continue;
			}
			if (level == 0) {
				if (rit->val == "-" || rit->val == "+") {
					curr_node = new Node(token(rit->val, OP));
					curr_node->left = rcalcNode(rit+1, rit_end);
					curr_node->right = rcalcNode(rit_begin, rit);
					return curr_node;
				}
			}
		}
	}

	for (auto rit = rit_begin; rit<rit_end; ++rit) {
		if (rit->symb == OP) {
			if (rit->val == ")") {
				++level;
				continue;
			}
			if (rit->val == "(") {
				--level;
				continue;
			}
			if (level == 0) {
				//std::cout<<*rit<<std::endl;
				curr_node = new Node(*rit);
				curr_node->left = rcalcNode(rit+1, rit_end);
				curr_node->right = rcalcNode(rit_begin, rit);
				return curr_node;
			}
	
		}
	}

	return nullptr;
}

void ExprStrParser::set_func() {
	expression.calc_func(&tree);
}

void ExprStrParser::parse(std::string& str){
	tokenize(str);
	//for (const auto& token: tokens) {
	//	std::cout<< token.val <<" ";
	//	//std::cout << token.val << " " << token.symb << std::endl;
	//}
	//std::cout<<std::endl;
	buildTokenTree();
	set_func();
	//tree.print();
}

void ExprStrParser::set_args(const float x) {
	expression.x = x;
}
void ExprStrParser::set_args(const std::map<std::string, float>& args) {
	expression.func_args = args;
}
void ExprStrParser::set_args(const float x, const std::map<std::string, float>& args) {
	expression.x = x;
	expression.func_args = args;
}
float ExprStrParser::calculate() const {
	return expression.expr();
}
float ExprStrParser::calculate(const float x) {
	expression.x = x;
	return expression.expr();
}
float ExprStrParser::calculate(const std::map<std::string, float>& args) {
	expression.func_args = args;
	return expression.expr();
}
float ExprStrParser::calculate(const float x, const std::map<std::string, float>& args) {
	expression.x = x;
	expression.func_args = args;
	return expression.expr();
}
