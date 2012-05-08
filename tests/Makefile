test check: test_nosweatconfigfileparser
	./test_nosweatconfigfileparser

test_nosweatconfigfileparser: ../NoSweatConfigFileParser.hpp test_nosweatconfigfileparser.cpp
	g++-4.7 -std=c++11 -I.. test_nosweatconfigfileparser.cpp -o test_nosweatconfigfileparser

clean:
	rm -rf test_nosweatconfigfileparser
