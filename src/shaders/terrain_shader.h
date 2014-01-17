

namespace octet {

  
  class terrain_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjection_index; 
	GLuint modelToCamera_index;

	GLuint light_direction_index;
	GLuint light_ambient_index;
	GLuint light_diffuse_index;
	GLuint light_specular_index;
	GLuint shininess_index;

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

		uniform sampler2D samplers[6];

		uniform vec3 light_direction;
		uniform vec4 light_diffuse;
		uniform vec4 light_ambient;
		uniform vec4 light_specular;
		uniform float shininess;

        void main() {
			vec4 pos_norm = normalize(pos_);
			// vec4 color_pos = vec4(pos_norm.y, pos_norm.x, pos_norm.z, 1.0f );
			// vec4 color_pos2 = vec4(pos_norm.y, 0.5f, 0.5f, 1.0f);
			vec4 heigh_color = vec4(pos_norm.y, 0, 0, 1); 
			vec3 nNorm = normalize(norm_);
			vec3 half_direction = normalize(light_direction + vec3(0, 0, 1));
			float diffuse_factor = max(dot(light_direction, nNorm), 0.0);
			float specular_factor = pow(max(dot(half_direction, nNorm), 0.0), shininess); 

			vec4 grass	= texture2D(samplers[0], uv_);
			vec4 sand	= texture2D(samplers[1], uv_);
			vec4 snow	= texture2D(samplers[2], uv_);
			vec4 rock	= texture2D(samplers[3], uv_);
			vec4 emission = texture2D(samplers[4], uv_);
			vec4 specular = texture2D(samplers[5], uv_);

			float slope = 1-nNorm.y;
			vec4 finalColor;

			float blendAmount;

			// select texture
			/*
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
			*/
			
			
			if (slope > 0.8 ) {
				finalColor = sand;
			} else {
				finalColor = grass;
			}
			

			gl_FragColor = finalColor*light_ambient + heigh_color; // vec4(slope*pos_norm.y, 0.0f, 0.0f, 1.0f);
						/*
						finalColor * light_ambient + 
						finalColor * light_diffuse * diffuse_factor +
						emission + 
						specular * light_specular * specular_factor;
						*/
        }
      );

      init_uniforms(vertex_shader, fragment_shader);
    }

	vec4 generate_terrainColor() {
		vec4 terrainColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		
	}

    void init_uniforms(const char *vertex_shader, const char *fragment_shader) {
      // use the common shader code to compile and link the shaders
      // the result is a shader program
      shader::init(vertex_shader, fragment_shader);

      // extract the indices of the uniforms to use later
      modelToProjection_index = glGetUniformLocation(program(), "modelToProjection");
	  modelToCamera_index = glGetUniformLocation(program(), "modelToCamera");

	  light_direction_index = glGetUniformLocation(program(), "light_direction");
	  shininess_index = glGetUniformLocation(program(), "shininess");
	  light_ambient_index = glGetUniformLocation(program(), "light_ambient");
	  light_diffuse_index = glGetUniformLocation(program(), "light_diffuse");
	  light_specular_index = glGetUniformLocation(program(), "light_specular");
	  // emissive_color_1_index = glGetUniformLocation(program(), "emissive_color_1");
	  // emissive_color_2_index = glGetUniformLocation(program(), "emissive_color_2");
	  texture_samplers_index = glGetUniformLocation(program(), "samplers");
    }

    void render(const mat4t &modelToProjection, const mat4t &modelToCamera, const vec4 &light_direction, float shininess, vec4 &light_ambient, vec4 &light_diffuse, vec4 &light_specular, int num_samplers = 6) {
      // tell openGL to use the program
      shader::render();

      // set the uniforms
	  // glUniform4fv(emissive_color_1_index, 1, emissive_color_1.get());
	  // glUniform4fv(emissive_color_2_index, 1, emissive_color_2.get());
	  glUniformMatrix4fv(modelToProjection_index, 1, GL_FALSE, modelToProjection.get()); 
	  glUniformMatrix4fv(modelToCamera_index, 1, GL_FALSE, modelToCamera.get());

	  glUniform3fv(light_direction_index, 1, light_direction.get());
	  glUniform4fv(light_ambient_index, 1, light_ambient.get());
	  glUniform4fv(light_specular_index, 1, light_specular.get());
	  glUniform4fv(light_diffuse_index, 1, light_diffuse.get());
	  glUniform1f(shininess_index, shininess);

	  static const GLint samplers[] = { 0, 1, 2, 3, 4, 6};
	  glUniform1iv(texture_samplers_index, num_samplers, samplers);
    }
  };
}
