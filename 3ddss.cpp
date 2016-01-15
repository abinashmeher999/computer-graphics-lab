#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

typedef struct tdpoint{
	float x;
	float y;
	float z;
	tdpoint(float x_=0.0, float y_=0.0, float z_=0.0): x(x_), y(y_), z(z_) {};
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
			file << "f " 	<< last_zero + it->corners[0] << " " << last_zero + it->corners[1] << " "
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

	void EndObj() {
		write(file_);
		points_.clear();
		faces_.clear();
	}
};

std::vector<tdp> Get3Dline(tdp start, tdp end) {
    tdp it_point = start;
    int dx = (int) end.x - start.x;
    int dy = (int) end.y - start.y;
    int dz = (int) end.z - start.z;
    int fxy = 2*dy - dx;
    int fxz = 2*dz - dx;

    std::vector<tdp> points_list;
    points_list.push_back(it_point);
    if (dx >= dy && dx >= dz) {
        while( it_point.x < end.x ) {
            it_point.x += 1;
            if (fxy > 0) {
                it_point.y += 1;
                fxy += 2 * ( dy - dx );
            } else {
                fxy += 2 * dy;
            }
            if (fxz > 0) {
                it_point.z += 1;
                fxz += 2 * ( dz - dx );
            } else {
                fxz += 2 * dz;
            }
            points_list.push_back(it_point);
        }
    }
    return points_list;
}

int main() {
	tdp test_centre(0,0,0);
	tdp test_centre2(2,2,2);
	tdhexhed cube = MakeVoxel(test_centre, 2);
	tdhexhed cube2 = MakeVoxel(test_centre2, 2);
	std::vector<face> facelist = GetFacesHexHed();
	std::ofstream file ("new.obj");
	ObjWriter objw(file);
	for (int i = 0; i < 8; i++) {
		objw.AddPoint(cube.vertices[i]);
	}
	for(std::vector<face>::iterator it = facelist.begin(); it != facelist.end(); it++) {
		objw.AddFace(it->corners[0], it->corners[1], it->corners[2], it->corners[3]);
	}
	objw.EndObj();
	for (int i = 0; i < 8; i++) {
		objw.AddPoint(cube2.vertices[i]);
	}
	for(std::vector<face>::iterator it = facelist.begin(); it != facelist.end(); it++) {
		objw.AddFace(it->corners[0], it->corners[1], it->corners[2], it->corners[3]);
	}
	objw.EndObj();

	return 0;
}
