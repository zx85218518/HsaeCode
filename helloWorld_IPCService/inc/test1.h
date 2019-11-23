#ifndef TEST1_H
#define TEST1_H
#include <iostream>
#include <string>

#include <stdint.h>
#include <stddef.h>


#define StartBlock()	(code_ptr = dst++, code = 1)
#define FinishBlock()	(*code_ptr = code)
size_t StuffData(const uint8_t *ptr, size_t length, uint8_t *dst);
size_t UnStuffData(const uint8_t *ptr, size_t length, uint8_t *dst);



class test1
{
public:
    test1();
    ~test1();
    void print();

	std::string str1;

};

#endif // TEST1_H
