/*! Name: Abinash Meher
 *  Roll No.: 13CS10003
 *  Assignment 1
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <cmath>
#include <sstream>

typedef struct tdpoint{
    float x;
    float y;
    float z;
    tdpoint(float x_=0.0, float y_=0.0, float z_=0.0): x(x_), y(y_), z(z_) {};
    tdpoint operator+(const tdpoint& point) {
        tdpoint result(x+point.x, y+point.y, z+point.z);
        return result;
    }
    tdpoint operator-(const tdpoint& point) {
        tdpoint result(x-point.x, y-point.y, z-point.z);
        return result;
    }
} tdp;

typedef struct tdhexhed{
    tdp vertices[8];
} tdhexhed;

typedef struct face{
    int corners[4];
    face(int one_, int two_, int three_, int four_):corners{one_,two_,three_,four_} {};
} face;

tdhexhed MakeVoxel(tdp centre, float side_length) {
    tdhexhed vox;
    for (int i = 0; i < 8; i++) {
        int j = i;
        vox.vertices[i] = centre;
        int rem = j%2;
        j /= 2;
        if (rem == 0) {
            vox.vertices[i].x = centre.x + side_length/2;
        } else {
            vox.vertices[i].x = centre.x - side_length/2;
        }
        rem = j%2;
        j /= 2;
        if (rem == 0) {
            vox.vertices[i].y = centre.y + side_length/2;
        } else {
            vox.vertices[i].y = centre.y - side_length/2;
        }
        rem = j%2;
        j /= 2;
        if (rem == 0) {
            vox.vertices[i].z = centre.z + side_length/2;
        } else {
            vox.vertices[i].z = centre.z - side_length/2;
        }
    }
    return vox;
}

std::vector<face> GetFacesHexHed() {
    std::vector<face> faces;
    face temp1(5,7,8,6);
    face temp2(8,4,2,6);
    face temp3(7,3,4,8);
    face temp4(1,2,4,3);
    face temp5(1,3,7,5);
    face temp6(1,5,6,2);
    faces.push_back(temp1);
    faces.push_back(temp2);
    faces.push_back(temp3);
    faces.push_back(temp4);
    faces.push_back(temp5);
    faces.push_back(temp6);
    return faces;
}



class ObjWriter {
    int last_zero;
    int line_counter;
    std::ofstream& file_;
    std::vector<tdp> points_;
    std::vector<face> faces_;
    void write(std::ofstream& file) {
        for(std::vector<tdp>::iterator it = points_.begin(); it != points_.end(); it++) {
            file << "v " << it->x << " " << it->y << " " << it->z << "\n";
            line_counter++;
        }
        for(std::vector<face>::iterator it = faces_.begin(); it != faces_.end(); it++) {
            file << "f "    << last_zero + it->corners[0] << " " << last_zero + it->corners[1] << " "
                    << last_zero + it->corners[2] << " " << last_zero + it->corners[3] << "\n";
            //line_counter++;
        }
        last_zero = line_counter;
    }

    public:
    ObjWriter(std::ofstream& file): file_(file), line_counter(0), last_zero(0) {};
    ~ObjWriter() {
        file_.close();
    }

    void AddPoint(tdp elem) {
        points_.push_back(elem);
    }

    int AddFace(int one, int two, int three, int four) {
        face temp(one, two, three, four);
        faces_.push_back(temp);
        return 1;
    }

    int AddFace(face face_elem) {
        faces_.push_back(face_elem);
        return 1;
    }

    void EndObj() {
        write(file_);
        points_.clear();
        faces_.clear();
    }
};


std::pair<float, float> switchToOctantZeroFrom(int octant, float x, float y) {
    switch(octant) {
        case 0: return std::make_pair(x, y);
        case 1: return std::make_pair(y, x);
        case 2: return std::make_pair(y, -x);
        case 3: return std::make_pair(-x, y);
        case 4: return std::make_pair(-x, -y);
        case 5: return std::make_pair(-y, -x);
        case 6: return std::make_pair(-y, x);
        case 7: return std::make_pair(x, -y);
    }
}

std::pair<float, float> switchFromOctantZeroTo(int octant, float x, float y) {
    switch(octant) {
        case 0: return std::make_pair(x, y);
        case 1: return std::make_pair(y, x);
        case 2: return std::make_pair(-y, x);
        case 3: return std::make_pair(-x, y);
        case 4: return std::make_pair(-x, -y);
        case 5: return std::make_pair(-y, -x);
        case 6: return std::make_pair(y, -x);
        case 7: return std::make_pair(x, -y);
    }
}

tdp switchTDPFromOctantZeroTo(int octant_xy, int octant_xz, tdp point) {
    tdp result;
    std::pair<float, float> transformed_xy = switchFromOctantZeroTo(octant_xy, point.x, point.y);
    std::pair<float, float> transformed_xz = switchFromOctantZeroTo(octant_xz, point.x, point.z);
    result.x = transformed_xy.first;
    result.y = transformed_xy.second;
    result.z = transformed_xz.second;
    return result;
}

tdp switchTDPToOctantZeroFrom(int octant_xy, int octant_xz, tdp point) {
    tdp result;
    std::pair<float, float> transformed_back_xy = switchToOctantZeroFrom(octant_xy, point.x, point.y);
    std::pair<float, float> transformed_back_xz = switchToOctantZeroFrom(octant_xz, point.x, point.z);
    result.x = transformed_back_xy.first;
    result.y = transformed_back_xy.second;
    result.z = transformed_back_xz.second;
    return result;
}

std::vector<tdp> Get3Dline(tdp start, tdp end, int scale) {
    int octant_number_xy;
    int octant_number_xz;
    int dx = (int) end.x - start.x;
    int adx = std::abs(dx);
    int dy = (int) end.y - start.y;
    int ady = std::abs(dy);
    int dz = (int) end.z - start.z;
    int adz = std::abs(dz);
    int fxy = 2*dy - dx;
    int fxz = 2*dz - dx;

    std::vector<tdp> points_list;

    //Finding out the octant in which the point lies
    // Octants:
    //  \2|1/
    //  3\|/0
    //  --+--
    //  4/|\7
    //  /5|6\

    if (dx >= 0) {
        if (dy >= 0) {
            if (adx >= ady) {
                octant_number_xy = 0;
            } else {
                octant_number_xy = 1;
            }
        } else {
            if (adx >= ady) {
                octant_number_xy = 7;
            } else {
                octant_number_xy = 6;
            }
        }
        if (dz >= 0) {
            if (adx >= adz) {
                octant_number_xz = 0;
            } else {
                octant_number_xz = 1;
            }
        } else {
            if (adx >= adz) {
                octant_number_xz = 7;
            } else {
                octant_number_xz = 6;
            }
        }
    } else {
        if (dy >= 0) {
            if (adx >= ady) {
                octant_number_xy = 3;
            } else {
                octant_number_xy = 2;
            }
        } else {
            if (adx >= ady) {
                octant_number_xy = 4;
            } else {
                octant_number_xy = 5;
            }
        }
        if (dz >= 0) {
            if (adx >= adz) {
                octant_number_xz = 3;
            } else {
                octant_number_xz = 2;
            }
        } else {
            if (adx >= adz) {
                octant_number_xz = 4;
            } else {
                octant_number_xz = 5;
            }
        }
    }

    tdp new_end = end - start;
    tdp it_point(0,0,0);

    points_list.push_back(start + it_point);
    tdp transformed_new_end = switchTDPToOctantZeroFrom(octant_number_xy, octant_number_xz, new_end);
    dx = transformed_new_end.x;
    dy = transformed_new_end.y;
    dz = transformed_new_end.z;
    if (dx >= dy && dx >= dz) {
        while( it_point.x < transformed_new_end.x ) {
            it_point.x += scale;
            if (fxy > 0) {
                it_point.y += scale;
                fxy += 2 * ( dy - dx );
            } else {
                fxy += 2 * dy;
            }
            if (fxz > 0) {
                it_point.z += scale;
                fxz += 2 * ( dz - dx );
            } else {
                fxz += 2 * dz;
            }
            tdp transformed_back_it_point = switchTDPFromOctantZeroTo(octant_number_xy, octant_number_xz, it_point);
            points_list.push_back(start + transformed_back_it_point);
        }
    }
    return points_list;
}

int main() {
    tdp start;
    std::cout << " Enter the coordinates of the start point <x y z>:" << std::endl;
    std::cin >> start.x >> start.y >> start.z;
    tdp end;
    std::cout << " Enter the coordinates of the end point <x y z>:" << std::endl;
    std::cin >> end.x >> end.y >> end.z;

    std::vector<tdp> line_points = Get3Dline(start, end, 1);
    std::stringstream file_name;
    file_name   << start.x << "_" << start.y << "_" << start.z << "_to_"
                << end.x << "_" << end.y << "_" << end.z << ".obj";

    std::ofstream file(file_name.str());
    ObjWriter objw(file);
    std::vector<face> facelist = GetFacesHexHed();
    for (std::vector<tdp>::iterator it = line_points.begin(); it != line_points.end(); it++) {
        tdhexhed temp_vox = MakeVoxel(*it, 1);
        for (int i = 0; i < 8; i++) {
            objw.AddPoint(temp_vox.vertices[i]);
        }
        for (std::vector<face>::iterator fit = facelist.begin(); fit != facelist.end(); fit++) {
            objw.AddFace(*fit);
        }
        objw.EndObj();
    }
    return 0;
}
