#define PMD_NO_DEPRECATED

#include "pmd.hpp"
#include <QtGui/QApplication>
#include "widget.hpp"
#include <primitives/sglgeosphere.h>
#include <recog.hpp>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Widget *w=new Widget;
    SGLqtSpace *glw= new SGLqtSpace(w->GLframe);
    PMD pmd("/opt/PMDSDK/bin/camboardnano","/opt/PMDSDK/bin/camboardnanoproc");
    if(pmd.good()){

        w->show();
		glw->Camera->MoveCamTo(SGLVektor(0,0,-20)); //look from behind
        auto tof=pmd.getRenderer();
       auto ball=SGLshPtr_new<SGLGeosphere >();

        tof->Move(-80,-60,-80);
        glw->registerObj(tof);
       glw->registerObj(ball);

        QTimer *timer = new QTimer(glw);
        glw->connect(timer, SIGNAL(timeout()), SLOT(updateGL()));
        timer->start(1000/30);

        w->updateConfig(pmd.hnd);

        std::thread t1(pmd);
		std::thread t2(pmd.getRecog(ball));
        app.exec();
        quit_thread=true;
        t1.join();
		t2.join();
        return 0;
    } else
        return -1;
}
