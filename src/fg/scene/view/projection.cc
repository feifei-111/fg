#include <fg/scene/view/projection.h>
#include <glm/gtc/matrix_transform.hpp>

namespace fg::scene::view {

void PerspProjGenerator::UpdateProj() const {
    proj_ = glm::perspective(glm::radians(fov_), aspect_, near_, far_);
    proj_dirty_ = false;
}

void PerspProjGenerator::SetFov(float fov) {
    if (fov > fov_max_)
        fov = fov_max_;
    else if (fov < fov_min_)
        fov = fov_min_;
    fov_ = fov;
    proj_dirty_ = true;
}

void PerspProjGenerator::AddFov(float fov_off) { SetFov(fov_ + fov_off); }

void PerspProjGenerator::SetRange(float near, float far) {
    near_ = near;
    far_ = far;
    proj_dirty_ = true;
}

void PerspProjGenerator::SetAspect(float aspect) {
    aspect_ = aspect;
    proj_dirty_ = true;
}

const glm::mat4& PerspProjGenerator::GetProj() const {
    if (proj_dirty_) UpdateProj();
    return proj_;
}

const float PerspProjGenerator::GetFov() const { return fov_; }

/* ==================================================================== */

void OrthoProjGenerator::UpdateProj() const {
    proj_ = glm::ortho(-half_w_, half_w_, -half_h_, half_h_, near_, far_);
    proj_dirty_ = false;
}

void OrthoProjGenerator::SetSize(float width, float height) {
    half_w_ = width * 0.5f;
    half_h_ = height * 0.5f;
    proj_dirty_ = true;
}

const glm::mat4& OrthoProjGenerator::GetProj() const {
    if (proj_dirty_) UpdateProj();
    return proj_;
}

}  // namespace fg::scene::view
