#include "ExprStrParser.h"

namespace ExprStrParser { //TODO: Fix comma

#define str_compare(s1, s2) (s1.compare(s2) == 0)

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
		try {
			expr = calc_nodes(tree->head);
		} catch (error_codes err) {
			expr = []() {return std::nanf(""); };
		}
	}

	std::function<float()> Expression::calc_nodes(const Node* node) {
		if (node == nullptr) { return nullptr; }
		const Token curr_token = node->value;

		const auto left = calc_nodes(node->left);
		const auto right = calc_nodes(node->right);

		switch (curr_token.type) {
		case Token::Number: 
		{
			const float num = std::stof(curr_token.val.data());
			return [=]() {return num; };
		}
		case Token::Identifier:
		{
			if(curr_token.val == "x") {
				return[=]() {return x; };
			} else {
				const std::string func_name = std::string(curr_token.val.data());
				return [=]() {return func_args[func_name]; };
			}
		}
		case Token::Plus:
			return [=]() {return left()+right(); };
		case Token::Minus:
			return [=]() {return left()-right(); };
		case Token::Star:
			return [=]() {return left()*right(); };
		case Token::Slash:
			return [=]() {return left()/right(); };
		case Token::Caret:
			return [=]() {return pow(left(), right()); };
		case Token::ExclamationMark:
			return [=]() {return std::tgamma(left()+1.0f); };
		case Token::Function:
			if (str_compare(curr_token.val,"log")) {
				return [=]() {return std::log(left()); };
			}
			if (str_compare(curr_token.val, "sin")) {
				return [=]() {return sin(left()); };
			}
			if (str_compare(curr_token.val, "cos")) {
				return [=]() {return cos(left()); };
			}
			if (str_compare(curr_token.val, "tan")) {
				return [=]() {return tan(left()); };
			}
			if (str_compare(curr_token.val, "arcsin")) {
				return [=]() {return asin(left()); };
			}
			if (str_compare(curr_token.val, "arccos")) {
				return [=]() {return acos(left()); };
			}
			if (str_compare(curr_token.val, "arctan")) {
				return [=]() {return atan(left()); };
			}
			if (str_compare(curr_token.val, "sqrt")) {
				return [=]() {return sqrt(left()); };
			}
			if (str_compare(curr_token.val, "ceil")) {
				return [=]() {return ceil(left()); };
			}
			if (str_compare(curr_token.val, "floor")) {
				return [=]() {return floor(left()); };
			}
			if (str_compare(curr_token.val, "round")) {
				return [=]() {return round(left()); };
			}
			if (str_compare(curr_token.val, "abs")) {
				return [=]() {return abs(left()); };
			}
			if (str_compare(curr_token.val, "mod")) {
				return [=]() {return std::fmod(left(), right()); };
			}
			if (str_compare(curr_token.val, "logn")) {
				return [=]() {return std::log(left())/std::log(right()); };
			}
			break;
		case Token::NUL:
			return []() {return 0.0f; };
		default:
			return []() {return 0.0f; };
		}
		return []() {return 0.0f; };
	}

	bool Parser::buildTokenTree() {
		try {
			tree.head = rcalcNode(tokenizer.tokens.rbegin(), tokenizer.tokens.rend());
			tokenizer.tokens.clear();
			return true;
		} catch (error_codes err) {
			tokenizer.tokens.clear();
			return false;
			//std::cout<<"ERROR: PARSE ERROR"<<std::endl;
		}
	}

	Node* Parser::rcalcNode(const std::vector<Token>::reverse_iterator& rit_begin, const std::vector<Token>::reverse_iterator& rit_end) {
		if (rit_end-rit_begin <= 0) {
			throw PARSE_ERROR;
		}
		if (rit_end-rit_begin == 1) {//leaf
			return new Node(*rit_begin);
		}

		if (rit_begin->type == Token::CloseParenthesis && (rit_end-1)->type == Token::OpenParenthesis) {//strips expression of side brackets
			int level = 0;
			for (auto rit = rit_begin+1; rit<rit_end-1; ++rit) {
				if (rit->type == Token::OpenParenthesis) {
					--level;
					if (level < 0) {
						break;
					}
					continue;
				}
				if (rit->type == Token::CloseParenthesis) {
					++level;
					continue;
				}
			}
			if (level == 0) {
				return rcalcNode(rit_begin + 1, rit_end - 1);
			}
		}

		Node* curr_node;

		if ((rit_end-1)->type == Token::Function) {//process COP
			if (rit_begin->type == Token::CloseParenthesis && (rit_end-2)->type == Token::OpenParenthesis) {
				int level = 0;
				for (auto rit = rit_begin+1; rit<rit_end-2; ++rit) {
					if (rit->type == Token::OpenParenthesis){
						--level;
						if (level < 0) {
							break;
						}
						continue;
					}
					if (rit->type == Token::CloseParenthesis) {
						++level;
						continue;
					}
					if (rit->type == Token::Comma && level == 0) {
						curr_node = new Node(*(rit_end-1));
						curr_node->left = rcalcNode(rit+1, rit_end-2);
						curr_node->right = rcalcNode(rit_begin+1, rit);
						return curr_node;
					}
				}
				if (level == 0) {
					curr_node = new Node(*(rit_end-1));
					curr_node->left = rcalcNode(rit_begin, rit_end-1);
					return curr_node;
				}
			}
		}

		int level = 0;
		for (auto rit = rit_begin; rit<rit_end; ++rit) {
			switch(rit->type) {
			case Token::CloseParenthesis:
				++level;
				break;
			case Token::OpenParenthesis:
				--level;
				break;
			case Token::Minus:
			case Token::Plus:
				if (level == 0) {
					curr_node = new Node(Token(*rit));
					curr_node->left = rcalcNode(rit+1, rit_end);
					curr_node->right = rcalcNode(rit_begin, rit);
					return curr_node;
				}
				break;
			}
		}

		level = 0;
		for (auto rit = rit_begin; rit<rit_end; ++rit) {
			switch(rit->type) {
			case Token::CloseParenthesis:
				++level;
				break;
			case Token::OpenParenthesis:
				--level;
				break;
			case Token::Star:
			case Token::Slash:
				if (level == 0) {
					curr_node = new Node(*rit);
					curr_node->left = rcalcNode(rit+1, rit_end);
					curr_node->right = rcalcNode(rit_begin, rit);
					return curr_node;
				}
				break;
			}
		}

		level = 0;
		for (auto rit = rit_begin; rit<rit_end; ++rit) {// ^ and ! need to be processed separately, because they should affect only the closest token(s)
			switch(rit->type) {
			case Token::CloseParenthesis:
				++level;
				break;
			case Token::OpenParenthesis:
				--level;
				break;
			case Token::Caret:
				if (level == 0) {
					curr_node = new Node(*rit);
					curr_node->left = rcalcNode(rit+1, rit_end);
					curr_node->right = rcalcNode(rit_begin, rit);
					return curr_node;
				}
				break;
			case Token::ExclamationMark:
				if (level == 0) {
					curr_node = new Node(*rit);
					curr_node->left = rcalcNode(rit+1, rit_end);
					return curr_node;
				}
				break;
			}
		}

		return nullptr;
	}

	void Parser::set_func() {
		expression.calc_func(&tree);
	}

	void Parser::parse(std::string& str) {
		expression.func_args.clear();
		tokenizer.Tokenize(str);
		if (buildTokenTree()) {
			set_func();
		}else {
			expression.expr = []() {return std::nanf(""); };
		}
		//tree.print();
	}

	std::map<std::string, float> Parser::get_args() {
		return expression.func_args;
	}

	void Parser::set_args(const float x) {
		expression.x = x;
	}
	void Parser::set_args(const std::string& name, const float value) {
		expression.func_args[name] = value;
	}
	void Parser::set_args(const std::pair<std::string, float>& arg) {
		expression.func_args[arg.first] = arg.second;
	}
	void Parser::set_args(const std::map<std::string, float>& args) {
		expression.func_args = args;
	}
	void Parser::set_args(const float x, const std::map<std::string, float>& args) {
		expression.x = x;
		expression.func_args = args;
	}
	float Parser::calculate() const {
		return expression.expr();
	}
	float Parser::calculate(const float x) {
		expression.x = x;
		return expression.expr();
	}
	float Parser::calculate(const std::string& name, const float value) {
		expression.func_args[name] = value;
		return expression.expr();
	}
	float Parser::calculate(const std::pair<std::string, float>& arg) {
		expression.func_args[arg.first] = arg.second;
		return expression.expr();
	}
	float Parser::calculate(const std::map<std::string, float>& args) {
		expression.func_args = args;
		return expression.expr();
	}
	float Parser::calculate(const float x, const std::map<std::string, float>& args) {
		expression.x = x;
		expression.func_args = args;
		return expression.expr();
	}

}