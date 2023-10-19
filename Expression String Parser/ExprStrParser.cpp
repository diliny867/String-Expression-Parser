#include "ExprStrParser.h"

namespace ExprStrParser { //TODO: Fix comma

#define str_compare(s1, s2) (s1.compare(s2) == 0)

	void Node::print(const std::string& prefix, const bool isLeft) const {
		std::cout << prefix;
		std::cout << (isLeft ? "|-- " : "L-- ");
		//std::cout << this << std::endl;
		std::cout << value << std::endl;

		if (node_count>0) {
			nodes[0]->print(prefix + (isLeft ? "|   " : "    "), true);
			for(std::size_t i=1;i<node_count;i++) {
				nodes[i]->print(prefix + (isLeft ? "|   " : "    "), false);
			}
		}
	}

	void Tree::print() const {
		head->print("", false);
	}


	void Expression::calcFunc(const Tree* tree) {
		try {
			expr = calcNodes(tree->head);
		} catch (error_codes err) {
			expr = []() {return std::nanf(""); };
		}
	}
	
	std::function<float()> Expression::calcNodes(const Node* node) {
		if (node == nullptr) { return nullptr; }
		const Token curr_token = node->value;

		const auto calculated_nodes = new std::function<float()>[node->node_count];

		for(std::size_t i=0;i<node->node_count;i++) {
			calculated_nodes[i] = calcNodes(node->nodes[i]);
		}
		
		switch (curr_token.type) {
		case Token::Number: 
		{
			const float num = std::stof(curr_token.val.data());
			return [=]() {return num; };
		}
		case Token::Identifier:
		{
			if(curr_token.val == "x") {
				return[=]() {
					return *x_var;
				};
			} else {
				const std::string func_name = std::string(curr_token.val.data());
				return [=]() {return (*other_vars)[func_name]; };
			}
		}
		case Token::Plus:
			return [=]() {return calculated_nodes[0]()+calculated_nodes[1](); };
		case Token::Minus:
			return [=]() {return calculated_nodes[0]()-calculated_nodes[1](); };
		case Token::Star:
			return [=]() {return calculated_nodes[0]()*calculated_nodes[1](); };
		case Token::Slash:
			return [=]() {return calculated_nodes[0]()/calculated_nodes[1](); };
		case Token::Caret:
			return [=]() {return pow(calculated_nodes[0](), calculated_nodes[1]()); };
		case Token::ExclamationMark:
			return [=]() {return std::tgamma(calculated_nodes[0]()+1.0f); };
		case Token::Function:
			if (str_compare(curr_token.val,"log")) {
				return [=]() {return std::log(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "sin")) {
				return [=]() {return sin(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "cos")) {
				return [=]() {return cos(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "tan")) {
				return [=]() {return tan(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "arcsin")) {
				return [=]() {return asin(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "arccos")) {
				return [=]() {return acos(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "arctan")) {
				return [=]() {return atan(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "sqrt")) {
				return [=]() {return sqrt(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "ceil")) {
				return [=]() {return ceil(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "floor")) {
				return [=]() {return floor(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "round")) {
				return [=]() {return round(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "abs")) {
				return [=]() {return abs(calculated_nodes[0]()); };
			}
			if (str_compare(curr_token.val, "mod")) {
				return [=]() {return std::fmod(calculated_nodes[0](), calculated_nodes[1]()); };
			}
			if (str_compare(curr_token.val, "logn")) {
				return [=]() {return std::log(calculated_nodes[0]())/std::log(calculated_nodes[1]()); };
			}
			break;
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
						curr_node->node_count = 2;
						curr_node->nodes = new Node*[2];
						curr_node->nodes[0] = rcalcNode(rit+1, rit_end-2);
						curr_node->nodes[1] = rcalcNode(rit_begin+1, rit);
						return curr_node;
					}
				}
				if (level == 0) {
					curr_node = new Node(*(rit_end-1));
					curr_node->node_count = 1;
					curr_node->nodes = new Node*[1];
					curr_node->nodes[0] = rcalcNode(rit_begin, rit_end-1);
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
					curr_node->node_count = 2;
					curr_node->nodes = new Node*[2];
					curr_node->nodes[0] = rcalcNode(rit+1, rit_end);
					curr_node->nodes[1] = rcalcNode(rit_begin, rit);
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
					curr_node->node_count = 2;
					curr_node->nodes = new Node*[2];
					curr_node->nodes[0] = rcalcNode(rit+1, rit_end);
					curr_node->nodes[1] = rcalcNode(rit_begin, rit);
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
					curr_node->node_count = 2;
					curr_node->nodes = new Node*[2];
					curr_node->nodes[0] = rcalcNode(rit+1, rit_end);
					curr_node->nodes[1] = rcalcNode(rit_begin, rit);
					return curr_node;
				}
				break;
			case Token::ExclamationMark:
				if (level == 0) {
					curr_node = new Node(*rit);
					curr_node->node_count = 1;
					curr_node->nodes = new Node*[1];
					curr_node->nodes[0] = rcalcNode(rit+1, rit_end);
					return curr_node;
				}
				break;
			}
		}

		return nullptr;
	}

	void Parser::Parse(std::string& str) {
		tokenizer.Tokenize(str);
		if (buildTokenTree()) {
			curr_expression.calcFunc(&tree);
		}else {
			curr_expression.expr = []() {return std::nanf(""); };
		}
		//tree.print();
	}

	Expression Parser::CopyExpression() {
		return curr_expression;
	}

	std::map<std::string, float> Expression::GetArgs() {
		return *other_vars;
	}
	void Expression::SetArgs(const float x) {
		*x_var = x;
	}
	void Expression::SetArgs(const std::string& name, const float value) {
		(*other_vars)[name] = value;
	}
	void Expression::SetArgs(const std::map<std::string, float>& args) {
		*other_vars = args;
	}
	float Expression::Calculate() {
		return expr();
	}
	float Expression::Calculate(const float x) {
		*x_var = x;
		return expr();
	}
	float Expression::Calculate(const std::string& name, const float value) {
		(*other_vars)[name] = value;
		return expr();
	}
	float Expression::Calculate(const std::map<std::string, float>& args) {
		*other_vars = args;
		return expr();
	}

	std::map<std::string, float> Parser::GetArgs() {
		return curr_expression.GetArgs();
	}
	void Parser::SetArgs(const float x) {
		curr_expression.SetArgs(x);
	}
	void Parser::SetArgs(const std::string& name, const float value) {
		curr_expression.SetArgs(name, value);
	}
	void Parser::SetArgs(const std::map<std::string, float>& args) {
		curr_expression.SetArgs(args);
	}
	float Parser::Calculate() {
		return curr_expression.Calculate();
	}
	float Parser::Calculate(const float x) {
		return curr_expression.Calculate(x);
	}
	float Parser::Calculate(const std::string& name, const float value) {
		return curr_expression.Calculate(name,value);
	}
	float Parser::Calculate(const std::map<std::string, float>& args) {
		return curr_expression.Calculate(args);
	}
}
