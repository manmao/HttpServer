#include "register.h"
#include "ServletImpl.h"

//在这个函数内注册你的Servlet
void regist_servlet(ServletRegister* registe)
{
    registe->register_servlet("/man/m", new ServletImpl());
    registe->register_servlet("/man/mao", new ServletImpl());
}

