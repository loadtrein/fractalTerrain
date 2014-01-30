

namespace octet {


  class sea_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjection_index; 
	GLuint modelToCamera_index;
  GLuint cameraToWorld_index;

	GLuint light_uniforms_index;
	GLuint num_lights_index;

	GLuint angle_index;
	GLuint terrain_length_index;

	// index for multi textures
	GLuint texture_samplers_index;

  GLuint cubeMap_sampler_index;



  public:
    void init() {
     
      // vertex shader
      const char vertex_shader[] = SHADER_STR(
		// setting a variable to varying permits to share among vertex shader and fragment shader
		varying vec4 pos_;
	    varying vec4 color_;
		varying vec3 norm_;
		varying vec2 uv_;
		varying vec3 tangent_;
		varying vec3 bitangent_;

		// attributes are passed with OPENGL glVertexAttribPointer
        attribute vec4 pos;
		attribute vec3 normal;
        attribute vec4 color;
		attribute vec2 uv;
		attribute vec3 tangent;
		attribute vec3 bitangent;

		// from glUniform
        uniform mat4 modelToProjection;
		uniform mat4 modelToCamera;
		uniform float angle;
		uniform int t_length;

		const float _pi = 3.14159;

		float rand(vec2 co){
			return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
		}

		float waveGenerator(vec2 direction, float amplitude, float wavelength, float speed, float x, float y) {
			float frequency = 2*_pi/wavelength;
			float phase = speed * frequency;
			float theta = dot(direction, vec2(x, y));
			return amplitude * sin(theta * frequency + angle * phase);
		}
      
        void main() {
		  pos_ = pos;
		  color_ = color;
		  uv_ = uv;
		  norm_ = normal;

		  norm_ = (modelToCamera * vec4(normal,0.0)).xyz;
		  tangent_ = (modelToCamera * vec4(tangent,0.0)).xyz;
		  bitangent_ = (modelToCamera * vec4(bitangent,0.0)).xyz;
		  vec4 npos = normalize(pos);

		  float half_length =  float(t_length/2);
		  float t_lengthf = float(t_length);

		  vec3 nnoise = noise3(npos.xyz);
		  float randPos = rand(vec2(pos.x, pos.z+angle/10000));

		  vec2 wave_vector = vec2(0.5/2-uv.x, 0.5/2-uv.y);
		  vec2 wave_vector_2 = vec2(uv.x, uv.y);
		  
		  // vec2 direction, float amplitude, float wavelength, float speed, float x, float y
		  float w0 = waveGenerator( wave_vector_2, 0.3, 10.0,  0.2, (pos.x+half_length), (pos.z+half_length) );
		  float w1 = waveGenerator( wave_vector_2, 0.1, 3.0, 0.1, pos.x, pos.z);
		  float w2 = waveGenerator( wave_vector_2, 0.2, 0.2, 0.1, -pos.x, -pos.z);
		  float w3 = waveGenerator( wave_vector_2, 0.2, 0.3, 0.1, -pos.x, pos.z);
		  float w4 = waveGenerator( wave_vector_2, 0.1, 0.4, 0.2, pos.x, -pos.z);
		  float w5 = waveGenerator( wave_vector_2, 5.0, t_length, 5.0, pos.x, pos.x);
		  float w6 = waveGenerator( wave_vector_2, 6.0,   t_length, 4.0, pos.z, pos.z);
		  float w7 = waveGenerator( wave_vector_2, 2.0,   t_length/2, 9.0, -pos.z, -pos.z);
		  float w8 = waveGenerator( wave_vector_2, 3.0,   t_length/2, 5.0, -pos.x, pos.z);
		  float w9 = waveGenerator( wave_vector, 0.5, 8.0, 0.2,(pos.x+half_length), (pos.z+half_length));
	

		  float final_wave = w0 + w1 + w2 + w3 + w4 + w5 + w6; //+ w7 + w8 + w9;
		
		  vec3 new_norm = normalize(vec3(wave_vector.x, final_wave, wave_vector.y));
		  norm_ = (modelToCamera * vec4(new_norm,0)).xyz;

		  vec4 new_pos = vec4(pos.x, pos.y + final_wave, pos.z, pos.w); 
		 
		  gl_Position = modelToProjection * new_pos; 



        }
      );

      // fragment shader
      const char fragment_shader[] = SHADER_STR(
		varying vec4 color_;
	    varying vec4 pos_;
		varying vec3 norm_;
		varying vec2 uv_;
		varying vec3 tangent_;
		varying vec3 bitangent_;

		uniform vec4 light_uniforms[9];
		uniform int num_lights;

		uniform sampler2D samplers[7];

    uniform samplerCube sampler;
    uniform vec4 cameraToWorld;

		
      void main() {
			float shininess = 50.0;
			vec3 bump = normalize(vec3(texture2D(samplers[5], uv_).xy-vec2(0.5, 0.5), 1.0));
			vec3 nnormal = norm_;
			vec3 diffuse_light = vec3(0.3, 0.3, 0.3);
			vec3 specular_light = vec3(0.0, 0.0, 0.0);

			for (int i=0; i<num_lights; ++i) {
				vec3 light_direction = light_uniforms[i*4 +2].xyz;
				vec3 light_color = light_uniforms[i*4+3].xyz;
				vec3 half_direction = normalize(light_direction + vec3(0.0, 0.0, 1.0));

				float diffuse_factor = max(dot(light_direction, nnormal), 0.2);
				float specular_factor = pow(max(dot(half_direction, nnormal), 0.7), shininess)*diffuse_factor;

				diffuse_light += diffuse_factor * light_color;
				specular_light += specular_factor * light_color;
			}

      vec4 color = texture2D(samplers[4], uv_*8);
      color.a = 0.5;

			vec4 diffuse = color;
			vec4 ambient = color;
			vec4 emission = texture2D(samplers[5], uv_);
			vec4 specular = texture2D(samplers[6], uv_);

			vec3 ambient_light = light_uniforms[0].xyz;

      vec4 eyeDir = pos_- cameraToWorld;
			
      vec3 reflectedDirection = normalize(reflect(normalize(eyeDir), normalize(norm_)));
      //reflectedDirection.y = -reflectedDirection.y;

      vec3 sky_box_Color = textureCube(sampler, reflectedDirection);

			gl_FragColor.xyz = mix(
				ambient_light * ambient.xyz +
				diffuse_light * diffuse.xyz +
				emission.xyz +
				specular_light * specular.xyz , sky_box_Color , 1.0);

			gl_FragColor.w = diffuse.w; 
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
      cameraToWorld_index =  glGetUniformLocation(program(), "cameraToWorld");

      light_uniforms_index = glGetUniformLocation(program(), "light_uniforms");
      num_lights_index = glGetUniformLocation(program(), "num_lights");
	
      angle_index = glGetUniformLocation(program(), "angle");
      terrain_length_index = glGetUniformLocation(program(), "t_length");

      texture_samplers_index = glGetUniformLocation(program(), "samplers");
	  
    }

    void render(const mat4t &modelToProjection, const mat4t &modelToCamera,const mat4t &cameraToWorld, const vec4 *light_uniforms, int num_light_uniforms, int num_lights, float angle, int terrain_length, int sampler) {
      // tell openGL to use the program
      shader::render();

        // set the uniforms
	    // glUniform4fv(emissive_color_1_index, 1, emissive_color_1.get());
	    // glUniform4fv(emissive_color_2_index, 1, emissive_color_2.get());
	    glUniformMatrix4fv(modelToProjection_index, 1, GL_FALSE, modelToProjection.get()); 
	    glUniformMatrix4fv(modelToCamera_index, 1, GL_FALSE, modelToCamera.get());
      glUniformMatrix4fv(cameraToWorld_index,1,GL_FALSE,cameraToWorld.get());

		glUniform4fv(light_uniforms_index, num_light_uniforms, (float*)light_uniforms);
		glUniform1i(num_lights_index, num_lights);

		glUniform1f(angle_index, angle);
		glUniform1i(terrain_length_index, terrain_length);

    glUniform1i(cubeMap_sampler_index, sampler);

	    static const GLint samplers[] = { 0, 1, 2, 3, 4, 5, 6};
	    glUniform1iv(texture_samplers_index, 7, samplers);
    }
  };
}
