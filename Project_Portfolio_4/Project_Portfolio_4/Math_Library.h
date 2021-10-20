#pragma once
#include <math.h>

struct VEC_2D
{
    float x, y;
};

struct VERTEX_2D
{
    VEC_2D pos;
    unsigned color; //attribute (helps describe infor about a location
    //and how it should be drawn
};
struct VEC_3D
{
    float x, y, z;
};

struct Rotation_Vertex
{
    float x, y, z, w;
    unsigned color;
};

struct Matrix_3D
{
    float x1, y1, z1, x2, y2, z2, x3, y3, z3;
};
struct VEC_4D
{
    float x, y, z, w;
};

struct VERTEX_4D
{
    VEC_4D pos;
    unsigned color;
};

struct MATRIX
{
	float m[4][4];
};

struct MATRIX_3
{
    float m[3][3];
};


MATRIX Matrix_Identity()
{
    MATRIX M =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return M;
}

MATRIX Matrix_Translation(float x, float y, float z)
{
    //MATRIX m =
    //{
    //    {1, 0, 0, x},
    //    {0, 1, 0, y},
    //    {0, 0, 1, z},
    //    {0, 0, 0, 1}
    //};
    //
    //return m;

    MATRIX m =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    };

    return m;


    //MATRIX M;
    //M.m[0][0] = 1.0f;
    //M.m[0][1] = 0.0f;
    //M.m[0][2] = 0.0f;
    //M.m[0][3] = 0.0f;
    //
    //M.m[3][0] = x;
    //M.m[3][1] = y;
    //M.m[3][2] = z;
    //
    //return M;
}

MATRIX Matrix_Rotation_X(float degree)
{
    degree *= 3.14 / 180;
    MATRIX M = {
        1, 0, 0, 0,
        0, cos(degree), (sin(degree) * -1), 0,
        0, sin(degree), cos(degree), 0,
        0, 0, 0, 1
    };
    return M;
}
MATRIX Matrix_Rotation_Y(float degree)
{
    degree *= 3.14 / 180;
    MATRIX M =
    {
        cos(degree), 0, sin(degree), 0 ,
        0, 1, 0, 0,
        (sin(degree) * -1), 0, cos(degree), 0,
        0, 0, 0, 1
    };
    return M;
}
MATRIX Matrix_Rotation_Z(float degree)
{
    degree *= 3.14 / 180;
    MATRIX M =
    {
        cos(degree), (sin(degree) * -1), 0, 0 ,
        sin(degree), cos(degree), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return M;
}

MATRIX Matrix_Multiplication(MATRIX a, MATRIX b)
{
	int i;
	int j;
    int k;
	//MATRIX c[4][4];
    MATRIX c;
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            c.m[i][j] = 0;
        }
    }
        
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            for (k = 0; k < 4; ++k) 
            {
                c.m[i][j] += a.m[i][k] * b.m[k][j];
            }

        }

    }
    return c;
        
}
MATRIX Transpose(MATRIX a)
{
    MATRIX temp =
    {
        a.m[0][0], a.m[1][0], a.m[2][0], a.m[3][0],
        a.m[0][1], a.m[1][1], a.m[2][1], a.m[3][1],
        a.m[0][2], a.m[1][2], a.m[2][2], a.m[3][2],
        a.m[0][3], a.m[1][3], a.m[2][3], a.m[3][3],
    };
    return temp;
}
float Find_Determinant(MATRIX_3 a)
{
    float det;
    det =
        (a.m[0][0] * ((a.m[1][1] * a.m[2][2]) - (a.m[1][2] * a.m[2][1])))
        - (a.m[0][1] * ((a.m[1][0] * a.m[2][2]) - (a.m[1][2] * a.m[2][0])))
        + (a.m[0][2] * ((a.m[1][0] * a.m[2][1]) - (a.m[1][1] * a.m[2][0])));
    return det;
}
MATRIX Matrix_Inverse(MATRIX m)
{
    //make 3x3, transpose it
    MATRIX_3 new_matrix = {
        m.m[0][0], m.m[1][0], m.m[2][0],
        m.m[0][1], m.m[1][1], m.m[2][1],
        m.m[0][2], m.m[1][2], m.m[2][2]
    };
    //take the last column(the position vector)
    VEC_3D vec =
    {
        m.m[3][0],
        m.m[3][1],
        m.m[3][2]
    };
    //multiply the position vector by the 3x3
    VEC_3D new_vec = {
         (vec.x * new_matrix.m[0][0]) + (vec.y * new_matrix.m[1][0]) + (vec.z * new_matrix.m[2][0]),
         (vec.x * new_matrix.m[0][1]) + (vec.y * new_matrix.m[1][1]) + (vec.z * new_matrix.m[2][1]),
         (vec.x * new_matrix.m[0][2]) + (vec.y * new_matrix.m[1][2]) + (vec.z * new_matrix.m[2][2])
    };

    m.m[0][0] = new_matrix.m[0][0];
    m.m[0][1] = new_matrix.m[0][1];
    m.m[0][2] = new_matrix.m[0][2];

    m.m[1][0] = new_matrix.m[1][0];
    m.m[1][1] = new_matrix.m[1][1];
    m.m[1][2] = new_matrix.m[1][2];

    m.m[2][0] = new_matrix.m[2][0];
    m.m[2][1] = new_matrix.m[2][1];
    m.m[2][2] = new_matrix.m[2][2];
    //negate the new vecor and put it into the matrix, overwriting the old position
    m.m[3][0] = new_vec.x * -1;
    m.m[3][1] = new_vec.y * -1;
    m.m[3][2] = new_vec.z * -1;
    
  
    return m;
}



VERTEX_4D Vertex_Matrix_Multiplication(VERTEX_4D a, MATRIX b)
{
    VERTEX_4D vector;
    vector.pos.x = a.pos.x * b.m[0][0] + a.pos.y * b.m[1][0] + a.pos.z * b.m[2][0] + a.pos.w * b.m[3][0];
    vector.pos.y = a.pos.x * b.m[0][1] + a.pos.y * b.m[1][1] + a.pos.z * b.m[2][1] + a.pos.w * b.m[3][1];
    vector.pos.z = a.pos.x * b.m[0][2] + a.pos.y * b.m[1][2] + a.pos.z * b.m[2][2] + a.pos.w * b.m[3][2];
    vector.pos.w = a.pos.x * b.m[0][3] + a.pos.y * b.m[1][3] + a.pos.z * b.m[2][3] + a.pos.w * b.m[3][3];
    vector.color = a.color;
     return vector;
}

MATRIX Matrix_Projection( float zNear, float zFar, float FOV)
{
    FOV *= 3.14 / 180;
    float YScale = 1 / tan(0.5 * FOV);
    //float XScale = YScale * (RASTER_WIDTH / static_cast<float>(RASTER_HEIGHT));
    
    MATRIX M =
    {
        0, 0, 0, 0,
        0, YScale, 0, 0,
        0, 0, zFar / (zFar - zNear), 1,
        0, 0,  -(zFar * zNear) / (zFar - zNear),0
    };
    return M;
}
