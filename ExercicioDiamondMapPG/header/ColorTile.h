#define DEBUG 1

#define DIRECTION_NO 1
#define DIRECTION_O 2
#define DIRECTION_SO 3
#define DIRECTION_N 4
#define DIRECTION_S 5
#define DIRECTION_NE 6
#define DIRECTION_E 7
#define DIRECTION_SE 8

class Tile {
public:
    glm::vec3 colorsRGB;
    bool isVisible;
    bool isSelected;
    bool isWalking;
    bool isMortal;

    //left point
    float Ax, Ay;
    //top point
    float Bx, By;
    //bottom point
    float Cx, Cy;
    //right point
    float Dx, Dy;

    Tile(){
        isVisible = true;
        isSelected = false;
    }

    Tile(float x0,float y0) {
        isVisible = true;
        isSelected = false;
        //left point
        Ax = x0;
        Ay = y0 + TILE_HEIGHT / 2.0f;
        //top point
        Bx = x0 + TILE_WIDTH / 2.0f;
        By = y0;
        //bottom point
        Cx = x0 + TILE_WIDTH / 2.0f;
        Cy = y0 + TILE_HEIGHT;

        //right point
        Dx = x0 + TILE_WIDTH;
        Dy = y0 + TILE_HEIGHT / 2.0f;
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


class ColorTiles {
public:

    float tileWidth, tileHeight;
    float numRows, numCols;
    float sum_tiles_heigth;
    int lastTileSelectedCol, lastTileSelectedRow;
    glm::mat4 modelMatrix;

    VerticesObject *vertices;

    Tile matrixColors[ROWS][COLS] = {};

    ColorTiles(float tileWidth, float tileHeight, float numCols, float numRows) {
        this->tileWidth = tileWidth;
        this->numCols = numCols;
        this->tileHeight = tileHeight;
        this->numRows = numRows;
        this->sum_tiles_heigth = numRows * tileHeight;
        this->modelMatrix = glm::mat4(1);
        this->lastTileSelectedCol = -1;
        this->lastTileSelectedRow = -1;


        this->setupVertices(tileWidth, tileHeight);

        this->createMatrixColors();
    }

    void setupVertices(float width, float height) {
        /*
            Comeca centralizado no zero
        */
        float verticesCoordinates[] = {
                // positions
                width / 2.0f, 0.0f, 0.0f,    // TOP
                width, height / 2.0f, 0.0f,    // RIGHT
                width / 2.0f, height, 0.0f,    // BOTTOM
                0.0f, height / 2.0f, 0.0f    // LEFT

        };

        vertices = new VerticesObject(verticesCoordinates, 12);
    }

    void calculoDesenhoDiamond(float &x, float &y, int row, int col) {
        x = row * (this->tileWidth / 2.0f) + col * (this->tileWidth / 2.0f);
        y = row * (this->tileHeight / 2.0f) - col * (this->tileHeight / 2.0f) + this->sum_tiles_heigth / 2.0f -
            (this->tileHeight / 2.0f);
    }

    void calculoDesenhoSlideMap(float &x, float &y, int row, int col) {
        x = ((float) col) * tileWidth + ((float) row) * (tileWidth / 2.0f);
        y = ((float) row) * tileHeight / 2.0f;
    }

    void calculoDesenhoLinear(float &x, float &y, int row, int col) {
        x = col*tileWidth;
        y = row*tileHeight;
    }

    void createMatrixColors(){
        for (int row = 0; row < numRows; row++) {
              for (int col = 0; col < numCols; col++) {
                    float x0,y0;
                    this->calculoDesenhoDiamond(x0,y0,row,col);
                    Tile t = Tile(x0,y0);
                    t.generateColor(row);
                    matrixColors[row][col] = t;
                  }
            }
    }

    void mouseMap(double xPos,double yPos) {
        //slide map
        //int rowClick = (int) (yPos / (tileHeight/2.0));
        //int columnClick = (int) ((xPos - (rowClick * (tileWidth/2.0)))/tileWidth);

        //diamond map projecao do click
        double x = (double) xPos;
        double y = ((double) yPos) - (((double) sum_tiles_heigth) / 2.0);
        double tw = (double) tileWidth;
        double th = (double) tileHeight;
        double row = (((2.0 * y / th) + (2.0 * xPos / tw))) / 2.0;
        double col = (2.0 * xPos / tw) - row;

        int rowClick = (int) row;
        int columnClick = (int) col;

        if (rowClick < 0 || columnClick < 0 || columnClick >= numCols || rowClick >= numRows)
            return;

        Tile tile = matrixColors[rowClick][columnClick];

        bool isClickValid = false;

        if (DEBUG == 1) {
            printf("\nxPos: %f", xPos);
            printf("\nyPos: %f", yPos);
            printf("\nRow: %d", rowClick);
            printf("\nColumn: %d\n", columnClick);
            printf("\nleftPoint x %f", tile.Ax);
            printf("\nleftPoint y %f\n", tile.Ay);
            printf("\ntopPointX x %f", tile.Bx);
            printf("\ntopPointY y %f\n", tile.By);
            printf("\nbottomPointX x %f", tile.Cx);
            printf("\nbottomPointY y %f\n", tile.Cy);
            printf("\nrightPointX x %f", tile.Dx);
            printf("\nrightPointY y %f\n", tile.Dy);
        }

        if (testPointCollision(tile.Ax, tile.Ay, tile.Bx, tile.By, tile.Cx, tile.Cy, xPos, yPos))
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
		
		//e==NO   r==N    t==NE
 		//d==O            g==E
		//c==SO   v==S    b==SE
		
        if (keys[GLFW_KEY_R] == 1) {
            do_a_movement(DIRECTION_N);
        } 
		else if (keys[GLFW_KEY_V] == 1) {
            do_a_movement(DIRECTION_S);
        } 
		else if (keys[GLFW_KEY_G] == 1) {
            do_a_movement(DIRECTION_E);
        } 
		else if (keys[GLFW_KEY_D] == 1) {
            do_a_movement(DIRECTION_O);
        } 
		else if (keys[GLFW_KEY_B] == 1) {
            do_a_movement(DIRECTION_SE);
        } 
		else if (keys[GLFW_KEY_E] == 1) {
            do_a_movement(DIRECTION_NO);
        }
		else if (keys[GLFW_KEY_T] == 1) {
            do_a_movement(DIRECTION_NE);
        } 
		else if (keys[GLFW_KEY_C] == 1) {
            do_a_movement(DIRECTION_SO);
        } 
	}

	void markLastTileSelected(bool flag){
		this->matrixColors[this->lastTileSelectedRow][this->lastTileSelectedCol].isSelected = flag;
	}

    void do_a_movement(int a) {
        if (a == DIRECTION_NO) {
            if (this->lastTileSelectedRow > 0) {
                markLastTileSelected(false);
                this->lastTileSelectedRow = this->lastTileSelectedRow - 1;
                markLastTileSelected(true);
            }
        }
        else if (a == DIRECTION_SE) {
            if (this->lastTileSelectedRow > -1 && this->lastTileSelectedRow < (numRows - 1)) {
                markLastTileSelected(false);
                this->lastTileSelectedRow = this->lastTileSelectedRow + 1;
                markLastTileSelected(true);
            }
        }
        else if (a == DIRECTION_NE) {
            if (this->lastTileSelectedCol > -1 && this->lastTileSelectedCol < (numCols - 1)) {
                markLastTileSelected(false);
                this->lastTileSelectedCol = this->lastTileSelectedCol + 1;
                markLastTileSelected(true);
            }
        }
        else if (a == DIRECTION_SO) {
            if (this->lastTileSelectedCol > 0) {
                markLastTileSelected(false);
                this->lastTileSelectedCol = this->lastTileSelectedCol- 1;
                markLastTileSelected(true);
            }
        }
		else if (a == DIRECTION_S) {
            if (this->lastTileSelectedCol > 0 && this->lastTileSelectedRow < (numRows - 1)) {
                markLastTileSelected(false);
                this->lastTileSelectedRow = this->lastTileSelectedRow + 1;
                this->lastTileSelectedCol = this->lastTileSelectedCol - 1;
                markLastTileSelected(true);
            }
        } 
		else if (a == DIRECTION_N) {
            if (this->lastTileSelectedCol < (numCols - 1) && this->lastTileSelectedRow > 0) {
                markLastTileSelected(false);
                this->lastTileSelectedRow = this->lastTileSelectedRow - 1;
                this->lastTileSelectedCol = this->lastTileSelectedCol + 1;
                markLastTileSelected(true);
            }
        }
		else if (a == DIRECTION_O) {
            if (this->lastTileSelectedCol > 0 && this->lastTileSelectedRow > 0) {
                markLastTileSelected(false);
                this->lastTileSelectedRow = this->lastTileSelectedRow - 1;
                this->lastTileSelectedCol = this->lastTileSelectedCol - 1;
                markLastTileSelected(true);
            }
        }
        else if (a == DIRECTION_E) {
            if (this->lastTileSelectedRow < (numRows - 1) && this->lastTileSelectedCol < (numCols - 1) && this->lastTileSelectedRow > -1) {
                markLastTileSelected(false);
                this->lastTileSelectedRow = this->lastTileSelectedRow + 1;
                this->lastTileSelectedCol = this->lastTileSelectedCol + 1;
                markLastTileSelected(true);
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
                float xi,yi;
                this->calculoDesenhoDiamond(xi,yi,row,col);


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

