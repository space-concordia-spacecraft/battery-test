#include "Utils.h"

float Interpolate( float xData[8],float yData[8], float x )
{
    int index = -1;

    for(int i = 0; i < 8; i ++) {
        if(x > xData[i]) {
            index = i;
            break;
        }
    }

    if(index == -1) {
        return yData[7];
    } else if (index - 1 == -1) {
        return yData[0];
    } else {
        return yData[index - 1] + ((yData[index]-yData[index - 1])/(xData[index]-xData[index - 1])) * (x - xData[index - 1]);
    }
}