#include <fg/scene/view/camera.h>
#include <glm/gtc/matrix_transform.hpp>

namespace fg::scene::view {

namespace {
const glm::vec3& WorldUp() {
    static const glm::vec3 world_up{0, 1.0f, 0};
    return world_up;
}
}  // namespace

void EulerAngleCamera::UpdateFront() const {
    front_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_.y = sin(glm::radians(pitch_));
    front_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front_);
    front_dirty_ = false;
}

void EulerAngleCamera::UpdateView() const {
    if (front_dirty_) UpdateFront();
    view_ = glm::lookAt(position_, position_ + front_, WorldUp());
    view_dirty_ = false;
}

void EulerAngleCamera::SetAngles(float yaw, float pitch) {
    if (pitch > 89.0f)
        pitch = 89.0f;
    else if (pitch < -89.0f)
        pitch = -89.0f;
    pitch_ = pitch;
    yaw_ = yaw;
    front_dirty_ = true;
    view_dirty_ = true;
}

void EulerAngleCamera::AddAngles(float yaw_off, float pitch_off) {
    SetAngles(yaw_ + yaw_off, pitch_ + pitch_off);
}

void EulerAngleCamera::SetPosition(const glm::vec3& position) {
    position_ = position;
    view_dirty_ = true;
}

void EulerAngleCamera::AddPosition(const glm::vec3& position_off) {
    position_ = position_ + position_off;
    view_dirty_ = true;
}

void EulerAngleCamera::PositionForward(float len) {
    if (front_dirty_) UpdateFront();
    position_ = position_ + len * front_;
    view_dirty_ = true;
}

void EulerAngleCamera::LookAt(const glm::vec3& target) {
    front_ = glm::normalize(target - position_);
    view_ = glm::lookAt(position_, target, WorldUp());
    pitch_ = glm::asin(front_.y);
    // glm::atan2 现在标了 experimental
    // 对于 scalar，其实就是封的 std::atan2，所以我们这里换一下
    // 不过我们还是用 glm::atan 就行了，它还是会分发到 std::atan2
    // 注意 glm::atan 有重载，如果传两个参数就是 atan2
    yaw_ = glm::atan(front_.z, front_.x);
    front_dirty_ = false;
    view_dirty_ = false;
}

const glm::mat4& EulerAngleCamera::GetView() const {
    if (view_dirty_) UpdateView();
    return view_;
}

const glm::vec3& EulerAngleCamera::GetPosition() const { return position_; }

const glm::vec3& EulerAngleCamera::GetFront() const {
    if (front_dirty_) UpdateFront();
    return front_;
}

const glm::vec3 EulerAngleCamera::GetRight() const {
    if (front_dirty_) UpdateFront();
    return glm::normalize(glm::cross(front_, WorldUp()));
}

const glm::vec3& EulerAngleCamera::GetUp() const { return WorldUp(); }

float EulerAngleCamera::GetYaw() const { return yaw_; }

float EulerAngleCamera::GetPitch() const { return pitch_; }

}  // namespace fg::scene::view
