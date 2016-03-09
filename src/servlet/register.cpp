#include "register.h"
#include "TestServlet2.h"
#include "TestServlet1.h"


//在这个函数内注册你的Servlet
void regist_servlet(ServletRegister* registe)
{
    registe->register_servlet("/man/m", new TestServlet1());
    registe->register_servlet("/man/mao", new TestServlet2());
}

