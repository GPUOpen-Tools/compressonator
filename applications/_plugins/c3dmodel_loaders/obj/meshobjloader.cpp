//=====================================================================
// Copyright 2018-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//=================================================================================================================================
//
//          Includes / defines / typedefs / static member variable initialization block
//
//=================================================================================================================================

// ignore VC++ warnings about fopen, fscanf, etc being deprecated
#if defined(_MSC_VER)
#if _MSC_VER >= 1400
#define _CRT_SECURE_NO_DEPRECATE
#endif
#endif

#include "meshobjloader.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <map>

//=================================================================================================================================
//
//          Constructor(s) / Destructor(s) Block
//
//=================================================================================================================================

//=================================================================================================================================
//
//          Public methods block
//
//=================================================================================================================================

//=================================================================================================================================
/// Loads a mesh from an OBJ file
/// \param strFileName   The file name to load from
/// \param mesh   objVertices  A set of vertices that is created from the OBJ
///               objFaces     A set of faces that is created from the OBJ
//=================================================================================================================================
int MeshObjLoader::LoadGeometry(const char* strFileName, CMODEL_DATA& mesh, CMIPS* cmips, CMP_Feedback_Proc pFeedbackProc)
{
    //----------------------------------------------------------------------
    // Data
    //----------------------------------------------------------------------
    std::vector<ObjVertex3D> vertices;
    std::vector<ObjVertex3D> normals;
    std::vector<ObjVertex2D> texCoords;

    bool bHasTexCoords = false;
    bool bHasNormals   = false;

    char cLine[256];  //A line of the obj file

    //----------------------------------------------------------------------
    // Read OBJ file
    //----------------------------------------------------------------------
    FILE* pFile = fopen(strFileName, "rt");

    // Check if the file was opened
    if (NULL == pFile)
    {
        // Open failed
        if (cmips)
        {
            cmips->Print("Error: Open %s failed", strFileName);
        }
        return (-1);

    }  // end if ( NULL == pFile )

    // get file size for progress bar update
    fseek(pFile, 0L, SEEK_END);
    int filelength = ftell(pFile);
    fseek(pFile, 0L, SEEK_SET);

    //Read until we hit the end of the file
    if (cmips)
    {
        cmips->SetProgress(0);
        char fname[_MAX_FNAME];
        getFileNameExt(strFileName, fname, _MAX_FNAME);
        cmips->Print("Loading %s ...", fname);
    }

    mesh.min_vertex  = {FLT_MAX, FLT_MAX, FLT_MAX};  // used for bounding box
    mesh.max_vertex  = {0.0f, 0.0f, 0.0f};           // used for bounding box
    int progressFreq = filelength / 20;
    while (!feof(pFile))
    {
        //Check the first char in the line
        int iStart = fgetc(pFile);

        // skip blank lines
        while ((iStart == 10) && (!feof(pFile)))
        {
            iStart = fgetc(pFile);
        }

        if (feof(pFile))
        {
            break;
        }

        //If the first letter is v, it is either a vertex, a text coord, or a vertex normal
        if (iStart == 'v')
        {
            //get the second char
            int iNext = fgetc(pFile);

            //if its a space, its a vertex coordinate
            if (iNext == ' ' || iNext == '\t')
            {
                ObjVertex3D vertex;

                //get the line
                fgets(cLine, 256, pFile);

                //get the vertex coords
                sscanf(cLine, " %f %f %f", &vertex.x, &vertex.y, &vertex.z);

                // calc bounding box
                if (vertex.x < mesh.min_vertex.x)
                    mesh.min_vertex.x = vertex.x;
                if (vertex.y < mesh.min_vertex.y)
                    mesh.min_vertex.y = vertex.y;
                if (vertex.z < mesh.min_vertex.z)
                    mesh.min_vertex.z = vertex.z;
                if (vertex.x > mesh.max_vertex.x)
                    mesh.max_vertex.x = vertex.x;
                if (vertex.y > mesh.max_vertex.y)
                    mesh.max_vertex.y = vertex.y;
                if (vertex.z > mesh.max_vertex.z)
                    mesh.max_vertex.z = vertex.z;

                //add to the vertex array
                vertices.push_back(vertex);
            }  // End if

            //if its a t, its a texture coord
            else if (iNext == 't')
            {
                ObjVertex2D texCoord;

                //get the line
                fgets(cLine, 256, pFile);

                //get the vertex coords
                sscanf(cLine, " %f %f", &texCoord.x, &texCoord.y);

                //add to the vertex array
                texCoords.push_back(texCoord);

                bHasTexCoords = true;
            }  // End else if

            //if its an n its a normal
            else if (iNext == 'n')
            {
                ObjVertex3D normal;

                //get the line
                fgets(cLine, 256, pFile);

                //get the vertex coords
                sscanf(cLine, " %f %f %f", &normal.x, &normal.y, &normal.z);

                //add to the vertex array
                normals.push_back(normal);

                bHasNormals = true;
            }  // End else if

            //else its something we don't support
            else
            {
                //scan the line and discard it
                fgets(cLine, 256, pFile);
            }  // End else
        }      // End if

        //if the first letter is f, its a face
        //if the first letter is f, its a face
        else if (iStart == 'f')
        {
            //read in the line
            fgets(cLine, 256, pFile);

            int numVerts = GetNumberOfVerticesInLine(cLine);

            int vertexIndices[256];
            int texCoordIndices[256];
            int normalIndices[256];

            assert(numVerts < 256);
            assert(numVerts > 0);

            ReadVertexIndices(cLine, numVerts, bHasTexCoords, bHasNormals, vertexIndices, texCoordIndices, normalIndices);

            int fCount;  // face count

            // Create triangles.
            //
            // Make a triangle fan if more than 3 vertices are specified
            //
            // If there are 3 vertices, 1 triangle
            // If there are 4 vertices, 2 triangles
            // If there are 5 vertices, 3 triangles
            // :
            for (fCount = 0; fCount < numVerts - 2; fCount++)
            {
                ObjFace face;
                face.vertexIndices[0] = vertexIndices[0];
                face.vertexIndices[1] = vertexIndices[fCount + 1];
                face.vertexIndices[2] = vertexIndices[fCount + 2];

                face.texCoordIndices[0] = texCoordIndices[0];
                face.texCoordIndices[1] = texCoordIndices[fCount + 1];
                face.texCoordIndices[2] = texCoordIndices[fCount + 2];

                face.normalIndices[0] = normalIndices[0];
                face.normalIndices[1] = normalIndices[fCount + 1];
                face.normalIndices[2] = normalIndices[fCount + 2];

                mesh.m_objFaces.push_back(face);
            }  // End for
        }      // End else if

        //if it isn't any of those, we don't care about it
        else
        {
            //read the whole line to advance
            fgets(cLine, 256, pFile);
        }  // End else

        if (pFeedbackProc && ((ftell(pFile) % progressFreq) == 0))
        {
            float fProgress = 100.f * ((float)(ftell(pFile)) / filelength);
            if (pFeedbackProc(fProgress, NULL, NULL))
            {
                break;
            }
        }

    }  // End while

    fclose(pFile);

    mesh.m_meshData.resize(1);
    BuildFinalVertices(vertices, normals, texCoords, mesh.m_objFaces, mesh.m_meshData[0], cmips, pFeedbackProc);

    return 0;
}  // End of LoadGeometry for CRmObjLoaderPlugIn

