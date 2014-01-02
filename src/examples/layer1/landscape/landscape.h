////////////////////////////////////////////////////////////////////////////////
//
// (C) M. Dimitri Alvarez 2013
//
// Fractal Landscape
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <cmath>
#include <math.h> 

     
namespace octet {


	class Landscape : public octet::app {
		

    terrain_mesh_handler terrain_mesh_handler_;

	 mat4t cameraToWorld;


    mat4t modelToWorld;

    color_shader color_shader_;
	  
	  // terrain shader
	  // terrain_shader terrain_shader_; 
	  terrain_new_shader terrain_new_shader_;

	  //phong_shader phong_shader_;

    PerlinNoiseGenerator perlinNoise;

    enum {   
      Terrain_Width = 100,
      Terrain_Length = 100,
      SEGMENTS = 65,
    };

    float randomLow;
    float randomHigh;
    bool debug;


    Point heightMap [SEGMENTS] [SEGMENTS];

    Tile wireFrameVertices [(SEGMENTS-1)*(SEGMENTS-1)];



  public:

    Landscape(int argc, char **argv) : app(argc, argv) {}

    // this is called once OpenGL is initialized
    void app_init() {
    
    color_shader_.init();

	  //terrain_shader_.init(); 
	  
	  //terrain_new_shader_.init();


	  // load textures 
	  GLuint texture_grass	= resources::get_texture_handle(GL_RGBA, "assets/terrain/grass.gif");
	  GLuint texture_sand	= resources::get_texture_handle(GL_RGBA, "assets/terrain/sand.gif"); 

	  //initialize terrain_mesh
	  //terrain_mesh_handler_.init(texture_grass, texture_sand); 

      cameraToWorld.loadIdentity();
      cameraToWorld.translate(Terrain_Width/2,6,Terrain_Length*1.6);
      modelToWorld.loadIdentity();

      srand (static_cast <unsigned> (time(0)));

      setTerrainParameters();
      
      setHeightMapInitialValues();

      setInitialCorners();

      setPointsAsExistingValues();

      diamondSquareAlgorithm();

      generateVerticesWireFrameModel();
	  
	  
	    //terrain_mesh_handler_.create_mesh(wireFrameVertices, sizeof(wireFrameVertices)/sizeof(wireFrameVertices[0])); 

    }

    void renderMap(Tile tile){
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
      
      // set up the uniforms for the shader
      color_shader_.render(modelToProjection, vec4(0.0,0.0,0.0,1));


      float vertices[] = {
        tile.points[0].getX(),  tile.points[0].getY(), tile.points[0].getZ(),
        tile.points[1].getX(),  tile.points[1].getY(), tile.points[1].getZ(),
        tile.points[2].getX(),  tile.points[2].getY(), tile.points[2].getZ(),
        tile.points[3].getX(),  tile.points[3].getY(), tile.points[3].getZ(),
      };

        // attribute_pos (=0) is position of each corner
        // each corner has 3 floats (x, y, z)
        // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
        glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices);
        glEnableVertexAttribArray(attribute_pos);
        
        glDrawArrays(GL_LINE_LOOP, 0, 4);

    } 

    void generateVerticesWireFrameModel(){

      int index=0;

      for(int i=0; i!=SEGMENTS;++i){
        for(int j=0; j!=SEGMENTS;++j){

          if( i<(SEGMENTS-1) && j<(SEGMENTS-1)){

            wireFrameVertices[index].points[0] = heightMap[i][j];
            wireFrameVertices[index].points[1] = heightMap[i+1][j];
            wireFrameVertices[index].points[2] = heightMap[i+1][j+1];
            wireFrameVertices[index].points[3] = heightMap[i][j+1];

            index++;

          }

        }
      }
    }

    void printHeightMap(){

      printf("\n");

      for(int i=0; i!=SEGMENTS;++i){
        for(int j=0; j!=SEGMENTS;++j){
          printf("%.2f ",heightMap[i][j].getY());
        }
        printf("\n");
      }

      printf("\n");
    }

    //------------------------------------PERTURBATION-----------------------------------------------------------

