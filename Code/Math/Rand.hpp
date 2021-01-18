#ifndef _RAND_HPP
#define _RAND_HPP 1

class RandomGenerator
{
public:
    static RandomGenerator& smt_Get()
    {
        static RandomGenerator l_Rnd;

        return l_Rnd;
    }

	int mt_Generate(void)
	{
		return std::rand();
	}
	int mt_Generate(int max)
	{
		if (max == 0)
			max = 1;
		return std::rand() % max;
	}
	int mt_Generate(int min, int max)
	{
		return min + mt_Generate(max - min);
	}
	int operator()()
	{
		return mt_Generate();
	}
	int operator()(int max)
	{
		return mt_Generate(max);
	}
	int operator()(int min, int max)
	{
		return mt_Generate(min, max);
	}
	float operator()(float min, float max)
	{
		float l_random = mt_Generate();
		l_random /= RAND_MAX;

		l_random = min + (l_random * (max - min));

		return l_random;
	}

private:
    RandomGenerator()
    {}
};


#endif // _RAND_HPP
