namespace octet {

	class terrain_mesh_handler {
		dynarray<mesh*> terrainMeshes;
    dynarray<mesh*> seaMeshes;
		float delta_height;
		int textures_[6];


	public:
		terrain_mesh_handler() {}


		void init(GLuint textures[]) {
			
			for (int i=0; i<6; i++) {
				textures_[i] = textures[i];
			}
		}



		void create_mesh_from_map(int size,  Point * const heightMap, int meshType) {
			
      if(meshType == 0){
        terrainMeshes.reset();
      }else if(meshType == 1){
        seaMeshes.reset();
      }

			int numMeshSegments = 0;
			int mesh_size = 0;

			if (size > 128) {
				int base = (int)size/128;
				int exponential = (int) std::powf(base, 2);
				numMeshSegments = exponential;//size/128 * exponential;
				mesh_size = 128;
			} else{
				numMeshSegments = 1;
				mesh_size = size-1;
			}

		
			int terrainSide_size = (int)( mesh_size*sqrt(numMeshSegments));

			for (int index_i=0; index_i<terrainSide_size; index_i+=mesh_size) {
				for (int index_j=0; index_j<terrainSide_size; index_j+=mesh_size) {

					mesh_builder m_builder;
					m_builder.init(mesh_size*4, mesh_size*6);
						
					for (int i=index_i; i<index_i+mesh_size; i++) {
						for (int j=index_j; j<index_j+mesh_size; j++) {
							add_face(size, i, j, &m_builder, heightMap);
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

			float lowestValue = get_lowestValue(size, heightMap);
			float highestValue = get_highestValue(size, heightMap);
			delta_height = highestValue-lowestValue;

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
			
			// add vertices with normals
			/*
			m_builder->add_vertex(v0, n0, 0, 0);
			m_builder->add_vertex(v1, n1, 0, 1);
			m_builder->add_vertex(v2, n2, 1, 1);
			m_builder->add_vertex(v3, n3, 0, 1);
			*/

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

		


		float get_highestValue(int size,  Point * const heightMap) {
			float h=0;
			for(int i=0; i<size*size; i++) {
				float n = heightMap[i].getY();
				if (n>h)
					h = n;
			}
			//printf("h %f \n", h);
			return h;
		}


		float get_lowestValue(int size,  Point * const heightMap) {
			float l=0; 
			for(int i=0; i<size*size; i++) {
				float n = heightMap[i].getY();
				if (n<l)
					l=n;
			}
			//printf("l %f \n", l);
			return l;
		}

		
		void render(terrain_shader &t_shader, mat4t &modelToWorld, mat4t &cameraToWorld, int render_mode) {
			// glEnable(GL_CULL_FACE);
			// glCullFace(GL_BACK);
			// glFrontFace(GL_CW);
			
			glShadeModel(GL_SMOOTH);

			mat4t modelToCamera;
			mat4t worldToCamera;
			mat4t modelToProjection = mat4t::build_camera_matrices(modelToCamera, worldToCamera, modelToWorld, cameraToWorld);

			float shininess = 30.0f;

			vec4 light_dir = vec4(0, 1, 0, 0).normalize()*worldToCamera;
			vec4 light_ambient = vec4(0.3f, 0.3f, 0.3f, 1.0f);
			vec4 light_diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			vec4 light_specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		
			t_shader.render(modelToProjection, modelToCamera, light_dir, shininess, light_ambient, light_diffuse, light_specular, delta_height); 

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures_[0]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textures_[1]);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textures_[2]);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, textures_[3]);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, textures_[4]); // emission
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, textures_[5]); // specular
			glActiveTexture(GL_TEXTURE6);


			

			for(int i=0; i!=terrainMeshes.size();++i){
				if (render_mode < 2) { 
						terrainMeshes[i]->set_mode(GL_TRIANGLES);
				} else {
						terrainMeshes[i]->set_mode(GL_LINES);
				} 

				terrainMeshes[i]->render();
			} 
		}

	};
}