#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
using std::string;

class Message
{
public:
    virtual void Cout(string msg) = 0;
    virtual void ProgressValue(int progress_value) = 0;
};

class StdMessage : public Message
{
public:
    virtual void Cout(string msg);
    virtual void ProgressValue(int progress_value);
};

#endif // MESSAGE_H
