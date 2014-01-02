

namespace octet {

  
  class terrain_new_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjection_index; 
	GLuint modelToCamera_index;

	// index for flat shader emissive color
	GLuint emissive_color_1_index; 
	GLuint emissive_color_2_index; 

  public:
    void init() {
     
      // vertex shader
      const char vertex_shader[] = SHADER_STR(
		// setting a variable to varying permits to share among vertex shader and fragment shader
		varying vec4 pos_;
		varying vec3 norm_;
	    varying vec4 color_;
		
      
		// attributes are passed with OPENGL glVertexAttribPointer
        attribute vec4 pos;
		attribute vec3 normal;
        attribute vec4 color;

        uniform mat4 modelToProjection;
		uniform mat4 modelToCamera;
      
        void main() {
		  pos_ = pos;
		  color_ = color;

		  // normal is calculated using the modelToCamera
		  norm_ = (modelToCamera * vec4(normal,0)).xyz;

          gl_Position = modelToProjection * pos;
        }
      );

      // fragment shader
      const char fragment_shader[] = SHADER_STR(
		varying vec4 color_;
	    varying vec4 pos_;
		varying vec3 norm_;

		uniform vec4 emissive_color_1;
		uniform vec4 emissive_color_2;
		
      
        void main() {
			
			vec4 pos_norm = normalize(pos_);
			vec4 color_pos = vec4(pos_norm.y, pos_norm.x, pos_norm.z, 1.0f );
			vec4 color_pos2 = vec4(pos_norm.y, 0.5f, 0.5f, 1.0f);
			vec3 nNorm = normalize(norm_);

			float slope = 1-nNorm.y;
			vec4 finalColor;

			gl_FragColor = vec4(slope*pos_norm.y, 0.0f, 0.0f, 1.0f);
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
	  modelToCamera_index = glGetUniformLocation(program(), "modelToCamera");
	  emissive_color_1_index = glGetUniformLocation(program(), "emissive_color_1");
	  emissive_color_2_index = glGetUniformLocation(program(), "emissive_color_2");
    }

    void render_color(const mat4t &modelToProjection, const mat4t &modelToCamera, const vec4 &emissive_color_1, const vec4 &emissive_color_2) {
      // tell openGL to use the program
      shader::render();

      // set the uniforms
	  glUniform4fv(emissive_color_1_index, 1, emissive_color_1.get());
	  glUniform4fv(emissive_color_2_index, 1, emissive_color_2.get());
	  glUniformMatrix4fv(modelToProjection_index, 1, GL_FALSE, modelToProjection.get()); 
	  glUniformMatrix4fv(modelToCamera_index, 1, GL_FALSE, modelToCamera.get());

	  

    }
  };
}
