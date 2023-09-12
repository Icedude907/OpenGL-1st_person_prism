#pragma once

#include <stdint.h>
#include <array>
#include <string>
#include <iostream>

#include <GL/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../util/util.h"

namespace Render{

    // struct ColourRGB8{
    //     std::array<uint8_t, 3> data;
    //     inline constexpr ColourRGB8(uint32_t colour): data({
    //         (uint8_t)(colour & 0xff0000 >> 16),
    //         (uint8_t)(colour & 0x00ff00 >> 8), 
    //         (uint8_t)(colour & 0x0000ff)}){}
    //     inline constexpr ColourRGB8(uint8_t red, uint8_t green, uint8_t blue): data({red, green, blue}){}
    //     inline constexpr ColourRGB8(float red, float green, float blue): data({
    //         (uint8_t)(256*red),
    //         (uint8_t)(256*green),
    //         (uint8_t)(256*blue)}){}
    // };
    // 255 alpha = completely visible
    // struct ColourRGBA8{
    //     std::array<uint8_t, 4> data;
    //     inline constexpr ColourRGBA8(uint32_t colour): data({
    //         (uint8_t)(colour & 0xff000000 >> 24),
    //         (uint8_t)(colour & 0x00ff0000 >> 16), 
    //         (uint8_t)(colour & 0x0000ff00 >> 8),
    //         (uint8_t)(colour & 0x000000ff >> 0)}){}
    //     inline constexpr ColourRGBA8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255): data({red, green, blue, alpha}){}
    //     inline constexpr ColourRGBA8(float red, float green, float blue, float alpha = 1.0f): data({
    //         (uint8_t)(256*red),
    //         (uint8_t)(256*green),
    //         (uint8_t)(256*blue),
    //         (uint8_t)(256*alpha)}){}
    // };

    template <GLenum texType>
    class Textures{
        GLuint ids; // TODO only supports 1
        public:
        inline Textures(){}
        inline Textures(GLsizei count){
            glGenTextures(1, &ids);
        }
        inline void Delete(){
            glDeleteTextures(1, &ids);
        }
        inline void use(){ glBindTexture(texType, ids); }
        inline static void unbind(){ glBindTexture(texType, 0); }
        inline static void activate(GLenum texture){ glActiveTexture(GL_TEXTURE0 + texture); }
        inline static void setParamI(GLenum name, GLint value){ glTexParameteri(texType, name, value); }
        inline static void setTexture2D(GLint mipmap, GLenum format, GLsizei width, GLsizei height, GLenum dataType, Util::AnyPtr data){
            glTexImage2D(texType, mipmap, format, width, height, 0, format, dataType, data);
        }
        // TODO: figure out how do
        inline static void generateMipmaps(){ glGenerateMipmap(texType); }
    };
    using Textures2D = Textures<GL_TEXTURE_2D>;

    template <GLenum type>
    class Shader{
        private:
        GLuint shader;
        public:
        Shader(const GLchar* sources[], const GLint lens[], const GLsizei n){
            shader = glCreateShader(type);
            glShaderSource(shader, n, sources, lens);
            glCompileShader(shader);
            #ifndef DEBUG_SHADERS
                printShaderCompilerErrors();
            #endif
        }
        inline Shader(const uint8_t* source, const GLint len)
        : Shader((const GLchar**)(&source), &len, 1){
        }
        inline Shader(const Util::ArrPtr<GLchar, GLint> source)
        : Shader(source.ptr, source.len){
        }

        inline void Delete(){ glDeleteShader(shader); }
        inline operator GLuint(){ return shader; }

        void printShaderCompilerErrors(){
            // Stores status of compilation
            GLint hasCompiled;
            // Character array to store error message in
            char infoLog[1024];
            glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
            if (hasCompiled == GL_FALSE){ // TODO: is this working?
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "SHADER_COMPILATION_ERROR for:" 
                    << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") 
                    << "\n" << infoLog << std::endl;
            }
        }
    };
    struct ShaderUniform{
        GLuint id;
        inline ShaderUniform(){}
        inline ShaderUniform(GLuint id): id(id){}
        inline operator GLuint(){return id;}

        inline void setUniform(GLfloat value){ glUniform1f(id, value); }
        inline void setUniform(GLint   value){ glUniform1i(id, value); }
        inline void setUniform(glm::mat4& value, GLboolean transpose = GL_FALSE){
            glUniformMatrix4fv(id, 1, transpose, glm::value_ptr(value));
        }
    };
    using VertexShader = Shader<GL_VERTEX_SHADER>;
    using FragmentShader = Shader<GL_FRAGMENT_SHADER>;
    class ShaderProgram{
        private:
        GLuint program;
        public:
        inline ShaderProgram(){} // TODO: only in default

        ShaderProgram(VertexShader vertex, FragmentShader fragment);

