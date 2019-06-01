#define DEBUG 1

class Tile {
public:
    glm::vec3 colorsRGB;
    bool isVisible;
    bool isSelected;

    Tile() {
        isVisible = true;
        isSelected = false;
    }

    void setColor(int R,int G,int B){
        float r;
        float g;
        float b;

        r = R/255.0f;
        g = G/255.0f;
        b = B/255.0f;

        colorsRGB = glm::vec3(r,g,b);
    }

    void generateColor(int row){
        bool isPair = (row%2==0);

        float r;
        float g;
        float b;

        if(isPair){
            r = 12/255.0f;
            g = 195/255.0f;
            b = 233/255.0f;
        } else{
            r = 23/255.0f;
            g = 79/255.0f;
            b = 214/255.0f;
        }
        colorsRGB = glm::vec3(r,g,b);
    }
};


class ColorTiles{
public:

    float tileWidth, tileHeight;
    float numRows, numCols;
    float sum_heigth;
    glm::mat4 modelMatrix;

    VerticesObject* vertices;

    Tile matrixColors[14][14]= {};

    ColorTiles(float tileWidth, float tileHeight,float numCols, float numRows)
    {
        this->tileWidth = tileWidth;
        this->numCols = numCols;
        this->tileHeight = tileHeight;
        this->numRows = numRows;
        this->sum_heigth = numRows*tileHeight;
        this->modelMatrix = glm::mat4(1);

        this->setupVertices(tileWidth, tileHeight);

        this->createMatrixColors();
    }

    void setupVertices(float width, float height) {
        /*
            Comeca centralizado no zero
        */
        float verticesCoordinates[] = {
                // positions
                width / 2.0f,   0.0f,           0.0f,	// TOP
                width ,         height / 2.0f,  0.0f,	// RIGHT
                width  / 2.0f,  height,         0.0f,	// BOTTOM
                0.0f,           height / 2.0f,  0.0f	// LEFT

        };

        vertices = new VerticesObject(verticesCoordinates, 12);
    }

    void createMatrixColors(){
        for (int row = 0; row < numRows; row++) {
              for (int col = 0; col < numCols; col++) {
                    Tile t = Tile();
                    t.generateColor(row);
                    matrixColors[row][col] = t;
                  }
            }
    }

    void testCliqueMouse(double xPos,double yPos) {

        int rowClick = (int) (yPos / (tileHeight/2.0));
        int columnClick = (int) ((xPos - (rowClick * (tileWidth/2.0)))/tileWidth);

        float x0 = ((float)columnClick)*tileWidth  + ((float)rowClick) *(tileWidth/2.0f) ;
        float y0 = ((float)rowClick)*tileHeight/2.0f ;

        //   _______B_______
        //   |              |
        // A |              |  D
        //   |              |
        //   |______________|
        //          C

        //
        //left point
        float Ax = x0;
        float Ay = y0 + tileHeight/2.0f;

        //top point
        float Bx = x0 + tileWidth/2.0f;
        float By = y0;

        //bottom point
        float Cx = x0 + tileWidth/2.0f;
        float Cy = y0 + tileHeight;

        //right point
        float Dx = x0 + tileWidth;
        float Dy = y0 + tileHeight/2.0f;

        bool isClickValid = false;

        if(DEBUG==1){
            printf("\nxPos: %f", xPos);
            printf("\nyPos: %f", yPos);
            printf("\nRow: %d", rowClick);
            printf("\nColumn: %d\n", columnClick);
            printf("\nx0: %f\n",x0);
            printf("\ny0: %f\n",y0);
            printf("\nleftPoint x %f",Ax);
            printf("\nleftPoint y %f\n",Ay);
            printf("\ntopPointX x %f",Bx);
            printf("\ntopPointY y %f\n",By);
            printf("\nbottomPointX x %f",Cx);
            printf("\nbottomPointY y %f\n",Cy);
            printf("\nrightPointX x %f",Dx);
            printf("\nrightPointY y %f\n",Dy);
        }

        if(xPos < Bx){
            //testar lado da esquerda
            if(DEBUG==1) printf("\nlado esquerda");

            if(testPointCollision(Ax,Ay, Bx, By,  Cx, Cy, xPos, yPos)) {
                if(DEBUG==1) printf("\nDEU BOM");
                isClickValid =true;
            } else {
                if(DEBUG==1) printf("\nNAO DEU TAO BOM");
                if(xPos<Bx  && yPos<Ay){
                    //caminha p cima
                    rowClick--;
                    isClickValid =true;
                }
            }

        } else{
            //testar lado da direita
            isClickValid =true;
            if(DEBUG==1) printf("\nlado direita");
            if(testPointCollision(Dx,Dy, Bx, By,  Cx, Cy, xPos, yPos)) {
                if(DEBUG==1) printf("\nDEU BOM");
            }else {
                if(DEBUG==1) printf("\nNAO DEU TAO BOM");
                if(xPos>Bx  && yPos<Dy){
                    //caminha p cima e direita
                    rowClick--;
                    columnClick++;
                }
            }

        }

        if(isClickValid==true){
            if(matrixColors[rowClick][columnClick].isVisible){
                if (matrixColors[rowClick][columnClick].isSelected) {
                    matrixColors[rowClick][columnClick].isSelected = false;
                } else {
                    matrixColors[rowClick][columnClick].isSelected = true;
                }
            }

        }
    }

