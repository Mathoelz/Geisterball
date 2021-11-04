//
//  Model.cpp
//  ogl4
//
//  Created by Philipp Lensing on 21.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Model.h"
#include "PhongShader.h"
#include <list>

#include <iostream>
using namespace std;

Model::Model() : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{
    
}

Model::Model(const char* ModelFile, bool FitSize, float scale) : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0), scale(1)
{
    this->scale = scale;
    bool ret = load(ModelFile, FitSize);
    if(!ret)
        throw std::exception();
}

Model::~Model()
{
    deleteNodes(&RootNode);
    
    delete [] this->pMeshes;
    delete [] this->pMaterials;
}

void Model::deleteNodes(Node* pNode)
{
    if(!pNode)
        return;
    for(unsigned int i=0; i<pNode->ChildCount; ++i)
        deleteNodes(&(pNode->Children[i]));
    if(pNode->ChildCount>0)
        delete [] pNode->Children;
    if(pNode->MeshCount>0)
        delete [] pNode->Meshes;
}

bool Model::load(const char* ModelFile, bool FitSize)
{
        // aiImportFiel liest eine Datei und liefert den Inhalt zurück
    const aiScene* pScene = aiImportFile( ModelFile,aiProcessPreset_TargetRealtime_Fast | aiProcess_TransformUVCoords );
    
    if (pScene == NULL || pScene->mNumMeshes <= 0) {
        return false;
    }
    
    Filepath = ModelFile;
    Path = Filepath;
    size_t pos = Filepath.rfind('/');
    if(pos == std::string::npos)
        pos = Filepath.rfind('\\');
    if(pos !=std::string::npos)
        Path.resize(pos+1);
    
    loadMeshes(pScene, FitSize);
    loadMaterials(pScene);
    loadNodes(pScene);
    
    return true;
}

/* Quellen: https://www.ics.com/blog/qt-and-opengl-loading-3d-model-open-asset-import-library-assimp
 
 */

void Model::loadMeshes(const aiScene* pScene, bool FitSize)
{
    Vector factor = Vector(1, 1, 1);

    if (pScene->HasMeshes()) {
        this->pMeshes = new Mesh[pScene->mNumMeshes];
        this->MeshCount = pScene->mNumMeshes;
            // BoundingBox berechnen
        this->calcBoundingBox(pScene, this->BoundingBox);
        //printf(" calcBoundingBox: %f, %f, %f", this->BoundingBox.Min.X, this->BoundingBox.Min.Y, this->BoundingBox.Min.Z);
        //printf(" calcBoundingBox: %f, %f, %f", this->BoundingBox.Max.X, this->BoundingBox.Max.Y, this->BoundingBox.Max.Z);
            // die Mashes des Knoten durchlaufen und alle Normalen, Texturkoordinate und Vertices speichern 
        for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
            this->pMeshes[i].MaterialIdx = pScene->mMeshes[i]->mMaterialIndex;
                // Vertex-Daten-Definition starten.
            pMeshes[i].VB.begin();
            for (unsigned int j = 0; j < pScene->mMeshes[i]->mNumVertices; j++) {
                    // Prüfen, ob Normalen vorhanden sind
                if (pScene->mMeshes[i]->HasNormals()) {
                        // Normalen hinzufügen 
                    this->pMeshes[i].VB.addNormal(pScene->mMeshes[i]->mNormals[j].x, 
                                                   pScene->mMeshes[i]->mNormals[j].y, 
                                                   pScene->mMeshes[i]->mNormals[j].z);
                }
                    // Zahl kleiner als 0x8
                if(pScene->mMeshes[i]->HasTextureCoords(0)){
                        // Texturen hinzufügen
                    this->pMeshes[i].VB.addTexcoord0(pScene->mMeshes[i]->mTextureCoords[0][j].x,
                                                     1 - pScene->mMeshes[i]->mTextureCoords[0][j].y);
                }
                    
                if(pScene->mMeshes[i]->HasTangentsAndBitangents()){
                    this->pMeshes[i].VB.addTexcoord1(pScene->mMeshes[i]->mTangents[j].x, pScene->mMeshes[i]->mTangents[j].y, pScene->mMeshes[i]->mTangents[j].z);
                    this->pMeshes[i].VB.addTexcoord2(pScene->mMeshes[i]->mBitangents[j].x, pScene->mMeshes[i]->mBitangents[j].y, pScene->mMeshes[i]->mBitangents[j].z);
                }

                    // Prüfen, ob Vertices vorhanden sind (Vertices zum Schluss zuweisen)
                if (pScene->mMeshes[i]->HasPositions()) {
                    // Mesh auf sinnvolle Größe skalieren
                    if (FitSize) {
                        factor = Vector(5, 5, 5);
                    }
                    // Vertex hinzufügen
                    this->pMeshes[i].VB.addVertex(pScene->mMeshes[i]->mVertices[j].x   * factor.X,
                                                    pScene->mMeshes[i]->mVertices[j].y * factor.Y,
                                                    pScene->mMeshes[i]->mVertices[j].z * factor.Z);
                    factor = Vector(1, 1, 1) * this->scale;
                }
            }
                // Vertex-Daten-Definition beenden.
            pMeshes[i].VB.end();


                // Index-Daten-Definition starten.
            pMeshes[i].IB.begin();
                // Prüfen, ob Flächen vorhanden sind
            if (pScene->mMeshes[i]->HasFaces()) {
                    // Die Meshes des Knotens sollen durchlaufen werden 
                for (unsigned int k = 0; k < pScene->mMeshes[i]->mNumFaces; k++) {
                    for (unsigned int l = 0; l < pScene->mMeshes[i]->mFaces->mNumIndices; l++) {
                            // Indices hinzufügen 
                        pMeshes[i].IB.addIndex(pScene->mMeshes[i]->mFaces[k].mIndices[l]);
                    }
                }
            }
                // Index-Daten-Definition beenden.
            pMeshes[i].IB.end();
        }
    }
}

