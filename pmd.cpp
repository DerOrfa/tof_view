#include "pmd.hpp"
#include <chrono>

void closePmd(){
    std::cout << "Closing all pmd devices" << std::endl;
    pmdCloseAll();
}
void onSig(int /*sig*/){
    closePmd();
    std::abort();
}



ToFTex::ToFTex(GLuint width, GLuint height):m_texwidth(width),m_texheight(height),buff(new float[width*height]){
    TexType=GL_TEXTURE_2D;
    valid=weich=false;
    glGenTextures(1, &ID);
    Scale(1./width,1./height,0);
    glPixelTransferf(GL_RED_SCALE,1.5/10000);
    glPixelTransferf(GL_GREEN_SCALE,1.5/10000);
    glPixelTransferf(GL_BLUE_SCALE,1.5/10000);
}

void ToFTex::doUpdate(){
    glBindTexture(GL_TEXTURE_2D,ID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,m_texwidth, m_texheight, 0,GL_LUMINANCE,GL_FLOAT,buff.get());
}


void ToFRender::prepareQuad(uint_fast16_t xpos, uint_fast16_t ypos){
    const size_t index=xpos+ypos*m_width;
    glNewList(start+index,GL_COMPILE);
    if(*(m_qmap.get()+index)>500){
        glPushMatrix();
        glTranslatef(0,0,*(m_zmap.get()+index)*-900); // SPACE
        glCallList(quad+(index));
        glPopMatrix();
    }
    glEndList();
}

ToFRender::ToFRender(GLuint width, GLuint height):
SGLFlMetaObj(MaterialPtr(),0,0,0,.1),
quad(glGenLists(width*height)),start(glGenLists(width*height)),
m_height(height),m_width(width),
m_zmap(new float[width*height]),m_qmap(new float[width*height])
{
    GLshort vert[][2]={{0,0},{1,0},{1,1},{0,1},{0,0}};

    for(uint_fast16_t x=0;x<m_width;x++)
        for(uint_fast16_t y=0;y<m_height;y++){
            glNewList(quad+x+y*m_width,GL_COMPILE);
            glBegin(GL_QUAD_STRIP);

            for(uint_fast8_t i=0;i<5;i++){
                glTexCoord2s(vert[i][0]+x,0+y);glVertex3s(vert[i][0]+x,0+y,vert[i][1]);
                glTexCoord2s(vert[i][0]+x,1+y);glVertex3s(vert[i][0]+x,1+y,vert[i][1]);
            }

            glEnd();
            glBegin(GL_QUADS);
            //top
            glTexCoord2s(0+x,1+y);
            glVertex3s(0+x,1+y,0);
            glVertex3s(0+x,1+y,1);

            glTexCoord2s(1+x,1+y);
            glVertex3s(1+x,1+y,1);
            glVertex3s(1+x,1+y,0);

            //bottom
            glTexCoord2s(1+x,0+y);
            glVertex3s(1+x,0+y,0);
            glVertex3s(1+x,0+y,1);

            glTexCoord2s(0+x,0+y);
            glVertex3s(0+x,0+y,1);
            glVertex3s(0+x,0+y,0);

            glEnd();

            glEndList();
        }


    for(uint i=0;i<m_width*m_height;i++)
        Objs.push_back(start+i);

    IgnoreLight=true;
    Mat->tex.reset(new ToFTex(width,height));
}

void ToFRender::compileSubObjects(){
    boost::dynamic_pointer_cast<ToFTex>(Mat->tex)->doUpdate();
    if(m_zmap && m_qmap)
        for(uint_fast16_t x=0;x<m_width;x++)
            for(uint_fast16_t y=0;y<m_height;y++)
                prepareQuad(x,y);
}



bool PMD::isOK(int err, const char err_msg[]){
    return isOK(hnd,err,err_msg);
}

bool PMD::isOK(PMDHandle hnd, int err, const char err_msg[])
{
    if (err != PMD_OK)
    {
        char reason[256];
        int res=pmdGetLastError (hnd, reason, 256);
        if(err){
            std::cerr << "Unknown error" << std::endl;
        } else
            std::cerr  << err_msg << ":" << reason << std::endl;
        return res;
    } else
        return true;
}

