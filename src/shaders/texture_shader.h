////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Single texture shader with no lighting

namespace octet {
  class texture_shader : public shader {
    // indices to use with glUniform*()
  protected:
    // index for model space to projection space matrix
    GLuint modelToProjectionIndex_;

    // index for texture sampler
    GLuint samplerIndex_;
  public:
    virtual void init() = 0;

    void render(const mat4t &modelToProjection, int sampler) {
      // tell openGL to use the program
      shader::render();

      // customize the program with uniforms
      glUniform1i(samplerIndex_, sampler);
      glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());
    }
  };
}