//=================================================================================================================================
//
//          Private methods block
//
//=================================================================================================================================

//=================================================================================================================================
/// Returns how many vertices are specified in one line read from OBJ
/// \param szLine  The line of text to count vertices in
//=================================================================================================================================

int MeshObjLoader::GetNumberOfVerticesInLine(const char* szLine)
{
    int  pos           = 0;
    int  vertCount     = 0;
    bool bInsideVertex = false;

    while (szLine[pos] != 0)
    {
        if (((szLine[pos] >= '0') && (szLine[pos] <= '9')) || (szLine[pos] == '/'))
        {
            if (bInsideVertex == false)
            {
                vertCount++;
                bInsideVertex = true;
            }  // End if
        }      // End if
        else if (szLine[pos] == ' ')
        {
            if (bInsideVertex)
            {
                bInsideVertex = false;
            }  // End if
        }      // End else

        pos++;
    }  // End while

    return vertCount;
}  // End of GetNumberOfVerticesInLine for CRmObjLoaderPlugIn

//=================================================================================================================================
/// Parses a face line
//=============================================================================================================================
void MeshObjLoader::ReadVertexIndices(const char* szLine,
                                      int         numVertsInLine,
                                      bool        bHasTexCoords,
                                      bool        bHasNormals,
                                      int*        pVertIndices,
                                      int*        pTexCoords,
                                      int*        pNormals)
{
    const char* szNext = szLine;

    while (((*szNext) < '0') || ((*szNext) > '9'))
    {
        // Skip any character that's not a number
        assert(*szNext != '\0');
        szNext++;
    }  // End while

    int i;

    for (i = 0; i < numVertsInLine; i++)
    {
        pVertIndices[i] = 0;
        pTexCoords[i]   = 0;
        pNormals[i]     = 0;

        int numRead;

        if (bHasTexCoords && bHasNormals)
        {
            numRead = sscanf(szNext, "%i/%i/%i", &pVertIndices[i], &pTexCoords[i], &pNormals[i]);

            if (numRead != 3)
            {
                sscanf(szNext, "%i//%i//%i", &pVertIndices[i], &pTexCoords[i], &pNormals[i]);
            }
        }  // End if

        else if (!bHasTexCoords && bHasNormals)
        {
            numRead = sscanf(szNext, "%i/%i", &pVertIndices[i], &pNormals[i]);

            if (numRead != 2)
            {
                sscanf(szNext, "%i//%i", &pVertIndices[i], &pNormals[i]);
            }
        }  // End if

        else if (bHasTexCoords && !bHasNormals)
        {
            numRead = sscanf(szNext, "%i/%i", &pVertIndices[i], &pTexCoords[i]);

            if (numRead != 2)
            {
                sscanf(szNext, "%i//%i", &pVertIndices[i], &pTexCoords[i]);
            }
        }  // End if

        else
        {
            numRead = sscanf(szNext, "%i", &pVertIndices[i]);
        }  // End else

        szNext = strchr(szNext, ' ');

        if (i < numVertsInLine - 1)
        {
            // If this is not the last one
            assert(szNext != NULL);

            while (((*szNext) < '0') || ((*szNext) > '9'))
            {
                // Skip any character that's not a number
                assert(*szNext != '\0');
                szNext++;
            }  // End while
        }      // End if
    }          // End for
}  // End of ReadVertexIndices for CRmObjLoaderPlugIn

