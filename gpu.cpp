/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <vector>
#include <algorithm>


/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU(){
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty

    activeVertexPuller = emptyID;
    activeProgram = emptyID;
    framebuffer.height = 0;
    framebuffer.width = 0;

}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU(){
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu
}

/// @}

//najde umístění bufferu v ID listu použitých ID
uint64_t GPU::findID(std::vector<ObjectID>& idlist, ObjectID id)
{
    uint64_t i = 0;
    for (i = 0; i < idlist.size(); i++)
    {
        if (idlist[i] == id)
        {
           return i;
        }
    }

    return emptyID;
}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

/**
 * @brief This function allocates buffer on GPU.
 *
 * @param size size in bytes of new buffer on GPU.
 *
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size) { 
  /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
  /// Velikost bufferu je v parameteru size (v bajtech).<br>
  /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
  /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>

    std::vector<char> space;
    space.resize(size);

  BufferID newid = emptyID;

  //Check if there is an avaible ID
  uint64_t i = 0;
  for (i = 0; i < usedID.size(); i++)
  {
      if (usedID[i] == emptyID)
      {
          usedID[i] = i;
          break;
      }
  }

  //if there is no, create a new ID
  if (newid == emptyID)
  {
      newid = usedID.size();
      usedID.push_back(newid);
  }

  //PLACE ID INTO BUFFERID
  for (i = 0; i < usedBufferID.size(); i++)
  {
      if (usedBufferID[i] == emptyID)
      {
          usedBufferID[i] = newid;
          bufferlist[i] = space;
          return newid;
      }
  }
  
  usedBufferID.push_back(newid);
  bufferlist.push_back(space);
  return newid;
}

/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) {
  /// \todo Tato funkce uvolní buffer na grafické kartě.
  /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
  /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.

    if (!isBuffer(buffer))
    {
        return;
    }
    
    usedID[buffer] = emptyID;

    uint64_t i = findID(usedBufferID, buffer);

    usedBufferID[i] = emptyID;
    bufferlist[i].clear();
}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data) {
  /// \todo Tato funkce nakopíruje data z cpu na "gpu".<br>
  /// Data by měla být nakopírována do bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje, kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) kam se data nakopírují.<br>
  /// Parametr data obsahuje ukazatel na data na cpu pro kopírování.<br>

    if (!isBuffer(buffer))
    {
        return;
    }

    //std::vector<char> * dest = NULL;

    uint64_t i = findID(usedBufferID, buffer);

    const uint8_t * first = (uint8_t *) data;
    const uint8_t * last = (uint8_t *) data;
    last += size;

    
    std::copy(first, last, &bufferlist[i][offset]);
}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes. 
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned. 
 */
