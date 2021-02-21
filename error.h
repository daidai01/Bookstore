/*
 * borrowed from StanfordCPPLib
 */

#ifndef BOOKSTORE_ERROR_H
#define BOOKSTORE_ERROR_H

#include <string>
#include <exception>
#include <iostream>
#include <cassert>

/*
 * Class: ErrorException
 * ---------------------
 * This exception is thrown by calls to the <code>error</code>
 * function.  Typical code for catching errors looks like this:
 *
 *<pre>
 *    try {
 *       ... code in which an error might occur ...
 *    } catch (ErrorException & ex) {
 *       ... code to handle the error condition ...
 *    }
 *</pre>
 *
 * If an <code>ErrorException</code> is thrown at any point in the
 * range of the <code>try</code> (including in functions called from
 * that code), control will jump immediately to the error handler.
 */

class ErrorException : public std::exception
{
public:
    ErrorException();

    ErrorException(std::string msg);

    virtual ~ErrorException() throw();

    virtual std::string getMessage() const;

    virtual const char *what() const throw();

private:
    std::string msg;
};

/*
 * Function: error
 * Usage: error(msg);
 * ------------------
 * Signals an error condition in a program by throwing an
 * <code>ErrorException</code> with the specified message.
 */

void error(std::string msg);

//#define error(x) std::cout<<x << __FILE__ << __FUNCTION__ << __LINE__<<std::endl


#endif //BOOKSTORE_ERROR_H
