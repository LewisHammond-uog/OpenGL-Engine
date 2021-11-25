#ifndef __WORLDTRANSFORM_H__
#define __WORLDTRANSFORM_H__

#include "glm/glm.hpp"

//Helper class used for the transform of an object in the world
class WorldTransform {
public:
    WorldTransform() {}

    void SetScale(float scale);
    void SetRotation(float x, float y, float z);
    void SetPosition(float x, float y, float z);
    void SetPosition(const glm::vec3& WorldPos);

    void Rotate(float x, float y, float z);

    glm::mat4 GetMatrix();

    glm::vec3 WorldPosToLocalPos(const glm::vec3& WorldPos);
    glm::vec3 WorldDirToLocalDir(const glm::vec3& WorldDir);

    glm::mat4 GetReversedTranslationMatrix();
    glm::mat4 GetReversedRotationMatrix();

    float GetScale() const { return m_scale; }
    glm::vec3 GetPos() const { return m_pos; }
    glm::vec3 GetRotation() const { return m_rotation; }

    static float ToRadians(float a_degrees);

private:

    glm::mat4 CreateScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
    glm::mat4 CreateRotationTransform(float RotateX, float RotateY, float RotateZ);
    glm::mat4 CreateTranslationTransform(float x, float y, float z);

    static glm::mat4 InitRotationX(float x);
    glm::mat4 InitRotationY(float y);
    glm::mat4 InitRotationZ(float z);

    float    m_scale = 1.0f;
    glm::vec3 m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
};


#endif // __WORLDTRANSFORM_H__