/* Quellen: https://documentation.help/assimp/material_8h.html
            https://stackoverflow.com/questions/7416445/what-is-use-of-c-str-function-in-c
            https://github.com/assimp/assimp/issues/408
            http://forum.lwjgl.org/index.php?topic=6549.0
            http://assimp.sourceforge.net/lib_html/materials.html
            https://cpp.hotexamples.com/examples/-/-/aiGetMaterialString/cpp-aigetmaterialstring-function-examples.html
            https://github.com/assimp/assimp/blob/master/samples/SimpleOpenGL/Sample_SimpleOpenGL.c
            https://cpp.hotexamples.com/de/examples/-/-/aiGetMaterialColor/cpp-aigetmaterialcolor-function-examples.html
            https://sourceforge.net/p/assimp/discussion/817653/thread/efd8e369/?limit=25
        
*/
void Model::loadMaterials(const aiScene* pScene)
{
    if (pScene->HasMaterials()) {
            // Material-Liste initialisieren
        this->pMaterials = new Material[pScene->mNumMaterials];
        this->MaterialCount = pScene->mNumMaterials;
        
        aiMaterial *mtl;

        aiColor4D diffuse;
        aiColor4D specular;
        aiColor4D ambient;
        aiString tex;
        float exp;

        for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
            mtl  = pScene->mMaterials[i];
                //Try to get the diffuse colour of material
            if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
                this->pMaterials[i].DiffColor.R = diffuse.r;
                this->pMaterials[i].DiffColor.G = diffuse.g;
                this->pMaterials[i].DiffColor.B = diffuse.b;
            }
                //Try to get the specular colour of material
            if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular)) {
                this->pMaterials[i].SpecColor.R = specular.r;
                this->pMaterials[i].SpecColor.G = specular.g;
                this->pMaterials[i].SpecColor.B = specular.b;
            }
                //Try to get the ambient colour of material
            if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient)) {
                this->pMaterials[i].AmbColor.R = ambient.r;
                this->pMaterials[i].AmbColor.G = ambient.g;
                this->pMaterials[i].AmbColor.B = ambient.b;
            }
            // Gibt an, ob der Schlüssel gefunden wurde. Wenn nicht, bleibt der Ausgangs-Float unverändert.
            if (AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &exp)) {
                //cout << " MaterialFloat: " << this->pMaterials[i].SpecExp;
                this->pMaterials[i].SpecExp = exp;
            }
                // Try to get the name of a material you would use
            if (AI_SUCCESS == aiGetMaterialString(mtl, AI_MATKEY_TEXTURE_DIFFUSE(0), &tex)) {
                    // Pfad und Dateiname verbinden (C_Str() gibt char Array zurück)
                std::string data = this->Path + tex.C_Str();
                    //pass std::string to a function that expects to work with a C - style string
                const char* c = data.c_str();
                cout << " FilePath: " << data;
                    // Texturen laden
                this->pMaterials[i].DiffTex = Texture::LoadShared(c);
            }
        }
    }
}

/* Quellen: https://computergraphics.stackexchange.com/questions/6064/aabb-bounding-boxes
 
 */
