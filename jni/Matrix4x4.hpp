#ifndef MATRIX4X4_HPP_
# define MATRIX4X4_HPP_

// Pre-calculated value of PI / 180.
#define kPI180   0.017453

// Pre-calculated value of 180 / PI.
#define k180PI  57.295780

// Converts degrees to radians.
#define degreesToRadians(x) (x * kPI180)

// Converts radians to degrees.
#define radiansToDegrees(x) (x * k180PI)

class Mat4
{
    public:
    Mat4()
    {
    }
    Mat4(const Mat4 & cp)
    {
        (*this) = cp;
    }
    inline void setidentity()
    {
        bzero(static_cast<void*>(m), sizeof(float) * 16);
        m[0] = m[5] = m[10] = m[15] = 1.0;
    }
    inline Mat4 multiply(const Mat4 &m2) const
    {
        Mat4 result;

        result[0] = m[0]*m2[0] + m[4]*m2[1] + m[8]*m2[2] + m[12]*m2[3];
        result[1] = m[1]*m2[0] + m[5]*m2[1] + m[9]*m2[2] + m[13]*m2[3];
        result[2] = m[2]*m2[0] + m[6]*m2[1] + m[10]*m2[2] + m[14]*m2[3];
        result[3] = m[3]*m2[0] + m[7]*m2[1] + m[11]*m2[2] + m[15]*m2[3];

        result[4] = m[0]*m2[4] + m[4]*m2[5] + m[8]*m2[6] + m[12]*m2[7];
        result[5] = m[1]*m2[4] + m[5]*m2[5] + m[9]*m2[6] + m[13]*m2[7];
        result[6] = m[2]*m2[4] + m[6]*m2[5] + m[10]*m2[6] + m[14]*m2[7];
        result[7] = m[3]*m2[4] + m[7]*m2[5] + m[11]*m2[6] + m[15]*m2[7];

        result[8] = m[0]*m2[8] + m[4]*m2[9] + m[8]*m2[10] + m[12]*m2[11];
        result[9] = m[1]*m2[8] + m[5]*m2[9] + m[9]*m2[10] + m[13]*m2[11];
        result[10] = m[2]*m2[8] + m[6]*m2[9] + m[10]*m2[10] + m[14]*m2[11];
        result[11] = m[3]*m2[8] + m[7]*m2[9] + m[11]*m2[10] + m[15]*m2[11];

        result[12] = m[0]*m2[12] + m[4]*m2[13] + m[8]*m2[14] + m[12]*m2[15];
        result[13] = m[1]*m2[12] + m[5]*m2[13] + m[9]*m2[14] + m[13]*m2[15];
        result[14] = m[2]*m2[12] + m[6]*m2[13] + m[10]*m2[14] + m[14]*m2[15];
        result[15] = m[3]*m2[12] + m[7]*m2[13] + m[11]*m2[14] + m[15]*m2[15];

        return result;
    }
    inline void setRotationX(float angle)
    {
        float radians = degreesToRadians(angle);
        m[5] = cosf(radians);
        m[6] = -sinf(radians);
        m[9] = -m[6];
        m[10] = m[5];
    }
    inline void setRotationY(float angle)
    {
        float radians = degreesToRadians(angle);
         m[0] = cosf(radians);
         m[2] = sinf(radians);
         m[8] = -m[2];
         m[10] = m[0];
    }
    inline void setRotationZ(float angle)
    {
        float radians = degreesToRadians(angle);
        m[0] = cosf(radians);
        m[1] = sinf(radians);
        m[4] = -m[1];
        m[5] = m[0];
    }
    inline void setTranslation(float x, float y, float z)
    {
        m[12] = x;
        m[13] = y;
        m[14] = z;
    }

    inline void setProjection(float near, float far, float angleOfView, float aspectRatio)
    {
        // These paramaters are about lens properties.
        // The "near" and "far" create the Depth of Field.
        // The "angleOfView", as the name suggests, is the angle of view.
        // The "aspectRatio" is the cool thing about this matrix. OpenGL doesn't
        // has any information about the screen you are rendering for. So the
        // results could seem stretched. But this variable puts the thing into the
        // right path. The aspect ratio is your device screen (or desired area) width divided
        // by its height. This will give you a number < 1.0 the the area has more vertical
        // space and a number > 1.0 is the area has more horizontal space.
        // Aspect Ratio of 1.0 represents a square area.

        // Some calculus before the formula.
        float size = near * tanf(degreesToRadians(angleOfView) / 2.0);
        float left = -size, right = size, bottom = -size / aspectRatio, top = size / aspectRatio;

        // First Column
        m[0] = 2 * near / (right - left);
        m[1] = 0.0;
        m[2] = 0.0;
        m[3] = 0.0;

        // Second Column
        m[4] = 0.0;
        m[5] = 2 * near / (top - bottom);
        m[6] = 0.0;
        m[7] = 0.0;

        // Third Column
        m[8] = (right + left) / (right - left);
        m[9] = (top + bottom) / (top - bottom);
        m[10] = -(far + near) / (far - near);
        m[11] = -1;

        // Fourth Column
        m[12] = 0.0;
        m[13] = 0.0;
        m[14] = -(2 * far * near) / (far - near);
        m[15] = 0.0;
    }
    inline float *ptr()
    {
        return m;
    }

    Mat4 operator*(const Mat4 &m2) const
    {
        return multiply(m2);
    }
    Mat4& operator=(const Mat4 &cp)
    {
        memcpy(static_cast<void*>(m), static_cast<const void*>(cp.m), sizeof(float) * 16);
        return *this;
    }
    float &operator[](int idx)
    {
        return m[idx];
    }
    const float &operator[](int idx) const
    {
        return m[idx];
    }
    private:
    float m[16];
};


#endif // MATRIX4X4_HPP_

