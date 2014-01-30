namespace octet {

	class terrain_mesh_handler {
		terrain_shader terrain_shader_;
		sea_shader sea_shader_;
    sky_box_shader sky_box_shader_;
		dynarray<mesh*> terrainMeshes;
		dynarray<mesh*> seaMeshes;
		vec4 light_uniforms_array[5];
		vec3 light_rotation;
		int num_light_uniforms;
		int num_lights;
		GLuint textures[7];
		int t_length;
		float angle;

    mesh skyboxMesh;
    GLuint sky_box_textureObj;
    dynarray<uint8_t> buffer;
    dynarray<uint8_t> images;

	public:
		terrain_mesh_handler() {}


		void init() {
			terrain_shader_.init();
			sea_shader_.init();
      sky_box_shader_.init();

			light_rotation = vec3(45.0f, 30.0f, 0.0f);

			memset(light_uniforms_array, 0, sizeof(light_uniforms_array));
			light_uniforms_array[0] = vec4(0.1f, 0.1f, 0.1f, 50.0f);
			light_uniforms_array[2] = vec4(sin(light_rotation[0]*3.1415926f/180.0f), sin(light_rotation[1]*3.1415926f/180.0f), cos(light_rotation[0]*3.1415926f/180.0f), 0.0f);
			light_uniforms_array[3] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			light_uniforms_array[4] = vec4(1.0f, 0.0f, 0.0f, 1.0f);
			num_light_uniforms = 5;
			num_lights = 1;

			// load textures 
			textures[0]	= resources::get_texture_handle(GL_RGBA, "assets/terrain/sand.gif");
			textures[1]	= resources::get_texture_handle(GL_RGBA, "assets/terrain/grass.gif"); 
			textures[2]	= resources::get_texture_handle(GL_RGBA, "assets/terrain/rock.gif");
			textures[3] = resources::get_texture_handle(GL_RGBA, "assets/terrain/snow.gif");
			textures[4] = resources::get_texture_handle(GL_RGBA, "assets/terrain/sea2.gif");
			textures[5] = resources::get_texture_handle(GL_RGB, "#000000");
			textures[6] = resources::get_texture_handle(GL_RGB, "#111111");


      skyboxMesh.make_cube(1000.0f);
      sky_box_textureObj = 0;
      createCubeMap();


			angle = 0;
		}

    uint8_t* getImage(const char *url){

      buffer.reset();
      images.reset();

      app_utils::get_url(buffer,url);
      uint16_t format = 0;
      uint16_t width = 0;
      uint16_t height = 0;
      const unsigned char *src = &buffer[0];
      const unsigned char *src_max = src + buffer.size();

      gif_decoder dec;
      dec.get_image(images, format, width, height, src, src_max);

      return &images[0];
    }

    void createCubeMap(){

      glGenTextures(1, &sky_box_textureObj);
      glBindTexture(GL_TEXTURE_CUBE_MAP, sky_box_textureObj);

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, getImage("assets/terrain/skybox1.gif"));
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, getImage("assets/terrain/skybox2.gif"));
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, getImage("assets/terrain/skybox3.gif"));
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, getImage("assets/terrain/skybox4.gif"));
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, getImage("assets/terrain/skybox5.gif"));
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, getImage("assets/terrain/skybox6.gif"));

      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

		void create_mesh_from_map(int size,  Point * const map, int meshType) {
			if(meshType == 0){
				terrainMeshes.reset();
			}else if(meshType == 1){
				seaMeshes.reset();
				t_length = size; 
			}
			 
			int numMeshSegments = 0;
			int mesh_size = 0;

			if (size > 128) {
				int base = (int)size/128;
				int exponential = int(std::powf((float)base, 2.0f));
				numMeshSegments = exponential;//size/128 * exponential;
				mesh_size = 128;
			} else{
				numMeshSegments = 1;
				mesh_size = size-1;
			}
			
			int terrainSide_size = int(mesh_size*sqrt((float)numMeshSegments));

			for (int index_i=0; index_i<terrainSide_size; index_i+=mesh_size) {
				for (int index_j=0; index_j<terrainSide_size; index_j+=mesh_size) {

					mesh_builder m_builder;
					m_builder.init(mesh_size*4, mesh_size*6);
						
					for (int i=index_i; i<index_i+mesh_size; i++) {
						for (int j=index_j; j<index_j+mesh_size; j++) {
							add_face(size, i, j, &m_builder, map);
						}
					}

					mesh *t_mesh = new mesh();
					t_mesh->init();
					m_builder.get_mesh(*t_mesh);

          if(meshType == 0){
            this->terrainMeshes.push_back(t_mesh);
          }else if(meshType == 1){
            this->seaMeshes.push_back(t_mesh);
          }			
				}
			}
		}


		void add_face(int size, int i, int j,  mesh_builder *m_builder,  Point * const heightMap) {
			unsigned short cur_vertex = (unsigned short) m_builder->vertices.size(); 

			Point p0, p1, p2, p3;
			vec4 v0, v1, v2, v3;
			vec4 n0, n1, n2, n3;

			// get points
			v0 = vec4(	    heightMap[i*size + j].getX(),       heightMap[i*size + j].getY(),       heightMap[i*size + j].getZ(), 1);
			v1 = vec4(    heightMap[i*size + j+1].getX(),     heightMap[i*size + j+1].getY(),     heightMap[i*size + j+1].getZ(), 1);
			v2 = vec4(heightMap[(i+1)*size + j+1].getX(), heightMap[(i+1)*size + j+1].getY(), heightMap[(i+1)*size + j+1].getZ(), 1);
			v3 = vec4(   heightMap[(i+1)*size +j].getX(),    heightMap[(i+1)*size +j].getY(),    heightMap[(i+1)*size +j].getZ(), 1);

			// get normals 
			n0 = get_norm( size,   i,   j, heightMap);
			n1 = get_norm( size,   i, j+1, heightMap);
			n2 = get_norm( size, i+1, j+1, heightMap);
			n3 = get_norm( size, i+1,   j, heightMap);
			

			m_builder->add_vertex(v0, n0, ((float)i/size), ((float)j/size));
      m_builder->add_vertex(v1, n1, ((float)i/size), ((float)(j+1)/size));
      m_builder->add_vertex(v2, n2, ((float)(i+1)/size), ((float)(j+1)/size));
      m_builder->add_vertex(v3, n3, ((float)(i+1)/size), ((float)j/size)); 

			
			
			m_builder->indices.push_back(cur_vertex+0);
			m_builder->indices.push_back(cur_vertex+1);
			m_builder->indices.push_back(cur_vertex+2);
			m_builder->indices.push_back(cur_vertex+0);
			m_builder->indices.push_back(cur_vertex+2);
			m_builder->indices.push_back(cur_vertex+3);
		}


		vec4 get_norm( int size, int i, int j, Point * const heightMap) {
			vec4 v0, v1, v2, v3;
			vec4 neutro( 0, 0, 0, 1);

			v0 = v1 = v2 = v3 = neutro;

			if (i!=0) {
				// vector i,j & i-1,j
				v0 = vec4(
					heightMap[i*size + j].getX()-heightMap[(i-1)*size + j].getX(), 
					heightMap[i*size + j].getY()-heightMap[(i-1)*size + j].getY(),
					heightMap[i*size + j].getZ()-heightMap[(i-1)*size + j].getZ(), 1);
			}

			if (j!=0) {
				// vector i,j & i,j-1
				v1 = vec4(
					heightMap[i*size + j].getX()-heightMap[i*size + j-1].getX(), 
					heightMap[i*size + j].getY()-heightMap[i*size + j-1].getY(),
					heightMap[i*size + j].getZ()-heightMap[i*size + j-1].getZ(), 1);
			}

			if (i!=size) {
				// vector i,j & i+1,j
				v2 = vec4(
					heightMap[(i+1)*size + j].getX()-heightMap[i*size + j].getX(), 
					heightMap[(i+1)*size + j].getY()-heightMap[i*size + j].getY(),
					heightMap[(i+1)*size + j].getZ()-heightMap[i*size + j].getZ(), 1);
			}

			if (j!=size) {
				// vector i,j & i,j+1
				v3 = vec4(
					heightMap[i*size + j+1].getX()-heightMap[i*size + j].getX(), 
					heightMap[i*size + j+1].getY()-heightMap[i*size + j].getY(),
					heightMap[i*size + j+1].getZ()-heightMap[i*size + j].getZ(), 1);
			}

			vec4 v01 = v0.cross(v1).normalize();
			vec4 v12 = v1.cross(v2).normalize();
			vec4 v23 = v2.cross(v3).normalize();
			vec4 v30 = v3.cross(v0).normalize();

			vec4 vSum = v01+v12+v23+v30;

			return vSum.normalize();
		}

		
		void render(mat4t &modelToWorld, mat4t &cameraToWorld, int render_mode, float min_height, float delta_height, int obj_render) {
			// glEnable(GL_CULL_FACE);
			// glCullFace(GL_BACK);
			// glFrontFace(GL_CW);
			
			glShadeModel(GL_SMOOTH);

			mat4t modelToCamera;
			mat4t worldToCamera;
			mat4t modelToProjection = mat4t::build_camera_matrices(modelToCamera, worldToCamera, modelToWorld, cameraToWorld);

			float shininess = 30.0f;
			mat4t light_movement; 

			vec4 light_dir = vec4(sin(angle), 1, 0, 0).normalize()*worldToCamera;
			vec4 light_ambient = vec4(0.3f, 0.3f, 0.3f, 1.0f);
			vec4 light_diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			vec4 light_specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		
			

			if (obj_render==1 || obj_render==0) {
				//terrain_shader_.render(modelToProjection, modelToCamera, light_dir, shininess, light_ambient, light_diffuse, light_specular, min_height, delta_height); 
				terrain_shader_.render_bump(modelToProjection, modelToCamera, light_uniforms_array, num_light_uniforms, num_lights, min_height, delta_height);
			
			
				for(int i=0; i!=terrainMeshes.size();++i){
					if (render_mode < 2) { 
							terrainMeshes[i]->set_mode(GL_TRIANGLES);
					} else {
							terrainMeshes[i]->set_mode(GL_LINES);
					} 

					terrainMeshes[i]->render();
				} 
			}
			
      glActiveTexture(GL_TEXTURE7);
      glBindTexture(GL_TEXTURE_CUBE_MAP,sky_box_textureObj);

			if (obj_render==2 || obj_render==0) {
				sea_shader_.render(modelToProjection, modelToCamera, cameraToWorld,  light_uniforms_array, num_light_uniforms, num_lights, angle, t_length, GL_TEXTURE7); 
				
				for(int i=0; i!=seaMeshes.size();++i){
					if (render_mode < 2) { 
						seaMeshes[i]->set_mode(GL_TRIANGLES);
					} else {
						seaMeshes[i]->set_mode(GL_LINES);
					} 

				seaMeshes[i]->render();
				} 
			}  
      
			angle += 0.1f;

      mat4t skyToWorld = mat4t(1.0f);
      skyToWorld.translate(cameraToWorld.row(3).x(), cameraToWorld.row(3).y(), cameraToWorld.row(3).z());

      modelToProjection = mat4t::build_projection_matrix(skyToWorld, cameraToWorld);

      sky_box_shader_.render(modelToProjection,modelToCamera,GL_TEXTURE7);

      glDepthMask(GL_FALSE);

      skyboxMesh.render();

      glDepthMask(GL_TRUE);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[0]);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textures[1]);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textures[2]);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, textures[3]);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, textures[4]);

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, textures[5]); // emission

			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, textures[6]); // specular

		}

	};
}