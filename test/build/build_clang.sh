clang++ ../main.cpp ../test_marshalling.cpp ../../library/src/globalmq.cpp ../../3rdparty/fmt/src/format.cc -I../../3rdparty/fmt/include -I../../library/include -std=c++2a -g -Wall -Wextra -Wno-unknown-attributes -Wno-c++2a-extensions -stdlib=libc++ -Wno-unused-variable -Wno-unused-parameter -Wno-empty-body -DNDEBUG -O3  -o test.bin