    void perturbation(float f, float d){
      int u, v;
      
      float temp [SEGMENTS][SEGMENTS];

      for (int i = 0; i != SEGMENTS; ++i){
        for (int j = 0; j != SEGMENTS; ++j){

          u = i + (int)(perlinNoise.Noise(f * i / (float)SEGMENTS, f * j / (float)SEGMENTS, 0) * d);
          v = j + (int)(perlinNoise.Noise(f * i / (float)SEGMENTS, f * j / (float)SEGMENTS, 1) * d);
          if (u < 0) u = 0; if (u >= SEGMENTS) u = SEGMENTS - 1;
          if (v < 0) v = 0; if (v >= SEGMENTS) v = SEGMENTS - 1;
          temp[i][j]=heightMap[u][v].getY();

        }
      }

      for (int i = 0; i != SEGMENTS; ++i){
        for (int j = 0; j != SEGMENTS; ++j){
          heightMap[i][j].setY(temp[i][j]);
        }
      }
    }

    //------------------------------------EROSION-----------------------------------------------------------------

    void thermalErosion(float talus){
      
      for(int i=0; i!=SEGMENTS;++i){
        for(int j=0; j!=SEGMENTS;++j){

          float dmax = 0.0f;
          float dtotal = 0.0;

          for (int u = -1; u <= 1; u++){
            for (int v = -1; v <= 1; v++){

              if((i+u) >= 0 && (i+u)<SEGMENTS && (j+v)>=0 && (j+v)<SEGMENTS){
                if(u!=0 || v!=0){

                  float di = heightMap[i][j].getY() - heightMap[i+u][j+v].getY();

                  if(di > talus){
                    dtotal+=di;

                    if(di > dmax){
                      dmax = di;
                    }
                  } 
                }
              }

            }
          }

          for (int u = -1; u <= 1; u++){
            for (int v = -1; v <= 1; v++){

              if((i+u) >= 0 && (i+u)<SEGMENTS && (j+v)>=0 && (j+v)<SEGMENTS){
                if(u!=0 || v!=0){

                  float di = heightMap[i][j].getY() - heightMap[i+u][j+v].getY();

                  if(di > talus){
                    heightMap[i+u][j+v].setY( heightMap[i+u][j+v].getY() + 0.3*(dmax - talus) * (di/dtotal));
                  } 
                }
              }

            }
          }

         
        }
      }
    }

    //---------------------------------SMOOTHING FILTERS----------------------------------------------------------

    //Apply 3x3 box filter with smoothing parameter
    void smooth3x3BoxFilter(){

      float k = 0.75;

      for(int i=0; i!=SEGMENTS;++i){
        for(int j=0; j!=SEGMENTS;++j){

          float total = 0.0f;
          float count = 0.0;

          for (int u = -1; u <= 1; u++){
            for (int v = -1; v <= 1; v++){

              if((i+u) >= 0 && (i+u)<SEGMENTS && (j+v)>=0 && (j+v)<SEGMENTS){
                if(u!=0 || v!=0){
                  total +=  heightMap[i + u][j + v].getY();
                  count++;
                }
              }

            }
          }
          heightMap[i][j].setY((total / count)* (1-k) +  heightMap[i][j].getY() * k);
        }
      }

    }

    void smootFilterRowsColumnsDisplacement(){
      float k = 0.75;

      /* Rows, left to right */
      for(int i = 1;i < SEGMENTS; i++)
        for (int j = 0;j < SEGMENTS; j++)
          heightMap[i][j].setY( heightMap[i-1][j].getY() * (1-k) + heightMap[i][j].getY() * k );

      /* Rows, right to left*/
      for(int i = SEGMENTS-2;i < -1; i--)
        for (int j = 0;j < SEGMENTS; j++)
          heightMap[i][j].setY( heightMap[i+1][j].getY() * (1-k) +  heightMap[i][j].getY() * k);

      /* Columns, bottom to top */
      for(int i = 0;i < SEGMENTS; i++)
        for (int j = 1;j < SEGMENTS; j++)
          heightMap[i][j].setY( heightMap[i][j-1].getY() * (1-k) +  heightMap[i][j].getY() * k);

      /* Columns, top to bottom */
      for(int i = 0;i < SEGMENTS; i++)
        for (int j = SEGMENTS; j < -1; j--)
          heightMap[i][j].setY( heightMap[i][j+1].getY() * (1-k) +  heightMap[i][j].getY() * k );
    }


    //---------------------------------DIAMOND SQUARE ALGORITHM--------------------------------------------------

