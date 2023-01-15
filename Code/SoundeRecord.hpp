#ifndef _SOUND_RECORD_HPP
#define _SOUND_RECORD_HPP 1

#include "_Prototype.hpp"
#include "../Submodules/swarm/include/swarm.hpp"

#include <SFML/Audio.hpp>

#include "../../../01-Code/00-Libraries/16-alglib/cpp/src/ap.h"
#include "../../../01-Code/00-Libraries/16-alglib/cpp/src/fasttransforms.h"

using namespace alglib;

namespace sr
{

class Effect
{
public:
    Effect(){}
    virtual ~Effect(){}

    virtual void mt_Apply(std::vector<double>& buffer) = 0;
};

class GainEffect : public Effect
{
public:

    void mt_Apply(std::vector<double>& buffer) override
    {
        for (std::size_t ii = 0; ii < buffer.size(); ii++)
        {
            buffer[ii] *= m_Gain;
        }
    }

    double m_Gain = 5.0;
};

class LowPassFilterEffect : public Effect
{
public:

    void mt_Apply(std::vector<double>& buffer) override
    {
        double l_Last_Raw_Value = 0.0;
        double l_Last_Output_Value = 0.0;
        double l_Tmp;

        /// https://dsp.stackexchange.com/questions/60277/is-the-typical-implementation-of-low-pass-filter-in-c-code-actually-not-a-typica
        for (std::size_t ii = 0; ii < buffer.size(); ii++)
        {
            l_Tmp = 0.04762 * buffer[ii] + 0.04762 * l_Last_Raw_Value + 0.9048 * l_Last_Output_Value;
            l_Last_Raw_Value = buffer[ii];
            l_Last_Output_Value = l_Tmp;
            buffer[ii] = l_Tmp;
        }
    }
};

class NoiseReduction : public Effect
{
public:

    void mt_Apply(std::vector<double>& buffer) override
    {
        complex_1d_array l_ret;
        real_1d_array l_array;
        std::size_t l_Size = 2;

        while (l_Size < buffer.size())
        {
            l_Size *= 2;
        }
        l_Size /= 2;

        l_array.setcontent(l_Size, buffer.data());
        fftr1d(l_array, l_ret);

        l_Size = l_ret.length() / 2;
        for (std::size_t ii = 0; ii < l_Size; ii++)
        {
            //
        }

        fftr1dinv(l_ret, l_array);

        buffer.clear();
        buffer.insert(buffer.end(), l_array.getcontent(), l_array.getcontent() + l_array.length());

        log_debug("[0] == [1] ? %d", buffer[0] == l_array(0));
    }
};

class Recorder : public sf::SoundRecorder
{
public:
    Recorder(sf::Mutex& m, std::vector<double>& normalized_samples)
     :  m_Mutex(m),
        m_Normalized_Captured_Samples(normalized_samples)
    {}

    ~Recorder()
    {
        stop();
    };

    bool onProcessSamples(const sf::Int16* samples, std::size_t sample_count) override
    {
        m_Mutex.lock();

        for (std::size_t ii = 0; ii < sample_count; ii++)
        {
            m_Normalized_Captured_Samples.push_back((double)samples[ii] / (double)std::numeric_limits<sf::Int16>::max());
        }

        m_Mutex.unlock();

        return true;
    }

    sf::Mutex& m_Mutex;
    std::vector<double>& m_Normalized_Captured_Samples;
};

class Stream : public sf::SoundStream
{
public:
    Stream(sf::Mutex& m, std::vector<double>& normalized_samples, std::vector<std::unique_ptr<Effect>>& effects, int sample_rate)
     :  m_Mutex(m),
        m_Normalized_Captured_Samples(normalized_samples),
        m_Effects(effects)
    {
        initialize(1, sample_rate);
    }