void GPU::getBufferData(BufferID buffer,
                        uint64_t offset,
                        uint64_t size,
                        void*    data)
{
  /// \todo Tato funkce vykopíruje data z "gpu" na cpu.
  /// Data by měla být vykopírována z bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) odkud se začne kopírovat.<br>
  /// Parametr data obsahuje ukazatel, kam se data nakopírují.<br>

    if (!isBuffer(buffer))
    {
        return;
    }

    uint64_t i = findID(usedBufferID, buffer);

    //std::copy(&bufferlist[i][offset], &bufferlist[i][offset + size - 1], (uint8_t *) data);
    std::copy_n(&bufferlist[i][offset], size, (char*)data);
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) { 
  /// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
  /// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
  /// Pro emptyId vrací false.<br>

    if (buffer == emptyID)
    {
        return false;
    }

    for (uint64_t i = 0; i < usedBufferID.size(); i++)
    {
        if (usedBufferID[i] == buffer)
        {
            return true;
        }
    }
    
    return false;
}

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller     (){
  /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
  /// Funkce by měla vrátit identifikátor nové tabulky.
  /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.

    VertexPullerID newid = emptyID;

    //Check if there is an avaible ID
    uint64_t i = 0;
    for (i = 0; i < usedID.size(); i++)
    {
        if (usedID[i] == emptyID)
        {
            usedID[i] = i;
            newid = i;
            break;
        }
    }

    //if there is no, create a new ID
    if (newid == emptyID)
    {
        newid = usedID.size();
        usedID.push_back(newid);
    }

    vertexpuller_t newvertex;
    newvertex.Indexing.bufferid = emptyID;
    newvertex.Indexing.indextype = IndexType::UINT8;

    head_t newhead;
    newhead.AtrType = AttributeType::EMPTY;
    newhead.buffer = emptyID;
    newhead.Enabled = false;
    newhead.offset = 0;
    newhead.stride = 0;
    for (int i = 0; i < maxAttributes; i++)
    {
        newvertex.head[i] = newhead;
    }

    //PLACE ID INTO vertexRID
    for (i = 0; i < usedVertexID.size(); i++)
    {
        if (usedVertexID[i] == emptyID)
        {
            usedVertexID[i] = newid;
            vertexlist[i] = newvertex;
            return newid;
        }
    }
    
    usedVertexID.push_back(newid);
    vertexlist.push_back(newvertex);
    return newid;
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void     GPU::deleteVertexPuller     (VertexPullerID vao){
  /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
  /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
  /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>

    if (!isVertexPuller(vao))
    {
        return;
    }

    usedID[vao] = emptyID;

    uint64_t i = findID(usedVertexID, vao);
    usedVertexID[i] = emptyID;
}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void     GPU::setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer){
  /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
  /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
  /// Parametr "stride" nastaví krok čtecí hlavy.<br>
  /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
  /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>

    if (!isVertexPuller(vao))
    {
        return;
    }

    uint64_t i = findID(usedVertexID, vao);

    vertexlist[i].head[head].AtrType = type;
    vertexlist[i].head[head].stride = stride;
    vertexlist[i].head[head].offset = offset;
    vertexlist[i].head[head].buffer = buffer;
}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void     GPU::setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer){
  /// \todo Tato funkce nastaví indexování vertex pulleru.
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
  /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>

    if (!isVertexPuller(vao))
    {
        return;
    }

    uint64_t i = findID(usedVertexID, vao);

    vertexlist[i].Indexing.indextype = type;
    vertexlist[i].Indexing.bufferid = buffer;
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller 
 * @param head head id
 */
void     GPU::enableVertexPullerHead (VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
  /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
  /// Parametr "head" volí čtecí hlavu.<br>

    if (!isVertexPuller(vao))
    {
        return;
    }

    uint64_t i = findID(usedVertexID, vao);

    vertexlist[i].head[head].Enabled = true;
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void     GPU::disableVertexPullerHead(VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
  /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>

    if (!isVertexPuller(vao))
    {
        return;
    }

    uint64_t i = findID(usedVertexID, vao);

    vertexlist[i].head[head].Enabled = false;
}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void     GPU::bindVertexPuller       (VertexPullerID vao){
  /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
  /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>

    if (!isVertexPuller(vao))
    {
        return;
    }

    uint64_t i = 0;
    for (i = 0; i < usedVertexID.size(); i++)
    {
        if (usedVertexID[i] == vao)
        {
            activeVertexPuller = i;
            break;
        }
    }
}

/**
 * @brief This function deactivates vertex puller.
 */
void     GPU::unbindVertexPuller     (){
  /// \todo Tato funkce deaktivuje vertex puller.
  /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.

    activeVertexPuller = emptyID;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool     GPU::isVertexPuller         (VertexPullerID vao){
  /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
  /// Pokud ano, funkce vrací true.

    if (vao == emptyID)
    {
        return false;
    }

    for (uint64_t i = 0; i < usedVertexID.size(); i++)
    {
        if (usedVertexID[i] == vao)
        {
            return true;
        }
    }

    return false;
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID        GPU::createProgram         (){
  /// \todo Tato funkce by měla vytvořit nový shader program.<br>
  /// Funkce vrací unikátní identifikátor nového proramu.<br>
  /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
  /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>

    ProgramID newid = emptyID;

    //Check if there is an avaible ID
    uint64_t i = 0;
    for (i = 0; i < usedID.size(); i++)
    {
        if (usedID[i] == emptyID)
        {
            usedID[i] = i;
            newid = i;
            break;
        }
    }

    //if there is no, create a new ID
    if (newid == emptyID)
    {
        newid = usedID.size();
        usedID.push_back(newid);
    }

    //create new Program
    program newprogram;
    newprogram.fragmentshader = NULL;
    newprogram.vertexshader = NULL;
    newprogram.attributes.resize(maxAttributes);

    //PLACE ID INTO vertexRID
    for (i = 0; i < usedProgramID.size(); i++)
    {
        if (usedProgramID[i] == emptyID)
        {
            usedProgramID[i] = newid;
            programlist[i] = newprogram;
            return newid;
        }
    }

    usedProgramID.push_back(newid);
    programlist.push_back(newprogram);

    return newid;
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void             GPU::deleteProgram         (ProgramID prg){
  /// \todo Tato funkce by měla smazat vybraný shader program.<br>
  /// Funkce smaže nastavení shader programu.<br>
  /// Identifikátor programu se stane volným a může být znovu využit.<br>

    if (!isProgram(prg))
    {
        return;
    }

    uint64_t i = findID(usedProgramID, prg);

    usedProgramID[i] = emptyID;
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader 
 * @param fs fragment shader
 */
void             GPU::attachShaders         (ProgramID prg,VertexShader vs,FragmentShader fs){
  /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.

    if (!isProgram(prg))
    {
        return;
    }

    uint64_t i = findID(usedProgramID, prg);
    programlist[i].vertexshader = vs;
    programlist[i].fragmentshader = fs;
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void             GPU::setVS2FSType          (ProgramID prg,uint32_t attrib,AttributeType type){
  /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
  /// V průběhu rasterizace vznikají fragment.<br>
  /// Fragment obsahují fragment atributy.<br>
  /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
  /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
  /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>

    if (!isProgram(prg))
    {
        return;
    }

    uint64_t i = findID(usedProgramID, prg);

    programlist[i].attributes[attrib] = type;
}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void             GPU::useProgram            (ProgramID prg){
  /// \todo tato funkce by měla vybrat aktivní shader program.

    if (!isProgram(prg))
    {
        return;
    }

    activeProgram = findID(usedProgramID, prg);
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool             GPU::isProgram             (ProgramID prg){
  /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
  /// Funkce vráti true, pokud program existuje.<br>

    if (prg == emptyID)
    {
        return false;
    }

    for (uint64_t i = 0; i < usedProgramID.size(); i++)
    {
        if (usedProgramID[i] == prg)
        {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform1f      (ProgramID prg,uint32_t uniformId,float     const&d){
  /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
  /// Parametr "prg" vybírá shader program.<br>
  /// Parametr "uniformId" vybírá uniformní proměnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
  /// Parametr "d" obsahuje data (1 float).<br>

    if (!isProgram(prg))
    {
        return;
    }

    uint64_t i = findID(usedProgramID, prg);

    programlist[i].uniforms.uniform[uniformId].v1 = d; 
}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform2f      (ProgramID prg,uint32_t uniformId,glm::vec2 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 2 floaty.

    if (!isProgram(prg))
    {
        return;
    }

    uint64_t i = findID(usedProgramID, prg);

    programlist[i].uniforms.uniform[uniformId].v2 = d;
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform3f      (ProgramID prg,uint32_t uniformId,glm::vec3 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 3 floaty.

    if (!isProgram(prg))
    {
        return;
    }

    uint64_t i = findID(usedProgramID, prg);

    programlist[i].uniforms.uniform[uniformId].v3 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform4f      (ProgramID prg,uint32_t uniformId,glm::vec4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 4 floaty.

    if (!isProgram(prg))
    {
        return;
    }

    uint64_t i = findID(usedProgramID, prg);

    programlist[i].uniforms.uniform[uniformId].v4 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniformMatrix4f(ProgramID prg,uint32_t uniformId,glm::mat4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává matici 4x4 (16 floatů).

    if (!isProgram(prg))
    {
        return;
    }

    uint64_t i = findID(usedProgramID, prg);

    programlist[i].uniforms.uniform[uniformId].m4 = d;
}

/// @}





/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer      (uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
  /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
  /// Buffery obsahují width x height pixelů.<br>
  /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
  /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
  /// Nultý pixel framebufferu je vlevo dole.<br>

    framebuffer.height = height;
    framebuffer.width = width;
    framebuffer.colorbuffer.clear();
    framebuffer.depthbuffer.clear();
    framebuffer.colorbuffer.resize(width * height * 4);
    framebuffer.depthbuffer.resize(width * height);
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer      (){
  /// \todo tato funkce by měla dealokovat framebuffer.

    framebuffer.height = 0;
    framebuffer.width = 0;
    framebuffer.colorbuffer.clear();
    framebuffer.depthbuffer.clear();
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void     GPU::resizeFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla změnit velikost framebuffer.

    framebuffer.width = width;
    framebuffer.height = height;
    framebuffer.colorbuffer.resize(width * height * 4);
    framebuffer.depthbuffer.resize(width * height);
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t* GPU::getFramebufferColor  (){
  /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>

    return &framebuffer.colorbuffer[0];
  //return nullptr;
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float* GPU::getFramebufferDepth    (){
  /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>

    return &framebuffer.depthbuffer[0];
  //return nullptr;
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth (){
  /// \todo Tato funkce by měla vrátit šířku framebufferu.

  return framebuffer.width;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight(){
  /// \todo Tato funkce by měla vrátit výšku framebufferu.

  return framebuffer.height;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void            GPU::clear                 (float r,float g,float b,float a){
  /// \todo Tato funkce by měla vyčistit framebuffer.<br>
  /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
  /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
  /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
  /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>

    if (r > 1.0f)
    {
        r = 1.0f;
    }
    if (g > 1.0f)
    {
        g = 1.0f;
    }
    if (b > 1.0f)
    {
        b = 1.0f;
    }
    if (a > 1.0f)
    {
        a = 1.0f;
    }

    //clear depth
    float *depthbuffer = getFramebufferDepth();
    for (uint64_t x = 0; x < (uint64_t) getFramebufferHeight() * getFramebufferWidth(); x++)
    {
        //cokoliv větší než 1 postačuje a 42 je cool :D
        depthbuffer[x] = 42;
    }

    //clear collor
    uint8_t *colorbuffer = getFramebufferColor();
    for (uint64_t x = 0; x < (uint64_t) getFramebufferHeight() * getFramebufferWidth() * 4; x++)
    {
        switch (x % 4)
        {
        case 0:
            colorbuffer[x] = r * 255;
            break;
        case 1:
            colorbuffer[x] = g * 255;
            break;
        case 2:
            colorbuffer[x] = b * 255;
            break;
        case 3:
            colorbuffer[x] = a * 255;
            break;
        default:
            break;
        }
    }
}



void            GPU::drawTriangles         (uint32_t  nofVertices){
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
  /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>

    //AVP active vertex puller
    vertexpuller_t& AVP = vertexlist[activeVertexPuller];
    //AP active program
    program& AP = programlist[activeProgram];

    uint32_t invokation = 0;
    OutVertex overtex;
    InVertex ivertex;
    OutVertex triangle[3];
    OutVertex striangle[3];

    uint8_t nofTriangles;

    std::vector<InFragment> infragments;
    std::vector<OutFragment> outfragments;

    OutFragment outfragment;

    uint8_t *framebuffercollor;
    float* framebufferdepth;

    for (; invokation < nofVertices; invokation++)
    {
        //vertexpuller
        pullvertex(ivertex, invokation);

        //vertexshader
        AP.vertexshader(overtex, ivertex, programlist[activeProgram].uniforms);

        //primitve assembly
        triangle[invokation % 3] = overtex;
        if ((invokation % 3) != 2)
        {
            continue;
        }

        //clipping
        nofTriangles = clipTriangle(triangle, striangle);
        if (nofTriangles == 0)
        {
            continue;
        }
        for (uint8_t j = 0; j < nofTriangles; j++)
        {
            if (j == 1)
            {
                //load vertices of secundary triangle into primary triangle
                for (uint8_t k = 0; k < 3; k++)
                {
                    triangle[k] = striangle[k];
                }
            }

        //perspektive division
            perspectiveDivision(triangle);

        //viewport transformation
            viewportTransformation(triangle);

        //rasterize
            triangleRaster(infragments, triangle);

        //fragment
            for (uint64_t i = 0; i < infragments.size(); i++)
            {
                infragments[i].gl_FragCoord[0] += 0.5;
                infragments[i].gl_FragCoord[1] += 0.5;
                AP.fragmentshader(outfragment, infragments[i], AP.uniforms);
                infragments[i].gl_FragCoord[0] -= 0.5;
                infragments[i].gl_FragCoord[1] -= 0.5;

        //perfragment operations
                framebufferdepth = getFramebufferDepth() + (uint64_t)infragments[i].gl_FragCoord[0] + ((uint64_t)infragments[i].gl_FragCoord[1]) * framebuffer.width;
                if (*framebufferdepth > infragments[i].gl_FragCoord[2])
                {
                    *framebufferdepth = infragments[i].gl_FragCoord[2];

                    framebuffercollor = getFramebufferColor() + ((uint64_t)infragments[i].gl_FragCoord[0]) * 4 + ((uint64_t)infragments[i].gl_FragCoord[1]) * 4 * framebuffer.width;
                    for (int a = 0; a < 4; a++)
                    {
                        framebuffercollor[a] = outfragment.gl_FragColor[a] * 255;
                    }

                }
            }

        //prepare for next triangle
            infragments.clear();
        }
    }
}

/// @}

//pulls one vertex, saves it into reference of InVertex
void GPU::pullvertex(InVertex& invertex, uint32_t& invokation)
{
    //AVP Active Vertex Puller
    vertexpuller_t& AVP = vertexlist[activeVertexPuller];
    
    //if indexing is off, vertex is surelly here
    invertex.gl_VertexID = invokation;

    //if not, we have to find it's location
    if (AVP.Indexing.bufferid != emptyID)
    {
        getBufferData(AVP.Indexing.bufferid, indexTypeSize(AVP.Indexing.indextype) * invokation, indexTypeSize(AVP.Indexing.indextype), &invertex.gl_VertexID);
    }

    //use the heads
    for (uint64_t i = 0; i < maxAttributes; i++)
    {
        if (AVP.head[i].Enabled == true)
        {
            //offset + stride * gl_VertexID
            switch (AVP.head[i].AtrType)
            {
            case AttributeType::EMPTY:
                //NOTHING
                break;
            case AttributeType::FLOAT:
                getBufferData(AVP.head[i].buffer, AVP.head[i].offset + AVP.head[i].stride * invertex.gl_VertexID, attributeTypeSize(AVP.head[i].AtrType), &invertex.attributes[i].v1);
                break;
            case AttributeType::VEC2:
                getBufferData(AVP.head[i].buffer, AVP.head[i].offset + AVP.head[i].stride * invertex.gl_VertexID, attributeTypeSize(AVP.head[i].AtrType), &invertex.attributes[i].v2);
                break;
            case AttributeType::VEC3:
                getBufferData(AVP.head[i].buffer, AVP.head[i].offset + AVP.head[i].stride * invertex.gl_VertexID, attributeTypeSize(AVP.head[i].AtrType), &invertex.attributes[i].v3);
                break;
            case AttributeType::VEC4:
                getBufferData(AVP.head[i].buffer, AVP.head[i].offset + AVP.head[i].stride * invertex.gl_VertexID, attributeTypeSize(AVP.head[i].AtrType), &invertex.attributes[i].v4);
                break;
            default:
                //NOTHING
                break;
            }
        }
    }
}

//clips triangle, splits them into two if needed and returns number of triangles
uint8_t GPU::clipTriangle(OutVertex *triangle, OutVertex *striangle)
{

    OutVertex& A = triangle[0];
    OutVertex& B = triangle[1];
    OutVertex& C = triangle[2];

    uint8_t state = 0;
    uint8_t badvertexes = 0;

    OutVertex new1;
    OutVertex new2;
    while (true)
    {
        switch (state)
        {
        case 0://we know nothing about triangle
            for (uint8_t i = 0; i < 3; i++)
            {
                if (!(-triangle[i].gl_Position[3] <= triangle[i].gl_Position[2]))
                {
                    badvertexes++;
                    state += pow(10, i);
                }
            }
            if (badvertexes == 0)
            {
                return 1; //all points are okay, we have one triangle
            }
            break;
        case 110: // B and C is wrong
            B = clipPoint(B, A);
            C = clipPoint(C, A);
            return 1;
            break;
        case 101: // A and C is wrong
            A = clipPoint(A, B);
            C = clipPoint(C, B);
            return 1;
            break;
        case 11: // A and B is wrong
            A = clipPoint(A, C);
            B = clipPoint(B, C);
            return 1;
            break;
        case 001: // A is wrong
            new1 = clipPoint(A, B);
            new2 = clipPoint(A, C);
            triangle[0] = new1;
            striangle[0] = new1;
            striangle[1] = new2;
            striangle[2] = C;
            return 2;
            break;
        case 010: // B is wrong
            new1 = clipPoint(B, A);
            new2 = clipPoint(B, C);
            triangle[1] = new1;
            striangle[0] = new1;
            striangle[1] = new2;
            striangle[2] = C;
            return 2;
            break;
        case 100: // C is wrong
            new1 = clipPoint(C, A);
            new2 = clipPoint(C, B);
            triangle[2] = new1;
            striangle[0] = new1;
            striangle[1] = new2;
            striangle[2] = B;
            return 2;
            break;
        case 111: //all points are off
            return 0;
            break;
        default:
            return 0;
            break;
        }
    }
}

OutVertex GPU::clipPoint(OutVertex badVertex, OutVertex goodVertex)
{
    program& AP = programlist[activeProgram];
    OutVertex newvertex;
    double t = (-goodVertex.gl_Position[3] - goodVertex.gl_Position[2]) / (badVertex.gl_Position[3] - goodVertex.gl_Position[3] + badVertex.gl_Position[2] - goodVertex.gl_Position[2]);

    for (int i = 0; i < 4; i++)
    {
        newvertex.gl_Position[i] = goodVertex.gl_Position[i] + t * (badVertex.gl_Position[i] - goodVertex.gl_Position[i]);
    }

    for (int i = 0; i < maxAttributes; i++)
    {
        switch (AP.attributes[i])
        {
        case AttributeType::FLOAT:
            newvertex.attributes[i].v1 = goodVertex.attributes[i].v1 + t * (badVertex.attributes[i].v1 - goodVertex.attributes[i].v1);
            break;
        case AttributeType::VEC2:
            newvertex.attributes[i].v2 = goodVertex.attributes[i].v2 + glm::vec2(t, t) * (badVertex.attributes[i].v2 - goodVertex.attributes[i].v2);
            break;
        case AttributeType::VEC3:
            newvertex.attributes[i].v3 = goodVertex.attributes[i].v3 + glm::vec3(t, t, t) * (badVertex.attributes[i].v3 - goodVertex.attributes[i].v3);
            break;
        case AttributeType::VEC4:
            newvertex.attributes[i].v4 = goodVertex.attributes[i].v4 + glm::vec4(t, t, t, t) * (badVertex.attributes[i].v4 - goodVertex.attributes[i].v4);
            break;
        default:
            break;
        }
    }

    return newvertex;
}

void GPU::perspectiveDivision(OutVertex* triangle)
{
    for (int a = 0; a < 3; a++)
    {
        for (int b = 0; b < 3; b++)
        {
            triangle[a].gl_Position[b] = triangle[a].gl_Position[b] / triangle[a].gl_Position[3];
        }
    }
}

void GPU::viewportTransformation(OutVertex* triangle)
{
    for (int a = 0; a < 3; a++)
    {
        triangle[a].gl_Position[0] = (triangle[a].gl_Position[0] + 1) / 2 * framebuffer.width;
        triangle[a].gl_Position[1] = (triangle[a].gl_Position[1] + 1) / 2 * framebuffer.height;
    }
}

void GPU::triangleRaster(std::vector<InFragment>& pixels, OutVertex* triangle)
{

    program& AP = programlist[activeProgram];

    double lowX = 42;
    double lowY = 42;
    double higX = 42;
    double higY = 42;

    OutVertex& A = triangle[0];
    OutVertex& B = triangle[1];
    OutVertex& C = triangle[2];

    //pokud se A.y == C.y pak použité algoritmy na výpočet barycentrických coordinát a na určení, zda-li je bod v trojúhelníku přestávají fungovat
    //drobné vychýlení neublíží výsledku a algoritmy budou funkční
    if (((double)A.gl_Position.y - C.gl_Position.y) == 0.0)
    {
        A.gl_Position.y += 0.001;
    }
        

    lowX = triangle[0].gl_Position[0];
    higX = lowX;
    lowY = triangle[0].gl_Position[1];
    higY = lowY;

    for (int i = 1; i < 3; i++)
    {
        if (triangle[i].gl_Position[0] > higX)
            higX = triangle[i].gl_Position[0];
        if (triangle[i].gl_Position[0] < lowX)
            lowX = triangle[i].gl_Position[0];
        if (triangle[i].gl_Position[1] > higY)
            higY = triangle[i].gl_Position[1];
        if (triangle[i].gl_Position[1] < lowY)
            lowY = triangle[i].gl_Position[1];
    }

    float l0;
    float l1;
    float l2;

    float h0 = A.gl_Position.w;
    float h1 = B.gl_Position.w;
    float h2 = C.gl_Position.w;

    float A0;
    float A1;
    float A2;

    float den = 1 / ((B.gl_Position.y - C.gl_Position.y) * (A.gl_Position.x - C.gl_Position.x) + (C.gl_Position.x - B.gl_Position.x) * (A.gl_Position.y - C.gl_Position.y));

    // code from https://github.com/SebLague/Gamedev-Maths/blob/master/PointInTriangle.cs 20.05.2020
    // Explanation of PointInTriangle method :
    // youtu.be/HYAgJN3x4GA?list=PLFt_AvWsXl0cD2LPxcjxVjWTQLxJqKpgZ

    double w1;
    double w2;
    double sACy = (double)C.gl_Position[1] - A.gl_Position[1];
    double sACx = (double)C.gl_Position[0] - A.gl_Position[0];
    double sABy = (double)B.gl_Position[1] - A.gl_Position[1];
    double sABx = (double)B.gl_Position[0] - A.gl_Position[0];
    double sPAy;

    for (uint64_t x = lowX; x <= higX; x++)
    {
        for (uint64_t y = lowY; y <= higY; y++)
        {
            if (x >= getFramebufferWidth() || x < 0 || y >= getFramebufferHeight() || x < 0)
            {
                //pokud by pixel byl mimo buffer
                continue;
            }

            sPAy = y + 0.5 - A.gl_Position[1];

            w1 = (A.gl_Position[0] * sACy + sPAy * sACx - (x + 0.5) * sACy) / (sABy * sACx - sABx * sACy);
            w2 = (sPAy - w1 * sABy) / sACy;


            //if (true)
            if (w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1)
            {
                InFragment newfragment;
                newfragment.gl_FragCoord[0] = x + 0.5;
                newfragment.gl_FragCoord[1] = y + 0.5;

                //compute barycentric coordinates
                //code from https://www.gamedev.net/forums/topic/621445-barycentric-coordinates-c-code-check/4920278/ 22.05.2020
                l0 = ((B.gl_Position.y - C.gl_Position.y) * (newfragment.gl_FragCoord.x - C.gl_Position.x) + (C.gl_Position.x - B.gl_Position.x) * (newfragment.gl_FragCoord.y - C.gl_Position.y)) * den;
                l1 = ((C.gl_Position.y - A.gl_Position.y) * (newfragment.gl_FragCoord.x - C.gl_Position.x) + (A.gl_Position.x - C.gl_Position.x) * (newfragment.gl_FragCoord.y - C.gl_Position.y)) * den;
                l2 = 1.0 - l0 - l1;

                //tohle tu musí zůstat, protože A0-3 se používá mezitím i pro jiné věci
                A0 = A.gl_Position[2];
                A1 = B.gl_Position[2];
                A2 = C.gl_Position[2];

                //add interpolation
                newfragment.gl_FragCoord[2] = ((((A0 * l0) / h0) + ((A1 * l1) / h1) + ((A2 * l2) / h2)) / ((l0 / h0) + (l1 / h1) + (l2 / h2)));
                for (int i = 0; i < maxAttributes; i++)
                {
                    switch (AP.attributes[i])
                    {
                    case AttributeType::FLOAT:
                        A0 = A.attributes[i].v1;
                        A1 = B.attributes[i].v1;
                        A2 = C.attributes[i].v1;
                        newfragment.attributes[i].v1 = ((((A0 * l0) / h0) + ((A1 * l1) / h1) + ((A2 * l2) / h2)) / ((l0 / h0) + (l1 / h1) + (l2 / h2)));
                        break;
                    case AttributeType::VEC2:
                        for (uint8_t m = 0; m < 2; m++)
                        {
                            A0 = A.attributes[i].v2[m];
                            A1 = B.attributes[i].v2[m];
                            A2 = C.attributes[i].v2[m];
                            newfragment.attributes[i].v2[m] = ((((A0 * l0) / h0) + ((A1 * l1) / h1) + ((A2 * l2) / h2)) / ((l0 / h0) + (l1 / h1) + (l2 / h2)));
                        }
                        break;
                    case AttributeType::VEC3:
                        for (uint8_t m = 0; m < 3; m++)
                        {
                            A0 = A.attributes[i].v3[m];
                            A1 = B.attributes[i].v3[m];
                            A2 = C.attributes[i].v3[m];
                            newfragment.attributes[i].v3[m] = ((((A0 * l0) / h0) + ((A1 * l1) / h1) + ((A2 * l2) / h2)) / ((l0 / h0) + (l1 / h1) + (l2 / h2)));
                        }
                        break;
                    case AttributeType::VEC4:
                        for (uint8_t m = 0; m < 4; m++)
                        {
                            A0 = A.attributes[i].v4[m];
                            A1 = B.attributes[i].v4[m];
                            A2 = C.attributes[i].v4[m];
                            newfragment.attributes[i].v4[m] = ((((A0 * l0) / h0) + ((A1 * l1) / h1) + ((A2 * l2) / h2)) / ((l0 / h0) + (l1 / h1) + (l2 / h2)));
                        }
                        break;
                    default:
                        break;
                    }
                }

                newfragment.gl_FragCoord[0] -= 0.5;
                newfragment.gl_FragCoord[1] -= 0.5;
                pixels.push_back(newfragment);
            }
        }
    }
    
}

uint64_t GPU::indexTypeSize(IndexType indextype)
{
    switch (indextype)
    {
    case IndexType::UINT8:
        return sizeof(uint8_t);
        break;
    case IndexType::UINT16:
        return sizeof(uint16_t);
        break;
    case IndexType::UINT32:
        return sizeof(uint32_t);
        break;
    default:
        return 0;
        break;
    }
}

uint64_t GPU::attributeTypeSize(AttributeType atrbtype)
{
    switch (atrbtype)
    {
    case AttributeType::EMPTY:
        return 0;
        break;
    case AttributeType::FLOAT:
        return sizeof(float);
        break;
    case AttributeType::VEC2:
        return sizeof(glm::vec2);
        break;
    case AttributeType::VEC3:
        return sizeof(glm::vec3);
        break;
    case AttributeType::VEC4:
        return sizeof(glm::vec4);
        break;
    default:
        return 0;
        break;
    }
}