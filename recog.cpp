#include "recog.hpp"
#include "pmd.hpp"
#include <algorithm>

void Recog::operator()()
{
    std::cout << "Started thread for face recognition" << std::endl;
	const float * oldmin;
    while(!quit_thread){
        const float * const start=m_zmap.get();
        const float * const end=m_zmap.get()+m_width*m_height;
        const float * min=start;

        for(const float *z=start,*q=m_qmap.get();z<end;z++,q++){
            if(*q>500 && *z<*min)
                min=z;
        }

        if(start!=min && oldmin!=min){
			const size_t idx= std::distance(start, min);
			const size_t idx_y=idx/m_width,idx_x=idx%m_width;
			if(m_marker){
				float posx=(idx_x-m_width/2.)/10,posy=(idx_y-m_height/2.)/10;
				m_marker->SetPosAndScale(posx,posy,(*min)*50-8,1);
				m_marker->compileNextTime();
			}
			oldmin=min;
		}
    }
    std::cout << "Ending thread for face recognition" << std::endl;
}

Recog::Recog(unsigned width, unsigned height, std::shared_ptr<float> zmap, std::shared_ptr<float> qmap,SGLshPtr<SGLObjBase> marker)
    :m_zmap(zmap),m_qmap(qmap),m_height(height),m_width(width),m_marker(marker)
{}
