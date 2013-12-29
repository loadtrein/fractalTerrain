

namespace octet {
  class terrain_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjectionIndex_;

    // index for texture sampler
    GLuint samplerIndex_0;
	GLuint samplerIndex_1;

  public:
    void init() {
      // vertex shader
      const char vertex_shader[] = SHADER_STR(
        varying vec2 uv_;

        attribute vec4 pos;
        attribute vec2 uv;

        uniform mat4 modelToProjection;

        void main() { gl_Position = modelToProjection * pos; uv_ = uv; }
      );

      // fragment shader
      const char fragment_shader[] = SHADER_STR(
        varying vec2 uv_;
        uniform sampler2D texture1;
		uniform sampler2D texture2;

        void main() { 
			vec4 color = texture2D(texture1, vec2(gl_TexCoord[0]));
			vec4 normal = texture2D(texture2, vec2(gl_TexCoord[1]));
			gl_FragColor = normal + color;
		}

		


      );
    
      // use the common shader code to compile and link the shaders
      // the result is a shader program
      shader::init(vertex_shader, fragment_shader);

      // extract the indices of the uniforms to use later
      modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
      samplerIndex_0 = glGetUniformLocation(program(), "sampler_0");
	  samplerIndex_1 = glGetUniformLocation(program(), "sampler_1"); 
    }

    void render(const mat4t &modelToProjection, int sampler_0, int sampler_1) {
      // tell openGL to use the program
      shader::render();

      // customize the program with uniforms
      glUniform1i(samplerIndex_0, sampler_0);
	  glUniform1i(samplerIndex_1, sampler_1);
      glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());
    }
  };
}

