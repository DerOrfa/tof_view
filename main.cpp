#define PMD_NO_DEPRECATED

#include "pmd.hpp"
#include <QtGui/QApplication>
#include "widget.hpp"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Widget *w=new Widget;
    SGLqtSpace *glw= new SGLqtSpace(w->GLframe);
    PMD pmd("/opt/PMDSDK/bin/camboardnano","/opt/PMDSDK/bin/camboardnanoproc");
    if(pmd.good()){

        w->show();
        SGLshPtr< ToFRender > tof=pmd.getRenderer();
        tof->Move(-80,-60,100);
        glw->registerObj(tof);

        QTimer *timer = new QTimer(glw);
        glw->connect(timer, SIGNAL(timeout()), SLOT(updateGL()));
        timer->start(1000/30);

        w->updateConfig(pmd.hnd);

        std::thread t(pmd);
        app.exec();
        PMD::finish();
        t.join();
        return 0;
    } else
        return -1;
}

bool PMD::m_quit=false;
