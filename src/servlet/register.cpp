#include "register.h"
#include "ServletImpl.h"

//�����������ע�����Servlet
void regist_servlet(ServletRegister *register)
{
    register->register_servlet("/man/m", new ServletImpl());
    register->register_servlet("/man/mao", new ServletImpl());
}