PMD::PMD(const char pmd_source_plugin[], const char pmd_proc_plugin[]):m_good(false),hnd(0){
    std::cout << "Trying to open pmd device " << std::endl;
    if(!isOK(pmdOpen (&hnd, pmd_source_plugin, "", pmd_proc_plugin, ""),"Could not connect"))
        return;

    if(!isOK(pmdUpdate (hnd),"Could not transfer data"))return;

    if(!isOK(pmdGetSourceDataDescription (hnd, this), "Could not get data description"))
        return;

    if (subHeaderType != PMD_IMAGE_DATA)
    {
        std::cerr << "Source data is not an image!" << std::endl;
        return;
    }

    processingCmdSet("AllowIntensityImage","on");

    isOK(pmdGetIntegrationTime(hnd,config().integ_time,0),"Problem querrying the intetragtion time");
    isOK(pmdGetModulationFrequency(hnd,config().modulation_freq,0),"Problem querrying the modulation frequency");

    config().averaging=processingCmdGet<bool>("GetAveraging");
    config().averaging_frames=processingCmdGet<bool>("GetAveragingFrames");

    config().throttle_frames=30;
    config().throttling=false;

    m_good=true;

    // make sure we close the pmd when closing
    struct sigaction neu_sig, alt_sig;
    neu_sig.sa_handler = onSig;
    sigemptyset (&neu_sig.sa_mask);
    neu_sig.sa_flags = SA_RESTART;
    sigaction (SIGINT, &neu_sig, &alt_sig);
    std::atexit(closePmd);
}

SGLshPtr<ToFRender> PMD::getRenderer(){
    if(m_good){
        render=SGLshPtr_new<ToFRender>(img.numColumns,img.numRows);
        vidmap=boost::dynamic_pointer_cast<ToFTex>(render->Mat->tex)->buff;
        qmap=render->m_qmap;
        zmap=render->m_zmap;
        return render;
    } else
        return render=SGLshPtr<ToFRender>();
}

void PMD::operator()(){
    std::cout << "Startet thread for ToF sensor" << std::endl;
    const size_t size=img.numColumns * img.numRows * sizeof (float);
    configuration &conf=config();

    std::chrono::microseconds frametime(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(1)) / conf.throttle_frames );
    std::chrono::high_resolution_clock::time_point start;

    while(!m_quit && m_good){
        if(conf.throttling)
            start=std::chrono::high_resolution_clock::now();

        if(conf.changed){
            isOK(pmdSetIntegrationTime(hnd,0,conf.integ_time[0]),"Failed to set integration time");
            isOK(pmdSetModulationFrequency(hnd,0,conf.modulation_freq[0]),"Failed to set modulation frequency");

            processingCmdSet("SetAveraging",conf.averaging);
            processingCmdSet("SetAveragingFrames",conf.averaging_frames);

            frametime=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(1)) / conf.throttle_frames;

            conf.changed=false;
        }

        m_good=
            isOK(pmdUpdate (hnd),"Failed getting data transfer data") &&
            isOK(pmdGetIntensities (hnd, vidmap.get(), size),"Could not get distances")&&
            isOK(pmdGetAmplitudes(hnd, qmap.get(), size),"Could not get quality map")&&
            isOK(pmdGetDistances (hnd, zmap.get(), size),"Could not get distances");

        if(m_good)
        {
            render->compileNextTime();
        }
        if(conf.throttling){
            const auto elapsed=std::chrono::high_resolution_clock::now() - start;
            if(elapsed < frametime){
                usleep(std::chrono::duration_cast<std::chrono::microseconds>(frametime- elapsed).count());
            }
        }
    }
    std::cout << "Ending thread for ToF sensor" << std::endl;
}

template<> bool PMD::processingCmdGet<bool>(const char cmd[]){
    char result[256];
    isOK(pmdProcessingCommand(hnd,result,256,cmd),"Failed to execute set command");

    return std::string(result)=="On" || std::string(result)=="on";
}
template<> bool PMD::processingCmdSet<bool>(std::string cmd, bool val){
    char result[256];
    return isOK(pmdProcessingCommand(hnd,result,256,(cmd+" "+(val?"on":"off")).c_str()),"Failed to execute set command");
}