        inline void use   (){ glUseProgram(program); }
        inline void Delete(){ glDeleteProgram(program); }
        inline ShaderUniform getUniform(const GLchar* name){
            return glGetUniformLocation(program, name);
        }

        void printCompilerErrors(){
            // Stores status of compilation
            GLint hasCompiled;
            // Character array to store error message in
            char infoLog[1024];
            glGetProgramiv(program, GL_LINK_STATUS, &hasCompiled);
            if (hasCompiled == GL_FALSE){
                glGetProgramInfoLog(program, 1024, NULL, infoLog);
                std::cout << "SHADER_LINKING_ERROR for:" << "PROGRAM" << "\n" << infoLog << std::endl;
            }
        }
    };
    // Misnomer:
    // Holds an array of data which is passed to shader via VertexArrays
    // (This can be vertex data, colour data, normal data, any data)
    class VertexBuffers{
        private:
        GLsizei n;
        GLuint buffers;
        public:
        inline VertexBuffers(){}
        inline VertexBuffers(GLsizei n): n(n){ glGenBuffers(n, &buffers); }
        
        inline void use   (){ glBindBuffer(GL_ARRAY_BUFFER, buffers); }
        inline void Delete(){ glDeleteBuffers(1, &buffers); }

        inline static void unbind(){ glBindBuffer(GL_ARRAY_BUFFER, 0); }

        inline static void setVertexBufferData(GLsizeiptr size, Util::AnyPtr data){
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        }
    };
    class ElementBuffers{
        private:
        GLsizei n;
        GLuint buffers;
        public:
        inline ElementBuffers(){}
        inline ElementBuffers(GLsizei n): n(n){ glGenBuffers(n, &buffers); }
        
        inline void use   (){ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers); }
        inline void Delete(){ glDeleteBuffers(1, &buffers); }

        inline static void unbind(){ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

        inline static void setElementBufferData(GLsizeiptr size, Util::AnyPtr data){
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        }
    };
    enum class AttribIPtr: GLenum{
        I8 = GL_BYTE, 
        U8 = GL_UNSIGNED_BYTE, 
        I16 = GL_SHORT, 
        U16 = GL_UNSIGNED_SHORT, 
        I32 = GL_INT, 
        U32 = GL_UNSIGNED_INT
    };
    enum class AttribPtr: GLenum{
        HALF_FLOAT = GL_HALF_FLOAT, 
        FLOAT = GL_FLOAT, 
        DOUBLE = GL_DOUBLE, 
        FIXED = GL_FIXED, 
        INT_2_10x3 = GL_INT_2_10_10_10_REV, 
        UINT_2_10x3 = GL_UNSIGNED_INT_2_10_10_10_REV, 
        UINT_10_11x2 = GL_UNSIGNED_INT_10F_11F_11F_REV,
        I8 = GL_BYTE, 
        U8 = GL_UNSIGNED_BYTE, 
        I16 = GL_SHORT, 
        U16 = GL_UNSIGNED_SHORT, 
        I32 = GL_INT, 
        U32 = GL_UNSIGNED_INT
    };
    enum class AttribLPtr: GLenum{
        DOUBLE = GL_DOUBLE,
    };
    // Arrays of VertexBuffers
    class VertexArrays{
        private:
        GLsizei n;
        GLuint arrays;
        public:
        inline VertexArrays(){}
        inline VertexArrays(GLsizei n): n(n){ glGenVertexArrays(n, &arrays); }

        // Links a Vertex Buffer to this Vertex Array
        inline void LinkBuffers(VertexBuffers& vertexBuffers, GLuint layoutIndex, GLuint numComponents, AttribPtr type, GLsizei stride, size_t offset, GLboolean normalized = GL_FALSE){
            vertexBuffers.use();
            glVertexAttribPointer(layoutIndex, numComponents, static_cast<GLenum>(type), normalized, stride, (void*)offset);
            glEnableVertexAttribArray(layoutIndex);
            // vertexBuffers.unbind();
        }
        inline void LinkBuffers(VertexBuffers& vertexBuffers, GLuint layoutIndex, GLuint numComponents, AttribIPtr type, GLsizei stride, size_t offset){
            vertexBuffers.use();
            glVertexAttribIPointer(layoutIndex, numComponents, static_cast<GLenum>(type), stride, (void*)offset);
            glEnableVertexAttribArray(layoutIndex);
        }
        inline void LinkBuffers(VertexBuffers& vertexBuffers, GLuint layoutIndex, GLuint numComponents, AttribLPtr type, GLsizei stride, size_t offset){
            vertexBuffers.use();
            glVertexAttribLPointer(layoutIndex, numComponents, static_cast<GLenum>(type), stride, (void*)offset);
            glEnableVertexAttribArray(layoutIndex);
        }

        inline void use   (){ glBindVertexArray(arrays); }
        inline void Delete(){ glDeleteVertexArrays(1, &arrays); }

        inline static void unbind(){ glBindVertexArray(0); }
    };
}