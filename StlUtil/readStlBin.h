#ifndef READBINARY_H
#define READBINARY_H

// include files

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>


// define the vec3d struct (a 3D vector with three components)
struct vec3d
{
    /// x-component of the 3D vector
    double x;

    /// y-component of the 3D vector
    double y;

    /// z-component of the 3D vector
    double z;
};

// A triangular element is defined using the three vertices and a normal vector defining the orientation
// of the element in relation to the surface of the solid object. The normal vector usually points outwards
// for each surface element.

class triangle
{
public:

    /// 3 components of the normal vector to the triangle
    vec3d normal;

    /// 3 coordinates of the three vertices of the triangle
    vec3d point[3];

};


int read_binary_STL_file(std::string STL_filename,std::vector<triangle> & facet,
                         double & x_min, double & x_max, double & y_min, double & y_max, double & z_min, double & z_max);
#endif
