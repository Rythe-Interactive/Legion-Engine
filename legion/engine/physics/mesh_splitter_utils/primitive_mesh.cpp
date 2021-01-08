#include <physics/mesh_splitter_utils/primitive_mesh.h>
#include <rendering/components/renderable.hpp>

namespace legion::physics
{
    ecs::EcsRegistry* PrimitiveMesh::m_ecs = nullptr;
    int PrimitiveMesh::count = 0;

    PrimitiveMesh::PrimitiveMesh
    (ecs::entity_handle pOriginalEntity, std::vector<std::shared_ptr<SplittablePolygon>>& pPolygons, rendering::material_handle pOriginalMaterial)
        : polygons(std::move(pPolygons)), originalMaterial(pOriginalMaterial), originalEntity(pOriginalEntity)
    {

    }

    ecs::entity_handle PrimitiveMesh::InstantiateNewGameObject()
    {
        auto ent = m_ecs->createEntity();

        mesh newMesh;
        populateMesh(newMesh);
      
        newMesh.calculate_tangents(&newMesh);

        sub_mesh newSubMesh;
        newSubMesh.indexCount = newMesh.indices.size();
        newSubMesh.indexOffset = 0;

        newMesh.submeshes.push_back(newSubMesh);

        //creaate modelH
        mesh_handle meshH = core::MeshCache::create_mesh("newMesh" + std::to_string(count), newMesh);
        auto modelH = rendering::ModelCache::create_model(meshH);
        count++;

        //create renderable
        mesh_filter meshFilter = mesh_filter( meshH );

        ent.add_components<rendering::mesh_renderable>(meshFilter,rendering::mesh_renderer( originalMaterial));

        //create transform
        auto [originalPosH, originalRotH, originalScaleH] = originalEntity.get_component_handles<transform>();
        auto [posH, rotH ,scaleH] = m_ecs->createComponents<transform>(ent);

        posH.write(originalPosH.read());
        rotH.write(originalRotH.read());
        scaleH.write(originalScaleH.read());

        math::vec3 initialPos = originalPosH.read();
        originalPosH.write(initialPos + math::vec3(7, 0, 0));
        //m_ecs->destroyEntity(originalEntity);

        return ent;
    }

    void PrimitiveMesh::SetECSRegistry(ecs::EcsRegistry* ecs)
    {
        m_ecs = ecs;
    }

    void PrimitiveMesh::populateMesh(mesh& mesh)
    {
        std::vector<uint>& indices = mesh.indices;
        std::vector<math::vec3>& vertices = mesh.vertices;
        std::vector<math::vec2>& uvs = mesh.uvs;
        std::vector<math::vec3>& normals = mesh.normals;

        //for each polygon in splittable polygon

        for (auto polygon : polygons)
        {
            polygon->ResetEdgeVisited();

            std::queue<meshHalfEdgePtr> unvisitedEdgeQueue;
            unvisitedEdgeQueue.push(polygon->GetMeshEdges().at(0));

            while (!unvisitedEdgeQueue.empty())
            {
                auto halfEdge = unvisitedEdgeQueue.front();
                unvisitedEdgeQueue.pop();

                if (!halfEdge->isVisited)
                {
                    halfEdge->MarkTriangleEdgeVisited();

                    auto [edge1, edge2, edge3] = halfEdge->GetTriangle();

                    vertices.push_back(edge1->position);
                    vertices.push_back(edge2->position);
                    vertices.push_back(edge3->position);

                    uvs.push_back(edge1->uv);
                    uvs.push_back(edge2->uv);
                    uvs.push_back(edge3->uv);

                    if (!edge1->isBoundary)
                    {
                        unvisitedEdgeQueue.push(edge1->pairingEdge);
                    }

                    if (!edge2->isBoundary)
                    {
                        unvisitedEdgeQueue.push(edge2->pairingEdge);
                    }

                    if (!edge3->isBoundary)
                    {
                        unvisitedEdgeQueue.push(edge3->pairingEdge);
                    }
                }
            }
        }

        for (int i = 0; i < vertices.size(); i++)
        {
            indices.push_back(i);
        }

        //TODO stop generating fake normals
        for (int i = 0; i < vertices.size(); i++)
        {
            normals.push_back(math::vec3(0, 1,0));
        }
    }
}
