namespace octet {

	class terrain_mesh_handler {
		mesh t_mesh;
		

	public:
		terrain_mesh_handler() {}


		void init() {}


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


		void debugRender() {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CW);

			t_mesh.render(); 
		}

	};


}