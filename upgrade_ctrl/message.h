#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
using std::string;

class Message
{
public:
    virtual void Cout(string msg) = 0;
};

class StdMessage : public Message
{
public:
    virtual void Cout(string msg);
};

#endif // MESSAGE_H
