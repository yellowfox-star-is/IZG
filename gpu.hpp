/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>
#include <vector>


//STRUCTS FOR VERTEXPULLER
struct head_t
{
    BufferID buffer;
    uint64_t offset;
    uint64_t stride;
    AttributeType AtrType;
    bool Enabled;
};

struct Indexing_t {
    BufferID bufferid;
    IndexType indextype;
};

struct vertexpuller_t
{
    
    Indexing_t Indexing;
    head_t head[maxAttributes];

};

struct FrameBuffer
{
    uint32_t width;
    uint32_t height;
    std::vector<uint8_t> colorbuffer;
    std::vector<float> depthbuffer;
};

//structs for programm
struct program
{
    VertexShader vertexshader = NULL;
    FragmentShader fragmentshader = NULL;
    Uniforms uniforms;
    std::vector<AttributeType>attributes;
};


/**
 * @brief This class represent software GPU
 */
class GPU{
  private:

      std::vector<std::vector<char>> bufferlist;
      std::vector<vertexpuller_t> vertexlist;
      std::vector<program> programlist;

      std::vector<ObjectID>usedID;
      std::vector<BufferID>usedBufferID;
      std::vector<VertexPullerID>usedVertexID;
      std::vector<ProgramID>usedProgramID;

      FrameBuffer framebuffer;

      uint64_t activeVertexPuller;
      uint64_t activeProgram;

      uint64_t findID(std::vector<ObjectID>& idlist, ObjectID id);
      uint64_t indexTypeSize(IndexType indextype);
      uint64_t attributeTypeSize(AttributeType atrbtype);

      //support functions
      void pullvertex(InVertex& invertex, uint32_t& invokation);
      uint8_t clipTriangle(OutVertex* triangle, OutVertex* striangle);
      OutVertex clipPoint(OutVertex badVertex, OutVertex goodVertex);
      void perspectiveDivision(OutVertex* triangle);
      void viewportTransformation(OutVertex* triangle);
      void triangleRaster(std::vector<InFragment>& pixels, OutVertex* triangle);
      void interpolate(InFragment& newvertex, OutVertex A, OutVertex B, OutVertex C);

  public:

    GPU();
    virtual ~GPU();

    //buffer object commands
    BufferID  createBuffer           (uint64_t size);
    void      deleteBuffer           (BufferID buffer);
    void      setBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void const* data);
    void      getBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void      * data);
    bool      isBuffer               (BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID  createVertexPuller     ();
    void      deleteVertexPuller     (VertexPullerID vao);
    void      setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer);
    void      setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer);
    void      enableVertexPullerHead (VertexPullerID vao,uint32_t head);
    void      disableVertexPullerHead(VertexPullerID vao,uint32_t head);
    void      bindVertexPuller       (VertexPullerID vao);
    void      unbindVertexPuller     ();
    bool      isVertexPuller         (VertexPullerID vao);

    //program object commands
    ProgramID createProgram          ();
    void      deleteProgram          (ProgramID prg);
    void      attachShaders          (ProgramID prg,VertexShader vs,FragmentShader fs);
    void      setVS2FSType           (ProgramID prg,uint32_t attrib,AttributeType type);
    void      useProgram             (ProgramID prg);
    bool      isProgram              (ProgramID prg);
    void      programUniform1f       (ProgramID prg,uint32_t uniformId,float     const&d);
    void      programUniform2f       (ProgramID prg,uint32_t uniformId,glm::vec2 const&d);
    void      programUniform3f       (ProgramID prg,uint32_t uniformId,glm::vec3 const&d);
    void      programUniform4f       (ProgramID prg,uint32_t uniformId,glm::vec4 const&d);
    void      programUniformMatrix4f (ProgramID prg,uint32_t uniformId,glm::mat4 const&d);

    //framebuffer functions
    void      createFramebuffer      (uint32_t width,uint32_t height);
    void      deleteFramebuffer      ();
    void      resizeFramebuffer      (uint32_t width,uint32_t height);
    uint8_t*  getFramebufferColor    ();
    float*    getFramebufferDepth    ();
    uint32_t  getFramebufferWidth    ();
    uint32_t  getFramebufferHeight   ();

    //execution commands
    void      clear                  (float r,float g,float b,float a);
    void      drawTriangles          (uint32_t  nofVertices);

    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{
    /// \todo zde si můžete vytvořit proměnné grafické karty (buffery, programy, ...)
    /// @}


};


