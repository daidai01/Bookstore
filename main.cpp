#include <iostream>
#include "blocklist.h"
#include "book.h"
#include "error.h"
#include "program.h"
#include "user.h"

using namespace std;

int main()
{
 //   freopen("../Data/BasicDataSet/testcase6.txt", "r", stdin);
 //   freopen("../myout.txt", "w", stdout);

    try
    {
        init();
    } catch (ErrorException &ex)
    {
        cout << ex.getMessage() << '\n';
    } catch (...)
    {

    }

    string line;
    while (getline(cin, line))
    {
        try
        {
            cout<<"# "<<line<<endl;
            runProgram(line);
        } catch (ErrorException &ex)
        {
            cout << ex.getMessage() << '\n';
        } catch (...)
        {

        }
    }

    return 0;
}
