#include <irrlicht.h>
#include <S3DVertex.h>
#include <cmath>
#include <random>
#include <vector>
#include <algorithm>

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

irr::core::vector3df vector_combine(irr::core::vector3df v1, irr::core::vector3df v2, float f1, float f2) {
    irr::core::vector3df result;
    result.X = f1 * v1.X + f2 * v2.X;
    result.Y = f1 * v1.Y + f2 * v2.Y;
    result.Z = f1 * v1.Z + f2 * v2.Z;
    return result;
}

struct node {
    irr::core::vector3df vert;
};

struct tile {
    irr::core::vector3df vert;
    irr::video::SColor color;
};

int main() {

    int size = 64;
    std::default_random_engine g;
    g.seed(std::random_device()());
    std::uniform_real_distribution<float> d(-16.0f, 16.0f);

    irr::IrrlichtDevice *device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(1280, 720), 32, false, false, false, NULL);
    if (!device) {
        return 1;
    }

    irr::video::IVideoDriver *driver = device->getVideoDriver();
    irr::scene::ISceneManager *manager = device->getSceneManager();

    irr::scene::ICameraSceneNode *camera = manager->addCameraSceneNodeFPS();
    camera->setPosition(irr::core::vector3df(0.0f, (float)size / 2.0f, (float)size / 1.5f));
    camera->setTarget(irr::core::vector3df(0.0f, 0.0f, 0.0f));

    irr::scene::SMesh *mesh = new irr::scene::SMesh();
    irr::scene::SMeshBuffer *buffer = new irr::scene::SMeshBuffer();

    mesh->addMeshBuffer(buffer);
    buffer->drop();

    struct node nodes[size + 1][size + 1];

    int i;
    int j;
    int k;

    for (i = 0; i < size + 1; ++i) {
        for (j = 0; j < size + 1; ++j) {
            nodes[i][j].vert = irr::core::vector3df((float)i - (float)size / 2.0f, d(g), (float)j - (float)size / 2.0f);
        }
    }

    int smooth = size / 4;
    for (i = 0; i < smooth; ++i) {
        irr::f32 temp[size + 1][size + 1];
        for (j = 0; j < size + 1; ++j) {
            for (k = 0; k < size + 1; ++k) {
                int avg = 1;
                temp[j][k] = nodes[j][k].vert.Y;
                if (j > 0) {
                    temp[j][k] = temp[j][k] + nodes[j - 1][k].vert.Y;
                    ++avg;
                }
                if (k > 0) {
                    temp[j][k] = temp[j][k] + nodes[j][k - 1].vert.Y;
                    ++avg;
                }
                if (j < size) {
                    temp[j][k] = temp[j][k] + nodes[j + 1][k].vert.Y;
                    ++avg;
                }
                if (k < size) {
                    temp[j][k] = temp[j][k] + nodes[j][k + 1].vert.Y;
                   ++avg;
                }
                temp[j][k] = temp[j][k] / (float)avg;
            }
        }
        for (j = 0; j < size + 1; ++j) {
            for (k = 0; k < size + 1; ++k) {
                nodes[j][k].vert.Y = temp[j][k];
            }
        }
    }

    std::vector<float> distrib(size * size);

    for (i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j) {
            distrib[i * size + j] = nodes[i][j].vert.Y;
            if (distrib[i * size + j] < nodes[i + 1][j].vert.Y) {
                distrib[i * size + j] = nodes[i + 1][j].vert.Y;
            }
            if (distrib[i * size + j] < nodes[i][j + 1].vert.Y) {
                distrib[i * size + j] = nodes[i][j + 1].vert.Y;
            }
            if (distrib[i * size + j] < nodes[i + 1][j + 1].vert.Y) {
                distrib[i * size + j] = nodes[i + 1][j + 1].vert.Y;
            }
        }
    }

    std::sort(distrib.begin(), distrib.end());

    float coeff = distrib[size * size * 0.708f];

    for (i = 0; i < size + 1; ++i) {
        for (j = 0; j < size + 1; ++j) {
            nodes[i][j].vert.Y = nodes[i][j].vert.Y - coeff;
        }
    }

    struct tile tiles[size][size];

    for (i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j) {
            if (nodes[i][j].vert.Y > 0.0f || nodes[i + 1][j].vert.Y > 0.0f || nodes[i][j + 1].vert.Y > 0.0f || nodes[i + 1][j + 1].vert.Y > 0.0f) {
                tiles[i][j].color = irr::video::SColor(255, 0, 255, 0);
                irr::f32 Y = (nodes[i][j].vert.Y + nodes[i + 1][j].vert.Y + nodes[i][j + 1].vert.Y +  nodes[i + 1][j + 1].vert.Y) / 4.0f;
                tiles[i][j].vert = irr::core::vector3df((float)i - (float)size / 2.0f + 0.5f, Y, (float)j - (float)size / 2.0f + 0.5f);
            } else {
                tiles[i][j].color = irr::video::SColor(255, 0, 0, 255);
                nodes[i][j].vert.Y = 0.0f;
                nodes[i + 1][j].vert.Y = 0.0f;
                nodes[i][j + 1].vert.Y = 0.0f;
                nodes[i + 1][j + 1].vert.Y = 0.0f;
                tiles[i][j].vert = irr::core::vector3df((float)i - (float)size / 2.0f + 0.5f, 0.0f, (float)j - (float)size / 2.0f + 0.5f);
            }
        }
    }

    irr::f32 gap = 0.1f;

    buffer->Vertices.reallocate(size * size * 12);
    buffer->Vertices.set_used(size * size * 12);
    for (i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j) {

            buffer->Vertices[(i * size + j) * 12 + 0] = irr::video::S3DVertex(tiles[i][j].vert, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));
            irr::core::vector3df temp = vector_combine(tiles[i][j].vert, nodes[i][j].vert, gap, 1.0f - gap);
            buffer->Vertices[(i * size + j) * 12 + 1] = irr::video::S3DVertex(temp, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));
            temp = vector_combine(tiles[i][j].vert, nodes[i][j + 1].vert, gap, 1.0f - gap);
            buffer->Vertices[(i * size + j) * 12 + 2] = irr::video::S3DVertex(temp, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));

            buffer->Vertices[(i * size + j) * 12 + 3] = irr::video::S3DVertex(tiles[i][j].vert, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));
            temp = vector_combine(tiles[i][j].vert, nodes[i][j + 1].vert, gap, 1.0f - gap);
            buffer->Vertices[(i * size + j) * 12 + 4] = irr::video::S3DVertex(temp, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));
            temp = vector_combine(tiles[i][j].vert, nodes[i + 1][j + 1].vert, gap, 1.0f - gap);
            buffer->Vertices[(i * size + j) * 12 + 5] = irr::video::S3DVertex(temp, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));

            buffer->Vertices[(i * size + j) * 12 + 6] = irr::video::S3DVertex(tiles[i][j].vert, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));
            temp = vector_combine(tiles[i][j].vert, nodes[i + 1][j + 1].vert, gap, 1.0f - gap);
            buffer->Vertices[(i * size + j) * 12 + 7] = irr::video::S3DVertex(temp, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));
            temp = vector_combine(tiles[i][j].vert, nodes[i + 1][j].vert, gap, 1.0f - gap);
            buffer->Vertices[(i * size + j) * 12 + 8] = irr::video::S3DVertex(temp, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));

            buffer->Vertices[(i * size + j) * 12 + 9] = irr::video::S3DVertex(tiles[i][j].vert, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));
            temp = vector_combine(tiles[i][j].vert, nodes[i + 1][j].vert, gap, 1.0f - gap);
            buffer->Vertices[(i * size + j) * 12 + 10] = irr::video::S3DVertex(temp, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));
            temp = vector_combine(tiles[i][j].vert, nodes[i][j].vert, gap, 1.0f - gap);
            buffer->Vertices[(i * size + j) * 12 + 11] = irr::video::S3DVertex(temp, irr::core::vector3df(0.0f, 1.0f, 0.0f), tiles[i][j].color, irr::core::vector2df(0, 0));
        }
    }

    buffer->Indices.reallocate(size * size * 12);
    buffer->Indices.set_used(size * size * 12);
    for (i = 0; i < size * size * 12; ++i) {
        buffer->Indices[i] = i;
    }

    buffer->recalculateBoundingBox();

    irr::scene::IMeshSceneNode *node = manager->addMeshSceneNode(mesh);

    node->setMaterialFlag(irr::video::EMF_LIGHTING, false);

    while (device->run()) {
        driver->beginScene(true, true, irr::video::SColor(0, 0, 0, 0));
        manager->drawAll();
        driver->endScene();
    }

    device->drop();

    return EXIT_SUCCESS;

}
