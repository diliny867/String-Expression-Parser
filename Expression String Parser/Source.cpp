#include <chrono>
#include <iomanip>  // << std::setprecision(10)

#include "ExprStrParser.h"


int main() {
	ExprStrParser parser;
	std::string expr = "-0.17*(25.215x*(-2+20)/(9-1))-log(25)^6";
	//std::string expr = "-x*log(25)^6";
	//std::string expr = "-(25.215x/(-2-20))*16-17";
	//std::string expr = "17";
	//std::string expr = "4+x*3";
	//std::string expr = "c- (x - y) *(c / (a + b+c))+c*b*d-((b*a)-b)";

	//std::map<std::string, float> args{{"x", 2.0f} };

	auto start = std::chrono::high_resolution_clock::now();
	parser.parse(expr);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Time taken to parse and set func: "<< static_cast<float>(duration.count())/1000.0f << " milliseconds" << std::endl;

	std::cout << "Answer: " << std::fixed << std::setprecision(10) << parser.calculate(3.0f) << std::defaultfloat << std::endl;
	constexpr std::size_t calc_count = 10000;
	start = std::chrono::high_resolution_clock::now();
	for (std::size_t i = 0; i<calc_count; i++) {
		parser.calculate(3.0f);
	}
	stop = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Time taken to calculate "<<calc_count<< " times: "<< static_cast<float>(duration.count())/1000.0f << " milliseconds" << std::endl;
	return 0;
}