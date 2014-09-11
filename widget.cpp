#include "pmd.hpp"
#include "widget.hpp"
#include <mutex>


void Widget::updateConfig(PMDHandle _hnd)
{
    hnd=_hnd;
    PMD::configuration &config=PMD::config();
    integ_time->setValue(config.integ_time[0]);
    mod_freq->setValue(config.modulation_freq[0]);

    isAveraging->setChecked(config.averaging);
    isAveraging->setChecked(config.throttling);

    numAveraging->setValue(config.averaging_frames);
    numAveraging->setEnabled(config.averaging);
    numThrott->setValue(config.throttle_frames);
    numThrott->setEnabled(config.throttling);

}

Widget::Widget(QWidget *parent): QWidget(parent)
{
    setupUi(this);
}


void Widget::on_integ_time_valueChanged(int value)
{
    unsigned result;
    if(PMD::isOK(hnd,pmdGetValidIntegrationTime(hnd,&result,0,CloseTo,value),"oops")){
        PMD::configuration &conf=PMD::config();
        if(conf.integ_time[0]!=result){
            conf.integ_time[0]=result;
            conf.changed=true;
        }
    }

    integ_time->setValue(result);
}

void Widget::on_mod_freq_valueChanged(int value)
{
    unsigned result;
    if(PMD::isOK(hnd,pmdGetValidModulationFrequency(hnd,&result,0,CloseTo,value),"oops")){
        PMD::configuration &conf=PMD::config();
        if(conf.modulation_freq[0]!=result){
            conf.modulation_freq[0]=result;
            conf.changed=true;
        }
    }

    mod_freq->setValue(result);
}

void Widget::on_isAveraging_toggled(bool checked)
{
    if(PMD::config().averaging!=checked){
        PMD::config().averaging=checked;
        PMD::config().changed=true;
    }
}

void Widget::on_numAveraging_valueChanged(int arg1)
{
    if(PMD::config().averaging_frames!=arg1){
        PMD::config().averaging_frames=arg1;
        PMD::config().changed=true;
    }
}

void Widget::on_isThrottling_toggled(bool checked)
{
    if(PMD::config().throttling!=checked){
        PMD::config().throttling=checked;
        PMD::config().changed=true;
    }
}

void Widget::on_numThrott_valueChanged(int arg1)
{
    if(PMD::config().throttle_frames!=unsigned(arg1)){
        PMD::config().throttle_frames=unsigned(arg1);
        PMD::config().changed=true;
    }
}
