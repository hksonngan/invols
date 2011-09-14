#ifndef DICOM_READER_H
#define DICOM_READER_H

#include <stdio.h>
#include <stdlib.h>
#include "AllDef.h"
#include <Math.h>
#include <ivec3.h>
#include "output.h"
#include "CT.h"


namespace DicomReader
{
	bool LoadDICOMDIR(wxString dicom_dir);

	float GetScale();// длина в миллиметрах (ширина коробки)

	/////
	extern wxString cur_dicom_dir;
	//extern float slice_thickness, spacing_between_slices,pixel_spacing;

}

#endif