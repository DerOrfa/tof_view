#ifndef PMD_HPP
#define PMD_HPP

#include <qt4_glue/sglqtspace.h>
#include <sglobj.h>
#include <pmdsdk2.h>
#include <thread>
#include <memory>
#include <signal.h>
#include <atomic>
#include <boost/lexical_cast.hpp>
#include "recog.hpp"

extern bool quit_thread;


class ToFTex:public SGLBaseTex{
    GLuint m_texwidth,m_texheight;
public:
    std::shared_ptr<float> buff;
    ToFTex(GLuint width, GLuint height);
    void doUpdate();
};

class ToFRender:public SGLFlMetaObj{
    GLuint quad,start;

    void prepareQuad(uint_fast16_t xpos,uint_fast16_t ypos);
    GLuint m_height,m_width;
public:
    std::shared_ptr<float> m_zmap,m_qmap;
    ToFRender(GLuint width,GLuint height);

    void compileSubObjects();
    SGLVektor getCenter() const{return SGLVektor();}
};

class PMD:public PMDDataDescription
{
    SGLshPtr<ToFTex> tex;
    bool m_good;
    SGLshPtr< ToFRender > render;
    std::shared_ptr<float> zmap,vidmap,qmap;
public:
    struct configuration{
        std::atomic_bool changed;
        unsigned integ_time[3],modulation_freq[3];
        bool averaging,throttling,bilat_filter;
        unsigned averaging_frames,throttle_frames,bilat_filtersize;
    };
    bool isOK(int err,const char err_msg[]);
    static bool isOK(PMDHandle hnd, int err,const char err_msg[]);
    static configuration &config(){
        static configuration conf;
        return conf;
    }

    PMDHandle hnd;
    PMD(const char pmd_source_plugin[],const char pmd_proc_plugin[]);
    SGLshPtr<ToFRender> getRenderer();
    Recog getRecog(SGLshPtr< SGLObjBase > marker = SGLshPtr<SGLObjBase>());

    template<typename T> T processingCmdGet(const char cmd[]){
        char result[256];
        isOK(pmdProcessingCommand(hnd,result,256,cmd),"Failed to execute set command");
        return boost::lexical_cast<T>(result);
    }

    template<typename T> bool processingCmdSet(std::string cmd,T val){
        std::string sval=boost::lexical_cast<std::string>(val);
        char result[256];
        return isOK(pmdProcessingCommand(hnd,result,256,(cmd+" "+sval).c_str()),"Failed to execute get command");
    }

    bool good(){return m_good;}
    void operator()();
};

template<> bool PMD::processingCmdGet<bool>(const char cmd[]);
template<> bool PMD::processingCmdSet<bool>(std::string cmd, bool val);

#endif // PMD_HPP
