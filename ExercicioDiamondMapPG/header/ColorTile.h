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
    int lastTileSelectedCol, lastTileSelectedRow;
    glm::mat4 modelMatrix;

    VerticesObject* vertices;

    Tile matrixColors[ROWS][COLS]= {};

    ColorTiles(float tileWidth, float tileHeight,float numCols, float numRows)
    {
        this->tileWidth = tileWidth;
        this->numCols = numCols;
        this->tileHeight = tileHeight;
        this->numRows = numRows;
        this->sum_heigth = numRows*tileHeight;
        this->modelMatrix = glm::mat4(1);
        this-> lastTileSelectedCol = -1;
        this-> lastTileSelectedRow = -1;


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
        //slide map
        //int rowClick = (int) (yPos / (tileHeight/2.0));
        //int columnClick = (int) ((xPos - (rowClick * (tileWidth/2.0)))/tileWidth);

        //diamond map projecao do click


        double x = (double) xPos;
        double y = ((double) yPos) - (((double) sum_heigth) / 2.0);
        double tw = (double) tileWidth;
        double th = (double) tileHeight;
        double row = (((2.0 * y / th) + (2.0 * xPos / tw))) / 2.0;
        double col = (2.0 * xPos / tw) - row;

        int rowClick = (int) row;
        int columnClick = (int) col;

//        int rowClick = (int) round(row);
//        int columnClick = (int) round(col);


        if (rowClick < 0 || columnClick < 0 || columnClick >= numCols || rowClick >= numRows)
            return;


        //slide map
        //float x0 = ((float)columnClick)*tileWidth  + ((float)rowClick) *(tileWidth/2.0f) ;
        //float y0 = ((float)rowClick)*tileHeight/2.0f ;

        //diamond
        float x0 = (float) (rowClick * (tileWidth / 2.0f) + columnClick * (tileWidth / 2.0f));
        float y0 = (float) ((rowClick - 1) * (tileHeight / 2.0f) - columnClick * (tileHeight / 2.0f) +
                            sum_heigth / 2.0f);


        //   _______B_______
        //   |              |
        // A |              |  D
        //   |              |
        //   |______________|
        //          C

        //
        //left point
        float Ax = x0;
        float Ay = y0 + tileHeight / 2.0f;

        //top point
        float Bx = x0 + tileWidth / 2.0f;
        float By = y0;

        //bottom point
        float Cx = x0 + tileWidth / 2.0f;
        float Cy = y0 + tileHeight;

        //right point
        float Dx = x0 + tileWidth;
        float Dy = y0 + tileHeight / 2.0f;

        bool isClickValid = false;

        if (DEBUG == 1) {
            printf("\nxPos: %f", xPos);
            printf("\nyPos: %f", yPos);
            printf("\nRow: %d", rowClick);
            printf("\nColumn: %d\n", columnClick);
            printf("\nx0: %f\n", x0);
            printf("\ny0: %f\n", y0);
            printf("\nleftPoint x %f", Ax);
            printf("\nleftPoint y %f\n", Ay);
            printf("\ntopPointX x %f", Bx);
            printf("\ntopPointY y %f\n", By);
            printf("\nbottomPointX x %f", Cx);
            printf("\nbottomPointY y %f\n", Cy);
            printf("\nrightPointX x %f", Dx);
            printf("\nrightPointY y %f\n", Dy);
        }

        if (testPointCollision(Ax, Ay, Bx, By, Cx, Cy, xPos, yPos))
            isClickValid = true;

        if(isClickValid==true){
            if (matrixColors[rowClick][columnClick].isVisible) {
                if (this->lastTileSelectedCol > -1
                    && this->lastTileSelectedRow > -1) {
                    this->matrixColors[lastTileSelectedRow][lastTileSelectedCol].isSelected = false;
                }

                    this->matrixColors[rowClick][columnClick].isSelected = true;
                    this->lastTileSelectedRow = rowClick;
                    this->lastTileSelectedCol = columnClick;
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
        bool collide = 0.001>abs(alpha - (theta + betha));
        return collide;
    }

    /*
      Função que responde às teclas pressionadas
  */
    void keyboard_reaction(int keys[1024]) {

        if (keys[GLFW_KEY_UP] == 1) {
            do_a_movement('e');
        } else if (keys[GLFW_KEY_DOWN] == 1) {
            do_a_movement('w');
        } else if (keys[GLFW_KEY_LEFT] == 1) {
            do_a_movement('n');
        } else if (keys[GLFW_KEY_RIGHT] == 1) {
            do_a_movement('s');
        }
    }

    void do_a_movement(char a) {
        if (a == 'n') {
            if (this->lastTileSelectedRow > 0) {
                this->matrixColors[this->lastTileSelectedRow][this->lastTileSelectedCol].isSelected = false;

                this->lastTileSelectedRow = this->lastTileSelectedRow - 1;
                this->lastTileSelectedCol = this->lastTileSelectedCol;

                this->matrixColors[this->lastTileSelectedRow][this->lastTileSelectedCol].isSelected = true;
            }
        } else if (a == 's') {
            if (this->lastTileSelectedRow < (numRows - 1)) {
                this->matrixColors[this->lastTileSelectedRow][this->lastTileSelectedCol].isSelected = false;

                this->lastTileSelectedRow = this->lastTileSelectedRow + 1;
                this->lastTileSelectedCol = this->lastTileSelectedCol;

                this->matrixColors[this->lastTileSelectedRow][this->lastTileSelectedCol].isSelected = true;
            }
        } else if (a == 'w') {
            if (this->lastTileSelectedCol > 0 && this->lastTileSelectedRow < (numRows - 1)) {
                this->matrixColors[this->lastTileSelectedRow][this->lastTileSelectedCol].isSelected = false;

                this->lastTileSelectedRow = this->lastTileSelectedRow + 1;
                this->lastTileSelectedCol = this->lastTileSelectedCol - 1;

                this->matrixColors[this->lastTileSelectedRow][this->lastTileSelectedCol].isSelected = true;
            }
        } else if (a == 'e') {
            if (this->lastTileSelectedCol < (numCols - 1) && this->lastTileSelectedRow > 0) {
                this->matrixColors[this->lastTileSelectedRow][this->lastTileSelectedCol].isSelected = false;

                this->lastTileSelectedRow = this->lastTileSelectedRow - 1;
                this->lastTileSelectedCol = this->lastTileSelectedCol + 1;

                this->matrixColors[this->lastTileSelectedRow][this->lastTileSelectedCol].isSelected = true;
            }
        }
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
                float yi = (row-1) * (tileHeight/2.0f) - col * (tileHeight/2.0f) + sum_heigth/2.0f;

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

