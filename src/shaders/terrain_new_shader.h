

namespace octet {

  
  class terrain_new_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjection_index; 

	// index for point heights
	// GLuint heights_index;

	// index for flat shader emissive color
	GLuint emissive_color_1_index; 
	GLuint emissive_color_2_index; 

  public:
    void init() {
     
      // vertex shader
      const char vertex_shader[] = SHADER_STR(
		varying vec4 color_;
		varying vec4 pos_;
      
        attribute vec4 pos;
        attribute vec4 color;

        uniform mat4 modelToProjection;
      
        void main() {
          color_ = color;
		  pos_ = pos;

          gl_Position = modelToProjection * pos;
        }
      );

      // fragment shader
      const char fragment_shader[] = SHADER_STR(
		varying vec4 color_;
	    varying vec4 pos_;

		uniform vec4 emissive_color_1;
		uniform vec4 emissive_color_2;
      
        void main() {
			if (pos_.y > 3){
				gl_FragColor = emissive_color_1;
			} else {
				gl_FragColor = emissive_color_2;
			}
        }
      );

      init_uniforms(vertex_shader, fragment_shader);
    }

    void init_uniforms(const char *vertex_shader, const char *fragment_shader) {
      // use the common shader code to compile and link the shaders
      // the result is a shader program
      shader::init(vertex_shader, fragment_shader);

      // extract the indices of the uniforms to use later
      modelToProjection_index = glGetUniformLocation(program(), "modelToProjection");
	  emissive_color_1_index = glGetUniformLocation(program(), "emissive_color_1");
	  emissive_color_2_index = glGetUniformLocation(program(), "emissive_color_2");
    }

    void render_color(const mat4t &modelToProjection, const vec4 &emissive_color_1, const vec4 &emissive_color_2) {
      // tell openGL to use the program
      shader::render();

      // set the uniforms
	  glUniform4fv(emissive_color_1_index, 1, emissive_color_1.get());
	  glUniform4fv(emissive_color_2_index, 1, emissive_color_2.get());
	  glUniformMatrix4fv(modelToProjection_index, 1, GL_FALSE, modelToProjection.get()); 

    }
  };
}
