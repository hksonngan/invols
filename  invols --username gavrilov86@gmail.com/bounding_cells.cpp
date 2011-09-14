#include "bounding_cells.h"
#include "Draw.h"


#define BC_SIZE 64


namespace CT
{
	short bounding_cells_data[BC_SIZE*BC_SIZE*BC_SIZE];
	ivec3 bounding_cells_data_size(BC_SIZE);


	void bc_Draw()
	{
		vec3 b1 = iso->GetBoundingBox(0);
		vec3 b2 = iso->GetBoundingBox(1);
		vec3 ss = (b2-b1)/(BC_SIZE);
		//b1+=ss*0.5;
		//glColor4d(1,1,1,0.8);
		glColor3d(1,1,1);
//		glDisable(GL_LIGHTING);

		short*pt_bc=bounding_cells_data;
		for(int k=0; k<BC_SIZE; k++)
		for(int j=0; j<BC_SIZE; j++)
		for(int i=0; i<BC_SIZE; i++)
		{
			if(*pt_bc)
			{
				vec3 cen = b1+ss*vec3(i,j,k);
	//			DrawSphere(b1+ss*vec3(i,j,k),0.003f);
	//			DrawCubeFrame(cen,cen+ss);
				DrawCube(cen+vec3(ss.x,0,0),cen+vec3(0,ss.y,ss.z));
			}

			pt_bc++;
		}
//		glEnable(GL_LIGHTING);

	}
	
}