namespace octet {

	class terrain_mesh_handler {
		dynarray<mesh*> terrainMeshes;
		int textures_[6];
		

	public:
		terrain_mesh_handler() {}


		void init(GLuint textures[]) {
			
			for (int i=0; i<6; i++) {
				textures_[i] = textures[i];
			}
		}


	    void create_mesh_old(Tile tiles[], int size, int length) {
		  terrainMeshes.reset();

		  int numTerrainSegments = 0;

		  if(size > 128*128){
			numTerrainSegments = size/(128*128);
			printf(" %i segm \n", numTerrainSegments);
			size = 128*128;
		  } else {
			numTerrainSegments = 1;
		  }      

		  int index=0;

		  for(int i=0; i!= numTerrainSegments; ++i){
			  mesh_builder m_builder; 
			  m_builder.init(size*4,size*6); 

			  /*
			  for(int j=index; j!=index+size; j++) {
				  Tile tile = tiles[j];
				  add_tile_facet_normal(&tile, &m_builder);
			  }
			  */
			  
			  for(int j=index; j<index+size; j++){
				  Tile tile = tiles[j];
				  add_tile_vertex_normal(&tile, &m_builder, j, 128, &tiles); 
			  }

			  /*
			  vec4 v_init = vec4(tiles[10].points[3].getX(), tiles[10].points[3].getY(), tiles[10].points[3].getZ(), 1.0f);
			  vec4 v_derived = vec4(tiles[11].points[3].getX(), tiles[11].points[3].getY(), tiles[11].points[3].getZ(), 1.0f);
			  vec4 v_debug = shift_column_right(&tiles, 10, 3);
			  printf(" init v = %f, %f, %f, %f \n", v_init.x(), v_init.y(), v_init.z(), v_init.w());
			  printf(" init v = %f, %f, %f, %f \n", v_derived.x(), v_derived.y(), v_derived.z(), v_derived.w()); 
			  printf(" init v = %f, %f, %f, %f \n", v_debug.x(), v_debug.y(), v_debug.z(), v_debug.w()); 
			  printf(" // \n");
			  */
          
			  mesh *t_mesh = new mesh();
			  t_mesh->init();
			  m_builder.get_mesh(*t_mesh);

			  this->terrainMeshes.push_back(t_mesh);
			  index+=size;
		  }
		}


