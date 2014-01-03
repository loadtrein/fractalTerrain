namespace octet {

	class terrain_mesh_handler {
		mesh t_mesh;
		int textures_[4];

	public:
		terrain_mesh_handler() {}


		void init(GLuint textures[]) {
			for (int i=0; i<4; i++) {
				textures_[i] = textures[i];
			}
		}


		void create_mesh( Tile tiles[], int size) {
			mesh_builder m_builder; 
			m_builder.init(size*4,size*6); 
			int counter = 0; 
			
			for(int i=0; i<size; i++) {
				Tile tile = tiles[i];

				add_tile_facet_normal(tile, &m_builder);
			}

			t_mesh.init();
			m_builder.get_mesh(t_mesh); 
		}


		// this method calculates the normal of the face
		// however the look is faceted
		// it can be inproved by calculating for every point the average normal - 
		void add_tile_facet_normal(Tile t, mesh_builder *m_builder){
			unsigned short cur_vertex = (unsigned short) m_builder->vertices.size(); 
			
			vec4 vector_1( t.points[0].getX(), t.points[0].getY(), t.points[0].getZ(), 1.0f);
			vec4 vector_2( t.points[1].getX(), t.points[1].getY(), t.points[1].getZ(), 1.0f);
			vec4 vector_3( t.points[2].getX(), t.points[2].getY(), t.points[2].getZ(), 1.0f);
			vec4 vector_4( t.points[3].getX(), t.points[3].getY(), t.points[3].getZ(), 1.0f);
			
			// vectors to make the cross product and find the normal
			vec4 n_v1(t.points[0].getX()-t.points[1].getX(), t.points[0].getY()-t.points[1].getY(), t.points[0].getZ()-t.points[1].getZ(), 1.0f);
			vec4 n_v2(t.points[0].getX()-t.points[2].getX(), t.points[0].getY()-t.points[2].getY(), t.points[0].getZ()-t.points[2].getZ(), 1.0f);

			vec4 nor_vec = n_v1 * n_v2;
			vec4 nor_vec_normalized = nor_vec.normalize();
			
			// add vertices with normals
			m_builder->add_vertex(vector_1, nor_vec_normalized, 0, 0);
			m_builder->add_vertex(vector_2, nor_vec_normalized, 0, 1);
			m_builder->add_vertex(vector_3, nor_vec_normalized, 1, 1);
			m_builder->add_vertex(vector_4, nor_vec_normalized, 0, 1);

			m_builder->indices.push_back(cur_vertex+0);
			m_builder->indices.push_back(cur_vertex+1);
			m_builder->indices.push_back(cur_vertex+2);
			m_builder->indices.push_back(cur_vertex+0);
			m_builder->indices.push_back(cur_vertex+2);
			m_builder->indices.push_back(cur_vertex+3);
		}


		// old add_tile method
		void add_tile(Tile t, mesh_builder *m_builder){
			unsigned short cur_vertex = (unsigned short) m_builder->vertices.size(); 
			
			vec4 vector_1( t.points[0].getX(), t.points[0].getY(), t.points[0].getZ(), 1.0f);
			vec4 vector_2( t.points[1].getX(), t.points[1].getY(), t.points[1].getZ(), 1.0f);
			vec4 vector_3( t.points[2].getX(), t.points[2].getY(), t.points[2].getZ(), 1.0f);
			vec4 vector_4( t.points[3].getX(), t.points[3].getY(), t.points[3].getZ(), 1.0f);
			
			// printf(" vector - %f, %f, %f, \n", vector_1.x(), vector_1.y(), vector_1.z());

			/*
			m_builder->add_vertex(vector_1, vec4(0, 0, 1, 0), 0, 0);
			m_builder->add_vertex(vector_2, vec4(0, 0, 1, 0), 0, 1);
			m_builder->add_vertex(vector_3, vec4(0, 0, 1, 0), 1, 1);
			m_builder->add_vertex(vector_4, vec4(0, 0, 1, 0), 0, 1); 
			*/
			
			// add vertices with normals
			m_builder->add_vertex(vector_1, vector_1.normalize(), 0, 0);
			m_builder->add_vertex(vector_2, vector_2.normalize(), 0, 1);
			m_builder->add_vertex(vector_3, vector_3.normalize(), 1, 1);
			m_builder->add_vertex(vector_4, vector_4.normalize(), 0, 1);
			
			//printf("vector %f, %f, %f, %f - normalize %f, %f, %f, %f \n", vector_1.x(), vector_1.y(), vector_1.z(), vector_1.w(), vector_1.normalize().x(), vector_1.normalize().y(), vector_1.normalize().z(), vector_1.normalize().w());
	

			m_builder->indices.push_back(cur_vertex+0);
			m_builder->indices.push_back(cur_vertex+1);
			m_builder->indices.push_back(cur_vertex+2);
			m_builder->indices.push_back(cur_vertex+0);
			m_builder->indices.push_back(cur_vertex+2);
			m_builder->indices.push_back(cur_vertex+3);
		}

		
		void debugRender( /* terrain_shader */ terrain_new_shader &t_shader, mat4t &modelToWorld, mat4t &cameraToWorld ) {
			// glEnable(GL_CULL_FACE);
			// glCullFace(GL_BACK);
			// glFrontFace(GL_CW);

			mat4t modelToCamera;
			mat4t worldToCamera;
			mat4t modelToProjection = mat4t::build_camera_matrices(modelToCamera, worldToCamera, modelToWorld, cameraToWorld);

			// t_shader.render(modelToProjection, 0, 1); 

			
			vec4 color_1(1, 0, 1, 1);
			vec4 color_2(0, 0, 1, 1);

			//new t_shader
			t_shader.render_color(modelToProjection, modelToCamera ); 

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures_[0]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textures_[1]);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textures_[2]);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, textures_[3]);


			t_mesh.render(); 
		}

	};


}