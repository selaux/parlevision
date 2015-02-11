#include "helloworldprocessor.h"

#include <QDebug>

#include "helloworldprocessor.h"
#include <plvcore/Pin.h>
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.hpp>

using namespace plv;

HelloWorldProcessor::HelloWorldProcessor() :
        m_someInt(1337),
        m_someDouble(1.23456),
        m_someBool(true),
        m_someString("hello")
{
    m_inputPin = createCvMatDataInputPin("input", this);
    m_outputPin = createCvMatDataOutputPin("output", this);
}

HelloWorldProcessor::~HelloWorldProcessor()
{
}

bool HelloWorldProcessor::init()
{
    return true;
}

bool HelloWorldProcessor::deinit() throw ()
{
    return true;
}

bool HelloWorldProcessor::start()
{
    return true;
}

bool HelloWorldProcessor::stop()
{
    return true;
}

bool HelloWorldProcessor::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    CvMatData src = m_inputPin->get();

    // allocate a target buffer
    CvMatData target;
    target.create( src.width(), src.height(), src.type() );

    // do a flip of the image
    const cv::Mat in = src;
    cv::Mat out = target;
    cv::flip( in, out, (int)m_someBool);

    // publish the new image
    m_outputPin->put( target );

    // update our "frame counter"
    this->setSomeInt(this->getSomeInt()+1);

    return true;
}
