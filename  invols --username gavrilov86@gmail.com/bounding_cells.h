#ifndef BOUNDING_CELLS_H
#define BOUNDING_CELLS_H

#include "CT.h"

namespace CT
{
	extern short bounding_cells_data[];			//���������� ���������, ����� ������������ ������������
	extern ivec3 bounding_cells_data_size;

	void bc_Draw();
};


#endif