    bool testPointCollision(float RefenceX,float RefenceY, float Bx,float By, float Cx,float Cy, float Px, float Py){
        float ABx = Bx-RefenceX;
        float ABy = By-RefenceY;
        float ABmodule = sqrt(pow(ABx,2)+pow(ABy,2));

        float normalABx = ABx / ABmodule;
        float normalABy = ABy / ABmodule;

        float ACx = Cx-RefenceX;
        float ACy = Cy-RefenceY;
        float ACmodule = sqrt(pow(ACx,2)+pow(ACy,2));

        float normalACx = ACx / ACmodule;
        float normalACy = ACy / ACmodule;

        float APx = Px-RefenceX;
        float APy = Py-RefenceY;
        float APmodule = sqrt(pow(APx,2)+pow(APy,2));

        float normalAPx = APx / APmodule;
        float normalAPy = APy / APmodule;

        float theta = acos(normalABx * normalAPx + normalABy * normalAPy);
        float alpha = acos(normalABx * normalACx + normalABy * normalACy);
        float betha = acos(normalACx * normalAPx + normalACy * normalAPy);

       // bool collide = alpha == (theta + betha);
        bool collide = 0.01>abs(alpha - (theta + betha));
        return collide;
    }

    void draw(Shader *shaderProgram) {

        // Define shaderProgram como o shader a ser utilizado
        shaderProgram->UseProgramShaders();

        // Define em quais vertices sera desenhado pelo shader
        vertices->bind(shaderProgram);

        for (int row = 0; row < numRows; row++) {
            for (int col = 0; col < numCols; col++) {

                //linear
                //float xi = col*tileWidth;
                //float yi = row*tileHeight;

                //slidemap
                //float xi = col*tileWidth  + row *(tileWidth/2.0f) ;
                //float yi = row*tileHeight/2.0f ;

                //diamond map
                float xi = row * (tileWidth/2.0f) + col * (tileWidth/2.0f);
                float yi = row * (tileHeight/2.0f) - col * (tileHeight/2.0f) + sum_heigth/2.0f - (tileHeight/2.0f);

                modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(xi, yi, 0.0));

                //define aonde desenhar
                glUniformMatrix4fv(
                        glGetUniformLocation(shaderProgram->Program, "matrix_OBJ"), 1,
                        GL_FALSE, glm::value_ptr(modelMatrix));



                //seleciona o tile a ser desenhado
                Tile tile = matrixColors[row][col];


                if(!tile.isSelected){
                    glUniform3fv(
                            glGetUniformLocation(shaderProgram->Program, "colorValues"), 1,
                            glm::value_ptr(tile.colorsRGB));
                } else {
                    glUniform3fv(
                            glGetUniformLocation(shaderProgram->Program, "colorValues"), 1,
                            glm::value_ptr(glm::vec3(10,255,10)));
                }


                // Define em quais vertices sera desenhado pelo shader
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

    }

};

