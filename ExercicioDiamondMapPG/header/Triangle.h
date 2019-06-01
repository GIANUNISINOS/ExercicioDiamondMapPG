//
// Created by Gian Boschetti on 2019-05-27.
//

class Triangle {
public:
    double A[2];
    double B[2];
    double C[2];

    Triangle() {
        A[0] = 0;
        A[1] = 0;

        B[0] = 0;
        B[1] = 0;

        C[0] = 0;
        C[1] = 0;
    }

    Triangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy) {
        A[0] = Ax;
        A[1] = Ay;

        B[0] = Bx;
        B[1] = By;

        C[0] = Cx;
        C[1] = Cy;
    }

    // Calcular a area do triangulo
    float area() {
        return triangleArea(A, B, C);
    }

    // Calcular area do triangulo passado
    float triangleArea(float* Ao, float* Bo, float* Co) {
        return abs(((Bo[0] - Ao[0]) * (Co[1] - Ao[1]) - (Co[0] - Ao[0]) * (Bo[1] - Ao[1]))/2.0f);
    }

};
