

namespace octet {

  
  class terrain_new_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjection_index; 
	GLuint modelToCamera_index;

	GLuint light_direction_index;
	GLuint light_ambient_index;
	GLuint light_diffuse_index;
	GLuint light_specular_index;
	GLuint shininess_index;


	// index for flat shader emissive color
	// GLuint emissive_color_1_index; 
	// GLuint emissive_color_2_index; 

	// index for multi textures
	GLuint texture_samplers_index;

  public:
    void init() {
     
      // vertex shader
      const char vertex_shader[] = SHADER_STR(
		// setting a variable to varying permits to share among vertex shader and fragment shader
		varying vec4 pos_;
	    varying vec4 color_;
		varying vec3 norm_;
		varying vec2 uv_;
		
      
		// attributes are passed with OPENGL glVertexAttribPointer
        attribute vec4 pos;
		attribute vec3 normal;
        attribute vec4 color;
		attribute vec2 uv;

		// from glUniform
        uniform mat4 modelToProjection;
		uniform mat4 modelToCamera;
		
      
        void main() {
		  pos_ = pos;
		  color_ = color;
		  uv_ = uv;

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
		varying vec2 uv_;

		// uniform vec4 emissive_color_1;
		// uniform vec4 emissive_color_2;
		uniform sampler2D samplers[4];

		
		
      
        void main() {
			
			vec4 pos_norm = normalize(pos_);
			vec4 color_pos = vec4(pos_norm.y, pos_norm.x, pos_norm.z, 1.0f );
			vec4 color_pos2 = vec4(pos_norm.y, 0.5f, 0.5f, 1.0f);
			vec3 nNorm = normalize(norm_);

			vec4 grass	= texture2D(samplers[0], uv_);
			vec4 sand	= texture2D(samplers[1], uv_);
			vec4 snow	= texture2D(samplers[2], uv_);
			vec4 rock	= texture2D(samplers[3], uv_);

			float slope = 1-nNorm.y;
			vec4 finalColor;

			float blendAmount;

			// select texture
			if ( slope < 0.2) {
				blendAmount = slope / 0.2f;
				finalColor = mix(grass, rock, blendAmount);
			}
			
			if ( (slope < 0.7f) && (slope >= 0.2f) ) {
				blendAmount = (slope-0.2f) * (1.0f / (0.7f- 0.2f));
				finalColor = mix(rock, sand, blendAmount);
			}

			if ( slope >= 0.7f) {
				finalColor = sand;
			}

			


			gl_FragColor = finalColor;  // vec4(slope*pos_norm.y, 0.0f, 0.0f, 1.0f);
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
	  // emissive_color_1_index = glGetUniformLocation(program(), "emissive_color_1");
	  // emissive_color_2_index = glGetUniformLocation(program(), "emissive_color_2");
	  texture_samplers_index = glGetUniformLocation(program(), "samplers");
    }

    void render_color(const mat4t &modelToProjection, const mat4t &modelToCamera, int num_samplers = 4) {
      // tell openGL to use the program
      shader::render();

      // set the uniforms
	  // glUniform4fv(emissive_color_1_index, 1, emissive_color_1.get());
	  // glUniform4fv(emissive_color_2_index, 1, emissive_color_2.get());
	  glUniformMatrix4fv(modelToProjection_index, 1, GL_FALSE, modelToProjection.get()); 
	  glUniformMatrix4fv(modelToCamera_index, 1, GL_FALSE, modelToCamera.get());

	  static const GLint samplers[] = { 0, 1, 2, 3, 4};
	  glUniform1iv(texture_samplers_index, num_samplers, samplers);
    }
  };
}
