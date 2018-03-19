#include <string_view>
using namespace std::string_view_literals;

#include <iostream>

int main(int argc,char ** argv) {
	if (argc<3) {
		std::cout << "error input format" << std::endl;
		return -1;
	}
	
	extern void publishTable(std::string_view a, std::string_view b);
	publishTable(argv[1],argv[2]);
	return 0;
}

/****************************************/

