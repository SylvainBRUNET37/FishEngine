#ifndef RIGID_BODY_H
#define RIGID_BODY_H

class RigidBody
{
public:
    JPH::Body* body;

    explicit RigidBody(JPH::Body* body) : body(body) {}

    RigidBody(const RigidBody&) = delete;
    RigidBody& operator=(const RigidBody&) = delete;

    RigidBody(RigidBody&& other) noexcept : body(std::exchange(other.body, nullptr)) {}
    RigidBody& operator=(RigidBody&& other) noexcept;

    ~RigidBody() noexcept { RemoveAndDestroyBody(); }

private:
    void RemoveAndDestroyBody() noexcept;
};


#endif