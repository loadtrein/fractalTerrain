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

		enum {   
		  SEGMENTS = 257,
		  S_SEGMENTS = SEGMENTS*2, 
		};

		color_shader color_shader_;
		texture_shader_alpha texture_shader_;

		PerlinNoiseGenerator perlinNoise;
		terrain_mesh_handler terrain_mesh_handler_;

		Point heightMap [SEGMENTS] [SEGMENTS];

		Point seaMap[S_SEGMENTS] [S_SEGMENTS];

		Tile wireFrameVertices [(SEGMENTS-1)*(SEGMENTS-1)];

		mat4t cameraToWorld;
		mat4t modelToWorld;

    float terrain_width;
    float terrain_length;
		float randomLow;
		float randomHigh;

		float deltaHeight;
    float max_height;
		float min_height;

    float cameraFactor;

		int renderMode;
    int wire;
		int obj_render;
		bool debug;

    //Hud font
    GLuint font_texture;

    // information for our text
    bitmap_font font;

    bool showText;

	  public:
		Landscape(int argc, char **argv) : app(argc, argv), font(512, 256, "assets/big.fnt") {}

		// this is called once OpenGL is initialized
		void app_init() {
		    color_shader_.init();
        texture_shader_.init();
			
			  //initialize terrain_mesh
			  terrain_mesh_handler_.init();

		    cameraToWorld.loadIdentity();
		    modelToWorld.loadIdentity();

		    srand (static_cast <unsigned> (time(0)));

		    setTerrainParameters();

        cameraToWorld.translate(this->terrain_width/2,6,this->terrain_length*1.6);
      
		    setMapsInitialValues();

		    setInitialCorners();

		    setPointsAsExistingValues();

		    diamondSquareAlgorithm();

			  calculateDeltaHeight();

		    generateVerticesWireFrameModel();
	  
			  create_meshes();

			  obj_render = 0;

        font_texture = resources::get_texture_handle(GL_RGBA, "assets/big_0_black.gif");

		}

		void create_meshes() {

			lower_perimeters();

			terrain_mesh_handler_.create_mesh_from_map(S_SEGMENTS, *seaMap, 1);
			terrain_mesh_handler_.create_mesh_from_map(SEGMENTS, *heightMap, 0);
			
		}


		void wireFrameRendering(Tile tile){
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
							heightMap[i+u][j+v].setY( heightMap[i+u][j+v].getY() + 0.5*(dmax - talus) * (di/dtotal));
					    } 
					  }
				  }
				}
			  }

			}
		  }
		}


		//---------------------------------SMOOTHING FILTER----------------------------------------------------------

		//Apply 3x3 box filter with smoothing parameter
		void smooth3x3BoxFilter() {

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
			  heightMap[i][j].setY(total / count);
			}
		  }
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

		float getSquareGeneratedValue( int row, int column, int distance, float rLow, float rHigh ) {
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

						int y_ = int(heightMap[row][column].getY());

						 if(y_ == 0.0f){
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

						float y_1 = heightMap[row1][column1].getY();

						if(y_1 == 0.0f){
						  heightMap[row1][column1].setY(getSquareGeneratedValue(row1,column1,distance/2,rLow,rHigh));
						  heightMap[row1][column1].setJustGenerated(true);

						}

						int row2 = k + distance/2;
						int column2 = l;

						float y_2 = heightMap[row2][column2].getY();

						if(y_2 == 0.0f){
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

    void calculateDeltaHeight() 
    {
      for(int i=0; i!=SEGMENTS;++i){
        for(int j=0; j!=SEGMENTS;++j){
          if(heightMap[i][j].getY() > this->max_height ){
            this->max_height = heightMap[i][j].getY();
          }

          if(heightMap[i][j].getY() < this->min_height ){
            this->min_height = heightMap[i][j].getY();
          }
        }
      }

      this->deltaHeight =  this->max_height - this->min_height;

      setSeaLevel();
    }

    void setSeaLevel(){

      for(int i=0; i!=S_SEGMENTS;++i){
        for(int j=0; j!=S_SEGMENTS;++j){
          seaMap[i][j].setY(this->min_height+(this->deltaHeight*0.20f));
        }
      }
    }


	void setPointsAsExistingValues() {
		for(int i=0; i!=SEGMENTS;++i){
			for(int j=0; j!=SEGMENTS;++j){
				heightMap[i][j].setJustGenerated(false);
			}
		}
	}


	void lower_perimeters() {
		 for(int i=0; i<SEGMENTS; ++i){
			 heightMap[i][0].setY(min_height);
			 heightMap[i][SEGMENTS-1].setY(min_height);
			 heightMap[0][i].setY(min_height);
			 heightMap[SEGMENTS-1][i].setY(min_height);
		 }
	}



		void setInitialCorners() {
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

		void setMapsInitialValues() {
		  float widthTerrainIncrement = ((float)this->terrain_width)/((float)SEGMENTS-1.0f);
		  float lenghtTerrainIncrement = ((float)this->terrain_length)/((float)SEGMENTS-1.0f);

      float widthSeaIncrement = ((float)this->terrain_width - widthTerrainIncrement*2)/((float)S_SEGMENTS-1.0f);
      float lenghtSeaIncrement = ((float)this->terrain_length - lenghtTerrainIncrement*2)/((float)S_SEGMENTS-1.0f);

		  for(int i=0; i!=SEGMENTS;++i){
        for(int j=0; j!=SEGMENTS;++j){
          heightMap[i][j].setX((float)(widthTerrainIncrement*((float)i)));
          heightMap[i][j].setY(0.0);
          heightMap[i][j].setZ((float)(lenghtTerrainIncrement*((float)j)));
        }
		  }


      float seaValueX = heightMap[1][1].getX();
      float setValueZ = heightMap[1][1].getZ();

      for(int i=0; i!=S_SEGMENTS;++i){
        for(int j=0; j!=S_SEGMENTS;++j){
          seaMap[i][j].setX(seaValueX);
          seaMap[i][j].setY(0.0);
          seaMap[i][j].setZ(setValueZ);
          setValueZ+=lenghtSeaIncrement;
        }
        setValueZ = heightMap[1][1].getZ();
        seaValueX+=widthSeaIncrement;
      }
      
		}



		void setTerrainParameters(){

      this->terrain_width = 400;
      this->terrain_length = 400;
		  this->randomLow = -100.0f;
		  this->randomHigh = 100.0f;
      
      resetMaxMinDeltaHeight();


      this->cameraFactor = 2.5f;

		  this->debug = false;
		  this->renderMode = 0;
      this->wire = 0;

      this->showText = true;

		}

    void resetMaxMinDeltaHeight() 
    {
      this->deltaHeight = 0.0f;
      this->max_height = 0.0f;
      this->min_height = 0;
    }


    //Draw text on the screen
    void draw_text(texture_shader &shader, float x, float y, float z, float scale, const char *text) {
      mat4t modelToWorld = cameraToWorld;
      modelToWorld.translate(x, y, z);
      modelToWorld.scale(scale, scale, 1);
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

      enum { max_quads = 80 };
      bitmap_font::vertex vertices[max_quads*4];
      uint32_t indices[max_quads*6];
      aabb bb(vec3(0, 0, 0), vec3(256, 256, 0));

      unsigned num_quads = font.build_mesh(bb, vertices, indices, max_quads, text, 0);
      glActiveTexture(GL_TEXTURE8);
      glBindTexture(GL_TEXTURE_2D, font_texture);

      shader.render(modelToProjection, 8);

      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].x );
      glEnableVertexAttribArray(attribute_pos);
      glVertexAttribPointer(attribute_uv, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].u );
      glEnableVertexAttribArray(attribute_uv);

      glDrawElements(GL_TRIANGLES, num_quads * 6, GL_UNSIGNED_INT, indices);
    } 


		void simulate() {
			  keyboard();
		}


		void keyboard() {
		  if(is_key_down('D')){
			cameraToWorld.translate(cameraFactor,0,0);
		  }

		  if(is_key_down('A')){
			cameraToWorld.translate(-cameraFactor,0,0);
		  }

		  if(is_key_down('W')){
			cameraToWorld.translate(0,0,-cameraFactor);
		  }

		  if(is_key_down('S')){
			cameraToWorld.translate(0,0,cameraFactor);
		  }

		  if (is_key_down('I')) {
			  cameraToWorld.rotateX(-2.5);
		  }

		  if (is_key_down('O')) {
			  cameraToWorld.rotateX(2.5);
		  }

		  if(is_key_down(key_up)){
			cameraToWorld.translate(0,cameraFactor,0);
		  }

		  if(is_key_down(key_down)){
			cameraToWorld.translate(0,-cameraFactor,0);
		  }

		  if(is_key_down('E')){
			cameraToWorld.rotateY(-2.5);
		  }

		  if(is_key_down('Q')){
			cameraToWorld.rotateY(2.5);
		  } 


		  //Default camera view
		  if(is_key_down(key_space)){
			cameraToWorld.loadIdentity();
			cameraToWorld.translate(this->terrain_width/2,6,this->terrain_length*1.6);
		  }

		  if(is_key_down(key_f1)){
        printf("3x3 Box Smoothed\n");
        smooth3x3BoxFilter();
        generateVerticesWireFrameModel();
        lower_perimeters();
        terrain_mesh_handler_.create_mesh_from_map(SEGMENTS, *heightMap, 0);
		  }

		  if(is_key_down(key_f2)){
        printf("PERTURBATION\n");
        perturbation(10.0,10.0);
        generateVerticesWireFrameModel();
        lower_perimeters();
        terrain_mesh_handler_.create_mesh_from_map(SEGMENTS, *heightMap, 0);
		  }

		  if(is_key_down(key_f3)){
        printf("THERMAL EROSION\n");
        thermalErosion(4/(float)SEGMENTS);
        generateVerticesWireFrameModel();
        lower_perimeters();
        terrain_mesh_handler_.create_mesh_from_map(SEGMENTS, *heightMap, 0);
		  }

		  //Regenerates terrain
		  if(is_key_down('G')){

			printf("Regenerating terrain...\n");

			setMapsInitialValues();

			setInitialCorners();

			diamondSquareAlgorithm();

      resetMaxMinDeltaHeight();

			calculateDeltaHeight();

			generateVerticesWireFrameModel();
      
			create_meshes(); 

			printf("Regenerated... \n");
		  }


		  if(is_key_down('M')) {
			  if(renderMode>2)
				  renderMode =0;
			  else renderMode++; 
		  }

      if(is_key_down('Z')){
        this->terrain_length+=10;
        this->cameraFactor+=0.05;
      }

      if(is_key_down('X')){
        if(terrain_length > 10){
          this->terrain_length-=10;
          if(cameraFactor > 2.5){
            this->cameraFactor-=0.05;
          }

        }
      }

      if(is_key_down('C')){
        this->terrain_width+=10;
        this->cameraFactor+=0.05;
      }

      if(is_key_down('V')){
        if(terrain_width > 10){
          this->terrain_width-=10;
          if(cameraFactor > 2.5){
            this->cameraFactor-=0.05;
          }
        }
      }

      if(is_key_down('B')){
        if(randomHigh >= 0 && randomLow <= 0){
          this->randomHigh+=10;
          this->randomLow-=10;
        }
      }

      if(is_key_down('N')){
        if(randomHigh > 0 && randomLow < 0){
          this->randomHigh-=10;
          this->randomLow+=10;
        }
      }

      if(is_key_down('T')){
        if(this->showText){
          this->showText = false;
        }else{
          this->showText = true;
        }
      }

		  if (is_key_down('1')) {
			  obj_render = 0;
		  }

		  if (is_key_down('2')) {
			  obj_render = 1;
		  }

		  if (is_key_down('3')) {
			  obj_render = 2;
		  }

      if (is_key_down('4')) {
        wire = 0;
      }

      if (is_key_down('5')) {
        wire = 1;
      }



		}

			// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {
			simulate();

			// set a viewport - includes whole window area
			glViewport(x, y, w, h);

			// background color
			glClearColor(0.53f, 0.81f, 0.98f, 1);
			//glClearColor(0, 0, 0, 1);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
			// allow Z buffer depth testing (closer objects are always drawn in front of far ones)
			glEnable(GL_DEPTH_TEST);
      
         
			// allow alpha blend (transparency when alpha channel is 0)
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


      if(this->showText){

        char title[25];

        sprintf(title, "--Terrain parameters--\n");

        draw_text(texture_shader_, - 1.8, + 1.8, - 3 ,1.0f/256, title);

        char text[100];

        sprintf(text, "Length: %d\nWidth: %d\nRoughness: %d",((int)this->terrain_length), ((int)this->terrain_width), ((int)this->randomHigh));

        draw_text(texture_shader_, - 1.8, + 1.2, - 3 ,1.0f/256, text);
      }



      if(wire == 1){
      	  for(int i=0;i!=sizeof(wireFrameVertices)/sizeof(wireFrameVertices[0]);++i){
      			  wireFrameRendering(wireFrameVertices[i]);
      		  } 
      }else{

        terrain_mesh_handler_.render(modelToWorld, cameraToWorld, renderMode, min_height, deltaHeight, obj_render);

      }

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
			  
			
		}

  };
}  
