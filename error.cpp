#include <exception>
#include <string>
#include <iostream>
#include <sstream>
#include "error.h"

using namespace std;

/* Definitions for the ErrorException class */

ErrorException::ErrorException()
{
    /* Empty */
}

ErrorException::ErrorException(string msg)
{
    this->msg = msg;
}

ErrorException::~ErrorException() throw()
{
    /* Empty */
}

string ErrorException::getMessage() const
{
    return msg;
}

const char *ErrorException::what() const throw()
{
    return ("Error: " + msg).c_str();
}

/*
 * Implementation notes: error
 * ---------------------------
 * Earlier implementations of error made it possible, at least on the
 * Macintosh, to help the debugger generate a backtrace at the point
 * of the error.  Unfortunately, doing so is no longer possible if
 * the errors are catchable.
 */

void error(string msg)
{
   // std::cout<< msg << __FILE__ << __FUNCTION__ << __LINE__<<std::endl;
    stringstream s;
  //  s<< msg << __FILE__ << __FUNCTION__ << __LINE__;
  //  string o;
  //  s>>o;
    throw ErrorException(msg);
}