#pragma once

#include <glm/glm.hpp>

namespace fg::scene::view {

// 暂时只支持 yaw + pitch，roll 一般需要用四元数来表示
class EulerAngleCamera {
    glm::vec3 position_;
    float pitch_;   // 俯仰角 in degree
    float yaw_;     // 偏向 in degree

    // 因为有 lazy update 的行为，这里必须加 mutable，否则不能用 const method
    mutable glm::vec3 front_;
    mutable bool front_dirty_=true;
    mutable glm::mat4 view_;
    mutable bool view_dirty_=true;

    // 如果用了 mutable，那么这个 const method 的语义应该是
    // const method 不改变这个 obj 数据的语义
    // 而不是 cosnt method 不改变这个 obj 的任意数据
    void UpdateFront() const ;
    void UpdateView() const;
public:
    EulerAngleCamera(const glm::vec3& position, const glm::vec3& front, float pitch=0.0f, float yaw=-90.0f){
        position_ = position;
        front_ = front;
        pitch_ = pitch;
        yaw_ = yaw;
    }
    void SetAngles(float pitch, float yaw);
    void AddAngles(float pitch_off, float yaw_off);
    void SetPosition(const glm::vec3& position);
    void AddPosition(const glm::vec3& position_off);
    void PositionForward(float len);
    void LookAt(const glm::vec3& target);
    const glm::mat4& GetView() const;
    const glm::vec3& GetPosition() const;
    const glm::vec3& GetFront() const;
    const glm::vec3 GetRight() const;
    const glm::vec3& GetUp() const;
    float GetYaw() const;
    float GetPitch() const;
};

}