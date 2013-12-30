namespace octet {

	class terrain_mesh_handler {
		mesh t_mesh;
		int texture_1;
		int texture_2; 

	public:
		terrain_mesh_handler() {}


		void init(int texture_1_, int texture_2_) {
			texture_1 = texture_1_; 
			texture_2 = texture_2_; 
		}


		void create_mesh( Tile tiles[], int size) {
			mesh_builder m_builder; 
			m_builder.init(size*4,size*6); 
			int counter = 0; 
			
			for(int i=0; i<size; i++) {
				Tile tile = tiles[i];
				add_tile(tile, &m_builder);
			}

			t_mesh.init();
			m_builder.get_mesh(t_mesh); 

			
		}

		void add_tile(Tile t, mesh_builder *m_builder){
			unsigned short cur_vertex = (unsigned short) m_builder->vertices.size(); 
			
			vec4 vector_1( t.points[0].getX(), t.points[0].getY(), t.points[0].getZ(), 1.0f);
			vec4 vector_2( t.points[1].getX(), t.points[1].getY(), t.points[1].getZ(), 1.0f);
			vec4 vector_3( t.points[2].getX(), t.points[2].getY(), t.points[2].getZ(), 1.0f);
			vec4 vector_4( t.points[3].getX(), t.points[3].getY(), t.points[3].getZ(), 1.0f);
			
			// printf(" vector - %f, %f, %f, \n", vector_1.x(), vector_1.y(), vector_1.z());

			m_builder->add_vertex(vector_1, vec4(0, 0, 1, 0), 0, 0);
			m_builder->add_vertex(vector_2, vec4(0, 0, 1, 0), 0, 1);
			m_builder->add_vertex(vector_3, vec4(0, 0, 1, 0), 1, 1);
			m_builder->add_vertex(vector_4, vec4(0, 0, 1, 0), 0, 1); 

			m_builder->indices.push_back(cur_vertex+0);
			m_builder->indices.push_back(cur_vertex+1);
			m_builder->indices.push_back(cur_vertex+2);
			m_builder->indices.push_back(cur_vertex+0);
			m_builder->indices.push_back(cur_vertex+2);
			m_builder->indices.push_back(cur_vertex+3);

		}


		void printMesh() {
			
		}


		void debugRender( /* terrain_shader */ terrain_new_shader &t_shader, mat4t &modelToWorld, mat4t &cameraToWorld ) {
			// glEnable(GL_CULL_FACE);
			// glCullFace(GL_BACK);
			// glFrontFace(GL_CW);

			mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

			//
			// t_shader.render(modelToProjection, 0, 1); 

			
			vec4 color_1(1, 0, 1, 1);
			vec4 color_2(0, 0, 1, 1);

			//new t_shader
			t_shader.render_color(modelToProjection, color_1, color_2); 

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture_2);


			t_mesh.render(); 
		}

	};


}