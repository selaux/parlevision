/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#include "RunningAverage.h"
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.hpp>
#include <limits>

using namespace plv;
using namespace plvopencv;

RunningAverage::RunningAverage() : m_weight(0.1), m_conversionFactor(1.0)
{
    m_inputPin = createCvMatDataInputPin( "input", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_inputPin->addAllChannels();
    m_inputPin->addSupportedDepth(CV_8U);
    m_inputPin->addSupportedDepth(CV_16U);
    m_inputPin->addSupportedDepth(CV_32F);

    m_outputPin->addAllChannels();
    m_outputPin->addSupportedDepth(CV_8U);
    m_outputPin->addSupportedDepth(CV_32F);
}

RunningAverage::~RunningAverage()
{
}

bool RunningAverage::process()
{
    CvMatData in = m_inputPin->get();
    const cv::Mat& src = in;

    if( m_avg.width() != in.width() || m_avg.height() != in.height() || in.type() != m_out.type() )
    {
        m_avg = CvMatData::create( in.width(), in.height(), CV_32F, in.channels() );
        m_tmp = CvMatData::create( in.width(), in.height(), CV_32F, in.channels() );
        m_out = CvMatData::create( in.width(), in.height(), in.type(), in.channels() );

        if( src.depth() == CV_8U )
        {
            m_conversionFactor = std::numeric_limits<unsigned char>::max();
            src.convertTo(m_avg, m_avg.type(), 1.0 / m_conversionFactor );
        }
        else if ( src.depth() == CV_16U )
        {
            m_conversionFactor = std::numeric_limits<unsigned short>::max();
            src.convertTo(m_avg, m_avg.type(), 1.0 / m_conversionFactor );
        }
        else // CV_32F
        {
            src.copyTo(m_avg);
            m_conversionFactor = 1.0;
        }
    }
    cv::Mat& tmp = m_tmp;

    // convert src to 32F
    src.convertTo(m_tmp, m_tmp.type(), 1.0 / m_conversionFactor);
    cv::accumulateWeighted(tmp, m_avg, m_weight);

    // convert avg back to src type
    const cv::Mat& avg = m_avg;
    avg.convertTo(m_out, m_out.type(), m_conversionFactor );

    m_outputPin->put(m_out);
    return true;
}

double RunningAverage::getWeight() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_weight;
}

void RunningAverage::setWeight(double w)
{
    QMutexLocker lock( m_propertyMutex );
    if( w >= 0.0 && w <= 1.0 )
    {
        m_weight = w;
    }
    emit weightChanged(m_weight);
}