		// this method calculates the normal of the face
		// however the look is faceted
		// it can be inproved by calculating for every point the average normal - 
		void add_tile_facet_normal(Tile *t, mesh_builder *m_builder){
			unsigned short cur_vertex = (unsigned short) m_builder->vertices.size(); 
			
			vec4 vector_1( t->points[0].getX(), t->points[0].getY(), t->points[0].getZ(), 1.0f);
			vec4 vector_2( t->points[1].getX(), t->points[1].getY(), t->points[1].getZ(), 1.0f);
			vec4 vector_3( t->points[2].getX(), t->points[2].getY(), t->points[2].getZ(), 1.0f);
			vec4 vector_4( t->points[3].getX(), t->points[3].getY(), t->points[3].getZ(), 1.0f);
			
			// vectors to make the cross product and find the normal
			vec4 n_v1(t->points[0].getX()-t->points[1].getX(), t->points[0].getY()-t->points[1].getY(), t->points[0].getZ()-t->points[1].getZ(), 1.0f);
			vec4 n_v2(t->points[0].getX()-t->points[2].getX(), t->points[0].getY()-t->points[2].getY(), t->points[0].getZ()-t->points[2].getZ(), 1.0f);

			vec4 nor_vec_normalized =  (n_v1 * n_v2).normalize();
			
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




		void add_tile_vertex_normal(Tile *t, mesh_builder *m_builder, int tile_number, int tile_length, Tile *tiles[]){
			unsigned short cur_vertex = (unsigned short) m_builder->vertices.size(); 
			
			vec4 vector_0( t->points[0].getX(), t->points[0].getY(), t->points[0].getZ(), 1.0f);
			vec4 vector_1( t->points[1].getX(), t->points[1].getY(), t->points[1].getZ(), 1.0f);
			vec4 vector_2( t->points[2].getX(), t->points[2].getY(), t->points[2].getZ(), 1.0f);
			vec4 vector_3( t->points[3].getX(), t->points[3].getY(), t->points[3].getZ(), 1.0f);
			
			/*
			vec4 norm_vec_normalized_0 = get_vertex_normal(tile_number, 0, tile_length, *tiles);
			vec4 norm_vec_normalized_1 = get_vertex_normal(tile_number, 1, tile_length, *tiles);
			vec4 norm_vec_normalized_2 = get_vertex_normal(tile_number, 2, tile_length, *tiles);
			vec4 norm_vec_normalized_3 = get_vertex_normal(tile_number, 3, tile_length, *tiles);
			*/

			
			vec4 norm_vec_normalized_0 = get_vertex_normal_2(tile_number, 0, tile_length, *tiles);
			vec4 norm_vec_normalized_1 = get_vertex_normal_2(tile_number, 1, tile_length, *tiles);
			vec4 norm_vec_normalized_2 = get_vertex_normal_2(tile_number, 2, tile_length, *tiles);
			vec4 norm_vec_normalized_3 = get_vertex_normal_2(tile_number, 3, tile_length, *tiles);
			

			// add vertices with normals
			m_builder->add_vertex(vector_0, norm_vec_normalized_0, 0, 0);
			m_builder->add_vertex(vector_1, norm_vec_normalized_1, 0, 1);
			m_builder->add_vertex(vector_2, norm_vec_normalized_2, 1, 1);
			m_builder->add_vertex(vector_3, norm_vec_normalized_3, 0, 1);

			m_builder->indices.push_back(cur_vertex+0);
			m_builder->indices.push_back(cur_vertex+1);
			m_builder->indices.push_back(cur_vertex+2);
			m_builder->indices.push_back(cur_vertex+0);
			m_builder->indices.push_back(cur_vertex+2);
			m_builder->indices.push_back(cur_vertex+3);
		}




		vec4 get_vertex_normal(int tile_number, int point_number, int tile_length, Tile tiles[]) {

				vec4 vector_p0;
				vec4 vector_p1;
				vec4 vector_p2;
				vec4 vector_p3;
				vec4 neutro(0, 0, 0, 1); 

				bool case_0 = false;
				bool case_1 = false;
				bool case_2 = false;
				bool case_3 = false;

				//printf(" point_number %i \n", point_number);

				if (tile_number < tile_length) {
					case_0 = true; // the tile is in the "lower" row
					//printf(" case_0 \n");
					if (tile_number == tile_length && point_number == 1) {
						case_1 = true;
						//printf(" in_case_1 \n");
					}
					if (tile_number == 0 && point_number == 0) {
						case_2 = true;
						//printf(" in_case_2 \n"); 
					}
				} else {
					if (tile_number % tile_length == 0) {
						case_1 = true; // tile is in the "right" column
						//printf(" case_1 \n");
					}
				
					if (tile_number % tile_length == 1) {
						case_2 = true; // tile is in the "left" column
						//printf(" case_2 \n");
					}
				
					if (tile_number > (tile_length*tile_length)-tile_length) {
						case_3 = true; // tile is in the "upper" row
						//printf(" case_3 \n");
					}
				}

				
				//printf(" - \n");

				if (point_number == 0) {
					// take point 1 same Tile
					vector_p0 = vec4(tiles[tile_number].points[1].getX(), tiles[tile_number].points[1].getY(), tiles[tile_number].points[1].getZ(), 1.0f);

					// take point 3 same Tile
					vector_p1 = vec4(tiles[tile_number].points[3].getX(), tiles[tile_number].points[3].getY(), tiles[tile_number].points[3].getZ(), 1.0f);

					if (case_1) {
						vector_p2 = neutro;
					} else {
						vector_p2 = vec4(tiles[tile_number-1].points[0].getX(), tiles[tile_number-1].points[0].getY(), tiles[tile_number-1].points[0].getZ(), 1.0f);
					}

					// take point 0 - tile n-lenght
					if (case_0) {
						vector_p3 = neutro;
					} else { 
						vector_p3 = vec4(tiles[tile_number-tile_length].points[0].getX(), tiles[tile_number-tile_length].points[0].getY(), tiles[tile_number-tile_length].points[0].getZ(), 1.0f);
					}

				} else if (point_number == 1) {
							// take point 0 same tile
							vector_p0 = vec4(tiles[tile_number].points[0].getX(), tiles[tile_number].points[0].getY(), tiles[tile_number].points[0].getZ(), 1.0f);

							// take point 2 same tile
							vector_p1 = vec4(tiles[tile_number].points[2].getX(), tiles[tile_number].points[2].getY(), tiles[tile_number].points[2].getZ(), 1.0f);

							// take point 1 - Tile n+1
							if (case_2) {
								vector_p2 = neutro;
							} else {
								vector_p2 = vec4(tiles[tile_number+1].points[1].getX(), tiles[tile_number+1].points[1].getY(), tiles[tile_number+1].points[1].getZ(), 1.0f);
							}

							// take point 1 - Tile n-leght
							if (case_0) {
								vector_p3 = neutro;
							} else {
								vector_p3 = vec4(tiles[tile_number-tile_length].points[1].getX(), tiles[tile_number-tile_length].points[1].getY(), tiles[tile_number-tile_length].points[1].getZ(), 1.0f);
							}

						} else if (point_number == 2) {
									// take point 1 same tile
									vector_p0 = vec4(tiles[tile_number].points[1].getX(), tiles[tile_number].points[1].getY(), tiles[tile_number].points[1].getZ(), 1.0f);

									// take point 3 same tile
									vector_p1 = vec4(tiles[tile_number].points[3].getX(), tiles[tile_number].points[3].getY(), tiles[tile_number].points[3].getZ(), 1.0f);

									// take point 2 - Tile n+1
									if (case_2) {
										vector_p2 = neutro;
									} else {
										vector_p2 = vec4(tiles[tile_number+1].points[2].getX(), tiles[tile_number+1].points[2].getY(), tiles[tile_number+1].points[2].getZ(), 1.0f);
									}

									// take point 2 - tile n+lenght
									if (case_3) {
										vector_p3 = neutro;
									} else {
										vector_p3 = vec4(tiles[tile_number+tile_length].points[2].getX(), tiles[tile_number+tile_length].points[2].getY(), tiles[tile_number+tile_length].points[2].getZ(), 1.0f);
									}

								} else {
											// take point 0 same tile
											vector_p0 = vec4(tiles[tile_number].points[1].getX(), tiles[tile_number].points[0].getY(), tiles[tile_number].points[0].getZ(), 1.0f);

											// take point 2 same tile
											vector_p1 = vec4(tiles[tile_number].points[2].getX(), tiles[tile_number].points[2].getY(), tiles[tile_number].points[2].getZ(), 1.0f);

											// take point 3 - Tile n-1
											if(case_1) {
												vector_p2 = neutro;
											} else {
												vector_p2 = vec4(tiles[tile_number-1].points[3].getX(), tiles[tile_number-1].points[3].getY(), tiles[tile_number-1].points[3].getZ(), 1.0f);
											}

											// take point 3 - Tile n+lenght 
											if(case_3) {
												vector_p3 = neutro;
											} else {
												vector_p3 = vec4(tiles[tile_number+tile_length].points[3].getX(), tiles[tile_number+tile_length].points[3].getY(), tiles[tile_number+tile_length].points[3].getZ(), 1.0f);
											}
								}


				vec4 v01 = (vector_p0 * vector_p1).normalize(); // normalized cross product
				vec4 v12 = (vector_p1 * vector_p2).normalize();
				vec4 v23 = (vector_p2 * vector_p3).normalize();
				vec4 v31 = (vector_p3 * vector_p0).normalize();

				vec4 finalVector = (v01 + v12 + v23 + v31).normalize(); 

				return finalVector;
		}



		vec4 get_vertex_normal_2(int tile_number, int point_number, int tile_length, Tile tiles[]) {

				vec4 vector_p0;
				vec4 vector_p1;
				vec4 vector_p2;
				vec4 vector_p3;
				vec4 neutro(0, 0, 0, 1); 

				bool case_0 = false; 
				bool case_1 = false;
				bool case_2 = false;
				bool case_3 = false;

				//printf(" point_number %i \n", point_number);

				if (tile_number <= tile_length) {
					case_0 = true; // the tile is in the "lower" row
					
					if ( (tile_number == tile_length && point_number == 1) || (tile_number == tile_length && point_number == 2) ) {
						case_1 = true; // tile is in the "right" column
					}
					else if ( (tile_number == 0 && point_number == 0) || (tile_number == 0 && point_number == 3) ) {
						case_2 = true; // tile is in the "left" column
					}
				} else {
					if (tile_number % tile_length == 0) {
						case_1 = true; // tile is in the "right" column
					}
				
					if (tile_number % tile_length == 1) {
						case_2 = true; // tile is in the "left" column
					}
				
					if (tile_number > (tile_length*tile_length)-tile_length) {
						case_3 = true; // tile is in the "upper" row
					}
				}

				if (point_number == 0) {
					// take point 1 same Tile
					vector_p0 = vector_from_sameTile(&tiles[tile_number], point_number, 1);

					// take point 3 same Tile
					vector_p1 = vector_from_sameTile(&tiles[tile_number], point_number, 3);

					if (case_1) {
						vector_p2 = neutro;
					} else {
						vector_p2 = vector_from_leftColumn(tiles, tile_number, point_number);
					}

					// take point 0 - tile n-lenght
					if (case_0) {
						vector_p3 = neutro;
					} else { 
						vector_p3 = vector_from_downRow(tiles, tile_number, point_number, tile_length);
					}

				} else if (point_number == 1) {
							// take point 0 same tile
							vector_p0 = vector_from_sameTile(&tiles[tile_number], point_number, 0);

							// take point 2 same tile
							vector_p1 = vector_from_sameTile(&tiles[tile_number], point_number, 2);

							// take point 1 - Tile n+1
							if (case_2) {
								vector_p2 = neutro;
							} else {
								vector_p2 = vector_from_rightColumn(tiles, tile_number, point_number);
							}

							// take point 1 - Tile n-leght
							if (case_0) {
								vector_p3 = neutro;
							} else {
								vector_p3 = vector_from_downRow(tiles, tile_number, point_number, tile_length); 
							}

						} else if (point_number == 2) {
									// take point 1 same tile
									vector_p0 = vector_from_sameTile(&tiles[tile_number], point_number, 1);

									// take point 3 same tile
									vector_p1 = vector_from_sameTile(&tiles[tile_number], point_number, 3);

									// take point 2 - Tile n+1
									if (case_2) {
										vector_p2 = neutro;
									} else {
										vector_p2 = vector_from_rightColumn(tiles, tile_number, point_number); 
									}

									// take point 2 - tile n+lenght
									if (case_3) {
										vector_p3 = neutro;
									} else {
										vector_p3 = vector_from_upRow(tiles, tile_number, point_number, tile_length);
									}

								} else {
											// take point 0 same tile
											vector_p0 = vector_from_sameTile(&tiles[tile_number], point_number, 0);

											// take point 2 same tile
											vector_p1 = vector_from_sameTile(&tiles[tile_number], point_number, 1);

											// take point 3 - Tile n-1
											if(case_1) {
												vector_p2 = neutro;
											} else {
												vector_p2 = vector_from_leftColumn(tiles, tile_number, point_number);
											}

											// take point 3 - Tile n+lenght 
											if(case_3) {
												vector_p3 = neutro;
											} else {
												vector_p3 = vector_from_upRow(tiles, tile_number, point_number, tile_length); 
											}
								}


				// vec4 v01 = (vector_p0 * vector_p1).normalize(); // normalized cross product
				// vec4 v12 = (vector_p1 * vector_p2).normalize();
				// vec4 v23 = (vector_p2 * vector_p3).normalize();
				// vec4 v31 = (vector_p3 * vector_p0).normalize();


				vec4 v01 = vector_p0.cross(vector_p1).normalize();
				vec4 v12 = vector_p1.cross(vector_p2).normalize();
				vec4 v23 = vector_p2.cross(vector_p3).normalize();
				vec4 v31 = vector_p3.cross(vector_p0).normalize();

				vec4 finalVector = (v01 + v12 + v23 + v31).normalize(); 

				return finalVector;
		}


		vec4 vector_from_rightColumn(Tile t[], int tn, int pn) {
			float x = t[tn+1].points[pn].getX() - t[tn].points[pn].getX();
			float y = t[tn+1].points[pn].getY() - t[tn].points[pn].getY();
			float z = t[tn+1].points[pn].getZ() - t[tn].points[pn].getZ();
			return vec4(x, y, z, 1.0f).normalize(); 
		}

		vec4 vector_from_leftColumn(Tile t[], int tn, int pn) {
			float x = t[tn].points[pn].getX() - t[tn-1].points[pn].getX();
			float y = t[tn].points[pn].getY() - t[tn-1].points[pn].getY();
			float z = t[tn].points[pn].getZ() - t[tn-1].points[pn].getZ();
			return vec4(x, y, z, 1.0f).normalize(); 
		}

		vec4 vector_from_upRow(Tile t[], int tn, int pn, int tLenght) {
			float x = t[tn+tLenght].points[pn].getX() - t[tn].points[pn].getX();
			float y = t[tn+tLenght].points[pn].getY() - t[tn].points[pn].getY();
			float z = t[tn+tLenght].points[pn].getZ() - t[tn].points[pn].getZ();
			return vec4(x, y, z, 1.0f).normalize(); 
		}

		vec4 vector_from_downRow(Tile t[], int tn, int pn, int tLenght) {
			float x = t[tn].points[pn].getX() - t[tn-tLenght].points[pn].getX();
			float y = t[tn].points[pn].getY() - t[tn-tLenght].points[pn].getY();
			float z = t[tn].points[pn].getZ() - t[tn-tLenght].points[pn].getZ();
			return vec4(x, y, z, 1.0f).normalize();
		}

		vec4 vector_from_sameTile(Tile *t, int pn1, int pn2) {
			float x = t->points[pn1].getX() - t->points[pn2].getX();
			float y = t->points[pn1].getY() - t->points[pn2].getY();
			float z = t->points[pn1].getZ() - t->points[pn2].getZ();
			return vec4(x, y, z, 1.0f).normalize();
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


		void create_mesh_from_heightMap(int size,  Point * const heightMap) {
			terrainMeshes.reset();

			int numTerrainSegments = 0;
			int mesh_size = 0;

			if (size > 128) {
				numTerrainSegments = size/128 *2;
				mesh_size = 128;
			} else{
				numTerrainSegments = 1;
			}

			int terrainSide_size = (int) mesh_size*sqrt(numTerrainSegments);

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

					this->terrainMeshes.push_back(t_mesh);
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
			
			// add vertices with normals
			m_builder->add_vertex(v0, n0, 0, 0);
			m_builder->add_vertex(v1, n1, 0, 1);
			m_builder->add_vertex(v2, n2, 1, 1);
			m_builder->add_vertex(v3, n3, 0, 1);
			
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

		
		void render( terrain_shader &t_shader, mat4t &modelToWorld, mat4t &cameraToWorld ) {
			// glEnable(GL_CULL_FACE);
			// glCullFace(GL_BACK);
			// glFrontFace(GL_CW);

			mat4t modelToCamera;
			mat4t worldToCamera;
			mat4t modelToProjection = mat4t::build_camera_matrices(modelToCamera, worldToCamera, modelToWorld, cameraToWorld);

			float shininess = 30.0f;

			vec4 light_dir = vec4(0, 1, 0, 0).normalize()*worldToCamera;
			vec4 light_ambient = vec4(0.3f, 0.3f, 0.3f, 1.0f);
			vec4 light_diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			vec4 light_specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		
			t_shader.render(modelToProjection, modelToCamera, light_dir, shininess, light_ambient, light_diffuse, light_specular); 

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
				terrainMeshes[i]->render();
			} 
		}

	};
}