    float getDiamondGeneratedValue(int row, int column, int distance, float rLow, float rHigh){

      float generatedValue =0.0;

      //We get the mean
      generatedValue = ( heightMap[std::abs(row-distance)][std::abs(column-distance)].getY() + 
                         heightMap[std::abs(row-distance)][std::abs(column+distance)].getY() + 
                         heightMap[std::abs(row+distance)][std::abs(column-distance)].getY() +
                         heightMap[std::abs(row+distance)][std::abs(column+distance)].getY() ) / 4;


      float r = rLow + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(rHigh-rLow)));

      //printf("Random number Diamond: %.2f\n\n",r);

      generatedValue += r;

      return generatedValue;
    }

    float getSquareGeneratedValue( int row, int column, int distance, float rLow, float rHigh ) 
    {
      float generatedValue =0.0;
      int counter = 0;

      if(column + distance < SEGMENTS){
        counter++;
        generatedValue+= heightMap[std::abs(row)][std::abs(column+distance)].getY();
      }

      if(row + distance < SEGMENTS){
        counter++;
        generatedValue+= heightMap[std::abs(row+distance)][std::abs(column)].getY();
      }

      if(column - distance >= 0){
        counter++;
        generatedValue+= heightMap[std::abs(row)][std::abs(column-distance)].getY();
      }

      if(row - distance >= 0){
        counter++;
        generatedValue+= heightMap[std::abs(row-distance)][std::abs(column)].getY();
      }

      //We get the mean
      generatedValue = generatedValue / counter;

      float r = rLow + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(rHigh-rLow)));

     //printf("Number Square: %.2f\n\n",generatedValue);

      generatedValue += r;

      return generatedValue;
    }

    void diamondSquareAlgorithm(){

      int distance = SEGMENTS-1;
      float rHigh = this->randomHigh;
      float rLow = this->randomLow;

      for(int iteration=0;iteration!=std::log10((double)SEGMENTS-1)/std::log10((double)2.0);++iteration){
        
          if(this->debug){  
            // Diamond step
            printf("Diamond step %d \n",iteration+1);
          }

          for(int i=0; i<SEGMENTS;i+=distance){
            for(int j=0; j<SEGMENTS;j+=distance){
          
              if(heightMap[i][j].getY() != 0.0f && !heightMap[i][j].isJustGenerated()){
                if( (i + distance < SEGMENTS) && (j + distance < SEGMENTS)){

                    int row = i + distance/2;
                    int column = j + distance/2;

                     if(heightMap[row][column].getY() == 0.0f){

                       heightMap[row][column].setY(getDiamondGeneratedValue(row,column,distance/2,rLow,rHigh));
                       heightMap[row][column].setJustGenerated(true);
                     }
                }
              }

            }
          }

          setPointsAsExistingValues();

          if(this->debug){  

            printHeightMap();

            // Square step
            printf("Square step %d \n",iteration+1);

          }

          for(int k=0; k<SEGMENTS;k+=distance/2){
            for(int l=0; l<SEGMENTS;l+=distance/2){

              if(heightMap[k][l].getY() != 0.0f && !heightMap[k][l].isJustGenerated()){
                if( (k + distance/2 < SEGMENTS) && (l + distance/2 < SEGMENTS)){

                    int row1 = k;
                    int column1 = l + distance/2;

                    if(heightMap[row1][column1].getY() == 0.0f){

                      heightMap[row1][column1].setY(getSquareGeneratedValue(row1,column1,distance/2,rLow,rHigh));
                      heightMap[row1][column1].setJustGenerated(true);
                    }

                    int row2 = k + distance/2;
                    int column2 = l;

                    if(heightMap[row2][column2].getY() == 0.0f){

                      heightMap[row2][column2].setY(getSquareGeneratedValue(row2,column2,distance/2,rLow,rHigh));
                      heightMap[row2][column2].setJustGenerated(true);
                    }
                
                }
              }
            }
          }

          setPointsAsExistingValues();
          

          if(this->debug){  
           printHeightMap();
          }

          
          distance /= 2;

          rLow /= 2;
          rHigh /= 2;

      }
          
    }

    void setPointsAsExistingValues(){
      for(int i=0; i!=SEGMENTS;++i){
        for(int j=0; j!=SEGMENTS;++j){
          heightMap[i][j].setJustGenerated(false);
        }
      }
    }

    void setInitialCorners() 
    {

      heightMap[0][0].setY( static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10.0f)) );
      heightMap[0][SEGMENTS-1].setY( static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10.0f)) );
      heightMap[SEGMENTS-1][0].setY( static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10.0f)) );
      heightMap[SEGMENTS-1][SEGMENTS-1].setY( static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/10.0f)) );


      /*heightMap[0][0].setY(0.1);
      heightMap[0][SEGMENTS-1].setY(0.1);
      heightMap[SEGMENTS-1][0].setY(0.1);
      heightMap[SEGMENTS-1][SEGMENTS-1].setY(0.1);*/
    
    }


    //-----------------------------------HEIGHT MAP INITIAL VALUES-----------------------------------------------

    void setHeightMapInitialValues(){

      float widhtIncrement = ((float)Terrain_Width)/((float)SEGMENTS-1.0f);
      float lenghtIncrement = ((float)Terrain_Length)/((float)SEGMENTS-1.0f);

      for(int i=0; i!=SEGMENTS;++i){
        for(int j=0; j!=SEGMENTS;++j){
          heightMap[i][j].setX((float)(widhtIncrement*((float)i)));
          heightMap[i][j].setY(0.0);
          heightMap[i][j].setZ((float)(lenghtIncrement*((float)j)));
        }
      }

    }

    void setTerrainParameters(){
      this->randomLow = -50.0;
      this->randomHigh = 50.0f;
      this->debug = false;
    }


    void simulate() {
		keyboard();
	}

	void keyboard() {
      if(is_key_down('D')){
        cameraToWorld.translate(1.5,0,0);
      }

      if(is_key_down('A')){
        cameraToWorld.translate(-1.5,0,0);
      }

      if(is_key_down('W')){
        cameraToWorld.translate(0,0,-1.5);
      }

      if(is_key_down('S')){
        cameraToWorld.translate(0,0,1.5);
      }

      if(is_key_down(key_up)){
        cameraToWorld.translate(0,+1.5,0);
      }

      if(is_key_down(key_down)){
        cameraToWorld.translate(0,-1.5,0);
      }

      if(is_key_down('E')){
        cameraToWorld.rotateY(-2.0);
      }

      if(is_key_down('Q')){
        cameraToWorld.rotateY(2.0);
      } 


      //Default camera view
      if(is_key_down(key_space)){
        cameraToWorld.loadIdentity();
        cameraToWorld.translate(Terrain_Width/2,6,Terrain_Length*1.6);
      }

      if(is_key_down(key_f1)){
        printf("Rows and Columns Smoothed\n");
        smootFilterRowsColumnsDisplacement();
        generateVerticesWireFrameModel();
      }

      if(is_key_down(key_f2)){
        printf("3x3 Box Smoothed\n");
        smooth3x3BoxFilter();
        generateVerticesWireFrameModel();
      }

      if(is_key_down(key_f3)){
        printf("PERTURBATION\n");
        perturbation(10.0,10.0);
        generateVerticesWireFrameModel();
      }

      if(is_key_down(key_f4)){
        printf("THERMAL EROSION\n");
        thermalErosion(4/(float)SEGMENTS);
        generateVerticesWireFrameModel();
      }

      //Regenerates terrain
      if(is_key_down('G')){

        printf("Regenerating terrain...\n");

        setHeightMapInitialValues();

        setInitialCorners();

        diamondSquareAlgorithm();

        generateVerticesWireFrameModel();

        printf("Regenerated...\n");
      }
    }

		// this is called to draw the world
	void draw_world(int x, int y, int w, int h) {
		simulate();

		// set a viewport - includes whole window area
		glViewport(x, y, w, h);

		// background color
		glClearColor(0.93f, 0.94f, 0.85f, 1);
		//glClearColor(0, 0, 0, 1);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
		// allow Z buffer depth testing (closer objects are always drawn in front of far ones)
		glEnable(GL_DEPTH_TEST);
      
         
		// allow alpha blend (transparency when alpha channel is 0)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	  
		// draw the Landscape
		for(int i=0;i!=sizeof(wireFrameVertices)/sizeof(wireFrameVertices[0]);++i){
			renderMap(wireFrameVertices[i]);
		}

		// draw the mesh
		//terrain_mesh_handler_.debugRender(terrain_new_shader_, modelToWorld, cameraToWorld); 

    }

  };

}  
