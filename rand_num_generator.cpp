
#include "rand_num_generator.hpp"

const t_uint RandNumGenerator::m_DEFAULT_SEED = 2006;

RandNumGenerator::RandNumGenerator()
{
	setSeed(m_DEFAULT_SEED);
}