//=================================================================================================================================
/// Buildup vertex hash map and update each face's final vertex index
/// Build final vertex array
//=================================================================================================================================
void MeshObjLoader::BuildFinalVertices(const std::vector<ObjVertex3D>& vertices,
                                       const std::vector<ObjVertex3D>& normals,
                                       const std::vector<ObjVertex2D>& texCoords,
                                       std::vector<ObjFace>&           faces,
                                       CMP_Mesh&                       meshData,
                                       CMIPS*                          cmips,
                                       CMP_Feedback_Proc               pFeedbackProc)
{
    if (cmips)
    {
        cmips->SetProgress(0);
        cmips->Print("Remove duplicate mesh vertices ...");
    }

    std::map<VertexHashData, VertexHashData, vertex_less> vertexHashMap;

    int          count = 0;
    unsigned int i;
    unsigned int vsize        = (unsigned int)vertices.size();
    unsigned int nsize        = (unsigned int)normals.size();
    unsigned int tsize        = (unsigned int)texCoords.size();
    int          progressFreq = (unsigned int)faces.size() / 20;

    for (i = 0; i < faces.size(); i++)
    {
        ObjFace& face = faces[i];

        // OBj's index is 1 based, so subtract 1 to make it zero based indices
        int j;

        for (j = 0; j < 3; j++)
        {
            VertexHashData vHash;
            vHash.finalIndex    = count;
            vHash.vertexIndex   = face.vertexIndices[j];
            vHash.texCoordIndex = face.texCoordIndices[j];
            vHash.normalIndex   = face.normalIndices[j];

            std::map<VertexHashData, VertexHashData, vertex_less>::iterator itr = vertexHashMap.find(vHash);

            if (itr == vertexHashMap.end())
            {
                // If this combination of vertexIndex,texCoordIndex and normalIndex is nout found in map
                vertexHashMap.insert(std::map<VertexHashData, VertexHashData, vertex_less>::value_type(vHash, vHash));
                face.finalVertexIndices[j] = vHash.finalIndex;
                count++;

                Vertex finalVertex = {0};

                // OBJ's indices are 1 base, so subtract 1
                if (vHash.vertexIndex > 0)
                {
                    finalVertex.px = vertices[vHash.vertexIndex - 1].x;
                    finalVertex.py = vertices[vHash.vertexIndex - 1].y;
                    finalVertex.pz = vertices[vHash.vertexIndex - 1].z;
                }

                if (vHash.texCoordIndex > 0)
                {
                    finalVertex.tx = texCoords[vHash.texCoordIndex - 1].x;
                    finalVertex.ty = texCoords[vHash.texCoordIndex - 1].y;
                }

                if (vHash.normalIndex > 0)
                {
                    finalVertex.nx = normals[vHash.normalIndex - 1].x;
                    finalVertex.ny = normals[vHash.normalIndex - 1].y;
                    finalVertex.nz = normals[vHash.normalIndex - 1].z;
                }

                meshData.vertices.push_back(finalVertex);
            }  // End if
            else
            {
                VertexHashData& vHashFound = (*itr).second;
                face.finalVertexIndices[j] = vHashFound.finalIndex;
            }  // End else
        }      // End for

        meshData.indices.push_back(faces[i].finalVertexIndices[1]);
        meshData.indices.push_back(faces[i].finalVertexIndices[2]);
        meshData.indices.push_back(faces[i].finalVertexIndices[0]);

        if (progressFreq > 0)
            if (pFeedbackProc && ((i % progressFreq) == 0))
            {
                float fProgress = 100.f * ((float)(i) / faces.size());
                if (pFeedbackProc(fProgress, NULL, NULL))
                {
                    break;
                }
            }
    }  // End for

    // Final Indices
    // for (i = 0; i < faces.size(); i++)
    // {
    //     meshData.indices.push_back(faces[i].finalVertexIndices[1]);
    //     meshData.indices.push_back(faces[i].finalVertexIndices[2]);
    //     meshData.indices.push_back(faces[i].finalVertexIndices[0]);
    // }

}  // End of BuildFinalVertices for CRmObjLoaderPlugIn

//=================================================================================================================================
// Utils: extract filename with extension from the full file name path
//=================================================================================================================================
void MeshObjLoader::getFileNameExt(const char* FilePathName, char* fnameExt, int maxbuffsize)
{
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char ext[_MAX_EXT];
    char fname[_MAX_FNAME];
    _splitpath_s(FilePathName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
    snprintf(fnameExt, maxbuffsize, "%s%s", fname, ext);
}