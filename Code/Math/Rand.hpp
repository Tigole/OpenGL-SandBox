#ifndef _RAND_HPP
#define _RAND_HPP 1

#include <cstddef>
#include <stack>

class RandomGenerator
{
public:
    static RandomGenerator& smt_Get()
    {
        static RandomGenerator l_Rnd;

        return l_Rnd;
    }

	uint32_t mt_Generate(void)
	{
		return mt_Lehmer();
	}
	uint32_t mt_Generate(uint32_t max)
	{
		if (max == 0)
			max = 1;
		return mt_Lehmer() % max;
	}
	uint32_t mt_Generate(uint32_t min, uint32_t max)
	{
		return min + mt_Generate(max - min);
	}
	float mt_Generate(float min, float max)
	{
		float l_random = mt_Generate();
		l_random /= std::numeric_limits<uint32_t>::max();

		l_random = min + (l_random * (max - min));

		return l_random;
	}
	uint32_t operator()()
	{
		return mt_Generate();
	}
	uint32_t operator()(uint32_t max)
	{
		return mt_Generate(max);
	}
	uint32_t operator()(uint32_t min, uint32_t max)
	{
		return mt_Generate(min, max);
	}
	float operator()(float min, float max)
	{
		return mt_Generate(min, max);
	}

	void mt_State_Stack()
	{
	    m_Stack.push(m_State);
	}
	void mt_State_Pop()
	{
	    if (m_Stack.size())
        {
            m_State = m_Stack.top();
            m_Stack.pop();
        }
	}
	void mt_State_Set(uint32_t new_state)
	{
	    m_State = new_state;
	}

private:
    RandomGenerator() : m_State(0)
    {}

    uint32_t m_State;

    std::stack<uint32_t> m_Stack;

    uint32_t mt_Lehmer()
    {
        uint64_t l_Tmp;
        uint32_t l_m1, l_m2;

        m_State += 0xe120fc15;

        l_Tmp = (uint64_t)m_State * 0x4a39b70d;
        l_m1 = (l_Tmp >> 32) ^ l_Tmp;

        l_Tmp = (uint64_t)l_m1 * 0x12fad5c9;
        l_m2 = (l_Tmp >> 32) ^ l_Tmp;

        return l_m2;
    }
};


#endif // _RAND_HPP
