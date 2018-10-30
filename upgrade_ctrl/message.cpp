#include <iostream>

#include "message.h"


void StdMessage::Cout(std::string msg)
{
	std::cout << msg << std::endl;
}

void StdMessage::ProgressValue(int progress_value)
{
    std::cout << "烧写进度: " << progress_value << "%" << std::endl;
}

Message::~Message()
{

}
