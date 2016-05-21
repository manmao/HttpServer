#include "register.h"
#include "TestServlet2.h"
#include "TestServlet1.h"


void regist_servlet(ServletRegister* registe)
{
    registe->register_servlet("/man/m", new TestServlet1());
    registe->register_servlet("/man/mao", new TestServlet2());
}

