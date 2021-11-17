#include "WorldTransform.h"

void WorldTransform::SetScale(float scale)
{
    m_scale = scale;
}


void WorldTransform::SetRotation(float x, float y, float z)
{
    m_rotation.x = x;
    m_rotation.y = y;
    m_rotation.z = z;
}


void WorldTransform::SetPosition(float x, float y, float z)
{
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
}


void WorldTransform::SetPosition(const glm::vec3 &WorldPos)
{
    m_pos = WorldPos;
}


void WorldTransform::Rotate(float x, float y, float z)
{
    m_rotation.x += x;
    m_rotation.y += y;
    m_rotation.z += z;
}


glm::mat4 WorldTransform::GetMatrix()
{
    glm::mat4 Scale = CreateScaleTransform(m_scale, m_scale, m_scale);
    glm::mat4 Rotation = CreateRotationTransform(m_rotation.x, m_rotation.y, m_rotation.z);
    glm::mat4 Translation = CreateTranslationTransform(m_pos.x, m_pos.y, m_pos.z);
    glm::mat4 WorldTransformformation = Translation * Rotation * Scale;
    return WorldTransformformation;
}


glm::mat4 WorldTransform::GetReversedTranslationMatrix()
{
    glm::mat4 ReversedTranslation;
    glm::vec3 negatedPos = glm::vec3(-m_pos.x, -m_pos.y, -m_pos.z);
    ReversedTranslation = CreateTranslationTransform(negatedPos.x, negatedPos.y, negatedPos.z);
    return ReversedTranslation;
}


glm::mat4 WorldTransform::GetReversedRotationMatrix()
{
    glm::mat4 ReversedRotation = CreateRotationTransform(-m_rotation.z, -m_rotation.y, -m_rotation.x);
    return ReversedRotation;
}

glm::mat4 WorldTransform::CreateScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
{
    glm::mat4 matrix;
    matrix[0][0] = ScaleX; matrix[0][1] = 0.0f;   matrix[0][2] = 0.0f;   matrix[0][3] = 0.0f;
    matrix[1][0] = 0.0f;   matrix[1][1] = ScaleY; matrix[1][2] = 0.0f;   matrix[1][3] = 0.0f;
    matrix[2][0] = 0.0f;   matrix[2][1] = 0.0f;   matrix[2][2] = ScaleZ; matrix[2][3] = 0.0f;
    matrix[3][0] = 0.0f;   matrix[3][1] = 0.0f;   matrix[3][2] = 0.0f;   matrix[3][3] = 1.0f;
    return matrix;
}

glm::mat4 WorldTransform::CreateRotationTransform(float RotateX, float RotateY, float RotateZ)
{
    glm::mat4 matrixX, matrixY, matrixZ;

    float x = glm::radians(RotateX);
    float y = glm::radians(RotateY);
    float z = glm::radians(RotateZ);

    matrixX = InitRotationX(x);
    matrixY = InitRotationY(y);
    matrixZ = InitRotationZ(z);

    return matrixZ * matrixY * matrixX;
}

glm::mat4 WorldTransform::CreateTranslationTransform(float x, float y, float z)
{
    glm::mat4 matrix;
    matrix[0][0] = 1.0f; matrix[0][1] = 0.0f; matrix[0][2] = 0.0f; matrix[0][3] = x;
    matrix[1][0] = 0.0f; matrix[1][1] = 1.0f; matrix[1][2] = 0.0f; matrix[1][3] = y;
    matrix[2][0] = 0.0f; matrix[2][1] = 0.0f; matrix[2][2] = 1.0f; matrix[2][3] = z;
    matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;
    return matrix;
}

glm::mat4 WorldTransform::InitRotationX(float x)
{
    glm::mat4 matrix;
    matrix[0][0] = 1.0f; matrix[0][1] = 0.0f; matrix[0][2] = 0.0f; matrix[0][3] = 0.0f;
    matrix[1][0] = 0.0f; matrix[1][1] = cosf(x); matrix[1][2] = -sinf(x); matrix[1][3] = 0.0f;
    matrix[2][0] = 0.0f; matrix[2][1] = sinf(x); matrix[2][2] = cosf(x); matrix[2][3] = 0.0f;
    matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;
    return matrix;
}

glm::mat4 WorldTransform::InitRotationY(float y)
{
    glm::mat4 matrix;
    matrix[0][0] = cosf(y); matrix[0][1] = 0.0f; matrix[0][2] = -sinf(y); matrix[0][3] = 0.0f;
    matrix[1][0] = 0.0f; matrix[1][1] = 1.0f; matrix[1][2] = 0.0f; matrix[1][3] = 0.0f;
    matrix[2][0] = sinf(y); matrix[2][1] = 0.0f; matrix[2][2] = cosf(y); matrix[2][3] = 0.0f;
    matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;
    return matrix;
}

glm::mat4 WorldTransform::InitRotationZ(float z)
{
    glm::mat4 matrix;
    matrix[0][0] = cosf(z); matrix[0][1] = -sinf(z); matrix[0][2] = 0.0f; matrix[0][3] = 0.0f;
    matrix[1][0] = sinf(z); matrix[1][1] = cosf(z); matrix[1][2] = 0.0f; matrix[1][3] = 0.0f;
    matrix[2][0] = 0.0f; matrix[2][1] = 0.0f; matrix[2][2] = 1.0f; matrix[2][3] = 0.0f;
    matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;
    return matrix;
}


glm::vec3 WorldTransform::WorldPosToLocalPos(const glm::vec3& WorldPos)
{
    glm::mat4 WorldToLocalTranslation = GetReversedTranslationMatrix();
    glm::mat4 WorldToLocalRotation = GetReversedRotationMatrix();
    glm::mat4 WorldToLocalTransformation = WorldToLocalRotation * WorldToLocalTranslation;
	glm::vec4 WorldPos4f = glm::vec4(WorldPos, 1.0f);
    glm::vec3 LocalPos4f = WorldToLocalTransformation * WorldPos4f;
    glm::vec3 LocalPos3f(LocalPos4f);
    return LocalPos3f;
}


glm::vec3 WorldTransform::WorldDirToLocalDir(const glm::vec3& WorldDirection)
{
    glm::mat3 World3f(GetMatrix());  // Initialize using the top left corner

    // Inverse local-to-world transformation using transpose
    // (assuming uniform scaling)
    glm::mat3 WorldToLocal = glm::transpose(World3f);

    glm::vec3 LocalDirection = WorldToLocal * WorldDirection;

    LocalDirection = glm::normalize(LocalDirection);

    return LocalDirection;
}