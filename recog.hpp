#ifndef RECOG_HPP
#define RECOG_HPP

#include <memory>
#include <sglobjbase.h>

class Recog
{
    std::shared_ptr<float> m_zmap,m_qmap;
    SGLshPtr<SGLObjBase> m_marker;
    unsigned m_height,m_width;

public:
    void operator()();
    Recog(unsigned width, unsigned height, std::shared_ptr<float> zmap, std::shared_ptr<float> qmap, SGLshPtr<SGLObjBase> marker=SGLshPtr<SGLObjBase>());
};

#endif // RECOG_HPP