    bool onGetData(sf::SoundStream::Chunk& data)
    {
        bool l_b_Ret;

        l_b_Ret = false;

        while(l_b_Ret == false)
        {
            sf::sleep(sf::milliseconds(20));

            m_Mutex.lock();

            if (m_Normalized_Captured_Samples.size() > 0)
            {
                for (std::size_t ii = 0; ii < m_Effects.size(); ii++)
                {
                    m_Effects[ii]->mt_Apply(m_Normalized_Captured_Samples);
                }

                m_Render_Samples.resize(m_Normalized_Captured_Samples.size(), 0);
                for (std::size_t ii = 0; ii < m_Normalized_Captured_Samples.size(); ii++)
                {
                    m_Render_Samples[ii] = m_Normalized_Captured_Samples[ii] * std::numeric_limits<sf::Int16>::max();
                }
                m_Normalized_Captured_Samples.clear();

                /*for (std::size_t ii = 0; ii < m_Render_Samples.size(); ii++)
                {
                    sf::Int16 l_Noise_Threshold = 150;
                    sf::Int16 l_Gain = 5;
                    sf::Int16 l_Sample = m_Render_Samples[ii];

                    l_Sample = l_Gain * l_Sample;
                    //l_Sample = std::max(std::min(l_Gain * l_Sample, (int)std::numeric_limits<sf::Int16>::max()), (int)std::numeric_limits<sf::Int16>::min());
                    l_Sample = std::abs(l_Sample) > (l_Gain * l_Noise_Threshold) ? l_Sample : 0;

                    m_Render_Samples[ii] = l_Sample;
                }*/

                data.samples = m_Render_Samples.data();
                data.sampleCount = m_Render_Samples.size();

                l_b_Ret = true;
            }

            m_Mutex.unlock();
        }

        //log_debug("m_Captured_Samples.size: %d\tm_Render_Samples.size: %d\tl_b_Ret: %d", m_Captured_Samples.size(), m_Render_Samples.size(), l_b_Ret);

        return l_b_Ret;
    }

    void onSeek(sf::Time offset)
    {}

    sf::Mutex& m_Mutex;
    std::vector<double>& m_Normalized_Captured_Samples;
    std::vector<std::unique_ptr<Effect>>& m_Effects;
    std::vector<sf::Int16> m_Render_Samples;
};

class Worker
{
public:

    Worker()
     :  m_Recorder(m_Mutex, m_Normalized_Captured_Samples),
        m_Stream(m_Mutex, m_Normalized_Captured_Samples, m_Effects, 44100)
    {
        m_Effects.push_back(std::make_unique<GainEffect>());
        m_Effects.push_back(std::make_unique<LowPassFilterEffect>());
        //m_Effects.push_back(std::make_unique<NoiseReduction>());
    }

    ~Worker()
    {
        m_Stream.stop();
        m_Recorder.stop();
    }

    void mt_Init()
    {
        int l_Sample_Rate = 44100;
        log_debug("Device name: '%s'", m_Recorder.getDevice().c_str());
        m_Recorder.start(l_Sample_Rate);
        sf::sleep(sf::milliseconds(100));
        m_Stream.play();
    }

    sf::Mutex m_Mutex;
    std::vector<double> m_Normalized_Captured_Samples;
    std::vector<std::unique_ptr<Effect>> m_Effects;

    Recorder m_Recorder;
    Stream m_Stream;
};


}


class SoundRecord : public Prototype
{
public:
    SoundRecord() : Prototype("Sound Buffer Recorder")
    {
        if (!sf::SoundBufferRecorder::isAvailable())
        {
            log_error("Sound Recorder unavailable");
        }
    }
    ~SoundRecord()
    {}

    bool mt_Specialized_Init() override
    {
        m_Worker.mt_Init();

        m_Worker.m_Stream.setVolume(100.0f);
        //m_Worker.m_Stream.setPitch(1.5f);

        log_debug("Global volume: %f", sf::Listener::getGlobalVolume());
        sf::Listener::setGlobalVolume(100);

        return true;
    }

    void mt_Handle_Event(const sf::Event& e) override
    {
        if (e.type == sf::Event::KeyReleased)
        {
            if (e.key.code == sf::Keyboard::Space)
            {
                m_Worker.m_Stream.play();
            }
        }
    }

    void mt_Update(float elapsed_time) override
    {
        //m_Worker.m_Stream.play();
    }

    void mt_Draw(sf::RenderTarget& target) override
    {
        char l_Buffer[128] = {'\0'};

        m_Worker.m_Mutex.lock();
        sprintf(l_Buffer, "Status: %d", m_Worker.m_Stream.getStatus());
        m_Worker.m_Mutex.unlock();

        Renderer2D::smt_Render_Text(target, l_Buffer, sf::Vector2f(0.0f, 0.0f));
    }

    sr::Worker m_Worker;
};


#endif // _SOUND_RECORD_HPP
