#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "vertex.h"

// 读取从matlab导出的txt文件，每行四个数x y z w，只留xyz   
inline std::vector<Vertex> readTxt(const std::string& fpath) {
    std::ifstream infile(fpath);
    assert(!infile.fail());
    std::vector<Vertex> res;
    std::string line, str;
    int i = 0;
    while (std::getline(infile, line))
    {
        float x, y, z, w;
        std::istringstream iss(line);
        if (!(iss >> x >> y >> z >> w)) {
            qDebug() << "error";
            break;
        }
        float t = (float)i / 100000.0f;
        res.push_back(Vertex({ x, y, z }, { t, 1.0f, 0.0f }));
        i++;
    }
    qDebug() << "read vertices: " << i;
    return res;
}

// 忽略文件头的一堆信息，只读取所有点的坐标   
inline std::vector<Vertex> readPly(const std::string& fpath) {
    std::ifstream infile(fpath);
    assert(!infile.fail());
    std::vector<Vertex> res;
    std::string line, str;
    int numVertices;
    // 找到顶点数所在行 
    while (std::getline(infile, line))
    {
        if (std::string::npos != line.find("element vertex")) {
            std::istringstream iss(line);
            // 读出顶点数量 
            if (!(iss >> str >> str >> numVertices)) {
                qDebug() << "error reading element vertex";
                // error
            }
            qDebug() << numVertices << " | " << line.c_str();
            break;
        }
    }
    // 找到顶点数据开始的行 
    while (std::getline(infile, line))
    {
        if (std::string::npos != line.find("end_header")) {
            break;
        }
    }
    // 开始读取 
    float x, y, z;
    for (int i = 0; i < numVertices; i++) {
        float t = (float)i / numVertices;
        std::getline(infile, line);
        std::istringstream iss(line);
        if (!(iss >> x >> y >> z)) {
            qDebug() << "error reading vertex: " << line.c_str();
        }
        else {
            res.push_back(Vertex(QVector3D(x, y, z), { t, 1.0f, 0.0f }));
        }
    }

    infile.close();
    return res;
}

