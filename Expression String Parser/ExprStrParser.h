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

#include "ExprtStrTokenizer.h"

namespace ExprStrParser { 

	enum error_codes {
		PARSE_ERROR,
		SOLVE_ERROR
	};

	class Node {
	public:
		Node() = default;
		Node(const Token& _val):value(_val) {}
		Token value;
		Node* left = nullptr;
		Node* right = nullptr;
		void print(const std::string& prefix, const bool isLeft) const;
	};
	class Tree {
	public:
		Tree() { head = new Node(); }
		void print() const;
		Node* head;
	};

	class Expression {
	public:
		Expression() = default;
		std::function<float()> expr;
		std::map<std::string, float> func_args;
		float x = 0.0f;//cache x, to call it and assign faster
		std::function<float()> calc_nodes(const Node* node);
		//float calc_nodes(const Node* node);
		void calc_func(const Tree* tree);
	};

	class Parser {
	private:
		Tokenizer tokenizer;
		Tree tree;
		Expression expression{};
		Node* rcalcNode(const std::vector<Token>::reverse_iterator& rit_begin, const std::vector<Token>::reverse_iterator& rit_end);
		bool buildTokenTree();
		void set_func();
	public:
		Parser() = default;
		void parse(std::string& str);
		std::map<std::string, float> get_args();
		void set_args(const float x);
		void set_args(const std::string& name, const float value);
		void set_args(const std::pair<std::string, float>& arg);
		void set_args(const std::map<std::string, float>& args);
		void set_args(const float x, const std::map<std::string, float>& args);
		float calculate()const;
		float calculate(const float x);
		float calculate(const std::string& name, const float value);
		float calculate(const std::pair<std::string, float>& arg);
		float calculate(const std::map<std::string, float>& args);
		float calculate(const float x, const std::map<std::string, float>& args);
	};

}