#include "pmd.hpp"
#include "widget.hpp"
#include <mutex>


void Widget::setupSpinbox(QSpinBox *box, bool enabled, int value)
{
    box->setValue(value);
    box->setEnabled(enabled);
}

void Widget::updateConfig(PMDHandle _hnd)
{
    hnd=_hnd;
    PMD::configuration &config=PMD::config();
    integ_time->setValue(config.integ_time[0]);

    isAveraging->setChecked(config.averaging);
    isThrottling->setChecked(config.throttling);
    isBilat->setChecked(config.bilat_filter);

    setupSpinbox(numAveraging,config.averaging,config.averaging_frames);
    setupSpinbox(numThrott,config.throttling,config.throttle_frames);
    setupSpinbox(numBilat,config.bilat_filter,config.bilat_filtersize);
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

void Widget::on_isAveraging_toggled(bool checked)
{
    if(PMD::config().averaging!=checked){
        PMD::config().averaging=checked;
        PMD::config().changed=true;
    }
}

void Widget::on_numAveraging_valueChanged(int arg1)
{
    if(PMD::config().averaging_frames!=unsigned(arg1)){
        PMD::config().averaging_frames=unsigned(arg1);
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

void Widget::on_isBilat_toggled(bool checked)
{
    if(PMD::config().bilat_filter!=checked){
        PMD::config().bilat_filter=checked;
        PMD::config().changed=true;
    }
}

void Widget::on_numBilat_valueChanged(int arg1)
{
    if(PMD::config().bilat_filtersize!=unsigned(arg1)){
        PMD::config().bilat_filtersize=unsigned(arg1);
        PMD::config().changed=true;
    }
}

void Widget::on_bottomCap_valueChanged(int arg1)
{
	if(PMD::config().bottomCap!=unsigned(arg1)){
		PMD::config().bottomCap=unsigned(arg1);
		PMD::config().changed=true;
	}
}

void Widget::on_topCap_valueChanged(int arg1)
{
	if(PMD::config().topCap!=unsigned(arg1)){
		PMD::config().topCap=unsigned(arg1);
		PMD::config().changed=true;
	}
}
