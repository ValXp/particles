#ifndef VECTOR3_HPP_
# define  VECTOR3_HPP_

class Vec3
{
    public:
    Vec3()
    {
    }
    Vec3(const Vec3 &cp)
    {
    }
    Vec3& operator=(const vec3 &cp)
    {
        memcpy(static_cast<void*>(v), static_cast<const void*>(cp.v), sizeof(float) * 3);
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
    float v[3];
};

#endif // VECTOR3_HPP_

