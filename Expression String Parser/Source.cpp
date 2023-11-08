#include <chrono>
#include <iostream>
#include <iomanip>  // << std::setprecision(10)

#include "ExprStrParser.h"


int main() {
	ExprStrParser::Parser parser;
	//std::string expr = "-0.17*(25.215x*(-2+20)/(9-1))-log(25)^6+4!+mod(x,1.1)";
	std::string expr = "a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a";
	//std::string expr = "cos(x)^2 + log(x/3) * round(4.5*(sin(x)))";

	//std::map<std::string, double> args{{"x", 2.0f} };

	auto start = std::chrono::high_resolution_clock::now();
	parser.Parse(expr);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Time taken to parse and set func: "<< static_cast<double>(duration.count())/1000.0 << " milliseconds" << std::endl;

	start = std::chrono::high_resolution_clock::now();
	auto expression = parser.CopyExpression();
	stop = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Time taken to copy expression: "<< static_cast<double>(duration.count())/1000.0 << " milliseconds" << std::endl;

	std::cout << "Answer: " << std::fixed << std::setprecision(10) << expression.Calculate(3.0) << std::defaultfloat << std::endl;

	constexpr std::size_t calc_count = 100000;
	start = std::chrono::high_resolution_clock::now();
	for (std::size_t i = 0; i<calc_count; i++) {
		expression.Calculate(3.0);
	}
	stop = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Time taken to calculate "<<calc_count<< " times: "<< static_cast<double>(duration.count())/1000.0 << " milliseconds" << std::endl;
	return 0;
}