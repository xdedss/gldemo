#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "vertex.h"


inline std::vector<Vertex> readPly(const std::string& fpath) {
    std::ifstream infile(fpath);
    std::vector<Vertex> res;
    std::string line, str;
    int numVertices;
    while (std::getline(infile, line))
    {
        if (std::string::npos != line.find("element vertex")) {
            std::istringstream iss(line);
            if (!(iss >> str >> str >> numVertices)) {
                qDebug() << "error reading element vertex";
                // error
            }
            qDebug() << numVertices << " | " << line.c_str();
            break;
        }
    }

    while (std::getline(infile, line))
    {
        if (std::string::npos != line.find("end_header")) {
            break;
        }
    }
    float x, y, z;
    for (int i = 0; i < numVertices; i++) {
        float t = (float)i / numVertices;
        std::getline(infile, line);
        std::istringstream iss(line);
        if (!(iss >> x >> y >> z)) {
            qDebug() << "error reading vertex: " << line.c_str();
        }
        else {
            res.push_back(Vertex(QVector3D(x, y, z) * 10, { t, 1.0f, 0.0f }));
        }
    }

    infile.close();
    return res;
}

