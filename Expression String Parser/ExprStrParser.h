#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <functional>
#include <map>
#include <set>
#include <cstring>
#include <cmath>
//#include <chrono>

#include "ExprStrTokenizer.h"

namespace ExprStrParser {

	enum error_codes {
		PARSE_ERROR,
		SOLVE_ERROR
	};

	class Node {
	public:
		Node():value(Token()) {};
		Node(const Token& _val):value(_val) {}
		Token value;
		std::size_t node_count = 0;
		Node** nodes;
		void print(const std::string& prefix, const bool isLeft) const;
	};
	class Tree {
	public:
		Tree() { head = new Node(); }
		void print() const;
		Node* head;
	};

	class Expression {
	private:
		friend class Parser;
		std::function<float()> expr;
		float* x_var = new float(0.0f); //cache x separately (if not pointer something sometimes goes wrong)
		std::map<std::string, float>* other_vars = new std::map<std::string, float>();
		std::function<float()> calcNodes(const Node* node);
		void calcFunc(const Tree* tree);
	public:
		std::map<std::string, float> GetArgs();
		void SetArgs(const float x);
		void SetArgs(const std::string& name, const float value);
		void SetArgs(const std::map<std::string, float>& args);
		float Calculate();
		float Calculate(const float x);
		float Calculate(const std::string& name, const float value);
		float Calculate(const std::map<std::string, float>& args);
	};

	class Parser {
	private:
		Tokenizer tokenizer;
		Tree tree;
		Expression curr_expression;
		Node* rcalcNode(const std::vector<Token>::reverse_iterator& rit_begin, const std::vector<Token>::reverse_iterator& rit_end);
		bool buildTokenTree();
	public:
		Parser() = default;
		void Parse(std::string& str);

		Expression CopyExpression();

		std::map<std::string, float> GetArgs();
		void SetArgs(const float x);
		void SetArgs(const std::string& name, const float value);
		void SetArgs(const std::map<std::string, float>& args);
		float Calculate();
		float Calculate(const float x);
		float Calculate(const std::string& name, const float value);
		float Calculate(const std::map<std::string, float>& args);
	};

}