#include "register.h"
#include "ServletImpl.h"

//�����������ע�����Servlet
void regist_servlet(ServletRegister* registe)
{
    registe->register_servlet("/man/m", new ServletImpl());
    registe->register_servlet("/man/mao", new ServletImpl());
}

