#include "camera.h"

Camera::Camera()
{
}


void Camera::setLensModel(vcl_vector<bool> flags)
{
	assert(flags.size()==7);
	for(int i=0; i<7; i++){
		if(flags[i])
			_lm.turnOn(i);
		else
			_lm.turnOff(i);
	}
}