// Die Box, die das Objekt umschließt (Siehe BoundingBox.jpg) 
void Model::calcBoundingBox(const aiScene* pScene, AABB& Box)
{
    // Jede Box wird für jedes Objekt neu erzeugt, deswegen können wir Min und Max überschreiben 
    Box.Min = Vector(0, 0, 0);
    Box.Max = Vector(0, 0, 0);
        // Gehe alle Meshes durch 
    for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
            // Gehe alle Vertices jeden Meshes durch 
        for (unsigned int j = 0; j < pScene->mMeshes[i]->mNumVertices; j++) {
                // Kleinste Vertices finden und speichern (Falls x,y,z kleiner als das abgespeicherte ist)
            if (pScene->mMeshes[i]->mVertices[j].x < Box.Min.X) {
                Box.Min.X = pScene->mMeshes[i]->mVertices[j].x;
            }
            if (pScene->mMeshes[i]->mVertices[j].y < Box.Min.Y) {
                Box.Min.Y = pScene->mMeshes[i]->mVertices[j].y;
            }
            if (pScene->mMeshes[i]->mVertices[j].z < Box.Min.Z) {
                Box.Min.Z = pScene->mMeshes[i]->mVertices[j].z;
            }
                // Größte Vertices finden und speichern (Falls x,y,z größer als das abgespeicherte ist)
            if (pScene->mMeshes[i]->mVertices[j].x > Box.Max.X) {
                Box.Max.X = pScene->mMeshes[i]->mVertices[j].x;
            }
            if (pScene->mMeshes[i]->mVertices[j].y > Box.Max.Y) {
                Box.Max.Y = pScene->mMeshes[i]->mVertices[j].y;
            }
            if (pScene->mMeshes[i]->mVertices[j].z > Box.Max.Z) {
                Box.Max.Z = pScene->mMeshes[i]->mVertices[j].z;
            }
        }
    }
}

void Model::loadNodes(const aiScene* pScene)
{
    deleteNodes(&RootNode);
    copyNodesRecursive(pScene->mRootNode, &RootNode);
}

void Model::copyNodesRecursive(const aiNode* paiNode, Node* pNode)
{
    pNode->Name = paiNode->mName.C_Str();
    pNode->Trans = convert(paiNode->mTransformation);
    
    if(paiNode->mNumMeshes > 0)
    {
        pNode->MeshCount = paiNode->mNumMeshes;
        pNode->Meshes = new int[pNode->MeshCount];
        for(unsigned int i=0; i<pNode->MeshCount; ++i)
            pNode->Meshes[i] = (int)paiNode->mMeshes[i];
    }
    
    if(paiNode->mNumChildren <=0)
        return;
    
    pNode->ChildCount = paiNode->mNumChildren;
    pNode->Children = new Node[pNode->ChildCount];
    for(unsigned int i=0; i<paiNode->mNumChildren; ++i)
    {
        copyNodesRecursive(paiNode->mChildren[i], &(pNode->Children[i]));
        pNode->Children[i].Parent = pNode;
    }
}

void Model::applyMaterial( unsigned int index)
{
    if(index>=MaterialCount)
        return;
    
    PhongShader* pPhong = dynamic_cast<PhongShader*>(shader());
    if(!pPhong) {
        //std::cout << "Model::applyMaterial(): WARNING Invalid shader-type. Please apply PhongShader for rendering models.\n";
        return;
    }
    
    Material* pMat = &pMaterials[index];
    pPhong->ambientColor(pMat->AmbColor);
    pPhong->diffuseColor(pMat->DiffColor);
    pPhong->specularExp(pMat->SpecExp);
    pPhong->specularColor(pMat->SpecColor);
    pPhong->diffuseTexture(pMat->DiffTex);
    pPhong->normalTexture(pMat->NormalTex);
}

void Model::draw(const BaseCamera& Cam)
{
    if(!pShader) {
        std::cout << "BaseModel::draw() no shader found" << std::endl;
        return;
    }
    pShader->modelTransform(transform());
    
    std::list<Node*> DrawNodes;
    DrawNodes.push_back(&RootNode);
    
    while(!DrawNodes.empty())
    {
        Node* pNode = DrawNodes.front();
        Matrix GlobalTransform;
        
        if(pNode->Parent != NULL)
            pNode->GlobalTrans = pNode->Parent->GlobalTrans * pNode->Trans;
        else
            pNode->GlobalTrans = transform() * pNode->Trans;
        
        pShader->modelTransform(pNode->GlobalTrans);
    
        for(unsigned int i = 0; i<pNode->MeshCount; ++i )
        {
            Mesh& mesh = pMeshes[pNode->Meshes[i]];
            mesh.VB.activate();
            mesh.IB.activate();
            applyMaterial(mesh.MaterialIdx);
            pShader->activate(Cam);
            glDrawElements(GL_TRIANGLES, mesh.IB.indexCount(), mesh.IB.indexFormat(), 0);
            mesh.IB.deactivate();
            mesh.VB.deactivate();
        }
        for(unsigned int i = 0; i<pNode->ChildCount; ++i )
            DrawNodes.push_back(&(pNode->Children[i]));
        
        DrawNodes.pop_front();
    }
}


Matrix Model::convert(const aiMatrix4x4& m)
{
    return Matrix(m.a1, m.a2, m.a3, m.a4,
                  m.b1, m.b2, m.b3, m.b4,
                  m.c1, m.c2, m.c3, m.c4,
                  m.d1, m.d2, m.d3, m.d4);
}





