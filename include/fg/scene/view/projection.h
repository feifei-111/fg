#pragma once

#include <glm/glm.hpp>

namespace fg::scene::view {

class PerspProjGenerator {
    float fov_ = 45.0f;  // 视角
    float fov_min_, fov_max_;
    float near_ = 0.1f, far_ = 100.0f;
    float aspect_;  // width / height

    mutable glm::mat4 proj_;
    mutable bool proj_dirty_ = true;

    void UpdateProj() const;

public:
    PerspProjGenerator(float aspect,
                       float fov,
                       float near,
                       float far,
                       float fov_min = 1.0f,
                       float fov_max = 45.0f)
        : aspect_(aspect),
          fov_(fov),
          near_(near),
          far_(far),
          fov_min_(fov_min),
          fov_max_(fov_max) {}
    void SetFov(float fov);
    void AddFov(float fov_off);
    void SetRange(float near, float far);
    void SetAspect(float aspect);
    const glm::mat4& GetProj() const;
    const float GetFov() const;
};

class OrthoProjGenerator {
private:
    float near_, far_;
    float half_h_;
    float half_w_;

    mutable glm::mat4 proj_;
    mutable bool proj_dirty_ = true;

    void UpdateProj() const;

public:
    OrthoProjGenerator(float width,
                       float height,
                       float near = 0.1f,
                       float far = 100.0f)
        : half_w_(width / 2.0f),
          half_h_(height / 2.0f),
          near_(near),
          far_(far) {}
    void SetSize(float width, float height);
    const glm::mat4& GetProj() const;
};

}  // namespace fg::scene::view
