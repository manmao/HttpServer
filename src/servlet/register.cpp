#include "register.h"
#include "ServletImpl.h"

//�����������ע�����Servlet
void regist_servlet(Config *config)
{
    config->register_servlet("/man/m", new ServletImpl());
    config->register_servlet("/man/mao", new ServletImpl());
}

