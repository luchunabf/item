#include "httplib.h"

                                                                                              
void Hello(const httplib::Request& req, httplib::Response& resp)
{
    resp.set_content("<h1>hello</h1>", "text/html");

}

int main()
{
    using namespace httplib;
      
    Server server;
    server.Get("/hello", Hello);
    server.listen("0.0.0.0", 9094);
    
    return 0;

}
