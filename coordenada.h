//
// Created by arkab-desk on 26/03/17.
//

#ifndef GRAFICADOR_COORDENADA_H
#define GRAFICADOR_COORDENADA_H


class coordenada{
private:
    float x;
    float y;
    float z;
public:

    float getX();
    float getY();
    float getZ();
    coordenada(float ,float ,float );

};

coordenada::coordenada(float x, float y, float z) {
    this->x=x;
    this->y=y;
    this->z=z;
}
float coordenada::getX() {
    return this->x;
}



float coordenada::getY() {
    return this->y;
}

float coordenada::getZ() {
    return this->z;
}




#endif //GRAFICADOR_COORDENADA_H
